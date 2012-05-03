/*
 * praid-ppd-thread.c
 *
 *  Created on: 06/10/2011
 *      Author: gonzalo
 */
#include <errno.h>
#include <unistd.h>
#include <pthread.h>

#include <linux-commons-strings.h>
#include <linux-commons-logging.h>
#include <linux-commons.h>
#include <linux-commons-errors.h>
#include <linux-commons-console-logging.h>
#include <linux-commons-list.h>

#include "nipc-messaging.h"

#include "praid-state.h"
#include "praid-queue.h"
#include "praid-endpoint.h"
#include "praid-configuration.h"
#include "praid-sync.h"
#include "praid-utils.h"

	void praid_ppd_processDisconnection(PPDConnectionStorage * storage);
	void praid_balancer_redistributeJobs(Queue jobs);
	void praid_ppd_thread_listener(PPDConnectionStorage * );
	void praid_ppd_thread_sender(PPDConnectionStorage * );
	void praid_ppd_checkIfContinueDenegatingRequests(uint8_t ppdid);



	ThreadMutex disconnetionMutex = PTHREAD_MUTEX_INITIALIZER;


	void praid_ppd_thread_launchNewSlaveThread(PPDConnectionStorage * aStorage){

		pthread_create(&aStorage->storageThreadListener , NULL , (void * (*)(void *)) praid_ppd_thread_listener , aStorage);
		pthread_create(&aStorage->storageThreadSender , NULL , (void * (*)(void *)) praid_ppd_thread_sender , aStorage);
	}




	void praid_ppd_thread_listener(PPDConnectionStorage * storage){

		while(praid_state_storage_isConnected(storage)){

			NipcMessage message = nipc_messaging_receive(storage->connection);

			if( nipc_mbuilder_isBlanckMessage(message) ){

				praid_state_storage_setDisconnected(storage);

				break;

			}else if(message.header.messageType == NIPC_MESSAGE_TYPE_SYNC_PROCESS){

				PPDConnectionStorage * dest = praid_sync_getSyncProcessState().destiny;

				SyncProcessState syncState = praid_sync_getSyncProcessState();

				if(message.header.operationId == NIPC_OPERATION_ID_SYNC_PUT_SECTOR){

					praid_sync_incrementSyncSectorsCounter();

					praid_endpoint_ppd_callSyncPutSector(dest->connection , message.payload.diskSector , message.header.payloadLength);

					praid_sync_incrementBytesSynchronized(message.header.payloadLength);

					praid_utils_printSynchingInformation(syncState);

				}else if (message.header.operationId == NIPC_OPERATION_ID_SYNC_END){

					praid_endpoint_ppd_callProcessJobs(dest->connection);

					praid_utils_printEndSynchingInformation(syncState);

					praid_utils_printClusterInformation();

					praid_sync_setReplicationStatusActive(FALSE);

					praid_utils_printLines();
				}

			}else{

				//printf("Recibiendo sectorId %i desde el ppd-%i\n" , message.payload.diskSector.sectorNumber , storage->id);

				praid_state_storage_decrementPendingResponses(storage);

				praid_endpoint_pfs_responseAndClose(message.payload.pfsSocket , message);

				Boolean eq(Job * j){
					return j->sectorId == message.payload.diskSector.sectorNumber;
				}

				Job * job = commons_list_getNodeByCriteria(storage->sendedJobs ,
						(Boolean (*)(Object))eq);

				if(job != NULL)
					commons_list_removeNode(storage->sendedJobs , job , free);
			}
		}

		praid_ppd_processDisconnection(storage);

	}


	void praid_ppd_processDisconnection(PPDConnectionStorage * storage){

		commons_misc_lockThreadMutex(&storage->disconnectionMutex);

		if(commons_console_logging_isDefault()){
			printf("[ Se ha desconectado el PPD %i ]\n" , storage->id);
		}
		log_info_t( commons_string_concat("Se ha desconectado el PPD " , commons_misc_intToString(storage->id)));

		praid_ppd_checkIfContinueDenegatingRequests(storage->id);

		Queue readingJobs = praid_utils_getReadingJobs(storage);

		if(readingJobs->size > 0){

			printf("[ %i trabajos de lectura huerfanos tras la caida del PPD-%i ]\n" , readingJobs->size , storage->id);

			if(commons_console_logging_isAll())
				praid_utils_printPendingJobs(readingJobs);

			praid_state_removePddStorage(storage);

			praid_balancer_redistributeJobs(readingJobs);

		}else{

			praid_state_removePddStorage(storage);
		}

		praid_utils_printClusterInformation();

		commons_misc_unlockThreadMutex(&storage->disconnectionMutex);

		//puts("finaliza el listener del ppd muerto");
	}



	void praid_ppd_thread_sender(PPDConnectionStorage * aStorage){

		while(praid_state_storage_isConnected(aStorage)){

			while( commons_queue_isEmpty(aStorage->pendingJobs));

			/*
			 * Si se esta realizando la replicacion entonces no se envian trabajos
			 */
			//printf("checkeando replicacion activa en ppd-%i \n" , aStorage->id);
			if(praid_sync_isReplicationActive()){
				//printf("replicacion activa, se esperan 10 segundos en ppd-%i \n"  , aStorage->id);
				sleep(10);
				continue;
			}

			commons_misc_lockThreadMutex(&aStorage->disconnectionMutex);

			if(!aStorage->connected){
				//puts("se elimina el ppd desde el hilo sender y se finaliza el hilo");
				//free(aStorage);
				commons_misc_unlockThreadMutex(&aStorage->disconnectionMutex);
				return;
			}

			NipcMessage message = praid_storage_queue_get(aStorage->pendingJobs);


			//printf("pidiendo sectorId %i al ppd-%i \n", message.payload.diskSector.sectorNumber , aStorage->id);

			praid_endpoint_ppd_sendMessage(aStorage->connection , message);

			praid_storage_queue_put(aStorage->sendedJobs , message);

			if(message.header.operationId == NIPC_OPERATION_ID_GET_SECTORS){
				praid_state_storage_incrementPendingResponses(aStorage);
			}

			commons_misc_unlockThreadMutex(&aStorage->disconnectionMutex);
		}
	}



	void praid_ppd_checkIfContinueDenegatingRequests(uint8_t ppdid){

		SyncProcessState state = praid_sync_getSyncProcessState();

		if( (state.source != NULL && state.source->id == ppdid)
				|| (state.destiny != NULL && state.destiny->id == ppdid)){
			if( praid_sync_isReplicationActive())
				praid_sync_setReplicationStatusActive(FALSE);
		}
	}







