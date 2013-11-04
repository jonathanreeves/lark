#ifndef __ASM_KTHREAD_H__
#define __ASM_KTHREAD_H__

/* make sure this can be included within lark/kthread.h */
#ifndef __KTHREAD_H__
#include <lark/kthread.h>
#endif

#define THREAD_MASK ((~KTHREAD_SIZE)+1)

static inline kthread_t *get_current(void)
{
	kthread_t *this;
	asm (
		"andl	%%esp, %0\n"
		: "=r" (this)
		: "0" (THREAD_MASK)
	);
	return this;
}

#define current get_current()

#endif
