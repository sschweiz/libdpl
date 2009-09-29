#include <stdio.h>
#include "../dpl_base.h"

int main(int argc, char **argv)
{
	DPL_TYPE(integer, int);  // dpl_integer_t

	dpl_integer_t v;

	v = 6;
	printf("%d\n", v);

	return 0;
}
