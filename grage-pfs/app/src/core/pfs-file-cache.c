/*
 * pfs-file-cache.c
 *
 *  Created on: 15/12/2011
 *      Author: joaquincito, fernandito y sebita
 */

#include "grage-commons.h"
#include "pfs-fat32.h"
#include "linux-commons-list.h"
#include "pfs-cache.h"
#include "pfs-endpoint.h"

#include "pfs-file-cache.h"


	DiskSector pfs_endpoint_callCachedGetFileSector(uint32_t sectorNumber , FatFile * f){

		DiskSector returningSector = pfs_endpoint_utils_getFromFileCache(sectorNumber , f);

		if(returningSector.sectorNumber != 0){
			return returningSector;
		}

		returningSector = pfs_endpoint_callGetSector(sectorNumber);

		pfs_endpoint_utils_putInFileCache(returningSector , f->cache);

		return returningSector;
	}


	DiskSector pfs_endpoint_utils_getFromFileCache(uint32_t sectorNumber , FatFile * f){

		DiskSector defaultSector;
		defaultSector.sectorNumber = 0;

		CacheSectorRecord * sectorRecord = pfs_cache_get_sector(sectorNumber, f->cache , pfs_cache_getCacheSectorsFatMaxCount());

		return pfs_endpoint_buildDiskSectorFromCacheCluster(sectorRecord);
	}

	void pfs_endpoint_utils_putInFileCache(DiskSector d , List cache){
		Boolean present = FALSE;

		Iterator * ite = commons_iterator_buildIterator(cache);

		while( commons_iterator_hasMoreElements(ite) ){
			CacheSectorRecord * nodo = (CacheSectorRecord *)commons_iterator_next(ite);
			if(nodo->sector.sectorNumber == d.sectorNumber){
				memcpy(nodo->sector.sectorContent , d.sectorContent , sizeof(d.sectorContent));
				nodo->modificado = TRUE;
				present = TRUE;
				break;
			}
		}
		free(ite);

		if(present == FALSE){
			DiskSector * disk = malloc(sizeof (DiskSector));
			memcpy(disk->sectorContent , d.sectorContent , sizeof d.sectorContent);
			disk->sectorNumber = d.sectorNumber;
			pfs_cache_put_sectors(disk , cache, pfs_cache_getFileCacheMaxCount());
		}
	}
