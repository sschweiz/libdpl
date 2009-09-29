#ifndef __DPL_LIST__
#define __DPL_LIST__

#include "dpl_base.h"

#define DPL_LIST_HEAD_KEY	-1
#define DPL_LIST_TAIL_KEY	-2
#define DPL_LIST_ERROR_KEY	-101
#define DPL_LIST_READY 0x01

#define DPL_LIST_TYPE_STACK 0	// FILO
#define DPL_LIST_TYPE_QUEUE 1	// FIFO

typedef struct __dpl_list_single
{
	void *satellite;

	struct __dpl_list_single *next;
} dpl_list_single;

typedef struct __dpl_list_double
{
	void *satellite;

	struct __dpl_list_double *next;
	struct __dpl_list_double *prev;
} dpl_list_double;

DPL_TYPE(node, dpl_list_double);
DPL_TYPE(iter, dpl_list_double*);

typedef struct __dpl_list_container
{
	dpl_iter_t head;
	dpl_iter_t tail;

	int count;
	int ready;
	unsigned char type;
} dpl_list_t;

// basic linked list helpers
DPL_FUNC(list_add, int)(dpl_iter_t iter, dpl_node_t e);
DPL_FUNC(list_del, int)(dpl_iter_t iter);
DPL_FUNC(list_find, dpl_iter_t)(dpl_iter_t start, void *sat, int (*compare)(void *, void *));
DPL_FUNC(list_make, dpl_node_t)(void *sat, int size, void (*assign)(void *, void *));

DPL_FUNC(list_init, void)(dpl_list_t *c, unsigned char type);
DPL_FUNC(list_destroy, void)(dpl_list_t *c);
DPL_FUNC(list_push, int)(dpl_list_t *c, dpl_node_t e);
DPL_FUNC(list_pop, int)(dpl_list_t *c);
DPL_FUNC(list_insert, int)(dpl_list_t *c, dpl_node_t e, dpl_iter_t iter);
DPL_FUNC(list_insert_before, int)(dpl_list_t *c, dpl_node_t e, dpl_iter_t iter);
DPL_FUNC(list_insert_after, int)(dpl_list_t *c, dpl_node_t e, dpl_iter_t iter);

// container iterator helper functions
DPL_FUNC(list_first, dpl_iter_t)(dpl_list_t *c);
DPL_FUNC(list_last, dpl_iter_t)(dpl_list_t *c);
DPL_FUNC(list_head, dpl_iter_t)(dpl_list_t *c);
DPL_FUNC(list_tail, dpl_iter_t)(dpl_list_t *c);
DPL_FUNC(list_next, dpl_iter_t)(dpl_iter_t e);
DPL_FUNC(list_prev, dpl_iter_t)(dpl_iter_t e);

void dpl_list_iter(dpl_iter_t *e) __nonull(1);

#endif
