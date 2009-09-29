#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <mpi.h>
#include "dpl_omp.h"
#include "dpl_types.h"
#include "dpl_func.h"

#ifdef _OPENMP
	#include <omp.h>
#endif

	/* intended use:
	 * 	int n = dpl_omp_numthreads();
	 * 	#pragma omp parallel <...> num_threads(n)
	 */
	int dpl_omp_numthreads()
	{
#ifdef _OPENMP
		int th_id, th_cnt;
		int x = 1;
		#pragma omp parallel private(th_id)
		{
			th_id = omp_get_thread_num();
			if (th_id == 0) { th_cnt = omp_get_num_threads(); }
		}

		#ifdef DPL_HYPERTHREADING
		x = DPL_HYPERTHREADING;
		if (x != 0)
		{
			th_cnt /= x;
		}
		#endif

		return th_cnt;
#else
		/* this cannont call dpl_warn() */
		printf("Warning: calling dpl_omp_numthreads() without openmp support!\n");
		return 0; /* maybe we can return 1 here */
#endif
	}

	void dpl_omp_detect(dpl_t *d)
	{
#ifdef _OPENMP
		int ths, size, rank;
		int *cnts = NULL;
		char buf[255], *hns = NULL;
		ths = dpl_omp_numthreads();

		MPI_Comm_size(d->comm, &size);
		MPI_Comm_rank(d->comm, &rank);

		if (rank == DPL_RANK_MASTER)
		{
			cnts = (int *)malloc(sizeof(int) * size);
			hns = (char *)malloc(sizeof(char) * size * 255);
		}

		gethostname(buf, 253);

		MPI_Gather(&ths, 1, MPI_INT, cnts, 1, MPI_INT, DPL_RANK_MASTER, d->comm);
		MPI_Gather(buf, 255, MPI_CHAR, hns, 255, MPI_CHAR, DPL_RANK_MASTER, d->comm);	

		if (rank == DPL_RANK_MASTER)
		{
			int i, j;
			printf("Detecting cluster with MPI and openmp\n");
			for (i = 0; i < size; i++)
			{
				for (j = 0; j < 255; j++) { buf[j] = hns[i*255+j]; }
				printf(" [%d] %s on %d node(s)\n", i, buf, cnts[i]);
			}
		}
#else
		dpl_error(d, DPL_WARN, "calling dpl_omp_detect() without openmp support!\n");
#endif
	}
