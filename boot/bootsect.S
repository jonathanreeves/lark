
#include "defs.h"

INITSEG = DEF_INITSEG
BOOTSEG = DEF_BOOTSEG
SETUPSEG = DEF_SETUPSEG
KERNSEG = DEF_KERNSEG
PFAT_BOOT_RECORD_OFFSET = DEF_PFAT_BOOT_RECORD_OFFSET
BIOS_SIGNATURE_OFFSET = DEF_BIOS_SIGNATURE_OFFSET
SECTORS_PER_TRACK = DEF_SECTORS_PER_TRACK

/*
 * ----------------------------------------------------------------------
 * entry point of the boot sector
 * ----------------------------------------------------------------------
*/
.code16
.text

.global boot_begin
boot_begin:       /* needed to calculate padding bytes to fill the sector */

        /* Copy the boot sector into INITSEG. */
        movw    $BOOTSEG, %ax
        movw    %ax,%ds                 /* source segment for string copy */
        xorw    %si,%si                 /* source index for string copy */
        movw    $INITSEG, %ax
        movw    %ax,%es               /* destination segment for string copy */
        xorw    %di,%di               /* destination index for string copy */
        cld                           /* clear direction flag */
        movw    $256,%cx                /* number of words to copy */
        rep
        movsw                           /* copy 512 bytes */

        jmpl    $INITSEG, $after_move

after_move: 
        /* Now we're executing in INITSEG */

        /* We want the data segment to refer to INITSEG */
        /* (since we've defined variables in the same place as the code) */
        movw    %ax,%ds                 /* ax still contains INITSEG */

        /* Put the stack in the place where we were originally loaded. */
        /* By definition, there is nothing important there now. */
        movw    $0,%ax
        movw    %ax,%ss
        movw  $((BOOTSEG << 4)+512-2), %sp

load_setup: 

        /* Load the setup code. */
        movw    setup_start,%ax
        movw    %ax, sec_count
        addw    setup_size,%ax
        movw    %ax, max_sector

load_setup.again: 
        movw    sec_count,%ax
        pushw   %ax          /* 1st param to read_fd_sector (log sec num) */
        pushw   $SETUPSEG 
        subw    setup_start,%ax       /* convert to 0-indexed */
        shlw    $9,%ax               /* multiply by 512 */
        pushw   %ax                  /*  ...to get 3rd param (byte offset) */

        /* read the sector from the floppy */
        call    read_fd_sector
        addw    $6,%sp                  /* clear 3 word params */

        /* on to next sector */
        incw    sec_count

        /* are we done? */
        movw    max_sector,%bx
        cmpw    %bx, sec_count
        jl      load_setup.again

load_kernel: 

       /*
        * Load the kernel image from sectors KERN_START_SEC..n of the
        * floppy into memory at KERNSEG.  Note that there are 128 sectors
        * per 64K segment.  So, when figuring out which segment to
        * load the sector into, we shift right by 7 bits (which is
        * equivalent to dividing by 128).
        */

        /* Figure out start sector and max sector */
        movw    kernel_start, %ax
        movw    %ax, sec_count
        addw    kernel_size, %ax
        movw    %ax, max_sector

load_kernel.again: 
        movw    sec_count,%ax           /* logical sector on the floppy */
        pushw   %ax          /* 1st param to read_fd_sector (log sec num) */
        subw    kernel_start,%ax         /* convert to 0-indexed */
        movw    %ax,%cx                 /* save in cx */
        shrw    $7,%ax                  /* divide by 128 */
        shlw    $12,%ax                 /*  ...and multiply by 0x1000 */
        addw    $KERNSEG, %ax        /*  ...to get base relative to KERNSEG */
        pushw   %ax             /* 2nd param to read_fd_sector (seg base) */
        andw    $0x7f,%cx               /* mod sector by 128 */
        shlw    $9,%cx                  /*  ...and multiply by 512 */
        pushw   %cx                   /* to get offset in segment (3rd parm) */

        /* read the sector from the floppy */

        call    read_fd_sector

        addw    $6,%sp                  /* clear 3 word params */

        /* on to next sector */
        incw    sec_count

        /* have we loaded all of the sectors? */
        movw    max_sector,%bx
        cmpw    %bx, sec_count
        jl      load_kernel.again


        /* Now we've loaded the setup code and the kernel image.
         Jump to setup code. */

        jmpl     $SETUPSEG, $0

/*
 * Read a sector from the floppy drive.
 * This code (and the rest of this boot sector) will have to
 * be re-written at some point so it reads more than one
 * sector at a time.
 *
 * Parameters:
 *     - "logical" sector number   [bp+8]
 *     - destination segment       [bp+6]
 *     - destination offset        [bp+4]
 */

read_fd_sector: 
        pushw   %bp                     /* set up stack frame */
        movw    %sp,%bp                 /* same */
        pushaw                          /* save all registers */
#if 0 
/* debug params */
        movw    8(%bp),%dx
        call    PrintHex
        call    PrintNL
        movw    6(%bp),%dx
        call    PrintHex
        call    PrintNL
        movw    4(%bp),%dx
        call    PrintHex
        call    PrintNL
#endif

        /* Sector = log_sec % SECTORS_PER_TRACK */
        /* Head = (log_sec / SECTORS_PER_TRACK) % HEADS */
        movw    8(%bp),%ax            /* get logical sector number from stack */
        xorw    %dx,%dx               /* dx is high part of dividend (== 0) */
        movw    $SECTORS_PER_TRACK, %bx /* divisor */
        divw    %bx                     /* do the division */
        movw    %dx,sec                 /* sector is the remainder */
        andw    $1,%ax               /* same as mod by HEADS==2 (slight hack) */
        movw    %ax,head

        /* Track = log_sec / (SECTORS_PER_TRACK*HEADS) */
        movw    8(%bp),%ax              /* get logical sector number again */
        xorw    %dx,%dx                 /* dx is high part of dividend */
        movw    $(SECTORS_PER_TRACK*2), %bx /* divisor */
        divw    %bx                     /* do the division */
        movw    %ax,track               /* track is quotient */

#if 0
/* debugging code */
        movw    sec,%dx
        call    PrintHex
        call    PrintNL
        movw    head,%dx
        call    PrintHex
        call    PrintNL
        movw    track,%dx
        call    PrintHex
        call    PrintNL
#endif

        /* Now, try to actually read the sector from the floppy,
         retrying up to 3 times. */

        movb    $0,num_retries

read_fd_sector.again: 
        movw    6(%bp),%ax              /* dest segment... */
        movw    %ax,%es                 /*   goes in es */
        movw    $((0x02<<8)|1), %ax
                                        /*  # secs = 1 in al */
        movw    track,%bx               /* track number... */
        movb    %bl,%ch                 /*   goes in ch */
        movw    sec,%bx                 /* sector number... */
        movb    %bl,%cl                 /*   goes in cl... */
        incb    %cl                 /*   but it must be 1-based, not 0-based */
        movw    head,%bx                /* head number... */
        movb    %bl,%dh                 /*   goes in dh */
        xorb    %dl,%dl                 /* hard code drive=0 */
        movw    4(%bp),%bx              /* offset goes in bx */
                                        /*   (es:bx points to buffer) */

        /* Call the BIOS Read Diskette Sectors service */
        int     $0x13

        /* If the carry flag is NOT set, then there was no error
         and we're done. */
        jnc     read_fd_sector.done

        /* Error - code stored in ah */
        movw    %ax,%dx
        call    PrintHex
        incb    num_retries
        cmpb    $3,num_retries
        jne     read_fd_sector.again

        /* If we got here, we failed thrice, so we give up */
        movw    $0xdead,%dx
        call    PrintHex
read_fd_sector.here: jmp     read_fd_sector.here

read_fd_sector.done: 

        popaw                           /* restore all regisiters */
        popw    %bp                     /* leave stack frame */
        ret


/*
 * The following were copied from ktk-0.2 bootsect.asm, and were presumably
 * from the Linux bootsect code.  I changed them a little so they
 * don't clobber the caller's registers.
 */

/* Print the word contained in the dx register to the screen. */

PrintHex: 
        pusha
        movw  $4,%cx            /* 4 hex digits */
PrintHex.PrintDigit: 
        rolw  $4,%dx            /* rotate so that lowest 4 bits are used */
        movw  $0xE0F,%ax        /* ah = request, al = mask for nybble */
        andb  %dl,%al
        addb  $0x90,%al     /* convert al to ascii hex (four instructions) */
        daa                 /* I've spent 1 hour to understand how it works.. */
        adcb  $0x40,%al
        daa
        int   $0x10
        loop  PrintHex.PrintDigit
        popa
        ret

/* Print a newline. */
PrintNL:                        /* print CR and NL */
        pushw   %ax
        movw    $0xE0D,%ax      /* CR */
        int     $0x10
        movb    $0xA,%al        /* LF */
        int     $0x10
        popw    %ax
        ret


/*
* ----------------------------------------------------------------------
* Variables
* ----------------------------------------------------------------------
*/

/* These are used by read_fd_sector */
head: .word 0
track: .word 0
sec: .word 0
num_retries: .byte 0

/* Used for loops reading sectors from floppy */
sec_count: .word 0
max_sector: .word 0

/* Padding to make the PFAT Boot Record sit just before the BIOS signature. */
.org PFAT_BOOT_RECORD_OFFSET, 0

/* PFAT boot record
 Describes how to load the setup program and kernel.
 The default values are appropriate for creating a boot
 floppy by concatenating the boot sector, setup program,
 and kernel image.  The buildFat program will change
 these values if the boot floppy is formatted as a PFAT
 filesystem. */

        .word 0
        .word 0

        .word 0
        .word 0

        .word 0
        .word 0

        .word 0
        .word 0

        .word 0
        .word 0

/* part of pfat boot record */
setup_start:             
        .word 1                   /* by default, setup is at first sector */

/* part of pfat boot record */
setup_size: .word NUM_SETUP_SECTORS

/* part of pfat boot record */
kernel_start: .word NUM_SETUP_SECTORS+1

/* part of pfat boot record */
kernel_size: .word NUM_KERN_SECTORS 

/* Finish by writing the BIOS signature to mark this as
 a valid boot sector. */
.org BIOS_SIGNATURE_OFFSET, 0
Signature: .word 0xAA55


