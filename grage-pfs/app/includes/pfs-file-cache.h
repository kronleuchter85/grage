/*
 * pfs-file-cache.h
 *
 *  Created on: 15/12/2011
 *      Author: joaquincito, fernandito y sebita
 */

#ifndef PFS_FILE_CACHE_H_
#define PFS_FILE_CACHE_H_

	DiskSector pfs_endpoint_callCachedGetFileSector(uint32_t , FatFile * );
	DiskSector pfs_endpoint_utils_getFromFileCache(uint32_t , FatFile * );
	void pfs_endpoint_utils_putInFileCache(DiskSector , List);

	DiskSector pfs_endpoint_buildDiskSectorFromCacheCluster(CacheSectorRecord * a);

#endif /* PFS_FILE_CACHE_H_ */
