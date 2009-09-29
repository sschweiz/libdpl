#ifndef __DPL_STACK__
#define __DPL_STACK__

#include "dpl_base.h"


#define DPL_OFFSET_INFINITY	0x7fffffff
#define DPL_OFFSET_EMPTY	-1

#ifndef __dpl_stype_t__defined
typedef unsigned long dpl_stype_t;
#define __dpl_stype_t__defined
#endif

#define DPL_VALUEOF(ptr)	((dpl_stype_t)ptr)

typedef struct __dpl_stack_t
{
	dpl_stype_t *ptr;
	dpl_stype_t bottom;
	int offset;
	size_t len;
	size_t n;
} dpl_stack_t;

extern dpl_stack_t dpl_global_stack;

DPL_FUNC(stack_init, void)(dpl_stack_t *s, size_t n);
DPL_FUNC(stack_resize, void)(dpl_stack_t *s, size_t n);
DPL_FUNC(stack_free, void)(dpl_stack_t *s);
DPL_FUNC(stack_push, void)(dpl_stack_t *s, dpl_stype_t p);
DPL_FUNC(stack_pop, dpl_stype_t)(dpl_stack_t *s);
DPL_FUNC(stack_bottom, dpl_stype_t)(dpl_stack_t *s);

DPL_FUNC(stack_empty, unsigned int)(dpl_stack_t *s);

size_t __inline dpl_stack_constraints(dpl_stack_t *s);
size_t __inline dpl_stack_size(dpl_stack_t *s);

/* push and pop the global stack */
DPL_FUNC(push, void)(dpl_stype_t p);
DPL_FUNC(pop, dpl_stype_t)();

#endif

