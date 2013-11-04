#include <asm/sysinfo.h>
#include <asm/context.h>
#include <asm/descr.h>
#include <lark/traps.h>
#include <lark/video.h>
#include <lark/types.h>

void handle_divide(int err_code, struct reg_context *regs)
{
	printk("exception: divide by zero\n");
	return;
}

void handle_breakpoint(int err_code, struct reg_context *regs)
{
	int status = 1;
	printk("exception: breakpoint\n");
	printk("ds = 0x%x\n", regs->ds);
	printk("eip = 0x%x\n", regs->eip);
	printk("cs = 0x%x\n", regs->cs);
	printk("eflags = 0x%x\n", regs->eflags);
	
	/* spin until status changes */
	while(status);
	
	return;
}

void handle_opcode(int err_code, struct reg_context *regs)
{
	printk("exception: invalid opcode\n");
	printk("ds = 0x%x\n", regs->ds);
	printk("eip = 0x%x\n", regs->eip);
	printk("cs = 0x%x\n", regs->cs);
	printk("eflags = 0x%x\n", regs->eflags);
	printk("currently unable to handle opcode fault\n");
	video_set_attr(VIDEO_WHITE, VIDEO_RED);
	printk("kernel panic!\n");
	while(1);
}

void handle_gpf(int err_code, struct reg_context *regs)
{
	printk("exception: invalid opcode\n");
	printk("ds = 0x%x\n", regs->ds);
	printk("eip = 0x%x\n", regs->eip);
	printk("cs = 0x%x\n", regs->cs);
	printk("eflags = 0x%x\n", regs->eflags);
	printk("currently unable to handle opcode fault\n");
	video_set_attr(VIDEO_WHITE, VIDEO_RED);
	printk("kernel panic!\n");
	while(1);
}

void traps_init(void)
{
	printk("initializing trap handlers\n");
	load_idt_descriptor((u32)trap_divide, LARK_KERNEL_CS, 0);
	load_idt_descriptor((u32)trap_no_err, LARK_KERNEL_CS, 1);
	load_idt_descriptor((u32)trap_no_err, LARK_KERNEL_CS, 2);
	load_idt_descriptor((u32)trap_breakpoint, LARK_KERNEL_CS, 3);
	load_idt_descriptor((u32)trap_no_err, LARK_KERNEL_CS, 4);
	load_idt_descriptor((u32)trap_no_err, LARK_KERNEL_CS, 5);
	load_idt_descriptor((u32)trap_opcode, LARK_KERNEL_CS, 6);
	load_idt_descriptor((u32)trap_no_err, LARK_KERNEL_CS, 7);
	load_idt_descriptor((u32)trap_double, LARK_KERNEL_CS, 8);
	load_idt_descriptor((u32)trap_no_err, LARK_KERNEL_CS, 9);
	load_idt_descriptor((u32)trap_bad_tss, LARK_KERNEL_CS, 10);
	load_idt_descriptor((u32)trap_with_err, LARK_KERNEL_CS, 11);
	load_idt_descriptor((u32)trap_stack, LARK_KERNEL_CS, 12);
	load_idt_descriptor((u32)trap_gpf, LARK_KERNEL_CS, 13);
	load_idt_descriptor((u32)trap_page, LARK_KERNEL_CS, 14);
	load_idt_descriptor((u32)trap_no_err, LARK_KERNEL_CS, 15);
	load_idt_descriptor((u32)trap_no_err, LARK_KERNEL_CS, 16);
	load_idt_descriptor((u32)trap_no_err, LARK_KERNEL_CS, 17);
	load_idt_descriptor((u32)trap_no_err, LARK_KERNEL_CS, 18);

}
