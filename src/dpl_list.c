#include "dpl_base.h"
#include "dpl_error.h"
#include "dpl_list.h"

int dpl_list_add(dpl_iter_t iter, dpl_node_t e) {
	dpl_node_t *n = __dpl_malloc(sizeof(dpl_node_t));
	if (n == NULL)
	{
		dpl_error(DPL_ERROR_MALLOC, DPL_DEFAULT_LEVEL);
		return DPL_LIST_ERROR_KEY;
	}

	n->next = iter->next;
	if (iter->next != NULL) { iter->next->prev = n; }
	iter->next = n;
	n->prev = iter;

	n->satellite = e.satellite;

	return 0;
}

int dpl_list_del(dpl_iter_t iter) {
	dpl_node_t *prev, *next;
	prev = iter->prev;
	next = iter->next;

	next->prev = prev;
	prev->next = next;
	
	__dpl_free(iter);
	return 0;
}

dpl_iter_t dpl_list_find(dpl_iter_t start, void *sat, int (*compare)(void *, void *)) {
	dpl_iter_t tmp = start;
	while (tmp != NULL)
	{
		if (compare(tmp->satellite, sat) == 0) { return tmp; }
		tmp = tmp->next;
	}

	dpl_error(DPL_ERROR_LISTNOKEY, DPL_DEFAULT_LEVEL);
	return NULL;
}

dpl_node_t dpl_list_make(void *sat, int size, void (*assign)(void *, void *)) {
	dpl_node_t t;

	t.satellite = __dpl_malloc(size);
	assign(t.satellite, sat);

	return t;
}

int dpl_list_push(dpl_list_t *c, dpl_node_t e) {
	int ret;
	if (c->ready != DPL_LIST_READY)
	{
		dpl_error(DPL_ERROR_LISTNOTREADY, DPL_DEFAULT_LEVEL);
		return DPL_LIST_ERROR_KEY;
	}

	ret = dpl_list_add(c->tail->prev, e);
	c->count++;

	return ret;
}

int dpl_list_pop(dpl_list_t *c) {
	int ret;
	if (c->ready != DPL_LIST_READY)
	{
		dpl_error(DPL_ERROR_LISTNOTREADY, DPL_DEFAULT_LEVEL);
		return DPL_LIST_ERROR_KEY;
	}

	if (c->type == DPL_LIST_TYPE_STACK)
	{
		ret = dpl_list_del(c->tail->prev);
	} else {
		ret = dpl_list_del(c->head->next);
	}

	return ret;
}

int dpl_list_insert(dpl_list_t *c, dpl_node_t e, dpl_iter_t iter) {
	return dpl_list_insert_after(c, e, iter);
}

int dpl_list_insert_before(dpl_list_t *c, dpl_node_t e, dpl_iter_t iter) {
	int ret;
	if (c->ready != DPL_LIST_READY)
	{
		dpl_error(DPL_ERROR_LISTNOTREADY, DPL_DEFAULT_LEVEL);
		return DPL_LIST_ERROR_KEY;
	}

	// you cannot add before the head
	if (iter == c->head)
	{
		dpl_list_add(c->head, e);
		ret = DPL_LIST_HEAD_KEY;
	} else { ret = dpl_list_add(iter->prev, e); }

	c->count++;

	return ret;
}

int dpl_list_insert_after(dpl_list_t *c, dpl_node_t e, dpl_iter_t iter) {
	int ret;
	if (c->ready != DPL_LIST_READY)
	{
		dpl_error(DPL_ERROR_LISTNOTREADY, DPL_DEFAULT_LEVEL);
		return DPL_LIST_ERROR_KEY;
	}

	// you cannot add after the tail
	if (iter == c->tail)
	{
		dpl_list_add(c->tail->prev, e);
		ret = DPL_LIST_TAIL_KEY;
	} else { ret = dpl_list_add(iter, e); }

	c->count++;

	return ret;
}

void dpl_list_init(dpl_list_t *c, unsigned char type) {
	c->tail = __dpl_malloc(sizeof(dpl_node_t));
	c->head = __dpl_malloc(sizeof(dpl_node_t));
	
	if ((c->tail == NULL) || (c->head == NULL))
	{
		dpl_error(DPL_ERROR_MALLOC, DPL_DEFAULT_LEVEL);
		return;
	}

	c->tail->next = NULL;
	c->head->prev = NULL;
	c->head->next = c->tail;
	c->tail->prev = c->head;

	c->ready = DPL_LIST_READY;
	c->type = type;
	c->count = 0;
}

dpl_iter_t dpl_list_first(dpl_list_t *c) {
	return c->head->next;
}

dpl_iter_t dpl_list_last(dpl_list_t *c) {
	return c->tail->prev;
}

dpl_iter_t dpl_list_head(dpl_list_t *c) {
	return c->head;
}

dpl_iter_t dpl_list_tail(dpl_list_t *c) {
	return c->tail;
}

dpl_iter_t dpl_list_prev(dpl_iter_t e) {
	return e->prev;
}

dpl_iter_t dpl_list_next(dpl_iter_t e) {
	return e->next;
}

void 
__nonull(1)
dpl_list_iter(dpl_iter_t *e) {
	dpl_iter_t z;
	z = (*e)->next;

	*e = z;
}
