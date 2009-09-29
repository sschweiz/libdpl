/**
 * @brief Function definitions for general DPL routines
 * @file dpl_func.h
 */

#ifndef __DPL_FUNC_H__
#define __DPL_FUNC_H__

#include <mpi.h>
#include <stdio.h>
#include <stdarg.h>
#include "dpl_types.h"

void dpltree_typesetup();

/**
 * @brief Initializer function for the dpl_t structure
 * @param d pointer to the dpl_t structure
 * @param comm the MPI communicator to use with the DPL structure
 * @param treefile File pointer to list of tree definitions
 * @param argc argc variable from main()
 * @param argv argv variable from main()
 * @param flags DPL use flags
 * @return Rank of current process
 */
int dpl_init(dpl_t *d, MPI_Comm comm, FILE *treefile, int argc, char *argv[], unsigned int flags);

/**
 * @brief Initializes the identity tree from a file
 * @param d pointer to dpl_t structure
 * @param treefile File pointer to list of tree definitions
 */
void dpl_init_tree(dpl_t *d, FILE *treefile);

/**
 * @brief Destroys the dpl_t structure and frees and allocated memory
 * @param d pointer to dpl_t structure
 * @return return code of MPI_Finalize()
 */
int dpl_destroy(dpl_t *d);

/**
 * @brief Prepares data to be distributed across the network
 * @param d pointer to dpl_t structure
 * @param input data input array
 * @param in_size size of input array
 * @param output dpl_prepared data output format
 * @param out_size the 'height' of the output of the 2-dimensional array
 *
 * This function serves to split a single array of data into smaller pieces
 * which can then be distributed across the network.  It used dpl_prepared as output
 * to generate a 3-dimensional array of length (grp_roots + grp_pseudoroots), width
 * (grp_nodes + 1) and height equal to the minimum amount the data can be split in this
 * method.  If the data cannot be divided evenly it is padded with zeros.
 * This function has considerable overhead if caching is turned off.
 */
void dpl_prepare_data(dpl_t *d, char *input, size_t in_size, dpl_prepared *output, size_t *out_size);

/**
 * @brief Obtains general info about the group of the current process
 * @param d pointer to dpl_t structure
 * @param group_size pointer to variable to contain the size of the group
 * @param roots pointer to variable to contain the number of roots in the group
 * @param pseudoroots pointer to variable to contain the number of pseudoroots in the group
 * @param nodes pointer to variable to contain the number of nodes in the group
 */
void dpl_group_info(dpl_t *d, size_t *group_size, size_t *roots, size_t *pseudoroots, size_t *nodes);

/**
 * @brief Sequentially pings all processes
 * @param d pointer to dpl_t structure
 * @param packet_size size of packet to ping with, in bytes
 */
void dpl_ping(dpl_t *d, unsigned int packet_size);
void dpl_ping_data(dpl_t *d, unsigned int packet_size, unsigned int points);

/**
 * @brief Generates a treefile from the initialized tree, or a master/slave tree from the processes
 * @param d pointer to dpl_t structure
 * @param out pointer to output file
 */
void dpl_generate_treefile(dpl_t *d, FILE *out);

/**
 * @brief Distributes data according to the distribute scheme and calls dplfunc on that data
 * @param d pointer to dpl_t structure
 * @param data prepared data
 * @param data_size size of the minimal array in data (as returned by dpl_prepare_data)
 * @param fp function pointer specified by dplfunc
 * @param args additional arguments to be passed to fp
 *
 * This function takes the prepared data and distributes it to each machine and process as specified
 * by the tree.  Each process then calls the fp function with their data.  The manipulated data is
 * then recombined at the calling process into the dpl_prepared array.
 */
void dpl_distribute(dpl_t *d, dpl_prepared data, size_t data_size, dplfunc fp, void *args);

/**
 * @brief Get process rank
 * @param d pointer to dpl_t structure
 * @return rank of calling process
 */
int dpl_get_rank(dpl_t *d);

/**
 * @brief Get network size
 * @param d pointer to dpl_t structure
 * @return size of the network
 */
int dpl_get_network(dpl_t *d);

/**
 * @brief Get world size
 * @param d pointer to dpl_t structure
 * @return world size of the network
 */
int dpl_get_world(dpl_t *d);

unsigned int dpl_is_master(dpl_t *d);

void dpl_error(dpl_t *d, int errtype, const char *msg, ...);
void dpl_printf(dpl_t *d, const char *msg, ...);

#endif
