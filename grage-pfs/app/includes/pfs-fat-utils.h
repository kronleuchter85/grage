/*
 * pfs-fat-utils.h
 *
 *  Created on: Dec 3, 2011
 *      Author: seba
 */

#ifndef PFS_FAT_UTILS_H_
#define PFS_FAT_UTILS_H_
#include "grage-commons.h"
#include "pfs-fat32.h"
#include "pfs-cache.h"


	typedef struct Node {
		uint32_t Cluster;
		struct Node * Next;
	} rsvCluster;

//	void pfs_fat_utils_cache_sectores_initialize();
//	void pfs_fat_utils_cache_put_sectores(DiskSector sector);
//	DiskSector pfs_fat_utils_cache_get_sectores(uint32_t sectorBuscado);
//	uint32_t pfs_fat_utils_cache_sectores_tiene_sector(uint32_t sectorBuscado);
//	void pfs_fat_utils_cache_sectores_registrar_acceso();

	//Ejemplifico abajo de cada una, como invocar a cada funcion
	void pfs_fat_utils_CreateList(rsvCluster **);
		//pfs-fat-utils-crearLista(&firstNode);
	void pfs_fat_utils_InsertNodo(rsvCluster ** , uint32_t );
		//insertNode(&firstNode, Cluster);
	void pfs_fat_utils_ShowList(rsvCluster * );
		//pfs_fat_utils_ShowList(firstNode);
	void pfs_fat_utils_FreeList(rsvCluster **);
		//pfs_fat_utils_FreeList(&firstNode);
	uint32_t pfs_fat_utils_GetNextCluster(rsvCluster ** );
		//pfs_fat_utils_GetNextCluster(&firstNode);
	uint32_t pfs_fat_utils_IsListEmpty(rsvCluster * );
		//pfs_fat_utils_IsListEmpty(*firstCluster);
	uint32_t pfs_fat_utils_FreeClustersQuantity();
	uint32_t pfs_fat_utils_BusyClustersQuantity();
	uint32_t pfs_fat_utils_FATsizeKilobytes();
/*
	uint32_t pfs_fat_fetchChar(LDirEntry *, int8_t );
	int8_t pfs_fat_getNameLength(LDirEntry *);
	void pfs_fat_extractName( LDirEntry * , uint16_t * , int8_t );
	char * pfs_fat_get_fileName(LDirEntry *);
	void pfs_fat_toDirent(struct dirent * , DirEntry , LDirEntry );
*/
#endif /* PFS_FAT_UTILS_H_ */
