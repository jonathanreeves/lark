#include <asm/sysinfo.h>
#include <asm/descr.h>
#include <lark/video.h>
#include <lark/kthread.h>
#include <lark/page.h>
#include <lark/traps.h>
#include <lark/sema.h>
#include <asm/pcibios.h>

#include <lib/string.h>

#define TICKSIZE 5000000

kthread_t *t1, *t2, *tmain;
kthread_t *next_thread;

sema_t tsema;

void thread1(void)
{
	printk("\nstarting thread 1\n");
	printk("initializing semaphore\n");
	sema_init(&tsema, 0);	
	while(1){
		printk("waiting on semaphore\n");
		sema_down(&tsema);
	}
}

void thread2(void)
{
	int i;
	printk("starting thread 2\n"); 
	while(1){
		i = 0;
		/* put in an artificial delay */
		while(i < TICKSIZE){
			i++;
		}
		printk("signaling semaphore\n");
		sema_up(&tsema);
	}
}

/* idle thread is rather unexciting */
void idle(void)
{
	while(1){
		schedule();
	}
}

void lark_start(int memsizekb, int memstart)
{

	video_init();
	printk("Detected %d KB of total memory\n", memsizekb);
	printk("Kernel image starts at 0x%08x\n", 0x100000);
	printk("kernel image ends at 0x%08x\n", memstart);
	
	traps_init();
	pcibios_init();

	memstart = page_alloc_init(memsizekb, memstart);
	memstart = page_lock_kernel(memstart);
	printk("kernel initialization finished\n");
	printk("free memory starts at 0x%08x\n", memstart);

	video_set_attr(VIDEO_GREEN, VIDEO_BLACK);
	printk("\nWelcome to LARK!\n");
	video_set_attr(VIDEO_WHITE, VIDEO_BLACK);

	/* now try some thread stuff */	
	t1 = kthread_new(thread1, 2); /* highest priority */
	make_runnable(t1);
	t2 = kthread_new(thread2, 1); /* next highest priority */
	make_runnable(t2);
	tmain = kthread_new(idle, 0);
	make_runnable(tmain);

//	printk("attempting to throw exception\n");
//	asm volatile("int $0x0\n\t" : : : "memory");
//	printk("done with exception\n");

	/* start everything */
	schedule();

}

