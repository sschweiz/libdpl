#include <dpl.h>
#include <omp.h>
#include <mpi.h>

/* requires ./configure --with-openmp */
int main(int argc, char **argv)
{
	dpl_t d;

	dpl_init(&d, MPI_COMM_WORLD, NULL, argc, argv, DPL_NO_TREE);
	dpl_omp_detect(&d);
	dpl_destroy(&d);

	return 0;
}
