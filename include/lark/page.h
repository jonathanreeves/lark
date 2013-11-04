#ifndef __PAGE_H__
#define __PAGE_H__

#include <lark/list.h>

#define MAX_PAGE_ORDER 20
#define PAGE_SHIFT 12

#define ALIGN(obj, bound) (( (obj) + ((bound)-1) ) & (~((bound) - 1)))
#define PAGE_ALIGN(x) ALIGN(x, 1<<PAGE_SHIFT)

struct page_block {
	struct list_node ltag;
	unsigned int addr;
	unsigned int order;
};

typedef struct page_block pageblk_t;

unsigned int page_alloc(unsigned int npages);
unsigned int page_alloc_init(unsigned int memsize_kb, unsigned int kern_end);
void page_dump_list(void);
int get_free_page_count(void);
unsigned int page_lock_kernel(unsigned int kern_end);

#endif
