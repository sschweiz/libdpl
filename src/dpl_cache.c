/**
 * @file dpl_cache.c
 * @brief Source code for functions listed in dpl_cache.h
 */

#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include "dpl_types.h"
#include "dpl_cache.h"

void dpl_update_cache(dpl_t *d)
{
	int i, j, grpcnt = 0;
	int found = 0;
	int *buf;

	if (d->tree == NULL)
	{
		printf("Error: dpl_update_cache() called on invalid tree!\n");
		return;
	}

	if (d->cache == NULL)
	{
		d->cache = (dpl_cache_t *)malloc(sizeof(dpl_cache_t));
	} else {
		/* if the cache is set up its member arrays and realloc */
		/* the world size may have changed */
		free(d->cache->grps);
		free(d->cache->fixed);
	}

	d->cache->fixed = (dpl_tree_t *)malloc(sizeof(dpl_tree_t) * d->world_size);

	buf = (int *)malloc(sizeof(int) * d->world_size);
	for (i = 0; i < d->world_size; i++) { buf[i] = -1; }

	for (i = 0; i < d->world_size; i++)
	{
		j = 0;
		found = 0;
		while (buf[j] != -1)
		{
			if (DPL_GET_GROUP(d->tree[i].type) == buf[j])
			{
				found = 1;
				break;
			}

			j++;
		}

		if (!found)
		{
			buf[j] = DPL_GET_GROUP(d->tree[i].type);
			grpcnt++;
		}

		/* also update the fixed tree */
		d->cache->fixed[i].rank = d->tree[i].rank;
		d->cache->fixed[i].parentid = d->tree[i].parentid;
		d->cache->fixed[i].type = d->tree[i].type;
	}

	d->cache->grps = (dpl_group_internal_t *)malloc(sizeof(dpl_group_internal_t) * grpcnt);
	DPL_DEBUG(d, "Initialized cache with %d group(s).", grpcnt);

	for (i = 0; i < grpcnt; i++)
	{
		d->cache->grps[i].grp_number = -1;
		d->cache->grps[i].grp_nodes = 0;
		d->cache->grps[i].grp_pseudoroots = 0;
		d->cache->grps[i].grp_roots = 0;
		d->cache->grps[i].grp_size = 0;
	}

	d->cache->grp_count = grpcnt;

	for (i = 0; i < d->world_size; i++)
	{
		for (j = 0; j < grpcnt; j++)
		{
			if (d->cache->grps[j].grp_number == -1)
			{
				d->cache->grps[j].grp_number = DPL_GET_GROUP(d->tree[i].type);
			}

			if (DPL_GET_GROUP(d->tree[i].type) == d->cache->grps[j].grp_number)
			{
				switch (DPL_GET_TYPE(d->tree[i].type))
				{
					case DPLTREE_TYPE_ROOT:
						d->cache->grps[j].grp_roots++;
						break;
					case DPLTREE_TYPE_NODE:
						d->cache->grps[j].grp_nodes++;
						break;
					case DPLTREE_TYPE_PSEUDOROOT:
						d->cache->grps[j].grp_pseudoroots++;
						break;
				}
				
				d->cache->grps[j].grp_size++;
				break;
			}
		}
	}

	/* finally, make the cache of arrays for the ranks */
	int cnt = 0;

	for (i = 0; i < grpcnt; i++)
	{
		d->cache->grps[i].grp_root_ranks = (int *)malloc(sizeof(int) * d->cache->grps[i].grp_roots);
		for (j = 0; j < d->world_size; j++)
		{	
			if ((DPL_GET_GROUP(d->tree[j].type) == d->cache->grps[i].grp_number) &&
			    (DPL_GET_TYPE(d->tree[j].type) == DPLTREE_TYPE_ROOT))
			{
				d->cache->grps[i].grp_root_ranks[cnt] = j;
				cnt++;
			}
		}

		cnt = 0;
		d->cache->grps[i].grp_pseudoroot_ranks = (int *)malloc(sizeof(int) * d->cache->grps[i].grp_pseudoroots);
		for (j = 0; j < d->world_size; j++)
		{	
			if ((DPL_GET_GROUP(d->tree[j].type) == d->cache->grps[i].grp_number) &&
			    (DPL_GET_TYPE(d->tree[j].type) == DPLTREE_TYPE_PSEUDOROOT))
			{
				d->cache->grps[i].grp_pseudoroot_ranks[cnt] = j;
				cnt++;
			}
		}

		cnt = 0;
		d->cache->grps[i].grp_node_ranks = (int *)malloc(sizeof(int) * d->cache->grps[i].grp_nodes);
		for (j = 0; j < d->world_size; j++)
		{	
			if ((DPL_GET_GROUP(d->tree[j].type) == d->cache->grps[i].grp_number) &&
			    (DPL_GET_TYPE(d->tree[j].type) == DPLTREE_TYPE_NODE))
			{
				d->cache->grps[i].grp_node_ranks[cnt] = j;
				cnt++;
			}
		}
	}

	/* free buffer */
	free(buf);
}

/* works in linear time as long as the cache is intact */
void dpl_group_info_cache(dpl_t *d,
	size_t *group_size, size_t *roots, size_t *pseudoroots, size_t *nodes)
{
	int grpnum;

	if (d->cache == NULL)
	{
		dpl_update_cache(d);
	}

	grpnum = DPL_GET_GROUP(d->tree[*(d->myrank)].type);

	(*group_size) = d->cache->grps[grpnum].grp_size;
	(*roots) = d->cache->grps[grpnum].grp_roots;
	(*pseudoroots) = d->cache->grps[grpnum].grp_pseudoroots;
	(*nodes) = d->cache->grps[grpnum].grp_nodes;
}
