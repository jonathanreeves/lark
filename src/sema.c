#include <lark/sema.h>
#include <lark/kthread.h>

void sema_init(sema_t *sema, int count)
{
	sema->ctr = count;
	list_init(&sema->wait_list);
}

void sema_up(sema_t *sema)
{
	sema->ctr++;
	/* wake up threads that are waiting */
	wakeup_queue(&sema->wait_list);
}

void sema_down(sema_t *sema)
{
	/* if the counter is 0, we must sleep */
	if(sema->ctr == 0) {
		suspend_current(&sema->wait_list);
	}
	sema->ctr--;
}

