#include <string.h>
#include <stdlib.h>
#include "hash.h"
#include "list.h"

/**
 * USING CRYPTO HASH
 * -----------------
 * Crypto Hash supports a generic range of data types.  This version uses a standard
 * seperate chaining hash table which is hashed with a very basic polynomial function.
 * I may add md5 or some other hashing algorithm with openssl later.  The first thing
 * you do is decaire a hash_t variable (for this example we will use h)
 *
 *    hash_t h;
 *
 * then you initialize the hash table.  You will be asked to choose a capacity at the
 * start.  This is necessary, but can be changed later with hash_resize().  In order
 * for the table to work, you need to hash char* variables.  Because the table is an
 * array of linked lists which use std_list_single and void*, you need to define a
 * function which tells the hash structure how to convert the void* in std_list_single
 * to char* for the hash function.  The function should look something like this:
 *
 *    char *sls_to_char(std_list_single s)
 *    {
 *        return (char*)s.satellite;
 *    }
 *
 * then you can call hash_init().
 *
 *    hash_init(&h, USER_INITIAL_CAPACITY, sls_to_char);
 *
 * Adding to the hash table is simple.  Call hash_add().  Note that you need to pass
 * the function an initialized std_list_single variable.  See list.c for info about
 * how to use void* linked lists.
 *
 *    std_list_single *n;
 *    hash_add(&h, n);
 *
 * finally, when you are done using the structure you should destroy it, freeing the
 * used memory to the heap.
 *
 *    hash_destroy(&h);
 */

unsigned int hash(char *s, int l)
{
	int i;
	unsigned int p;
	int prime = 127;

	if (l <= 0) { return 0; }

	p = s[0];
	for (i = 1; i < l; i++)
	{
		p = s[i] + (prime * p);
	}

	return p;
}

void hash_init(hash_t *h, unsigned int capacity, char* (*get_from_sls)(std_list_single))
{
	int i;

	h->table = (std_list_single **)malloc(sizeof(std_list_single *) * capacity);
	for (i = 0; i < capacity; i++) { h->table[i] = NULL; }

	h->size = 0;
	h->capacity = capacity;
	h->loadfactor = 0.0;
	h->get_from_sls = get_from_sls;
}

void hash_destroy(hash_t *h)
{
	free(h->table);

	h->capacity = 0;
	h->size = 0;
	h->loadfactor = 0.0;
}

/*
 * get the void* std_list_single structure that resides at the index given
 * by c which matches c using compare
 */
std_list_single* hash_get(hash_t *h, void *c, int (*compare)(void *, void *))
{
	unsigned int index;
	char *s;
	std_list_single *cur;

	cur->satellite = c;
	s = h->get_from_sls((*cur));
	index = (hash(s, strlen(s)) % h->capacity);

	if (h->table[index] == NULL) { return NULL; }
	else {
		cur = h->table[index];
		while (cur != NULL)
		{
			if (compare(c, cur->satellite) == 0)
			{
				return cur;
			}
			cur = cur->next;
		}
	}

	return NULL;
}

int hash_resize(hash_t *h, unsigned int newcap)
{
}

int hash_add(hash_t *h, std_list_single *n)
{
	unsigned int index;
	char *s = h->get_from_sls((*n));
	std_list_single *cur;

	/* here we need to know what to hash (s) because n is void* */
	index = (hash(s, strlen(s)) % h->capacity);
	if (h->table[index] == NULL) { h->table[index] = n; }
	else {
		cur = h->table[index];
		while (cur->next != NULL) { cur = cur->next; }
		cur->next = n;
	}

	h->size++;
	h->loadfactor = (double)h->size / (double)h->capacity;
	/*
	 * if we were not using seperate chaining we would need to rehash here
	 * if (loadfactor > 0.72) { hash_rehash(hash_t *h); }
	 */
}

