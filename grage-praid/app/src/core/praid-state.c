/*
 * praid-state.c
 *
 *  Created on: 06/10/2011
 *      Author: gonzalo
 */

#include <unistd.h>
#include <pthread.h>

#include <linux-commons.h>
#include <linux-commons.h>
#include <linux-commons-socket.h>
#include <linux-commons-queue.h>

#include "praid-state.h"
#include "praid-queue.h"


	ThreadMutex buildingStoragesMutex = PTHREAD_MUTEX_INITIALIZER;


	List ppdStorages;
	uint8_t availablePpdId = 0;


	List praid_state_getPpdStorages(){
		return ppdStorages;
	}

	Boolean praid_state_storage_eq(PPDConnectionStorage * s1 , PPDConnectionStorage * s2){
		if(s1 == NULL || s2 == NULL)
			return FALSE;
		return (s1->connection == s2->connection);
	}

	void praid_state_initializeStorages(){
		ppdStorages = commons_list_buildList(NULL , (Boolean (*)(void *, void *)) praid_state_storage_eq , commons_list_ORDER_ALWAYS_LAST);
	}

	void praid_state_addPpdStorage(PPDConnectionStorage * aState){
		commons_list_addNode(ppdStorages , aState);
	}

	PPDConnectionStorage * praid_state_buildPPDConnectionStorage(ListenSocket aSocket){

		PPDConnectionStorage * storage = (PPDConnectionStorage * ) malloc(sizeof(PPDConnectionStorage));

		commons_misc_lockThreadMutex(&buildingStoragesMutex);
		storage->id = availablePpdId++;
		commons_misc_unlockThreadMutex(&buildingStoragesMutex);

		storage->pendingResponses = 0;
		storage->pendingJobs = commons_queue_buildQueue((Boolean (*)(void *, void *)) praid_jobs_eq);
		storage->sendedJobs = commons_queue_buildQueue((Boolean (*)(void *, void *)) praid_jobs_eq);

		ThreadMutex disconnectionMutex = PTHREAD_MUTEX_INITIALIZER;
		storage->disconnectionMutex = disconnectionMutex;

		storage->connection = aSocket;
		storage->connected = TRUE;

		return storage;
	}

	PPDConnectionStorage * praid_state_buildPPDConnectionStorageFromId(ListenSocket ls , uint8_t ppdId , uint32_t size){
		PPDConnectionStorage * storage = praid_state_buildPPDConnectionStorage(ls);
		storage->id = ppdId;
		storage->volumeSize = size;
		return storage;
	}


	void praid_state_storage_incrementPendingResponses(PPDConnectionStorage * storage){
		storage->pendingResponses += 1;
	}

	void praid_state_storage_decrementPendingResponses(PPDConnectionStorage * storage){
		storage->pendingResponses -= 1;
	}


	void praid_state_storage_setDisconnected(PPDConnectionStorage * storage){
		storage->connected = FALSE;
	}

	Boolean praid_state_storage_isConnected(PPDConnectionStorage * s){
		return s->connected;
	}

	void praid_state_removePddStorage(PPDConnectionStorage * storage){

		void removingStorage(PPDConnectionStorage * s){

			//praid_state_storage_setDisconnected(s);
			//remover la cola del storage
			close(s->connection);
			//free(s);
		}

		commons_list_removeNode(ppdStorages , storage , (void (*)(Object)) removingStorage);
	}


	Boolean praid_state_isThereAnyPpdConnected(){
		return ( praid_state_getPpdStorages() != NULL
				&& praid_state_getPpdStorages()->size > 0 );
	}


