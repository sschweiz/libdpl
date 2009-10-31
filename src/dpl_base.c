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

void *
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

void *
__malloc
__dpl_malloc(size_t len)
{
	void *ptr = NULL;
	ptr = malloc(len);
	if ((!ptr) && (!len)) { ptr = malloc(1); }
	if (!ptr) {
		/* die! */
	}

	return ptr;
}

void __dpl_free(void *ptr)
{
	free(ptr);
}

void *
__malloc
__dpl_malloc32(int32_t n) {
	void *ptr;
	int32_t *p = NULL;
	ptr = malloc(sizeof(int32_t)*(n+1));
	if (ptr) {
		p = (int32_t *)ptr;
		p[0] = n;
		p++;
	} else {
		/* die! */
	}

	return (void *)p;
}

int32_t
__dpl_msize32(void *ptr) {
	int32_t *p = (int32_t *)ptr;
	if (p) {
		p--;
		if (!p) { /* die! */ }
	} else {
		/* die! */
	}

	return p[0];
}

void *
__malloc
__dpl_malloc_smart(size_t n)
{
	int32_t *p;
	char *cp;
	void *ptr = NULL;

	ptr = malloc(n+sizeof(int32_t)+1);
	if (ptr) {
		/* set malloc'd bit */
		cp = (char *)ptr;
		cp[0] = 0x01;
		cp++;
		/* set allocation size */
		p = (int32_t *)cp;
		p[0] = (int32_t)n;
		p++;
	} else {
		/* die! */
	}

	return (void *)p;
}
