#include <asm/common.h>
#include <asm/sysinfo.h>
#include <asm/context.h>
#include <asm/multiboot.h>
#include <asm/pic.h>


IMPORT_SYM(_end)
IMPORT_SYM(lark_start)
IMPORT_SYM(printk)

.text

EXPORT_SYM(kernel_head)
kernel_head:
/* 
 * ebx conatins a pointer with boot parameters built either by
 * GRUB, or by us manually. The stucture is defined in 
 * lark/multiboot.h. GRUB says that to be compatible, we must 
 * put the header within the first 8k of our kernel. Thus we 
 * have plenty of instruction space to copy our boot parameters
 * to a safe and permanent place in case we want to do a fast
 * reset later.
 */
	/* store the boot parameters to their final home */
	cld
	movl	$sysinfo, %edi
	movl	%ebx, %esi
	movl	$BOOT_INFO_SIZE, %ecx
	shrl	$2, %ecx
	rep;	movsl
	/* skip the header and idt/gdt section */
	jmp	kernel_body

/* permanent home for the bootinfo struct */
.balign 4
EXPORT_SYM(sysinfo)
sysinfo:
	.fill BOOT_INFO_SIZE, 1, 0

/* and here's where we'll put the multiboot header */
.balign 4
grub_header:
	.long MBOOT_MAGIC
	.long MBOOT_FLAGS
	.long MBOOT_CHECKSUM

/* --------------------------------------------------------------
 * This is the beginning of the kernel code. Note that to do a 
 * fast reboot we will come back to this point and start over 
 * --------------------------------------------------------------
 */
 
kernel_body:
	/* get the ugly work done: remap interrupts right away */
	subl	$8, %esp
	movl	$LARK_IRQ_START, 0(%esp)
	movl	$(LARK_IRQ_START+8), 4(%esp)
	call	remap_pics 

	/* load the idt with a dummy interrupt handler */
	subl	$4, %esp # left room for 2 args above
	movl	$interrupt_blank, 0(%esp)
	movl	$LARK_KERNEL_CS, 4(%esp)
	movl	$0, 8(%esp)

	call	load_idt_descriptor

	/* now repeat that guy 255 times. Note that there is some
         * subtelty since idt entries are 64-bit */
	cld
	movl	$255*2, %ecx
	movl	$kidt, %esi
	movl	$(kidt+8), %edi
	rep;	movsl

	/* and load the idt register */
	lidt	kidt_ptr

	/* load the gdt with our default descriptors. stack still has room for 
           3 arguments. */ 
	movl	$SYS_MEM_START, 0(%esp)
	movl	8+sysinfo, %eax               # grab the highmem size
	addl	$1024, %eax                   # add 1024 to get total
                                              # memory in kb
	movl	%eax, 4(%esp)                 # pass as the limit
	movl	$LARK_KERNEL_CS_IDX, 8(%esp)
	call	load_gdt_cs

	movl	$SYS_MEM_START, 0(%esp)       # repeat for ds
                                              # second stack element 
                                              # still ok
	movl	$LARK_KERNEL_DS_IDX, 8(%esp)
	call	load_gdt_ds

        /* load the task state segment */
	addl	$4, %esp   # clear one argument
	movl	$ktss, 0(%esp)
	movl	$LARK_KERNEL_TSS_IDX, 4(%esp)
	call	load_gdt_tss

	/* all done now load the pointer */
	lgdt	kgdt_ptr

 	/* flush the instruction buffer */
	jmpl	$LARK_KERNEL_CS, $kgdt_loaded

kgdt_loaded:
	/* reload the segment registers */
	movl	$LARK_KERNEL_DS, %eax
	movl	%eax, %ds
	movl	%eax, %es
	movl	%eax, %fs
	movl	%eax, %gs
	movl	%eax, %ss

        /* set the task register to the new task segment */ 
	movl	$LARK_KERNEL_TSS, %eax
	ltr	%ax

	/* now is a good time to load a new stack */
	movl	$LARK_INIT_STACK, %esp

	/* make room for 2 args on the stack */
	subl	$8, %esp

	/* get ready to call the main kernel function */
	movl	8+sysinfo, %eax               # grab the highmem size
	addl	$1024, %eax                   # add 1024 to get total
	movl	%eax, 0(%esp)
       
	movl	$_end, %eax 
	movl	%eax, 4(%esp)  # arg 2 is mem start
	call	lark_start

lark_shutdown:
	/* 
	 *  TODO shutdown kernel here (for when we return) and
	 *  provide a fast reset
	 */
	pushl	$ret_string
	call	printk
	addl	$4, %esp
1:
	jmp	1b

/* void load_gdt_cs(u32 base, u32 size_kb, u8 idx) */
EXPORT_SYM(load_gdt_cs)
load_gdt_cs:
	pushl	%eax
	pushl	%ebx
	pushl	%ecx

	movl	$0x00c09a00, %eax   # this is a code segment
load_gdt_common:
	movzbl	12+12(%esp), %ebx
	leal	kgdt(,%ebx,8), %ebx
	movl	8+12(%esp), %ecx # size in kb
	shrl	$2, %ecx  # convert to pages
	decl	%ecx # subtract 1 for limit
	movw	%cx, 0(%ebx)   # lowest 16 bits of the gdt
	andl	$0xf0000, %ecx  # upper 4 bits masked
	orl	%ecx, %eax # now combined with flags
	movl	%eax, 4(%ebx) # write bits 32-55 of the gdt
	movl	4+12(%esp), %eax # now grabbing the base
	movw	%ax, 2(%ebx) # set the lower 16 bits of base
	roll	$8, %eax
	movb	%al, 7(%ebx) # set the last 8 bits of base
	roll	$8, %eax
	movb	%al, 4(%ebx)

	popl	%ecx
	popl	%ebx
	popl	%eax

	ret

/* void load_gdt_ds(u32 base, u32 size_kb, u8 idx) */
EXPORT_SYM(load_gdt_ds)
load_gdt_ds:
	pushl	%eax
	pushl	%ebx
	pushl	%ecx

	movl	$0x00c09200, %eax   # this is a data segment
	jmp	load_gdt_common     # cheap way to get a function


/* void load_gdt_tss(u32 base, u8 idx) */
EXPORT_SYM(load_gdt_tss)
load_gdt_tss:
	pushl	%eax
	pushl	%ebx

	movl	8+8(%esp), %ebx
	leal	kgdt(,%ebx,8), %ebx

	movw	$103, 0(%ebx) # limit is always 103

	movl	4+8(%esp), %eax  # grab the base
	movl	%eax, 2(%ebx)  # set the lower 24 bits of the base
	roll	$8, %eax
	movb	%al, 7(%ebx)  # set upper byte of base

	movw	$0x0089, %ax   # this is a task segment
	movw	%ax, 5(%ebx)   # set the flags

	popl	%ebx
	popl	%eax

 	ret


/* void load_idt_descriptor(u32 addr, u16 sel, u8 idx) 
 * 
 * note that gcc treats shorts like longs on the stack
 */
EXPORT_SYM(load_idt_descriptor)
load_idt_descriptor:
	pushl	%eax
	pushl	%ebx

	movzbl	12+8(%esp), %ebx
	lea	kidt(,%ebx,8), %ebx
	movw	$0x8e00, 4(%ebx) 
	movw	8+8(%esp), %ax
	movw	%ax, 2(%ebx)
	movl	4+8(%esp), %eax
	movw	%ax, 0(%ebx)
	shrl	$16, %eax
	movw	%ax, 6(%ebx)

	popl	%ebx
	popl	%eax

	ret

/* this is only temporary */
EXPORT_SYM(interrupt_blank)
interrupt_blank:
	gp_register_save
	movw	$LARK_KERNEL_DS, %ax
	movw	%ax, %ds
	movw	%ax, %es
	pushl	$noint_string
	call	printk
	addl	$4, %esp
	gp_register_restore
	iret

io_delay:
	jmp	1f
1:	ret


/* void remap_pics(irqlow, irqhigh) */
EXPORT_SYM(remap_pics)
remap_pics:
	pushl	%eax
	/* see asm/pic.h for details on defaults */
	movb	$PIC_ICW1_DEFAULT, %al
	outb	%al, $PIC_MASTER_CMD_PORT
	call	io_delay
	outb	%al, $PIC_SLAVE_CMD_PORT
	call	io_delay

	/* put IRQs 0-7 to location in first argument 
	 * put IRQs 8-15 to location in second argument */
	movb	8(%esp), %al
	outb	%al, $PIC_MASTER_DATA_PORT
	call	io_delay
	movb	12(%esp), %al
	outb	%al, $PIC_SLAVE_DATA_PORT
	call	io_delay
	
	/* IR2 connected to slave, slave given ID of 2 */
	movb	$(1<<PIC_MSTR_ICW3_POS_IR2), %al
	outb	%al, $PIC_MASTER_DATA_PORT
	call	io_delay
	movb	$PIC_SLAVE_ICW3_ID2, %al
	outb	%al, $PIC_SLAVE_DATA_PORT
	call	io_delay

	/* set PICS to 8086 mode */
	movb	$PIC_ICW4_DEFAULT, %al
	outb	%al, $PIC_MASTER_DATA_PORT
	call	io_delay
	outb	%al, $PIC_SLAVE_DATA_PORT
	call	io_delay
	
	/* mask out all hardware interrupts */
	movb	$0xff, %al
	outb	%al, $PIC_SLAVE_DATA_PORT
	call	io_delay
	outb	%al, $PIC_MASTER_DATA_PORT
	popl	%eax
	ret


noint_string: 
	.string "Unexpected interrupt!\n"
ret_string:
	.string "Kernel has returned, shutting down\n"


EXPORT_SYM(kgdt_ptr)
kgdt_ptr:
	.word GDT_MAX_SELECTORS*8-1
	.long kgdt

EXPORT_SYM(kidt_ptr)
kidt_ptr:
	.word IDT_MAX_INTERRUPTS*8-1
	.long kidt


.data
/* as usual, support 256 interrupts */
.balign 4
EXPORT_SYM(kidt)
kidt:
	.fill IDT_MAX_INTERRUPTS, 8, 0

/* support 32 gdt entries */
.balign 4
EXPORT_SYM(kgdt)
kgdt:
	.fill GDT_MAX_SELECTORS, 8, 0

/* support exactly one task state segment. Task state segments are
 * comprised of 26 32-bit entries */
.balign 4
EXPORT_SYM(ktss)
ktss:
	.fill 26, 4, 0

