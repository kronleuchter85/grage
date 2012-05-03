/*
 * pfs-cache.c
 *
 *  Created on: 15/09/2011
 *      Author: Fernando
 */

#include <time.h>
#include <sys/timeb.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "linux-commons-list.h"
#include "grage-commons.h"
#include "linux-commons-file.h"
#include "linux-commons-strings.h"
#include "pfs-configuration.h"
#include "pfs-cache.h"

#include "pfs-state.h"

	List listaCacheFat;

	List pfs_cache_getListaCacheFat() {
		return listaCacheFat;
	}
	void pfs_cache_setListaCacheFat(List lista) {
		listaCacheFat = lista;
	}



	uint32 blockCacheMaxCount;

	void pfs_cache_setFileCacheMaxCount(uint32 count) {
		blockCacheMaxCount = count;
	}
	uint32 pfs_cache_getFileCacheMaxCount() {
		return blockCacheMaxCount;
	}



	uint32 cacheSectorsFatMaxCount;

	void pfs_cache_setCacheSectorsFatMaxCount(uint32 count) {
		cacheSectorsFatMaxCount = count;
	}
	uint32 pfs_cache_getCacheSectorsFatMaxCount() {
		return cacheSectorsFatMaxCount;
	}



	Boolean pfs_cache_habilitada() {
		if (pfs_configuration_getCacheSize() == 0)
			return FALSE;
		return TRUE;
	}


	Boolean pfs_cache_isFatSectorReserved(uint32 sectorNumber) {
		Volume * v = pfs_state_getVolume();
		if (sectorNumber <= (v->rsv) + (v->fatSize))
			return TRUE;
		return FALSE;
	}

	List pfs_cache_sectors_initialize() {

		Boolean eq(void * s1, void * s2) {
			CacheSectorRecord * p1 = (CacheSectorRecord *) s1;
			CacheSectorRecord * p2 = (CacheSectorRecord *) s2;
			return p1->sector.sectorNumber == p2->sector.sectorNumber;
		}
		Boolean ordenar(void * s1, void * s2) {
			CacheSectorRecord * p1 = (CacheSectorRecord *) s1;
			CacheSectorRecord * p2 = (CacheSectorRecord *) s2;
			return p1->estado < p2->estado;
		}
		return (commons_list_buildList(NULL, eq, commons_list_ORDER_ALWAYS_FIRST));
	}


	void pfs_cache_initialize() {
		Volume * v = pfs_state_getVolume();
		pfs_state_initializeOpenFiles();

		uint32_t fatSize = v->fatSize;

		pfs_cache_setCacheSectorsFatMaxCount(fatSize * 20 / 100);
		pfs_cache_setListaCacheFat(pfs_cache_sectors_initialize());
		pfs_cache_setFileCacheMaxCount(pfs_configuration_getCacheSize() * 2);
	}


	void pfs_cache_sectors_registrar_acceso(List listaCacheSectors) {
		Iterator * fuckingIterator = commons_iterator_buildIterator(
				listaCacheSectors);

		while (commons_iterator_hasMoreElements(fuckingIterator)) {
			((CacheSectorRecord *) commons_iterator_next(fuckingIterator))->estado++;
		}
		free(fuckingIterator);
	}

	CacheSectorRecord * pfs_cache_get_sector(uint32 sectorID, List cache, uint32 maxSectorQty) {
		Boolean get(CacheSectorRecord * a) {
			return (a->sector.sectorNumber == sectorID);
		}
		pfs_cache_sectors_registrar_acceso(cache);
		return commons_list_getNodeByCriteria(cache, (Boolean (*)(Object))get);
	}

	void pfs_cache_put_sectors(DiskSector * sectorNuevo, List listaCacheSectors,uint32 sectorsMaxCount) {
		CacheSectorRecord * auxNode;
		CacheSectorRecord * nodo = (CacheSectorRecord *) malloc(sizeof(CacheSectorRecord));

		if (commons_list_getSize(listaCacheSectors) >= sectorsMaxCount) {
			Iterator * fuckingIterator = commons_iterator_buildIterator(listaCacheSectors);
			nodo->estado = 0;
			while (commons_iterator_hasMoreElements(fuckingIterator)) {
				auxNode = (CacheSectorRecord *) commons_iterator_next(fuckingIterator);
				if (nodo->estado <= auxNode->estado) {
					//nodo=auxNode;
					break;
				}
			}
			pfs_endpoint_callPutSector(auxNode->sector);
			commons_list_removeNode(listaCacheSectors, auxNode, free);
			free(fuckingIterator);
		}
		//AGREGO EL NODO NUEVO
		nodo->estado = 0;
		memcpy(nodo->sector.sectorContent, sectorNuevo->sectorContent, sizeof sectorNuevo->sectorContent);
		nodo->sector.sectorNumber = sectorNuevo->sectorNumber;
		nodo->modificado = FALSE;
		commons_list_addNode(listaCacheSectors, nodo);
		pfs_cache_sectors_registrar_acceso(listaCacheSectors);
	}

