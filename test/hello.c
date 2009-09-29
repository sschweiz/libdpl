#include <mpi.h>
#include <dpl.h>

int main(int argc, char **argv)
{
	dpl_t d;
	dpl_init(&d, MPI_COMM_WORLD, NULL, argc, argv, DPL_NO_TREE);

	printf("Hello World!\n");

	dpl_destroy(&d);
	return 0;
}
