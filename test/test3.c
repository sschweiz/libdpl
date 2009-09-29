#include <dpl.h>
#include <mpi.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

int main(int argc, char **argv)
{
	dpl_t d;
	FILE *h;

	h = fopen("dplhostfile", "r");

	dpl_init(&d, MPI_COMM_WORLD, h, argc, argv, DPL_USE_CACHE | DPL_HEARTBEAT);

	sleep(5);

	dpl_destroy(&d);

	fclose(h);
	return 0;
}
