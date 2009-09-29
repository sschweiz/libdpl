#include <dpl.h>
#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv)
{
	int cnt;
	dpl_t d;
	FILE *h;

	h = fopen("gentf", "rw");

	dpl_init(&d, MPI_COMM_WORLD, NULL, argc, argv, DPL_USE_DEBUG | DPL_NO_TREE);
	for (cnt = 0; cnt < d.world_size; cnt++)
	{
//		printf("d.tree[%d].rank: %d\n", cnt, d.tree[cnt].rank);
	}

//	dpl_generate_treefile(&d, h);
//	dpl_init_tree(&d, h);
//	dpl_update_cache(&d);


	dpl_destroy(&d);

	fclose(h);
	return 0;
}
