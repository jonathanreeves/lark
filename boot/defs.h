/* 
 * Definitions for use in GeekOS boot code
 * Copyright (c) 2001, David H. Hovemeyer <daveho@cs.umd.edu>
 * $Revision: 1.2 $

 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".

 * A lot of this code is adapted from Kernel Toolkit 0.2
 * and Linux version 2.2.x, so the following copyrights apply:

 * Copyright (C) 1991, 1992 Linus Torvalds
 * modified by Drew Eckhardt
 * modified by Bruce Evans (bde)
 * adapted for Kernel Toolkit by Luigi Sgro
 */

#ifndef DEFS_ASM
#define DEFS_ASM

/* BIOS loads the boot sector at offset 0 in this segment */
#define DEF_BOOTSEG 0x07C0

/* We'll move the boot sector up to higher memory. */
/* Note that the "ISA hole" begins at segment 0xA000 == 640K. */
#define DEF_INITSEG 0x9000

/* Put the setup code here, just after the boot sector. */
#define DEF_SETUPSEG 0x9020

/* Load our "Kernel" at this segment, which starts at 64K.
   The number of sectors in the kernel, NUM_KERN_SECTORS,
   will be passed on the command line. */
#define DEF_KERNSEG 0x1000

/* Size of PFAT boot record. */
/* Keep up to date with <geekos/pfat.h>. */
#define DEF_PFAT_BOOT_RECORD_SIZE 28

/* Offset of BIOS signature word in boot sector. */
#define DEF_BIOS_SIGNATURE_OFFSET 510

/* Offset of PFAT boot record in boot sector. */
#define DEF_PFAT_BOOT_RECORD_OFFSET DEF_BIOS_SIGNATURE_OFFSET - DEF_PFAT_BOOT_RECORD_SIZE

/* Video memory segment */
#define  DEF_VIDSEG 0xb800

/* The following information is correct for a 1.44M floppy.
   Yes, I'm hard coding this. */
#define  DEF_SECTORS_PER_TRACK 18
#define  DEF_HEADS 2
#define  DEF_CYLINDERS 80

/* Kernel code and data segment selectors. 
   Keep these up to date with defs.h. */
#define   DEF_KERNEL_CS 1<<3 /* kernel code segment is GDT entry 1 */
#define   DEF_KERNEL_DS 2<<3 /* kernel data segment is GDT entry 2 */

/* Pages for context object and stack for initial kernel thread -
   the one we construct for Main().  Keep these up to date with defs.h.
   We put them at 1MB, for no particular reason. */
#define   DEF_KERN_THREAD_OBJ (1024*1024 + 0x400000)
#define   DEF_KERN_STACK DEF_KERN_THREAD_OBJ + 4096

#define DEF_BOOT_INFO_SIZE (13*4)

#endif

