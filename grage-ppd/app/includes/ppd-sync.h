/*
 * ppd-sync.h
 *
 *  Created on: 08/11/2011
 *      Author: gonzalo
 */

#include <linux-commons-file.h>
#include <grage-commons.h>

#ifndef PPD_SYNC_H_
#define PPD_SYNC_H_


	void ppd_sync_processFileSectors(File * fatFile);
	void ppd_sync_replicatePut(uint16_t bytesCount , DiskSector sector);
	void ppd_sync_finishReplication();
	void ppd_sync_replicateGet();
	void ppd_sync_processFileSectors(File * fatFile);


#endif /* PPD_SYNC_H_ */
