/*
 * praid-state.h
 *
 *  Created on: 06/10/2011
 *      Author: gonzalo
 */




#include "linux-commons-queue.h"
#include "linux-commons-socket.h"

#ifndef PRAID_STATE_H_
#define PRAID_STATE_H_



	typedef struct {

		uint8_t id;

		Queue pendingJobs;
		uint16_t pendingResponses;

		ListenSocket connection;
		Boolean connected;

		pthread_t storageThreadListener;
		pthread_t storageThreadSender;

		uint32_t volumeSize;

		Queue sendedJobs;


		ThreadMutex disconnectionMutex;

	} PPDConnectionStorage;


	List praid_state_getPpdStorages();

	Boolean praid_state_storage_eq(PPDConnectionStorage * s1 , PPDConnectionStorage * s2);

	void praid_state_initializeStorages();
	void praid_state_addPpdStorage(PPDConnectionStorage * aState);

	PPDConnectionStorage * praid_state_buildPPDConnectionStorage(ListenSocket aSocket);
	PPDConnectionStorage * praid_state_buildPPDConnectionStorageFromId(ListenSocket ls , uint8_t ppdId , uint32_t size);
	PPDConnectionStorage * praid_balancer_selectStorage();

	void praid_state_removePddStorage(PPDConnectionStorage * storage);


	void praid_state_storage_incrementPendingResponses(PPDConnectionStorage * storage);
	void praid_state_storage_decrementPendingResponses(PPDConnectionStorage * storage);
	void praid_state_storage_setDisconnected(PPDConnectionStorage * storage);
	Boolean praid_state_storage_isConnected(PPDConnectionStorage * s);

	Boolean praid_sync_isReplicationActive();
	void praid_sync_setReplicationStatusActive(Boolean status);

	Boolean praid_state_isThereAnyPpdConnected();


#endif /* PRAID_STATE_H_ */
