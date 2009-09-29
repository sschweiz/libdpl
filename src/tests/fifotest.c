#include <stdio.h>
#include <stdlib.h>

#include "../dpl_fifo.h"

int main(int argc, char **argv)
{
	int i;
	float z;
	dpl_fifo_t f;

	dpl_fifo_init(&f, 7, sizeof(float));


	for (i = 0; i < dpl_fifo_get_length(&f); i++)
	{
		z = (float)i * 3.1415926535f;
		dpl_fifo_write(&f, &z);
	}

	dpl_fifo_point_begin(&f);

	for (i = 0; i < dpl_fifo_get_length(&f); i++)
	{
		dpl_fifo_read(&f, &z);
		printf("%f\n", z);
	}

	dpl_fifo_destroy(&f);

	return 0;
}
