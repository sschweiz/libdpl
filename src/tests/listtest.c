#include <stdio.h>
#include <stdlib.h>

#define DPL_DEFAULT_LEVEL DPL_ANNOY
#include "../dpl_list.h"

void streq(void *a, void *b)
{
	char *c, *d;
	c = a;
	d = b;

	sprintf(c, "%s", d);
}

int main(int argc, const char *argv[])
{
	dpl_list_t c;
	dpl_node_t newl;
	dpl_iter_t tmp;
	int i;
	char *buf = (char *)malloc(3);

	dpl_list_init(&c, DPL_LIST_TYPE_STACK);

	sprintf(buf, "dog");
	newl = dpl_list_make(buf, 3, streq);
	dpl_list_push(&c, newl);
	sprintf(buf, "cat");
	newl = dpl_list_make(buf, 3, streq);
	dpl_list_push(&c, newl);
	sprintf(buf, "hat");
	newl = dpl_list_make(buf, 3, streq);
	dpl_list_insert(&c, newl, dpl_list_last(&c)->prev);

	printf("Forward ->: ");
	tmp = dpl_list_first(&c);
	while (tmp != dpl_list_tail(&c))
	{
		char *tc;
		tc = tmp->satellite;
		printf("%s ", tc);
		tmp = tmp->next;
	}
	printf("\n");
//	list_del(list_container_last(&c)->prev);
	tmp = dpl_list_last(&c);
	while (tmp != dpl_list_head(&c))
	{
		char *tc;
		tc = tmp->satellite;
		printf("%s ", tc);
		tmp = tmp->prev;
	}
	printf(":<- Backward\n");

	return 0;
}
