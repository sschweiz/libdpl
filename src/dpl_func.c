/**
 * @file dpl_func.c
 * @brief Source code for functions listed in dpl_func.h
 */

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <mpi.h>
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include "dpl_func.h"
#include "dpl_types.h"
#include "dpl_cache.h"
#include "dpl_thread.h"
#include "dpl_omp.h"

void dpltree_typesetup()
{
	MPI_Datatype otypes[2];
	MPI_Aint offsets[2], extent;
	int blockcount[2];

	offsets[0] = 0;
	otypes[0] = MPI_INT;
	blockcount[0] = 2;

	MPI_Type_extent(MPI_INT, &extent);
	offsets[1] = 2 * extent;
	otypes[1] = MPI_CHAR;
	blockcount[1] = 1;

	MPI_Type_struct(2, blockcount, offsets, otypes, &dpltree_type);
	MPI_Type_commit(&dpltree_type);
}

int dpl_init(dpl_t *d,
	     MPI_Comm comm,
	     FILE *treefile,
	     int argc,
	     char *argv[],
	     unsigned int flags)
{
	int rc;
	rc = MPI_Init(&argc, &argv);

	if (rc != MPI_SUCCESS)
	{
		perror("Unable to initialize MPI");
		MPI_Abort(comm, rc);

		return rc;
	}

	d->myrank = (int *)malloc(sizeof(int));
	MPI_Comm_rank(comm, d->myrank);

#ifdef _OPENMP
	MPI_Comm_size(comm, &(d->network_size));
	int n = dpl_omp_numthreads();
	MPI_Allreduce(&n, &(d->world_size), 1, MPI_INT, MPI_SUM, comm);
#else
	MPI_Comm_size(comm, &(d->world_size));
	/* resolve network size when we init the tree */
#endif

	d->comm = comm;
	d->flags = flags;
	d->stat = (MPI_Status *)malloc(sizeof(MPI_Status) * d->world_size);
	d->req = (MPI_Request *)malloc(sizeof(MPI_Request) * d->world_size);

	d->cache = NULL;
	d->tree = NULL;

	MPI_Barrier(comm);
	dpltree_typesetup();

	if (!(d->flags & DPL_NO_TREE))
	{
		dpl_init_tree(d, treefile);
		MPI_Barrier(comm);
		if (d->flags & DPL_USE_CACHE)
		{
			dpl_update_cache(d);
		}
	}

	if (d->flags & DPL_HEARTBEAT)
	{
		d->heartbeat = (dpl_heartbeat_t *)malloc(sizeof(dpl_heartbeat_t));
		dpl_init_heartbeat(d->heartbeat, 1);
	}
		
#ifdef _OPENMP
	DPL_DEBUG(d, "MPI up %d of %d", *(d->myrank) + 1, d->network_size);
#else
	DPL_DEBUG(d, "MPI up %d of %d", *(d->myrank) + 1, d->world_size);
#endif

	return rc;
}

int dpl_destroy(dpl_t *d)
{
	int rc;
	
	if (d->cache != NULL)
	{
		free(d->cache->fixed);
		free(d->cache->grps);
		free(d->cache);
	}

	if (d->tree != NULL)
	{
		free(d->tree);
	}

	if (d->flags & DPL_HEARTBEAT)
	{
		dpl_destroy_heartbeat(d->heartbeat);
		free(d->heartbeat);
	}

	free(d->myrank);
	free(d->stat);
	free(d->req);

	MPI_Type_free(&dpltree_type);
	MPI_Barrier(d->comm);
	rc = MPI_Finalize();

	return rc;
}

void dpl_group_info(dpl_t *d,
	size_t *group_size, size_t *roots, size_t *pseudoroots, size_t *nodes)
{
	dpl_tree_t tree;
	int curgrp, i;

	*group_size = 0;
	*nodes = 0;
	*roots = 0;
	*pseudoroots = 0;

	if (d->tree != NULL)
	{
		tree = d->tree[*(d->myrank)];
		curgrp = DPL_GET_GROUP(tree.type);

		for (i = 0; i < d->world_size; i++)
		{
			if (DPL_GET_GROUP(d->tree[i].type) == curgrp)
			{
				(*group_size)++;
				switch (DPL_GET_TYPE(d->tree[i].type))
				{
					default:
					case DPLTREE_TYPE_UNKNOWN:
						break;
					case DPLTREE_TYPE_ROOT:
						(*roots)++;
						break;
					case DPLTREE_TYPE_PSEUDOROOT:
						(*pseudoroots)++;
						break;
					case DPLTREE_TYPE_NODE:
						(*nodes)++;
						break;
				}
			}
		}
	}
}

void dpl_init_tree(dpl_t *d, FILE *treefile)
{
	char h[64], t[64], bsend[255];
	char *hns;
	int g;

	int i, j, cur = 0;
	unsigned char found = 0;
	
	hns = (char *)malloc(sizeof(char) * d->world_size * 255);

	gethostname(bsend, 253);
	DPL_DEBUG(d, "Loading host information for %s", bsend);
	MPI_Gather(bsend, 255, MPI_CHAR, hns, 255, MPI_CHAR, DPL_RANK_MASTER, d->comm);

	if (d->tree != NULL)
	{
		dpl_error(d, DPL_WARN, "calling dpl_init_tree() on initialized tree!\n");
		free(d->tree);
	}

	d->tree = (dpl_tree_t *)malloc(sizeof(dpl_tree_t) * d->world_size);

	if (*(d->myrank) == DPL_RANK_MASTER)
	{
		while (!feof(treefile))
		{
			fscanf(treefile, "%s %s %d\n", h, t, &g);

			d->tree[cur].type = 0;
			for (i = cur; i < d->world_size; i++)
			{
				for (j = 0; j < 255; j++) { bsend[j] = hns[i*255+j]; }
				if (strcmp(h, bsend) == 0)
				{
					d->tree[cur].rank = i;
					found = 1;
					break;
				}
			}

			if (found)
			{
				if (strcmp(t, "node") == 0)
				{
					d->tree[cur].type |= DPLTREE_TYPE_NODE;
				} else if (strcmp(t, "root") == 0) {
					d->tree[cur].type |= DPLTREE_TYPE_ROOT;
				} else if (strcmp(t, "pseudoroot") == 0) {
					d->tree[cur].type |= DPLTREE_TYPE_PSEUDOROOT;
				} else {
					d->tree[cur].type |= DPLTREE_TYPE_UNKNOWN;
				}
	
				d->tree[cur].type |= (((char)g << 2) & DPLTREE_MASK_GROUP);
				cur++;
			} else {
				dpl_error(d, DPL_ERROR, "Error: `%s' hostname not found on network!\n", h);
			}
		}
	}

	MPI_Barrier(d->comm);
	MPI_Bcast(d->tree, d->world_size, dpltree_type, DPL_RANK_MASTER, d->comm);

	free(hns);
}

/* will pad with zeros to make output even */
/* takes ~ O(in_size) with caching */
void dpl_prepare_data(dpl_t *d,
	char *input, size_t in_size,
	dpl_prepared *output, size_t *out_size)
{
	unsigned int grp, gn, gp, gr;
	int i, j, k;
	int pad, len;
	dpl_prepared out;

	/* acquire group info, use cached data if available */
	if (d->flags & DPL_USE_CACHE)
	{
		dpl_group_info_cache(d, &grp, &gr, &gp, &gn);
	} else {
		dpl_group_info(d, &grp, &gr, &gp, &gn);
	}

	/* calculate the padding number */
	pad = grp - (in_size % grp);
	len = (in_size / grp) + (pad > 0 ? 1 : 0);

	if ((gn % (gr + gp)) != 0)
	{
		printf("Error: nodes not divisible by roots!\n");
		output = NULL;
		out_size = 0;
		return;
	}
	gn = gn / (gr + gp);

	/* malloc the output structure */
	out = (dpl_prepared)malloc(sizeof(char **) * (gr + gp));
	for (i = 0; i < (gr + gp); i++)
	{
		out[i] = (char **)malloc(sizeof(char *) * (gn + 1));
		for (j = 0; j < (gn + 1); j++)
		{
			out[i][j] = (char *)malloc(len);
			for (k = 0; k < len; k++)
			{
				int index = i*(gn + 1)*len + j*len + k;
				if (index >= in_size)
				{
					out[i][j][k] = (char)0;
				} else {
					out[i][j][k] = input[index];
				}
			}
		}
	}

	(*out_size) = len;
	(*output) = out;
}

void dpl_prepare_aux(dpl_t *d,
	void *input, size_t input_size, DPL_DATATYPE input_type,
	dpl_prepared_t *output)
{
	size_t grp, gn, gp, gr;
	unsigned int pad, len;
	int i, j, k;
	dpl_prepared out = NULL;

	if (d->flags & DPL_USE_CACHE)
	{
		dpl_group_info_cache(d, &grp, &gr, &gp, &gn);
	} else {
		dpl_group_info(d, &grp, &gr, &gp, &gn);
	}

	if ((gn % (gr + gp)) != 0)
	{
		dpl_error(d, DPL_ERROR, "nodes not divisible by roots!\n");
		output = NULL;
		return;
	}
	
	pad = grp - (input_size % grp);
	len = (input_size / grp) + (pad > 0 ? 1 : 0);

	switch (input_type)
	{
		case DPL_CHAR: out = (dpl_prepared)malloc(DPL_TC_S(signed char **) * (gr + gp)); break;
		case DPL_INT: out = (dpl_prepared)malloc(DPL_TC_S(signed int **) * (gr + gp)); break;
		case DPL_SHORT: out = (dpl_prepared)malloc(DPL_TC_S(signed short **) * (gr + gp)); break;
		case DPL_LONG: out = (dpl_prepared)malloc(DPL_TC_S(signed long **) * (gr + gp)); break;
		case DPL_UNSIGNED_CHAR: out = (dpl_prepared)malloc(DPL_TC_S(unsigned char **) * (gr + gp)); break;
		case DPL_UNSIGNED_SHORT: out = (dpl_prepared)malloc(DPL_TC_S(unsigned short **) * (gr + gp)); break;
		case DPL_UNSIGNED_LONG: out = (dpl_prepared)malloc(DPL_TC_S(unsigned long **) * (gr + gp)); break;
		case DPL_UNSIGNED: out = (dpl_prepared)malloc(DPL_TC_S(unsigned int **) * (gr + gp)); break;
		case DPL_FLOAT: out = (dpl_prepared)malloc(DPL_TC_S(float **) * (gr + gp)); break;
		case DPL_DOUBLE: out = (dpl_prepared)malloc(DPL_TC_S(double **) * (gr + gp)); break;
		case DPL_LONG_DOUBLE: out = (dpl_prepared)malloc(DPL_TC_S(long double **) * (gr + gp)); break;
	}
	for (i = 0; i < (gr + gp); i++)
	{
		DPL_D_TC_ARR1(out, signed char, i) = malloc(DPL_TC_S(signed char *) * (gn + 1)); break;
		for (j = 0; j < (gn + 1); j++)
		{
			DPL_D_TC_ARR2(out, signed char, i, j) = malloc(DPL_TC_S(signed char) * len); break;
			for (k = 0; k < len; k++)
			{
				DPL_D_TC_ARR3(out, signed char, i, j, k) = DPL_TC_A(signed char, input)[i*(gn+1)*len + j*len + k];
			}
		}
	}
}

void dpl_ping(dpl_t *d, unsigned int packet_size)
{
	char *packet = (char *)malloc(packet_size);
	char *hns = (char *)malloc(sizeof(char) * d->world_size * 255);
	char bsend[255], tmp[255], ltype;
	struct timeval now, later;
	double rl, rn;
	int i, j, rc;
	
	for (i = 0; i < packet_size; i++) { packet[i] = rand() % 255; }
/*	for (i = 0; i < d->world_size; i++) { hns[i] = (char *)malloc(sizeof(char) * 255); } */
	MPI_Barrier(d->comm);

	gethostname(bsend, 253);
	MPI_Gather(bsend, 255, MPI_CHAR, hns, 255, MPI_CHAR, DPL_RANK_MASTER, d->comm);

	if (*(d->myrank) == DPL_RANK_MASTER)
	{
		printf("DPL Ping process - libdpl 1.0.0\n");
		printf("[%d]: pinging nodes with %d bytes(s)\n", *(d->myrank), packet_size);
		ltype = 'L';
#ifdef _OPENMP
		/* resolve only for network_size because there will not be world_size
		 * elements in the hns array.  with openmp we are assured that the other
		 * nodes are (R) or (L) and we do not need to check.
		 */
		for (i = 1; i < d->network_size; i++)
#else
		for (i = 1; i < d->world_size; i++)
#endif
		{
			for (j = 0; j < 255; j++) { bsend[j] = hns[i*255+j]; tmp[j] = hns[(i-1)*255+j]; }
			if (strcmp(bsend, tmp) == 0)
			{
				MPI_Send(packet, packet_size, MPI_CHAR, i, DPL_DEFAULT_TAG, d->comm);
				rc = MPI_Recv(packet, packet_size, MPI_CHAR, i, DPL_DEFAULT_TAG, d->comm, &(d->stat[*(d->myrank)]));

				if (rc != MPI_SUCCESS)
				{
					printf("  => node %d resolve failed (node may be down)\n", i);
				} else {
					printf("  => skipped report on %d (%c)\n", i, ltype);
				}
			} else {
				ltype = 'R';
				gettimeofday(&now, NULL);
				MPI_Send(packet, packet_size, MPI_CHAR, i, DPL_DEFAULT_TAG, d->comm);
				rc = MPI_Recv(packet, packet_size, MPI_CHAR, i, DPL_DEFAULT_TAG, d->comm, &(d->stat[*(d->myrank)]));
				gettimeofday(&later, NULL);

				rn = ((double)now.tv_sec + (double)now.tv_usec * 1e-6);
				rl = ((double)later.tv_sec + (double)later.tv_usec * 1e-6);

				if (rc != MPI_SUCCESS)
				{
					printf("  => node %d resolve failed (node may be down)\n", i);
				} else {
					printf("  => received from %d delay: %.3lf ms\n", i, ((rl - rn) * 1000));
				}
			}
		}
		printf("\n");
		printf("Finished on %d processes.\n", i);
		printf("  L - local process, ping via memcpy, loopback, or thread\n");
		printf("  R - remote local process, ping time same as previous process\n");
	} else {
		MPI_Recv(packet, packet_size, MPI_CHAR, DPL_RANK_MASTER, DPL_DEFAULT_TAG, d->comm, &(d->stat[*(d->myrank)]));
		MPI_Send(packet, packet_size, MPI_CHAR, DPL_RANK_MASTER, DPL_DEFAULT_TAG, d->comm);
	}
}

void dpl_ping_data(dpl_t *d, unsigned int packet_size, unsigned int points)
{
	char *packet = (char *)malloc(packet_size);
	char *hns = (char *)malloc(sizeof(char) * d->world_size * 255);
	char bsend[255], tmp[255], ltype;
	struct timeval now, later;
	double rl, rn, last = 0;
	double ave = 0, min = 1e6, max = 0, jitter = 0;
	int i, j, rc = 0, x;
	
	for (i = 0; i < packet_size; i++) { packet[i] = rand() % 255; }
/*	for (i = 0; i < d->world_size; i++) { hns[i] = (char *)malloc(sizeof(char) * 255); } */
	MPI_Barrier(d->comm);

	gethostname(bsend, 253);
	MPI_Gather(bsend, 255, MPI_CHAR, hns, 255, MPI_CHAR, DPL_RANK_MASTER, d->comm);

	if (*(d->myrank) == DPL_RANK_MASTER)
	{
		printf("DPL Statistical Ping process - libdpl 1.0.0\n");
		printf("[%d]: pinging nodes with %d data points in %d byte packets\n", *(d->myrank), points, packet_size);
		printf("                      ave,min,max,jitter\n");
		ltype = 'L';
#ifdef _OPENMP
		/* resolve only for network_size because there will not be world_size
		 * elements in the hns array.  with openmp we are assured that the other
		 * nodes are (R) or (L) and we do not need to check.
		 */
		for (i = 1; i < d->network_size; i++)
#else
		for (i = 1; i < d->world_size; i++)
#endif
		{
			for (j = 0; j < 255; j++) { bsend[j] = hns[i*255+j]; tmp[j] = hns[(i-1)*255+j]; }
			if (strcmp(bsend, tmp) == 0)
			{
				for (x = 0; x < points; x++)
				{
					MPI_Send(packet, packet_size, MPI_CHAR, i, DPL_DEFAULT_TAG, d->comm);
					rc = MPI_Recv(packet, packet_size, MPI_CHAR, i, DPL_DEFAULT_TAG, d->comm, &(d->stat[*(d->myrank)]));
					
				}

				if (rc != MPI_SUCCESS)
				{
					printf("  => node %d resolve failed (node may be down)\n", i);
				} else {
					printf("  => skipped report on %d (%c)\n", i, ltype);
				}
			} else {
				ltype = 'R';
				for (x = 0; x < points; x++)
				{
					gettimeofday(&now, NULL);
					MPI_Send(packet, packet_size, MPI_CHAR, i, DPL_DEFAULT_TAG, d->comm);
					rc = MPI_Recv(packet, packet_size, MPI_CHAR, i, DPL_DEFAULT_TAG, d->comm, &(d->stat[*(d->myrank)]));
					gettimeofday(&later, NULL);

					rn = ((double)now.tv_sec + (double)now.tv_usec * 1e-6);
					rl = ((double)later.tv_sec + (double)later.tv_usec * 1e-6);
					ave += (rl - rn);
					if (x != 0)
					{
						jitter += fabs(last - (rl - rn));
					}
					if ((rl - rn) < min) { min = (rl - rn); }
					if ((rl - rn) > max) { max = (rl - rn); }
					last = (rl - rn);
				}

				if (rc != MPI_SUCCESS)
				{
					printf("  => node %d resolve failed (node may be down)\n", i);
				} else {
					/* jitter is defined as how much the latency varies from ping to ping
					   on average measured in ms */
					printf("  => received from %d: %.3lf,%.3lf,%.3lf,%.3lf ms\n", i, ave/points * 1000, min*1000, max*1000, jitter/points * 1000);
				}

				/* reset vars */
				min = 1e6;
				max = 0;
				jitter = 0;
				ave = 0;
			}
		}
		printf("\n");
		printf("Finished on %d processes.\n", i);
		printf("  L - local process, ping via memcpy, loopback, or thread\n");
		printf("  R - remote local process, ping time same as previous process\n");
	} else {
		for (x = 0; x < points; x++)
		{
			MPI_Recv(packet, packet_size, MPI_CHAR, DPL_RANK_MASTER, DPL_DEFAULT_TAG, d->comm, &(d->stat[*(d->myrank)]));
			MPI_Send(packet, packet_size, MPI_CHAR, DPL_RANK_MASTER, DPL_DEFAULT_TAG, d->comm);
		}
	}

}

void dpl_generate_treefile(dpl_t *d, FILE *out)
{
	char bsend[255];
	char *hns;
	int i, j;

	hns = (char *)malloc(sizeof(char) * d->world_size * 255);

	gethostname(bsend, 253);
	DPL_DEBUG(d, "Loading host information for %s", bsend);
	MPI_Gather(bsend, 255, MPI_CHAR, hns, 255, MPI_CHAR, DPL_RANK_MASTER, d->comm);

	if (*(d->myrank) == DPL_RANK_MASTER)
	{
		if (d->tree == NULL) /* used with DPL_NO_TREE */
		{
			for (i = 0; i < d->world_size; i++)
			{
				for (j = 0; j < 255; j++) { bsend[j] = hns[i*255+j]; }
				fprintf(out, "%s ", bsend);
				if (i == 0) { fprintf(out, "root 0\n"); }
				else { fprintf(out, "node 0\n"); }
			}
		} else {
			for (i = 0; i < d->world_size; i++)
			{
				for (j = 0; j < 255; j++) { bsend[j] = hns[i*255+j]; }
				fprintf(out, "%s ", bsend);
				switch (DPL_GET_TYPE(d->tree[i].type))
				{
					case DPLTREE_TYPE_ROOT:
						fprintf(out, "root ");
						break;
					case DPLTREE_TYPE_NODE:
						fprintf(out, "node ");
						break;
					case DPLTREE_TYPE_PSEUDOROOT:
						fprintf(out, "pseudoroot ");
						break;
					default:
					case DPLTREE_TYPE_UNKNOWN:
						fprintf(out, "- ");
						break;
				}
				fprintf(out, "%d\n", DPL_GET_GROUP(d->tree[i].type));
			}
		}
	}
}

void dpl_distribute(dpl_t *d, dpl_prepared data, size_t data_size, dplfunc fp, void *args)
{
}

int dpl_get_rank(dpl_t *d)
{
	return (*(d->myrank));
}

int dpl_get_network(dpl_t *d)
{
	return d->network_size;
}

int dpl_get_world(dpl_t *d)
{
	return d->world_size;
}

unsigned int dpl_is_master(dpl_t *d)
{
	return (dpl_get_rank(d) == DPL_RANK_MASTER);
}

void dpl_error(dpl_t *d, int errtype, const char *msg, ...)
{
	va_list ap;

	if (!(d->flags & DPL_SUPPRESS_ERROR))
	{
		switch (errtype)
		{
			default:
			case DPL_ERROR: printf("[%d] Error: ", *(d->myrank)); break;
			case DPL_WARN: printf("[%d] Warning: ", *(d->myrank)); break;
			case DPL_CRITICAL: printf("[%d] Critical Error: ", *(d->myrank)); break;
			case DPL_FAILURE: printf("[%d] Failure: ", *(d->myrank)); break;
			case DPL_SUCCESS: printf("[%d] Success: ", *(d->myrank)); break;
		}

		va_start(ap, msg);
		vprintf(msg, ap);
		va_end(ap);
	}
}

void dpl_printf(dpl_t *d, const char *msg, ...)
{
	va_list ap;

	printf("[%d] ", *(d->myrank));

	va_start(ap, msg);
	vprintf(msg, ap);
	va_end(ap);
}
