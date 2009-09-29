#ifndef __FIFO_H
#define __FIFO_H

#include <stdlib.h>
#include <string.h>

typedef struct __fifo_t
{
	void **member;

	size_t write;
	size_t read;

	size_t len;
	size_t size;
	size_t typesize;
} dpl_fifo_t;

static __inline size_t dpl_fifo_get_length(const dpl_fifo_t *f)
{
	return f->len;
}

static __inline size_t dpl_fifo_get_elements(const dpl_fifo_t *f)
{
	return f->size;
}

static __inline void dpl_fifo_increment(dpl_fifo_t *f)
{
	f->read++;
	if (f->read >= f->size)
	{
		f->read = 0;
	}
}

static __inline void dpl_fifo_decrement(dpl_fifo_t *f)
{
	f->read--;
	if (f->read < 0)
	{
		f->read = (f->size)-1;
	}
}

/*
 *	f		type: fifo_t*
 * 	ptr		type: void*
 */
#define dpl_fifo_begin(f, ptr)		((f)->member[0])
#define dpl_fifo_end(f, ptr)		((f)->member[(f)->size-1])
#define dpl_fifo_current(f, ptr)	(ptr = (f)->member[(f)->read])

/*
 *  f		type: fifo_t*
 */
#define dpl_fifo_point_begin(f)		((f)->read = 0)
#define dpl_fifo_point_end(f)		((f)->read = ((f)->size)-1)

static __inline void dpl_fifo_init(dpl_fifo_t *f, size_t len,
	const size_t typesize)
{
	unsigned int i;

	f->member = (void **)malloc(sizeof(void *) * len);
	for (i = 0; i < len; i++)
	{
		f->member[i] = (void *)malloc(typesize);
	}

	f->len = len;
	f->size = 0;
	f->read = 0;
	f->write = 0;
	f->typesize = typesize;
}

static __inline void dpl_fifo_write(dpl_fifo_t *f, const void *n)
{
	memcpy(f->member[f->write], n, f->typesize);

	f->write++;
	f->size++;
	if (f->size > f->len) { f->size = f->len; }
	if (f->write >= f->len) { f->write = 0; }
}

static __inline void dpl_fifo_read(dpl_fifo_t *f, void *n)
{
	if (f->size > 0)	/* check collisions */
	{
		memcpy(n, f->member[f->read], f->typesize);

		f->read++;
		f->size--;
		if (f->read >= f->len) { f->read = 0; }
	} else { f->size = 0; }
}

static __inline void dpl_fifo_destroy(dpl_fifo_t *f)
{
	unsigned int i;

	for (i = 0; i < f->len; i++)
	{
		free(f->member[i]);
	}

	free(f->member);
}


#endif /* __FIFO_H */

