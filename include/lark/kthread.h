#ifndef __KTHREAD_H__
#define __KTHREAD_H__

#include <lark/list.h>
#include <lark/page.h>

typedef struct kthread {
	unsigned int flags; /* runnable, waiting, etc */
	unsigned int esp;
	unsigned int stack_base;
	unsigned int vm86int; /* vm86 interrupt flag */
	int priority; /* support negative priority? */
	LIST_ENTRY;
	unsigned int tid;
} kthread_t;

kthread_t *kthread_new(void (*ktfunc)(void), int priority);
void thread_switch(kthread_t *cur, kthread_t *next);
void thread_switch_isr(void);
void wakeup_queue(struct list_node *queue);
void suspend_current(struct list_node *queue);
void schedule(void);
void make_runnable(kthread_t *th);

/* allocate 2 pages for a thread */
#define KTHREAD_PAGES 2
#define KTHREAD_SIZE (KTHREAD_PAGES<<PAGE_SHIFT)

/* this include requires above definitions */
#include <asm/kthread.h>

#endif
