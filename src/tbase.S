#include <asm/sysinfo.h>
#include <asm/common.h>
#include <asm/context.h>

.text

#define TRAP_FUNC (GP_REG_CONTEXT_SIZE-8)
#define ERR_CODE (GP_REG_CONTEXT_SIZE+8)

.align 4
trap_common:
	/* make room to save context, using the fact that we
	 * already have two element pushed (trap handler)
	 * and error code */
	subl	$(GP_REG_CONTEXT_SIZE-8), %esp
	movl	%eax, 0(%esp)  # save eax
	movl	%ebx, 4(%ebp)  # save ebx
	movl	TRAP_FUNC(%esp), %eax   # grab the handler
	movl	ERR_CODE(%esp), %ebx   # grab the error code

        /* now save the rest of the context */
	movl	%ecx, 8(%esp)
	movl	%edx, 12(%esp)
	movl	%esi, 16(%esp)
	movl	%edi, 20(%esp)
	movl	%ebp, 24(%esp)
	movw	%ds, 28(%esp)
	movw	%es, 32(%esp)
	movw	%fs, 36(%esp)
	movw	%gs, 40(%esp)

	/* get a new (kernel) data descriptor */
	movl	$LARK_KERNEL_DS, %ecx
	movl	%ecx, %ds
	movl	%ecx, %es
	movl	%ecx, %fs
	movl	%ecx, %gs

	/* set-up the stack for the handler function call */
	pushl	%esp
	pushl	%ebx

	/* and call the trap handler */
	call	*%eax
	addl	$8, %esp # get rid of arguments 

	/* restore all registers */
	gp_register_restore
	iret

int_unknown:
	/* unknown faults will print a message and be ignored */
	movl	ERR_CODE(%esp), %eax  # get the error code
	subl	$8, %esp # get ready for a printk
	movl	%eax, 4(%esp)
	movl	$no_int_str, 0(%esp)
	call	printk
	addl	$8, %esp 
	ret 

/* dummy fault handler for faults with error code */
EXPORT_SYM(trap_with_err)
trap_with_err:
	pushl	$int_unknown
	jmp	trap_common

/* dummy fault handler for faults with no error code */
EXPORT_SYM(trap_no_err)
trap_no_err:
	pushl	$0
	pushl	$int_unknown
	jmp	trap_common

/* divide by zero error */
.align 4
EXPORT_SYM(trap_divide)
IMPORT_SYM(handle_divide) # see traps.c
trap_divide:
	pushl	$0
	pushl	$handle_divide
	jmp	trap_common

/* breakpoint interrupt exception */
EXPORT_SYM(trap_breakpoint)
IMPORT_SYM(handle_breakpoint) # see traps.c
trap_breakpoint:
	pushl	$0
	pushl	$handle_breakpoint
	jmp	trap_common

/* invalid opcode fault */
EXPORT_SYM(trap_opcode)
IMPORT_SYM(handle_opcode) # see traps.c
trap_opcode:
	pushl	$0
	pushl	$handle_opcode
	jmp	trap_common

/* double fault */
EXPORT_SYM(trap_double)
trap_double:
	pushl	$int_unknown
	jmp	trap_common

/* bad TSS exception */
EXPORT_SYM(trap_bad_tss)
trap_bad_tss:
	pushl	$int_unknown
	jmp	trap_common

/* stack fault exception */
EXPORT_SYM(trap_stack)
trap_stack:
	pushl	$int_unknown
	jmp	trap_common

/* general protection fault */
EXPORT_SYM(trap_gpf)
trap_gpf:
	pushl	$int_unknown
	jmp	trap_common

/* page fault */
EXPORT_SYM(trap_page)
trap_page:
	pushl	$int_unknown
	jmp	trap_common

.data

no_int_str:
.string "unknown fault with error code 0x%x\n"

