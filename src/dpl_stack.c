#include "dpl_stack.h"
#include "dpl_error.h"

#include <stdlib.h>

dpl_stack_t dpl_global_stack;

void dpl_stack_init(dpl_stack_t *s, size_t n)
{
	if (s == NULL) { s = &dpl_global_stack; }
	s->ptr = (dpl_stype_t *)malloc(sizeof(dpl_stype_t) * n);
	if (s->ptr == NULL)
	{
		dpl_error(DPL_ERROR_MALLOC, DPL_DEFAULT_LEVEL);
		s->len = 0;
		return;
	}

	s->bottom = 0;
	s->offset = 0;
	s->len = n;
	s->n = 0;
}

void dpl_stack_resize(dpl_stack_t *s, size_t n)
{
	if (s == NULL) { s = &dpl_global_stack; }
	if (s->len == 0)
	{
		dpl_stack_init(s, n);
	} else {
		s->ptr = realloc(s->ptr, n);
		if (s->ptr == NULL)
		{
			dpl_error(DPL_ERROR_MALLOC, DPL_DEFAULT_LEVEL);
			s->len = 0;
			return;
		}
		s->bottom = 0;
		s->offset = 0;
		s->len = n;
		s->n = 0;
	}
}

void dpl_stack_free(dpl_stack_t *s)
{
	if (s == NULL) { s = &dpl_global_stack; }
	s->bottom = 0;
	s->len = 0;
	s->offset = 0;
	s->n = 0;
	free(s->ptr);
}

void dpl_stack_push(dpl_stack_t *s, dpl_stype_t p)
{
	if (s == NULL) { s = &dpl_global_stack; }
	if (s->offset == DPL_OFFSET_EMPTY) { s->offset = 0; }
	if (s->offset >= s->len)
	{
		dpl_error(DPL_ERROR_STACKOVERFLOW, DPL_DEFAULT_LEVEL);
		s->offset = DPL_OFFSET_INFINITY;
		return;
	}

	s->ptr[s->offset] = p;
	s->bottom = p;
	s->offset++;
	s->n++;
}

unsigned int dpl_stack_empty(dpl_stack_t *s)
{
	if (s == NULL) { s = &dpl_global_stack; }
	if ((s->offset == DPL_OFFSET_EMPTY) || (s->n == 0))
	{
		return 1;
	}

	return 0;
}

dpl_stype_t dpl_stack_pop(dpl_stack_t *s)
{
	dpl_stype_t r;
	if (s == NULL) { s = &dpl_global_stack; }
	if (s->len == 0) { return 0; }

	s->offset--;
	if (s->offset <= 0)
	{
		if (s->offset == 0)
		{
			r = s->ptr[s->offset];
			s->n--;
		} else {
			dpl_error(DPL_ERROR_STACKEMPTY, DPL_DEFAULT_LEVEL);
			r = DPL_VALUEOF(NULL);
		}
		s->offset = DPL_OFFSET_EMPTY;
	} else {
		r = s->ptr[s->offset];
		s->n--;
	}

	return r;
}

dpl_stype_t dpl_stack_bottom(dpl_stack_t *s)
{
	if (s == NULL) { s = &dpl_global_stack; }
	return (s->bottom);
}

size_t dpl_stack_constraints(dpl_stack_t *s)
{
	if (s == NULL) { s = &dpl_global_stack; }
	return s->len;
}

size_t dpl_stack_size(dpl_stack_t *s)
{
	if (s == NULL) { s = &dpl_global_stack; }
	return s->n;
}

void dpl_push(dpl_stype_t p)
{
	if (dpl_global_stack.offset == DPL_OFFSET_EMPTY) { dpl_global_stack.offset = 0; }
	if (dpl_global_stack.offset >= dpl_global_stack.len)
	{
		dpl_error(DPL_ERROR_STACKOVERFLOW, DPL_DEFAULT_LEVEL);
		dpl_global_stack.offset = DPL_OFFSET_INFINITY;
		return;
	}

	dpl_global_stack.ptr[dpl_global_stack.offset] = p;
	dpl_global_stack.bottom = p;
	dpl_global_stack.offset++;
	dpl_global_stack.n++;
}

dpl_stype_t dpl_pop()
{
	dpl_stype_t r;
	if (dpl_global_stack.len == 0) { return 0; }

	dpl_global_stack.offset--;
	if (dpl_global_stack.offset <= 0)
	{
		if (dpl_global_stack.offset == 0)
		{
			r = dpl_global_stack.ptr[dpl_global_stack.offset];
			dpl_global_stack.n--;
		} else {
			dpl_error(DPL_ERROR_STACKEMPTY, DPL_DEFAULT_LEVEL);
			r = DPL_VALUEOF(NULL);
		}
		dpl_global_stack.offset = DPL_OFFSET_EMPTY;
	} else {
		r = dpl_global_stack.ptr[dpl_global_stack.offset];
		dpl_global_stack.n--;
	}

	return r;
}

