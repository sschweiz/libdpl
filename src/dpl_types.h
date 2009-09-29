/**
 * @file dpl_types.h
 * @brief Global variable and type definitions
 *
 * The Distributed Performance Library uses a tree structure to
 * keep track of the duties and identities of each process.  This
 * information can be dynamically changing to perform seperate
 * parallel operations.  A cache of the tree is available, at the
 * cost of machine memory, to speed up calculations involving
 * tree traversal and lookup.
 *
 */
#ifndef __DPL_TYPES_H__
#define __DPL_TYPES_H__

#include <mpi.h>
#include "dpl_thread.h"

#ifndef __GNUC__
#define __inline__ __inline
#define __dpl_internal /* nothing */
#define __dpl_nonnull /* nothing */
#else
#define __dpl_internal __attribute__ ((__section__(".dpl")))
#define __dpl_nonnull __attribute__ ((nonnull))
#endif

#define DPL_DEFAULT_TAG 100
/** @brief MPI Rank of the master process (usually 0) */
#define DPL_RANK_MASTER 0

/* dpl_tree_t defined */
#define DPLTREE_TYPE_NODE	0x000
#define DPLTREE_TYPE_ROOT	0x001
#define DPLTREE_TYPE_PSEUDOROOT	0x002
#define DPLTREE_TYPE_UNKNOWN	0x003

#define DPLTREE_MASK_TYPE	0x003
#define DPLTREE_MASK_GROUP	0x0fc

#define DPL_GET_TYPE(x) (x & DPLTREE_MASK_TYPE)
#define DPL_GET_GROUP(x) ((x & DPLTREE_MASK_GROUP) >> 2)

/* return and error codes */
#define DPL_SUCCESS	0
#define DPL_FAILURE	-1
#define DPL_ERROR	1
#define DPL_WARN	2
#define DPL_CRITICAL	255

/* debugging */
#define DPL_DEBUG(d, x, s...)				\
	if (d->flags & DPL_USE_DEBUG)			\
	{						\
		printf("[%d] DEBUG: ", *(d->myrank));	\
		printf(x, ## s);			\
		printf("\n");				\
	}

/** @brief DPL standard use flag defined for completeness */
#define DPL_STANDARD	0x00000
/** @brief DPL cache flag.  Turns on cache speedup optimizations. */
#define DPL_USE_CACHE	0x00001
/** @brief DPL heartbeat flag.  Turns on fork process which moniters nodes via a ping. */
#define DPL_HEARTBEAT	0x00002
/** @brief DPL slave flag.  Specifies a configuration where, by default, DPL_RANK_MASTER is
 * the only root and the rest of the processes are slave nodes. */
#define DPL_ONLYSLAVES	0x00004
/** @brief DPL debug flag.  Turns on debugging information. */
#define DPL_USE_DEBUG	0x00008
/** @brief DPL no tree flag.  Does not parse a treefile (treefile can be passed to init as NULL) */
#define DPL_NO_TREE	0x00010
/** @brief DPL error suppression flag.  Does not output error messages. */
#define DPL_SUPPRESS_ERROR 0x00020

/* helper macros for typecasting with *** arrays */
#define DPL_TC_A(type, x) ((type *)x)
#define DPL_TC_S(type) (sizeof(type))
#define DPL_D_TC_ARR3(arr, type, i, j, k) (((type *)(((type **)(((type ***)arr)[i]))[j]))[k])
#define DPL_D_TC_ARR2(arr, type, i, j)    (((type **)(((type ***)arr)[i]))[j])
#define DPL_D_TC_ARR1(arr, type, i)       (((type ***)arr)[i])

#define DPL_CHAR		0x0cf00 /* signed char */
#define DPL_INT			0x0cf01 /* signed int */
#define DPL_SHORT		0x0cf02 /* signed short */
#define DPL_LONG		0x0cf03 /* signed long */
#define DPL_UNSIGNED_CHAR	0x0cf04 /* unsigned char */
#define DPL_UNSIGNED_SHORT	0x0cf05 /* unsigned short */
#define DPL_UNSIGNED_LONG	0x0cf06 /* unsigned long */
#define DPL_UNSIGNED		0x0cf07 /* unsigned int */
#define DPL_FLOAT		0x0cf08 /* float */
#define DPL_DOUBLE		0x0cf09 /* double */
#define DPL_LONG_DOUBLE		0x0cf0a /* long double */

/**
 * If your computer uses hyperthreading and you would like to
 * expand the library to treat each hyperthread as a seperate
 * core, set this to your splitting number
 *
 * NOTE: This should only be used if you know exactly what you
 * are doing
 */
#define DPL_HYPERTHREADING	0

/** @brief dpl function typedef
 * 
 * This is a function that is called by dpl_distribute.  It is designed to
 * be as simple, yet generic as possible.  The first parameter is an array
 * of char* that contain the parallel data.  The second parameter is the size
 * of that data array.  The third parameter is a void* which can hold any
 * additional args that the user may want to pass to that function.
 */
typedef void (*dplfunc)(char *, unsigned int, void *);
typedef int RANK;
typedef int DPL_DATATYPE;
typedef char*** dpl_prepared;

/** @brief proprietary type for sending distributed data */
typedef struct __dplpreparedstruct
{
	dpl_prepared data;
	unsigned int data_size;
	DPL_DATATYPE type;
} dpl_prepared_t;

/** @brief DPL tree structure */
typedef struct __mpitreestruct
{
	/** @brief Current leaf rank */
	int rank;
	/** @brief Rank of current leaf parent */
	int parentid;
	/** @brief Type char of current leaf.  Bits 0-1 are the leaf type, bits 2-7 are the leaf group. */
	char type;
} dpl_tree_t;

/** @brief DPL group cache structure.  Intended for internal use.*/
typedef struct __mpigrpintstruct
{
	/** @brief Size of group */
	int grp_size;

	/** @brief Number of nodes in the group*/
	int grp_nodes;
	/** @brief Array of ranks of nodes in the group*/
	int *grp_node_ranks;
	/** @brief Number of pseudoroots in the group */
	int grp_pseudoroots;
	/** @brief Array of ranks of pseudoroots in the group */
	int *grp_pseudoroot_ranks;
	/** @brief Number of roots in the group */
	int grp_roots;
	/** @brief Array of ranks of roots in the group */
	int *grp_root_ranks;

	/** @brief Group number */
	int grp_number;
} dpl_group_internal_t;

/** @brief DPL cache structure */
typedef struct __mpicachestruct
{
	/** @brief Number of different groups */
	int grp_count;
	/** @brief Array of cached group information */
	dpl_group_internal_t *grps;
	/** @brief Pointer to cached dpl identity tree array */
	dpl_tree_t *fixed;
} dpl_cache_t;

/** @brief DPL master structure */
typedef struct __mpidplstruct
{
	/** @brief MPI communicator */
	MPI_Comm comm;
	/** @brief MPI status array for each process */
	MPI_Status *stat;
	/** @brief MPI request array for each process */
	MPI_Request *req;

	/** @brief size of the communicator */
	int world_size;
	/** @brief number of machines on the network */
	int network_size;
	/** @brief DPL use flags */
	unsigned int flags;
	/** @brief rank variable for current process */
	RANK *myrank;

	/** @brief pointer to cache structure (NULL if caching is not used) */
	dpl_cache_t *cache;
	/** @brief pointer to dpl identity tree array */
	dpl_tree_t *tree;
	/** @brief pointer to dpl heartbeat thread controller */
	dpl_heartbeat_t *heartbeat;
} dpl_t;

MPI_Datatype dpltree_type;

#endif
