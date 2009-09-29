/**
 * @brief DPL Extra functions header
 * @file dpl_extra.h
 *
 * @author Stephen Schweizer
 *
 * This file is included seperately from the main dpl library.  The functions
 * are still part of the library however to use them you must include this
 * file in addition to dpl.h.  The functions and definitions in this file
 * are considered extra because they provide functionality for the class without
 * needing it passed as a reference.
 */
#ifndef __DPL_EXTRA_H__
#define __DPL_EXTRA_H__

/**
 * @brief generates a treefile from the hostfile or machinefile used with mpi
 * @param in hostfile or machinefile
 * @param out treefile output
 *
 * This function can be used to parse a hostfile used with openmpi or mpich and
 * create a master/slave treefile from it.  It supports only openmpi hostfiles
 * as used with the --hostfile arg on mpiexec, or mpich machinefiles as used with
 * the -machinefile arg on mpiexec.
 *
 */
void dpl_generate_from_hostfile(FILE *in, FILE *out);

/**
 * @brief calculates n choose 2 combination
 * @param n intereger input
 */
int dpl_nchoose2(int n);

#endif
