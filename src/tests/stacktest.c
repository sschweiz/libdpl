#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define DPL_REPORT_ERRORS
#define DPL_DEFAULT_LEVEL DPL_ANNOY
#include "../dpl_error.h"
#include "../dpl_stack.h"

int main(int argc, char **argv)
{
	/* null uses the global stack */
	dpl_stack_init(NULL, 10);
	int a, b, *c;
	char buf[16];

	a = 5;
	b = 6;

	dpl_push(a);
	printf("%d\n", dpl_pop());

	dpl_push(b);	/* push b */
	b *= 10;	/* modify the value of b */
	b = dpl_pop();	/* restore value of b */
	printf("%d\n", b);

	/* this is valid because the stack can also hold
	 pointers to the data (so if the data is gone the stack
	 value is 0 */
	c = (int *)malloc(sizeof(int));
	*c = 7;
	/* use the DPL_VALUEOF(x) macro to store a pointer */
	dpl_push(DPL_VALUEOF(c));
	free(c);
	printf("%d\n", *(int *)dpl_pop());

	/* it can also hold strings! (as a pointer) */
	strcpy(buf, "Hello World!\0");
	dpl_push(DPL_VALUEOF(buf));
	dpl_assert(dpl_stack_size(NULL) == 1);
	printf("%s\n", dpl_pop());

	dpl_assert(dpl_stack_empty(NULL));
	dpl_assert(dpl_stack_constraints(NULL) == 10);
	
	return 0;
}

