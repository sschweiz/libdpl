#ifndef __DPL_H__
#define __DPL_H__

/**
 * @mainpage Distributed Performance Library
 * @section Introduction
 * The Distributed Performance Library (DPL) is a set of functions
 * designed to make it easy to create a dynamic and generic interface
 * for distributed and parallel computing.  It was written for the
 * Hubble Sphere Hydrogen Survey by Stephen Schweizer <schweizer@cmu.edu>.  The code is
 * free and open source, but used with permission.  It is copyright
 * 2008 Carnegie Mellon University.
 *
 * @section Basic Usage
 * The library is designed to be user friendly.  It can be run by
 * @code
 * #include <dpl.h>
 * #include <mpi.h>
 * #include <stdio.h>
 *
 * int main(int argc, char **argv)
 * {
 * 	dpl_t d;
 *	FILE *treefile = fopen("treefile", "r");
 *
 * 	dpl_init(&d, MPI_COMM_WORLD, treefile, argc, argv, DPL_USE_CACHE | DPL_USE_DEBUG);
 *
 * 	...
 *
 * 	dpl_destroy(&d);
 * 	return 0;
 * }
 * @endcode
 * You then compile it using mpi to link the library
 * @code
 * mpicc <filename> -ldpl
 * @endcode
 * The code is written in ANSI C using the C version of MPI.  It was designed to work
 * with open-mpi.
 *
 * @section The Treefile
 * DPL uses a distribution identity tree.  This tree is initialized in a file created by the user.
 * For each process on the network, the user must add a line to the treefile in the following
 * format.
 * @code
 * <hostname> <type> <group>
 * @endcode
 * Where <hostname> is the hostname of the process.  This could be the FQDN if your network is set
 * up that way.  You can resolve the hostname by running the `hostname' command.  Additionally, you
 * can have DPL generate a treefile with the dpl_generate_treefile function.  The <type> argument is
 * the process' type or role in the tree.  These can be either node, root, or pseudoroot.  The <group>
 * argument is the group number.  This is an integer value from 0-64.  The contents of this file are
 * case sensitive.
 *
 * Since it is possible to compile libdpl with openmp support, you want to make sure that if you use
 * this option that the number of lines in your treefile match the number of threads openmp will make
 * on each machine in your network.  Note that if hyperthreading it turned on, it will more than
 * likely be the case that you will see 2 cores per physical processor.  You can override the
 * DPL_HYPERTHREADING macro in this case to revert back to the number of physical processors you have
 * so as to not overclock your machine.
 *
 * @section Caching
 * You may find it ideal to use different distribution trees for different functions of your
 * code.  DPL understands this, and as a result, it will reference the current identity tree
 * every time it wants to perform a distribution related function.  Unfortunately, this can
 * create an unnecessary amount of overhead.  To combat this, DPL can be called with DPL_USE_CACHE.
 * If this is done, the library will cache the most recent version of the tree as well as
 * any group information related to this tree for constant time lookup.  As a result, whenever
 * you, the developer, wants to use a different distribution scheme you must be sure to call
 * dpl_update_cache afterwards.
 *
 * DPL also has a build in heartbeat function which creates a forked thread off of the DPL_RANK_MASTER
 * which pings all processes at a routine interval.  If the ping is higher than a certain amount, or
 * a process does not respond, DPL can automatically take this process out of the tree and reconfigure
 * itself accordingly.  With caching enabled, the heartbeat must call dpl_update_cache as well.
 */

#include "dpl/dpl_types.h"
#include "dpl/dpl_func.h"
#include "dpl/dpl_cache.h"
#include "dpl/dpl_thread.h"
#include "dpl/dpl_omp.h"
#include "dpl/dpl_extra.h"

#endif
