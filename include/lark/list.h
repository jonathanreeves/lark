#ifndef __LIST_H__
#define __LIST_H__

struct list_node {
	struct list_node *prev;
	struct list_node *next;
};

typedef struct list_node list_t;

#define LIST_ENTRY struct list_node ltag
#define LIST_ENTRY_INIT struct list_node ltag = {&ltag, &ltag};

#define DECLARE_LIST(name) \
	list_t name = {&name, &name};

#define list_node_owner(node, type) \
	( (type *)( (char *)node - (char *)(&((type *)0)->ltag) ) )

#define list_loop(list, ctr) \
	for(ctr = (list)->next; ctr != list; ctr = (ctr)->next)

#define list_loop_back(list, ctr) \
	for(ctr = (list)->prev; ctr != list; ctr = (ctr)->prev)

#define list_init_obj(obj) list_init(&(obj)->ltag)
static void inline list_init(list_t *node)
{
	node->next = node;
	node->prev = node;
}

#define list_prepend_obj(list, obj) __list_prepend(list, &(obj)->ltag)
#define list_prepend_node(list, node) __list_prepend(list, node)
static void inline __list_prepend(list_t *node, list_t *new)
{
	new->prev = node->prev;
	new->next = node;

	node->prev->next = new;
	node->prev = new;
}

#define list_append_obj(list, obj) __list_append(list, &(obj)->ltag)
#define list_append_node(list, obj) __list_append(list, obj)
static void inline __list_append(list_t *node, list_t *new)
{
	new->prev = node;
	new->next = node->next;

	node->next->prev = new;
	node->next = new;
}

/* this one is dangerous to use */
#define list_del_obj(obj) list_del_node(&(obj)->ltag)

/* use only with an initialized list */
static void inline list_del_node(list_t *node)
{
	node->prev->next = node->next;
	node->next->prev = node->prev;
}

static void inline list_clear(list_t *node)
{
	node->next = node;
	node->prev = node;
}


#define list_ismember(list, obj) __list_ismember(list, &(obj)->ltag)
static int inline __list_ismember(list_t *list, list_t *check)
{
	list_t *ctr;
	list_loop(list, ctr){
		if(ctr == check){
			return 1;
		}
	}
	return 0;
}

static int inline list_isempty(list_t *list)
{
	return (list->next == list);
}

#endif
