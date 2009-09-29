#ifndef __DPL_TREE__
#define __DPL_TREE__

#include "dpl_base.h"

#define DPL_RB_BLACK	0
#define DPL_RB_RED	1

#define DPL_STRICT_VERIFY

typedef struct __dpl_node_rb_t
{
	void *value;

	unsigned int color;

	struct __dpl_tree_tb_t *parent;
	struct __dpl_tree_tb_t *right;
	struct __dpl_tree_tb_t *left;
} dpl_node_rb_t;

DPL_TYPE(iter_rb, dpl_node_rb_t*);

typedef struct __dpl_tree_rb_t
{
	dpl_iter_rb_t root;
} dpl_tree_rb_t;

typedef void (*dpl_rb_assign)(void *a, void *b);
typedef int (*dpl_rb_compare)(void *a, void *b);

/* relationship */
dpl_iter_rb_t dpl_rb_grandparent(dpl_iter_rb_t n);
dpl_iter_rb_t dpl_rb_uncle(dpl_iter_rb_t n);
dpl_iter_rb_t dpl_rb_sibling(dpl_iter_rb_t n);

/* adherance to red-black tree properties */
void dpl_rb_verify(dpl_tree_rb_t *v);
unsigned int dpl_rb_color(dpl_iter_rb_t n);

/* creation */
void dpl_rb_tree_init(dpl_tree_rb_t *t);
dpl_node_rb_t dpl_rb_make(void *sat, size_t size, dpl_rb_assign assign);

/* insertion, rotation, lookup, deletion */
dpl_rb_iter_t dpl_rb_node_find(dpl_tree_rb_t *t, void *value, dpl_rb_compare compare);
void *dpl_rb_find(dpl_tree_rb_t *t, void *value, dpl_rb_compare compare);
void dpl_rb_rotate_left(dpl_tree_rb_t *t, dpl_rb_iter_t n);
void dpl_rb_rotate_right(dpl_tree_rb_t *t, dpl_rb_iter_t n);
void dpl_rb_replace(dpl_tree_rb_t *t, dpl_rb_iter_t *o, dpl_rb_iter_t *n);
void dpl_rb_insert(dpl_tree_rb_t *t, dpl_rb_iter_t n, dpl_rb_compare compare);
void dpl_rb_remove(dpl_tree_rb_t *t, dpl_rb_iter_t n, dpl_rb_compare compare);

#endif
