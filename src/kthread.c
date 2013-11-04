#include <asm/sysinfo.h>
#include <asm/context.h>
#include <lark/kthread.h>
#include <lark/page.h>
#include <lark/video.h>

#define ESP_START (KTHREAD_SIZE - KTHREAD_CONTEXT_SIZE - 4)
DECLARE_LIST(run_list);

kthread_t *kthread_new(void (*ktfunc)(void), int priority)
{
	kthread_t *thread;
	unsigned int *ptr;
	int i;

	thread = (kthread_t *)page_alloc(KTHREAD_PAGES);

	if(thread == (kthread_t *)0) {
		printk("error: no memory\n");
		return (kthread_t *)0;
	}

	/* setup thread elements */
	thread->flags = 0;
	thread->priority = priority;
	thread->stack_base = (unsigned int)thread;
	thread->esp = (unsigned int)thread + ESP_START;
	list_init_obj(thread);
	
	/* Setup the initial stack context. GP registers should be 0 */
	ptr = (unsigned int *)thread->esp;
	for(i = 0; i < 7; i++){
		ptr[i] = 0;
	}
	/* initialize the segment registers */
	ptr[7]=LARK_KERNEL_DS;
	ptr[8]=LARK_KERNEL_DS;
	ptr[9]=LARK_KERNEL_DS;
	ptr[10]=LARK_KERNEL_DS;
       
	ptr[11] = (unsigned int)ktfunc;
	ptr[12] = LARK_KERNEL_CS;
        ptr[13] = 0;	
	
	/* FIXME: get a better thread ID */
	thread->tid = 0;
	return thread;
}

void make_runnable(kthread_t *th)
{
	/* delete this thread from any other potential list */
	list_del_obj(th);
	list_append_obj(&run_list, th);
}

void schedule(void)
{
	struct kthread *best, *test;
	struct list_node *ctr;
	int bp;

	/* initialize these guys */
	best = list_node_owner(run_list.next, struct kthread);
	bp = best->priority;

	/* just find the best priority right now */
	/* note that the idle thread should always be on the run list */
	for(ctr = run_list.next->next; ctr != &run_list; ctr = ctr->next) {
		test = list_node_owner(ctr, struct kthread);

		if(test->priority > bp) {
			best = test;
			bp = best->priority;
		}
	}
	if(best != current) {
		thread_switch(current, best);
	}
}

void wakeup_queue(struct list_node *queue)
{
	struct list_node *ctr;
	/* move each element on the wait queue to the run queue */
	/* FIXME: this can probably be optimized */
	while((ctr = queue->next) != queue) {
		list_del_node(ctr);
		list_append_node(&run_list, ctr);
	}
	schedule();	
}

void suspend_current(struct list_node *queue)
{
	kthread_t *cur;
	/* current thread must be on the run queue */
	cur = current;
	if(list_ismember(&run_list, cur)) {
		list_del_obj(current);
		list_append_obj(queue, current);
	} else {
		printk("error: can't suspend thread not on run queue\n");
	}
	schedule();
}

void print_context(unsigned int *esp){
	printk("eax = 0x%08x\n", esp[0]);
	printk("ebx = 0x%08x\n", esp[1]);
	printk("ecx = 0x%08x\n", esp[2]);
	printk("edx = 0x%08x\n", esp[3]);
	printk("esi = 0x%08x\n", esp[4]);
	printk("edi = 0x%08x\n", esp[5]);
	printk("ebp = 0x%08x\n", esp[6]);
	printk("ds = 0x%x\n", esp[7]);
	printk("es = 0x%x\n", esp[8]);
	printk("fs = 0x%x\n", esp[9]);
	printk("gs = 0x%x\n", esp[10]);
	printk("ret = 0x%08x\n", esp[11]);
	printk("cs = 0x%x\n", esp[12]);
	printk("eflags = 0x%08x\n\n", esp[13]);
}
