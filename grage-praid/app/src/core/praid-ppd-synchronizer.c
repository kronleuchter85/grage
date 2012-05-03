/*
 * praid_ppd_synchronizer.c
 *
 *  Created on: 03/11/2011
 *      Author: gonzalo
 */

#include <linux-commons.h>
#include <linux-commons-list.h>

#include "praid-sync.h"
#include "praid-queue.h"
#include "praid-state.h"
#include "praid-endpoint.h"
#include "praid-configuration.h"



	Boolean replicationStatusActive;

	Boolean praid_sync_isReplicationActive(){
		return replicationStatusActive;
	}
	void praid_sync_setReplicationStatusActive(Boolean status){
		replicationStatusActive = status;
	}


	SyncProcessState syncState;

	void praid_sync_setSyncProcessState(SyncProcessState s){
		syncState = s;
	}
	SyncProcessState praid_sync_getSyncProcessState(){
		return syncState;
	}


	void praid_sync_incrementSyncSectorsCounter(){
		syncState.sectorsCounter += 1;
	}
	void praid_sync_incrementBytesSynchronized(uint32_t bytes){
		syncState.bytesSynchronized += bytes;
	}





	SyncProcessState praid_sync_buildSyncProcessState( PPDConnectionStorage * src , PPDConnectionStorage * dest){
		SyncProcessState syncProcess;

		syncProcess.sectorsCounter = 0;
		syncProcess.bytesSynchronized = 0;
		syncProcess.source = src;
		syncProcess.destiny = dest;
		syncProcess.startTime = commons_misc_getCurrentTime();

		return syncProcess;
	}



	Boolean praid_ppd_sync_isValidReplication(){
		return (praid_state_getPpdStorages()->size >= 2)
				&& praid_configuration_getEnableReplication();
	}


	PPDConnectionStorage * praid_ppd_sync_selectMasterStorage(){
		Iterator * ite = commons_iterator_buildIterator(praid_state_getPpdStorages());
		PPDConnectionStorage * storage = commons_iterator_next(ite);
		commons_misc_doFreeNull((void**)ite);
		return storage;
	}


	void praid_ppd_sync_fireSynchronization(PPDConnectionStorage * source ,
			PPDConnectionStorage * dest){

		praid_sync_setReplicationStatusActive(TRUE);

		SyncProcessState syncProcess = praid_sync_buildSyncProcessState( source , dest);
		praid_sync_setSyncProcessState(syncProcess);

		praid_endpoint_ppd_callSyncGetSector(source->connection , 0);
	}



