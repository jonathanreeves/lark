#include <lark/page.h>
#include <lark/video.h>
#include <asm/util.h>

/* array size depends on memory amount */
static struct page_block *all_pages; 

static int n_free;
static int n_total;
static int total_order;

static list_t free_list[MAX_PAGE_ORDER] = 
	{{&free_list[0], &free_list[0]},
	 {&free_list[1], &free_list[1]},
	 {&free_list[2], &free_list[2]},
	 {&free_list[3], &free_list[3]},
	 {&free_list[4], &free_list[4]},
	 {&free_list[5], &free_list[5]},
	 {&free_list[6], &free_list[6]},
	 {&free_list[7], &free_list[7]},
	 {&free_list[8], &free_list[8]},
	 {&free_list[9], &free_list[9]},
	 {&free_list[10], &free_list[10]},
	 {&free_list[11], &free_list[11]},
	 {&free_list[12], &free_list[12]},
	 {&free_list[13], &free_list[13]},
	 {&free_list[14], &free_list[14]},
	 {&free_list[15], &free_list[15]},
	 {&free_list[16], &free_list[16]},
	 {&free_list[17], &free_list[17]},
	 {&free_list[18], &free_list[18]},
	 {&free_list[19], &free_list[19]}};



int get_free_page_count(void)
{
	return n_free;
}

void page_subdivide(struct list_node *pgnode)
{
	struct page_block *cur, *new;
	int pgidx, nxtpg;

	/* get the page block associated with the node */
	cur = list_node_owner(pgnode, struct page_block);
	
	/* get the list indices for the two page blocks that will result */
	pgidx = cur->addr >> PAGE_SHIFT;
	nxtpg = pgidx + (1 << (cur->order-1));

	/* adjust the current block to have order one less */
	cur->order--;
	list_del_node(pgnode);
	list_prepend_obj(&free_list[cur->order], cur);

	/* set up the new page block. Address should be static */
	new = &all_pages[nxtpg];
	new->order = cur->order;
	list_prepend_obj(&free_list[new->order], new);
	
}

void page_free(unsigned int addr)
{
	unsigned int idx, order, blksize;
	struct page_block *blk, *buddy;

	/* find the page block associated with this address */
	idx = addr >> PAGE_SHIFT;
	blk = &all_pages[idx];
	order = blk->order;

	/* attach this to the appropriate free list */
	list_prepend_obj(&free_list[order], blk);

	blksize = 1 << order;
	n_free += blksize;

	/* now merge buddies */
	while(order <= total_order){

		/* get the buddy, always merging with the same address */
		buddy = &all_pages[idx^blksize];

		/* if the order isn't the same or the buddy isn't free, 
		 * we have no more hope */
		if(buddy->order != blk->order || list_isempty(&buddy->ltag)){
			break;
		}
		
		list_del_obj(buddy);
		list_del_obj(blk);
		
		if(buddy->addr < blk->addr){
			swap(&buddy, &blk);
			idx = idx^blksize;
		}

		blk->order++;
		list_prepend_obj(&free_list[blk->order], blk);
		/* critical: We must reset the deleted block */
		list_init_obj(buddy);

		order = order + 1;
		blksize = blksize << 1;
	}
}


unsigned int page_alloc(unsigned int npages)
{
	int i, order;
	struct page_block *pg;

	/* FIXME: this should be one function */	
	order = log2(npages); 
	if(((1 << order) - 1) & npages){
		order = order+1;
	}

	/* find the first nonempty page list of order greater than or
	 * equal do the requested order */	
	i = order;
	while(list_isempty(&free_list[i]) && (i++ < total_order));

	/* subdivide accordingly */
	while(i > order){
		/* divide the first available page block on the list */
		page_subdivide(free_list[i--].next);
	}

	/* now we're ready to allocate */
	pg = list_node_owner(free_list[order].next, struct page_block);
	list_del_node(free_list[order].next);
	/* critcal...must reinitialize the tag */
	list_init_obj(pg);

	n_free -= (1 << order);
	return pg->addr;
}

unsigned int page_alloc_init(unsigned int memsize_kb, unsigned int kern_end)
{
	int i;

	n_total = memsize_kb >> 2;
	all_pages = (struct page_block *)ALIGN(kern_end, 4);

	total_order = log2(n_total); 

	printk("Initalizing page allocator\n");
	/* initialize page addresses */
	for(i = 0; i < n_total; i++){
		all_pages[i].addr = i << PAGE_SHIFT; 
		all_pages[i].order = 0;
		list_init_obj(&all_pages[i]);
	}

	/* setup one page block covering all memeory */
	all_pages[0].order = total_order;
	list_prepend_obj(&free_list[total_order], &all_pages[0]);

	n_free = n_total;
	printk("%d total pages in system\n", n_total);

	return (ALIGN(kern_end, 4) + (n_total*sizeof(pageblk_t)));
}

void page_dump_list(void)
{
	int i;
	struct list_node *cur;
	struct page_block *blk;

	for(i = 0; i <= total_order; i++){
		printk("page list %d = \n", i);
		list_loop(&free_list[i], cur){
			blk = list_node_owner(cur, pageblk_t);
			printk("\taddr = 0x%08x\n", blk->addr);
			printk("\torder = %d\n", blk->order);
		}
		printk("\n");
	}
}

unsigned int page_lock_kernel(unsigned int kern_end)
{
	unsigned int kpages;
	unsigned int addr;
	
	kern_end = PAGE_ALIGN(kern_end);
	kpages = kern_end >> PAGE_SHIFT;

	addr = page_alloc(kpages);
	if(addr != 0){
		printk("error: some pages already allocated\n");
	}

	printk("%d total pages available\n", get_free_page_count());
	return kern_end;
}
