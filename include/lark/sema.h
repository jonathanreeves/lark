#ifndef __SEMA_H__
#define __SEMA_H__

#include <lark/list.h>

struct sema {
	list_t wait_list;
	unsigned int ctr;
};

typedef struct sema sema_t;

void sema_init(sema_t *sema, int count);
void sema_up(sema_t *sema);
void sema_down(sema_t *sema);

#endif
