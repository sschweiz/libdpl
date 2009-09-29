#include <dpl.h>
#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv)
{
	int cnt;
	dpl_t d;
	FILE *h;

	h = fopen("dplhostfile", "r");

	dpl_init(&d, MPI_COMM_WORLD, h, argc, argv, DPL_USE_CACHE);
	for (cnt = 0; cnt < d.world_size; cnt++)
	{
//		printf("d.tree[%d].rank: %d\n", cnt, d.tree[cnt].rank);
	}

	dpl_ping(&d, 64);
//	dpl_init_tree(&d, h);
	dpl_destroy(&d);

	fclose(h);
	return 0;
}
