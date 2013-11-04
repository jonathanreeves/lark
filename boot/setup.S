/* 
 * setup code
 * does switch to 32-bit protected mode, tries to fake what
 * Grub does in stage2
*/

#include "defs.h"

SETUPSEG = DEF_SETUPSEG
KERNSEG = DEF_KERNSEG
KERNEL_CS = DEF_KERNEL_CS
KERNEL_DS = DEF_KERNEL_DS
KERN_STACK = DEF_KERN_STACK
BOOT_INFO_SIZE=DEF_BOOT_INFO_SIZE

.code16
.text

start_setup: 

        # Redefine the data segment so we can access variables
        # declared in this file.
        movw    $SETUPSEG, %ax
        movw    %ax,%ds

        # Use int 15h to find out size of extended memory in KB.
        # Extended memory is the memory above 1MB.  So by
        # adding 1MB to this amount, we get the total amount
        # of system memory.  We can only detect 64MB this way,
        # but that's OK for now.
mem_detect_801: 
	stc
	xorw	%cx,%cx
	xorw	%dx,%dx
	movw	$0xe801,%ax
	int	$0x15
	jc	mem_detect_88

	cmpw	$0x0,%cx
	jne	e801usecxdx
	movw	%ax,%cx
	movw	%bx,%dx

e801usecxdx: 
	andl	$0xffff,%edx
	shll	$6,%edx
	movl	%edx,bmem_size_kb  # ds is set so this works
	andl	$0xffff,%ecx
	addl	%ecx,bmem_size_kb # same here

mem_detect_88: 
	movb    $0x88,%ah
	int     $0x15
	addw    $1024,%ax       # 1024 KB == 1 MB
	movw    %ax,mem_size_kbytes
	# Kill the floppy motor.
	call    kill_fd_motor

	# Block interrupts, since we can't meaningfully handle them yet
	# and we no longer need BIOS services.
	cli

	# Set up IDT and GDT registers
	lidt    IDT_pointer
	lgdt    GDT_pointer

        # enable the A20 address line
        call    A20_enable

        # Switch to protected mode!
        movw    $0x01,%ax
        lmsww   %ax

        # Jump to 32 bit code.
        jmpl $KERNEL_CS, $(SETUPSEG<<4 + setup_32)

.code32
setup_32: 
	movw	$KERNEL_DS, %ax
	movw	%ax, %ds
	movw	%ax, %es
	movw	%ax, %fs
	movw	%ax, %gs
	movw	%ax, %ss

	/* the 16-bit code copied the kernel to address 0x10000 
	 * because that was the best it could do in 16-bit real
         * Now we have 32-bit code, move it to its final home at 
         * 0x100000
	 */
	movl	$NUM_KERN_SECTORS, %ecx
	shll	$7, %ecx            # number of kernel dwords
	movl	$0x100000, %edi
	movl	$(KERNSEG<<4), %esi
	cld
	rep;	movsl
	
        # Create the stack for the initial kernel thread.
        movl     $(KERN_STACK + 4096), %esp

	# the boot_info struct
	movl	$1024, SETUPSEG<<4 + boot_info + 4   # mem start
	movl	SETUPSEG<<4 + bmem_size_kb, %eax     # high mem size
	movl	%eax, SETUPSEG<<4 + boot_info + 8
	movl	$((SETUPSEG<<4) + boot_info), %ebx

	# and jump to the kernel base code
	jmpl	$KERNEL_CS, $0x100000

.code16

# Kill the floppy motor.
# This code was shamelessly stolen from Linux.
kill_fd_motor: 
        movw    $0x3f2,%dx
        xorb    %al,%al
        outb    %al,%dx
        ret

# Linux uses this code.
# The idea is that some systems issue port I/O instructions
# faster than the device hardware can deal with them.
io_delay: 
        jmp     io_delay.done
io_delay.done: ret

# Enable the A20 address line, so we can correctly address
# memory above 1MB.
A20_enable: 
        movb    $0xD1,%al
        outb    %al, $0x64
        call    io_delay
        movb    $0xDF,%al
        outb    %al, $0x60
        call    io_delay
        ret


# ----------------------------------------------------------------------
# Setup data
# ----------------------------------------------------------------------
.code32
mem_size_kbytes: .word 0
bmem_size_kbytes: .word 0

.align 4
boot_info:
	.fill BOOT_INFO_SIZE, 1, 0

# ----------------------------------------------------------------------
# The GDT.  Creates flat 32-bit address space for the kernel
# code, data, and stack.  Note that this GDT is just used
# to create an environment where we can start running 32 bit
# code.  The kernel will create and manage its own GDT.
# ----------------------------------------------------------------------

# GDT initialization stuff
NUM_GDT_ENTRIES = 3 # number of entries in GDT
GDT_ENTRY_SZ = 8    # size of a single GDT entry

.align 8, 0
GDT: 
        # Descriptor 0 is not used
        .word 0
        .word 0
        .word 0
        .word 0

        # Descriptor 1: kernel code segment
        .word 0xFFFF    # bytes 0 and 1 of segment size
        .word 0x0000    # bytes 0 and 1 of segment base address
        .byte 0x00      # byte 2 of segment base address
        .byte 0x9A      # present, DPL=0, non-system, code, non-conforming,
                        #   readable, not accessed
        .byte 0xCF      # granularity=page, 32 bit code, upper nibble of size
        .byte 0x00      # byte 3 of segment base address

        # Descriptor 2: kernel data and stack segment
        # NOTE: what Intel calls an "expand-up" segment
        # actually means that the stack will grow DOWN,
        # towards lower memory.  So, we can use this descriptor
        # for both data and stack references.
        .word 0xFFFF    # bytes 0 and 1 of segment size
        .word 0x0000    # bytes 0 and 1 of segment base address
        .byte 0x00      # byte 2 of segment base address
        .byte 0x92      # present, DPL=0, non-system, data, expand-up,
                        #   writable, not accessed
        .byte 0xCF      # granularity=page, big, upper nibble of size
        .byte 0x00      # byte 3 of segment base address

GDT_pointer: 
        .word NUM_GDT_ENTRIES*GDT_ENTRY_SZ 
        .long SETUPSEG<<4 + GDT 

IDT_pointer: 
        .word 0
        .long 00


