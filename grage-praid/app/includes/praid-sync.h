/*
 * praid-sync.h
 *
 *  Created on: 03/11/2011
 *      Author: gonzalo
 */

#include <pthread.h>
#include <stdio.h>
#include <dirent.h>
#include <stdint.h>
#include <linux-commons.h>
#include "grage-commons.h"

#include "praid-state.h"

#ifndef PRAID_SYNC_H_
#define PRAID_SYNC_H_





	typedef struct {

		uint32_t sectorsCounter;
		uint32_t bytesSynchronized;

		PPDConnectionStorage * source;
		PPDConnectionStorage * destiny;

		Time startTime;

	} SyncProcessState;


	void praid_ppd_sync_fireSynchronization(PPDConnectionStorage * , PPDConnectionStorage * );

	Boolean praid_ppd_sync_isValidReplication();

	void praid_sync_setSyncProcessState(SyncProcessState s);
	SyncProcessState praid_sync_getSyncProcessState();
	void praid_sync_incrementBytesSynchronized();

	void praid_sync_incrementSyncSectorsCounter();

#endif /* PRAID_SYNC_H_ */
