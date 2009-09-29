/**
 * @file dpl_omp.h
 * @brief DPL support of openmp (compiled by adding --with-openmp to configure)
 * @author Stephen Schweizer
 */

#ifndef __DPL_OMP_H__
#define __DPL_OMP_H__

#include "dpl_types.h"

	/**
	 * @brief Calculates the number of cores seen by openmp adjusted for hyperthreading
	 * @returns number of cores seen by openmp adjusted for hyperthreading
	 *
	 * You can override DPL_HYPERTHREADING if you notice that the number of reported
	 * cores on your machine is more than the number of physical cores.  In most cases
	 * the number is twice as many as the actual physical core count.  In this case
	 * use the code
	 * @code
	 * #undef  DPL_HYPERTHREADING
	 * #define DPL_HYPERTHREADING 2
	 * @endcode
	 * If hyperthreading is turned on in your kernel new threads <em>should</em>
	 * spawn on different processors (instead of say, 2 threads spawning on the same).
	 * However, this functionality is not guarenteed by libdpl.  In fact, if you are
	 * using a 2.4 kernel, it almost certainly will not behave in this way.  It is
	 * suggested instead that you disable hyperthreading in both your kernel and
	 * your BIOS settings.
	 */
	int dpl_omp_numthreads();

	/**
	 * @brief Uses openmp and MPI to detect the number of processors on all
	 * servers on the network
	 * @param d pointer to dpl_t structure 
	 */
	void dpl_omp_detect(dpl_t *d);

#endif
