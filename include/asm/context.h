#ifndef __CONTEXT_H__
#define __CONTEXT_H__

#define GP_REG_CONTEXT_SIZE 11*4  /* 11 registers as dwords */
/* thread context size including eflags and cs */
#define KTHREAD_CONTEXT_SIZE (GP_REG_CONTEXT_SIZE+(2*4))


#ifdef __ASM__

/* general purpose registers. Don't use the push instruction.
 * gcc doesn't because each push has an inherent subtract and
 * thus takes longer when there are lots of them */
#define gp_register_save \
	subl	$(11*4), %esp; \
	movw	%gs, 10*4(%esp); \
	movw	%fs, 9*4(%esp); \
	movw	%es, 8*4(%esp); \
	movw	%ds, 7*4(%esp); \
	movl	%ebp, 6*4(%esp); \
	movl	%edi, 5*4(%esp); \
	movl	%esi, 4*4(%esp); \
	movl	%edx, 3*4(%esp); \
	movl	%ecx, 2*4(%esp); \
	movl	%ebx, 1*4(%esp); \
	movl	%eax, 0(%esp); 

/* same thing with pops */
#define gp_register_restore \
	movl	0(%esp), %eax; \
	movl	1*4(%esp), %ebx; \
	movl	2*4(%esp), %ecx; \
	movl	3*4(%esp), %edx; \
	movl	4*4(%esp), %esi; \
	movl	5*4(%esp), %edi; \
	movl	6*4(%esp), %ebp; \
	movw	7*4(%esp), %ds; \
	movw	8*4(%esp), %es; \
	movw	9*4(%esp), %fs; \
	movw	10*4(%esp), %gs; \
	addl	$(11*4), %esp;

#else  /* can be used in C functions */

struct reg_context {
	int eax;
	int ebx;
	int ecx;
	int edx;
	int esi;
	int edi;
	int ebp;
	int ds;
	int es;
	int fs;
	int gs;
	int eip;
	int cs;
	int eflags;
	/* these last two are only valid when switching from user
	 * to kernel mode */
	int esp;
	int ss;
};


#define gp_register_save \
	__asm__ __volatile__ ( \
		"subl	$(11*4), %%esp\n\t" \
		"movw	%%gs, 10*4(%%esp)\n\t" \
		"movw	%%fs, 9*4(%%esp)\n\t" \
		"movw	%%es, 8*4(%%esp)\n\t" \
		"movw	%%ds, 7*4(%%esp)\n\t" \
		"movl	%%ebp, 6*4(%%esp)\n\t" \
		"movl	%%edi, 5*4(%%esp)\n\t" \
		"movl	%%esi, 4*4(%%esp)\n\t" \
		"movl	%%edx, 3*4(%%esp)\n\t" \
		"movl	%%ecx, 2*4(%%esp)\n\t" \
		"movl	%%ebx, 1*4(%%esp)\n\t" \
		"movl	%%eax, 0(%%esp)\n\t" \
		: \
		: \
		: "memory" \
		);

#define gp_register_restore \
	__asm__ __volatile__ ( \
		"movl	0(%%esp), %%eax\n\t" \
		"movl	1*4(%%esp), %%ebx\n\t" \
		"movl	2*4(%%esp), %%ecx\n\t" \
		"movl	3*4(%%esp), %%edx\n\t" \
		"movl	4*4(%%esp), %%esi\n\t" \
		"movl	5*4(%%esp), %%edi\n\t" \
		"movl	6*4(%%esp), %%ebp\n\t" \
		"movw	7*4(%%esp), %%ds\n\t" \
		"movw	8*4(%%esp), %%es\n\t" \
		"movw	9*4(%%esp), %%fs\n\t" \
		"movw	10*4(%%esp), %%gs\n\t" \
		"addl	$(11*4), %%esp\n\t" \
		: \
		: \
		: "memory" \
		);

#endif

#endif
