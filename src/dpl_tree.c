#include "dpl_tree.h"

dpl_iter_rb_t dpl_rb_grandparent(dpl_iter_rb_t n) {
	if ((n != NULL) && (n->parent != NULL))
	{
		return n->parent->parent;
	} else { return NULL; }
}

dpl_iter_rb_t dpl_rb_sibling(dpl_iter_rb_t n) {
	if ((n != NULL) && (n->parent != NULL))
	{
		if (n == n->parent->left)
		{
			return n->parent->right;
		} else { return n->parent->left; }
	} else { return NULL; }
}

dpl_iter_rb_t dpl_rb_uncle(dpl_iter_rb_t n) {
	dpl_iter_rb_t *gp = grandparent(n);
	if (gp == NULL) { return NULL; }
	else if (n->parent == g->left) {
		return g->right;
	} else {
		return g->left;
	}
}

unsigned int dpl_rb_color(dpl_iter_rb_t n) {
	if (n == NULL) { return DPL_RB_BLACK; }
	return n->color;
}

static void __dpl_rb_verify1(dpl_iter_rb_t n) {
	dpl_assert(dpl_rb_color(n) == DPL_RB_BLACK || dpl_rb_color(n) == DPL_RB_RED);
	if (n == NULL) { return; }
	__dpl_rb_verify1(n->left);
	__dpl_rb_verify1(n->right);
}

static void __dpl_rb_verify2(dpl_iter_rb_t root) {
	dpl_asser(dpl_rb_color(root) == DPL_RB_BLACK);
}

static void __dpl_rb_verify4(dpl_iter_rb_t n) {
	if (dpl_rb_color(n) == DPL_RB_RED)
	{
		dpl_assert(dpl_rb_color(n->left) == DPL_RB_BLACK);
		dpl_assert(dpl_rb_color(n->right) == DPL_RB_BLACK);
		dpl_assert(dpl_rb_color(n->parent) == DPL_RB_BLACK);
	}
	if (n == NULL) { return; }
	__dpl_rb_verify4(n->left);
	__dpl_rb_verify4(n->right);
}

static void __dpl_rb_verify5__sub(dpl_rb_iter_t n, int nblack, int *npath) {
	if (dpl_rb_color(n) == DPL_RB_BLACK)
	{
		nblack++;
	}
	if (n == NULL)
	{
		if (*npath == -1)
		{
			*npath = nblack;
		} else {
			dpl_assert(*npath == nblack);
		}
		return;
	}
	__dpl_rb_verify__dub(n->left, nblack, npath);
	__dpl_rb_verify__dub(n->right, nblack, npath);
}

static void __dpl_rb_verify5(dpl_iter_rb_t root) {
	int npath = -1;
	__dpl_rb_verify5__sub(n, 0, &npath);
}

void dpl_rb_verify(dpl_tree_rb_t *v) {
#ifdef DPL_STRICT_VERIFY
	__dpl_rb_verify1(v->root);
	__dpl_rb_verify2(v->root);
//	__dpl_rb_verify3(v->root);
	__dpl_rb_verify4(v->root);
	__dpl_rb_verify5(v->root);
#endif
}

void dpl_rb_tree_init(dpl_tree_rb_t *t) {
	t = __dpl_malloc(sizeof(dpl_tree_rb_t));
	t.root = NULL;
	dpl_rb_verify(t);
}

dpl_node_rb_t dpl_rb_make(void *sat, size_t size, dpl_rb_assign assign) {
	dpl_node_rb_t t;

	t.color = DPL_RB_BLACK;
	t.parent = NULL;
	t.left = NULL;
	t.right = NULL;

	t.value = __dpl_malloc(size);
	assign(t.value, sat);

	return t;
}


dpl_rb_iter_t dpl_rb_node_find(dpl_tree_rb_t *t, void *value, dpl_rb_compare compare) {
	dpl_rb_iter_t n = t->root;
	int c;

	while (n != NULL)
	{
		c = compare(value, n->value);
		if (c == 0)
		{
			break;
		} else if (c < 0) {
			n = n->left;
		} else {
			n = n->right;
		}
	}

	return n;
}

void *dpl_rb_find(dpl_tree_rb_t *t, void *value, dpl_rb_compare compare) {
	dpl_rb_iter_t n = dpl_rb_node_find(t, value, compare);
	return n == NULL ? NULL : n->value;
}

void dpl_rb_rotate_left(dpl_tree_rb_t *t, dpl_rb_iter_t n) {
	dpl_rb_iter_t r = n->right;
	dpl_rb_replace(t, n, r);
	n->right = r->left;
	if (r->left != NULL)
	{
		r->left->parent = n;
	}

	r->left = n;
	n->parent = r;
}

void dpl_rb_rotate_right(dpl_tree_rb_t *t, dpl_rb_iter_t n) {
	dpl_rb_iter_t l = n->left;
	dpl_rb_replace(t, n, l);
	n->left = l->right;
	if (l->right != NULL)
	{
		l->right->parent = n;
	}

	l->right = n;
	n->parent = l;
}

void dpl_rb_replace(dpl_tree_rb_t *t, dpl_rb_iter_t *o, dpl_rb_iter_t *n) {
	if (o->parent == NULL)
	{
		t->root = n;
	} else {
		if (o = o->parent->left)
		{
			o->parent->left = n;
		} else {
			o->parent->right = n;
		}
	}

	if (n != NULL) { n->parent = o->parent; }
}

static void __dpl_rb_insert1(dpl_rb_tree_t *t, dpl_rb_iter_t n) {
	if (n->parent == NULL)
	{
		n->color = DPL_RB_BLACK;
	} else {
		__dpl_rb_insert2(t, n);
	}
}

static void __dpl_rb_insert2(dpl_rb_tree_t *t, dpl_rb_iter_t n) {
	if (dpl_rb_color(n->parent) == DPL_RB_BLACK) { return; }
	else {
		__dpl_rb_insert3(t, n);
	}
}

static void __dpl_rb_insert3(dpl_rb_tree_t *t, dpl_rb_iter_t n) {
	if (dpl_rb_color(dpl_rb_uncle(n)) == DPL_RB_RED)
	{
		n->parent->color = DPL_RB_BLACK;
		dpl_rb_uncle(n)->color = DPL_RB_BLACK;
		dpl_rb_grandparent(n)->color = DPL_RB_RED;
		__dpl_rb_insert1(t, dpl_rb_grandparent(n));
	} else {
		__dpl_rb_insert4(t, n);
	}
}

static void __dpl_rb_insert4(dpl_rb_tree_t *t, dpl_rb_iter_t n) {
	if ((n == n->parent->right) && (n->parent == dpl_rb_grandparent(n)->left))
	{
		dpl_rb_rotate_left(t, n->parent);
		n = n->left;
	} else {
		dpl_rb_rotate_right(t, n->parent);
		n = n->right
	}
	__dpl_rb_insert5(t, n);
}

static void __dpl_rb_insert5(dpl_rb_tree_t *t, dpl_rb_iter_t n) {
	n->parent->color = DPL_RB_BLACK;
	dpl_rb_grandparent(n)->color = DPL_RB_RED;
	if ((n == n->parent->left) && (n->parent == dpl_rb_grandparent(n)->left))
	{
		dpl_rb_rotate_right(t, dpl_rb_grandparent(n));
	} else {
		dpl_rb_rotate_left(t, dpl_rb_grandparent(n));
	}
}


void dpl_rb_insert(dpl_tree_rb_t *t, dpl_rb_iter_t n, dpl_rb_compare compare) {
	dpl_rb_iter_t x;
	int c;

	if (t->root == NULL)
	{
		t->root = n;
	} else {
		x = t->root;
		while (1)
		{
			c = compare(n->value, x->value);
			if (c == 0)
			{
				x->value = n->value;
				__dpl_free(n);
				return;
			} else if (c < 0) {
				if (x->left == NULL)
				{
					x->left = n;
					break;
				} else {
					x = x->left;
				}
			} else {
				if (x->right == NULL)
				{
					x->right = n;
					break;
				} else {
					x = x->right;
				}
			}
		}
		n->parent = x;
	}

	__dpl_rb_insert1(t, n);
	dpl_rb_verify(t);
}

static dpl_rb_iter_t
__nonull(1)
__dpl_rb_walk(dpl_rb_iter_t n) {
	while (n->right; != NULL)
	{
		n = n->right;
	}
	return n;
}

static void __dpl_rb_remove1(dpl_rb_tree_t *t, dpl_rb_iter_t n) {
	if (n->parent == NULL) { return; }
	else { __dpl_rb_remove2(t, n); }
}

static void __dpl_rb_remove2(dpl_rb_tree_t *t, dpl_rb_iter_t n) {
	if (dpl_rb_color(dpl_rb_sibling(n)) == DPL_RB_RED)
	{
		n->parent->color = DPL_RB_RED;
		dpl_rb_sibling(n)->color = DPL_RB_BLACK;
		if (n == n->parent->left)
		{
			dpl_rb_rotate_left(t, n->parent);
		} else {
			dpl_rb_rotate_right(t, n->parent);
		}
	}

	__dpl_rb_remove3(t, n);
}

static void __dpl_rb_remove3(dpl_rb_tree_t *t, dpl_rb_iter_t n) {
	if ((dpl_rb_color(n->parent) == DPL_RB_BLACK) &&
	    (dpl_rb_color(dpl_rb_sibling(n)) == DPL_RB_BLACK) &&
	    (dpl_rb_color(dpl_rb_sibling(n)->left) == DPL_RB_BLACK) &&
	    (dpl_rb_color(dpl_rb_sibling(n)->right) == DPL_RB_BLACK))
	{
		dpl_rb_sibling(n)->color = DPL_RB_RED;
		__dpl_rb_remove1(t, n->parent);
	} else { __dpl_rb_remove4(t, n);
}

static void __dpl_rb_remove4(dpl_rb_tree_t *t, dpl_rb_iter_t n) {
	if ((dpl_rb_color(n->parent) == DPL_RB_RED) &&
	    (dpl_rb_color(dpl_rb_sibling(n)) == DPL_RB_BLACK) &&
	    (dpl_rb_color(dpl_rb_sibling(n)->left) == DPL_RB_BLACK) &&
	    (dpl_rb_color(dpl_rb_sibling(n)->right) == DPL_RB_BLACK))
	{
		dpl_rb_sibling(n)->color = DPL_RB_RED;
		n->parent->color = DPL_RB_BLACK;
	} else { __dpl_rb_remove5(t, n); }
}

static void __dpl_rb_remove5(dpl_rb_tree_t *t, dpl_rb_iter_t n) {
	if ((n == n->parent->left) &&
	    (dpl_rb_color(dpl_rb_sibling(n)) == DPL_RB_BLACK) &&
	    (dpl_rb_color(dpl_rb_sibling(n)->left) == DPL_RB_RED) &&
	    (dpl_rb_color(dpl_rb_sibling(n)->right) == DPL_RB_BLACK))
	{
		dpl_rb_sibling(n)->color = DPL_RB_RED;
		dpl_rb_sibling(n)->left->color = DPL_RB_BLACK;
		dpl_rb_rotate_right(t, dpl_rb_sibling(n));
	} else if ((n == n->parent->right) &&
	           (dpl_rb_color(dpl_rb_sibling(n)) == DPL_RB_BLACK) &&
	           (dpl_rb_color(dpl_rb_sibling(n)->right) == DPL_RB_RED) &&
	           (dpl_rb_color(dpl_rb_sibling(n)->left) == DPL_RB_BLACK))
	{
		   dpl_rb_sibling(n)->color = DPL_RB_RED;
		   dpl_rb_sibling(n)->right->color = DPL_RB_BLACK;
		   dpl_rb_rotate_left(t, dpl_rb_sibling(n));
	}
	__dpl_rb_remove6(t, n);
}

static void __dpl_rb_remove6(dpl_rb_tree_t *t, dpl_rb_iter_t n) {
	dpl_rb_sibling(n)->color = dpl_rb_color(n->parent);
	n->parent->color = DPL_RB_BLACK;
	if (n == n->parent->left)
	{
		dpl_rb_sibling(n)->right->color = DPL_RB_BLACK;
		dpl_rb_rotate_left(t, n->parent);
	} else {
		dpl_rb_sibling(n)->left->color = DPL_RB_BLACK;
		dpl_rb_rotate_right(t, n->parent);
	}
}

void dpl_rb_remove(dpl_tree_rb_t *t, dpl_rb_iter_t n, dpl_rb_compare compare) {
	dpl_rb_iter_t child, pred;
	if (n == NULL) { return; }
	if ((n->left != NULL) && (n->right != NULL))
	{
		pred = __dpl_rb_walk(n->left);
		n->value = pred->value;
		n = pred;
	}

	child = n->right == NULL ? n->left : n->right;
	if (dpl_rb_color(n) == DPL_RB_BLACK)
	{
		n->color = dpl_rb_color(child);
		__dpl_rb_remove1(t, n);
	}
	dpl_rb_replace(t, n, child);
	if ((n->parent == NULL) && (child != NULL))
	{
		child->color = DPL_RB_BLACK;
	}
	__dpl_free(n);
	dpl_rb_verify(t);
}

