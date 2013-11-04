#include <asm/common.h>
#include <asm/context.h>
#include <asm/sysinfo.h>

//IMPORT_SYM(current)
IMPORT_SYM(next_thread)

.text

/* void thread_switch(kthread_t *cur, kthread_t *next) 
 * where kthead_t is defined as:
 *	unsigned int flags
 *	unsigned int esp
 *	void *stack_base
 *	int priority
 *	unsigned int tid
 */
.globl thread_switch
thread_switch:
	cli
	/* grab the return address */
	pushl	%eax
	movl	4(%esp), %eax
	/* clear both of them */
	addl	$8, %esp
	/* save the flags. Context of eax is still safe */
	pushfl
	subl	$8, %esp
	movl	%eax, 0(%esp) # set the return address    
	movl	4(%esp), %eax # restore the eax context
	movl	$LARK_KERNEL_CS, 4(%esp) # set code segment
	
	/* save all registers */
	gp_register_save
	/* first grab the parameters */
	movl	KTHREAD_CONTEXT_SIZE+4(%esp), %eax # current thread
	movl	KTHREAD_CONTEXT_SIZE+8(%esp), %ebx # next thread


thread_switch_common:
	/* actual thread switch */
	movl	%esp, 4(%eax)
	movl	4(%ebx), %esp 

	gp_register_restore
	/* will return with interrupts disabled */
	iret

/* Here's the thread-switching ISR. This will be mapped to interrupt 
 * number 0x40 (64) because I like that number and it's far away from
 * hardware interrupts. This interrupt should search for the best
 * runnable thread and choose it to run.
 */
.globl thread_switch_isr
thread_switch_isr:
	gp_register_save
//	movl	current, %eax
	movl	next_thread, %ebx
	jmp thread_switch_common

.data

debugstr: 
.string "thread flags = 0x%08x\n"
