#ifndef __CRYPTO_HASH_
#define __CRYPTO_HASH_

#include "dpl_list.h"

unsigned int dpl_hash(char *s, int l);

typedef struct __dpl_hash_struct
{
	std_list_single **table;

	unsigned int size;
	unsigned int capacity;
	double loadfactor;

	char* (*get_from_sls)(std_list_single);
} dpl_hash_t;

void dpl_hash_init(dpl_hash_t *h, unsigned int capacity, char* (*get_from_sls)(std_list_single));
void dpl_hash_destroy(dpl_hash_t *h);

int dpl_hash_rehash(dpl_hash_t *h);
int dpl_hash_add(dpl_hash_t *h, std_list_single *n);
std_list_single *dpl_hash_get(dpl_hash_t *h, void *c, int (*compare)(void *, void *));
int dpl_hash_resize(dpl_hash_t *h, unsigned int newcap);

#endif
