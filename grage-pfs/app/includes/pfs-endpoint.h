/*
 * pfs-endpoint.h
 *
 *  Created on: 15/09/2011
 *      Author: Fernando
 */

#include "grage-commons.h"
//#include "pfs-fat32.h"
#include <stdint.h>

#ifndef PFS_ENDPOINT_H_
#define PFS_ENDPOINT_H_

	//void pfs_endpoint_callPutSector(DiskSector , FatFile *);
	DiskSector pfs_endpoint_callGetSector(uint32_t);

	ListenSocket pfs_endpoint_doHandshake(void);

	DiskSector pfs_endpoint_callPooledGetSector(uint32_t sectorNumber);
	void pfs_endpoint_callPooledPutSector( DiskSector diskSector);
	DiskSector pfs_endpoint_callNonPooledGetSector(uint32_t sectorNumber);
	void pfs_endpoint_callNonPooledPutSector( DiskSector diskSector);
	DiskSector pfs_endpoint_buildAndSendGet(ListenSocket ds, uint32_t sectorNumber);
	void pfs_endpoint_bulidAndSendPut(ListenSocket ds , DiskSector diskSector);

	void pfs_endpoint_blocks_callExecuteGetSector(ListenSocket , uint32_t);
	DiskSector pfs_endpoint_blocks_callReturnGetSector(ListenSocket);

	//DiskSector pfs_endpoint_callCachedGetSector(uint32_t , FatFile  *);
	void pfs_endpoint_callCachedPutSector(DiskSector);


#endif /* PFS_ENDPOINT_H_ */
