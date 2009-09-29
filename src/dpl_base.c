#include <stdio.h>
#include <stdlib.h>

#include "dpl_base.h"

/** fast zero memory with no error checking */
void
__fast
__nonull(1)
_zmemzero(void *b, size_t n)
{
	register char *c = (char *)b;

	while (n != 0)
	{
		*c++ = 0;
		n--;
	}
}

void
__fast
_zmalloc(size_t n)
{
}

void
__noreturn
__dpl_fail(const char *assertion, const char *file, unsigned int line, const char *function)
{
	fprintf(stderr, "%s:%d: %s: dpl assertion failed: %s\n", file, line, function, assertion);
	abort();
}

void*
__malloc
__dpl_malloc(size_t len)
{
	return malloc(len);
}

void __dpl_free(void *ptr)
{
	free(ptr);
}

