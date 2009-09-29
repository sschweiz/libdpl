/**
 * @brief DPL cache functionality definitions
 * @file dpl_cache.h
 */

#ifndef __DPL_CACHE_H__
#define __DPL_CACHE_H__

#include <sys/types.h>
#include "dpl_types.h"

/**
 * @brief Updates the dpl cache
 * @param d pointer to dpl_t structure
 *
 * Users should never modify the cache directly.  Instead,
 * edit the distribution identity tree and call this function.
 */
void dpl_update_cache(dpl_t *d);

/**
 * @brief Cached version of dpl_group_info()
 * @see dpl_group_info
 */
void dpl_group_info_cache(dpl_t *d,
	size_t *group_size, size_t *roots, size_t *pseudoroots, size_t *nodes);

#endif
