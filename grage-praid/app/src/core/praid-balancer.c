/*
 * praid-queue.c
 *
 *  Created on: 06/10/2011
 *      Author: gonzalo
 */
#include <stdlib.h>

#include <linux-commons-list.h>
#include <nipc-messaging.h>
#include <linux-commons-console-logging.h>

#include "praid-state.h"
#include "praid-queue.h"


#define PRAID_BALANCER_WEIGHT_PENDING_RESPONSE			1
#define PRAID_BALANCER_WEIGHT_PENDING_REQUEST			2



	void praid_balancer_redistributeJobs(Queue jobs);
	void praid_balancer_assignJob(PPDConnectionStorage * storage , Job * job);


	ThreadMutex loadBalancingMutex = PTHREAD_MUTEX_INITIALIZER;
	ThreadMutex redistributionMutex = PTHREAD_MUTEX_INITIALIZER;



	uint32_t praid_balancer_storageRanking(PPDConnectionStorage * storage){
		if(storage->pendingJobs == NULL){
			puts("[ Alerta roja, la cola de un storage esta en null, cagamos! ]");
			exit(1);
		}

		return storage->pendingJobs->size * PRAID_BALANCER_WEIGHT_PENDING_REQUEST
				+ storage->pendingResponses * PRAID_BALANCER_WEIGHT_PENDING_RESPONSE;
	}


	PPDConnectionStorage * praid_balancer_selectStorage(){

		commons_misc_lockThreadMutex(&loadBalancingMutex);

		Iterator * storages = commons_iterator_buildIterator(praid_state_getPpdStorages());

		uint32_t minimumRanking = 0;
		PPDConnectionStorage * selected = NULL;

		while( commons_iterator_hasMoreElements( storages ) ){

			PPDConnectionStorage * storage = commons_iterator_next(storages);

			if( selected == NULL || (praid_balancer_storageRanking(storage) < minimumRanking) ){

				minimumRanking = praid_balancer_storageRanking(storage);
				selected = storage;

				if(praid_balancer_storageRanking(storage) == 0)
					break;
			}
		}

		free(storages);
		commons_misc_unlockThreadMutex(&loadBalancingMutex);

		return selected;
	}








	void praid_balancer_redistributeJobs(Queue jobs){

		commons_misc_lockThreadMutex(&redistributionMutex);

		Iterator * ite = commons_iterator_buildIterator(jobs);

		printf("[ Redistribuyendo %i trabajos en %i ppds conectados ]\n" , jobs->size , praid_state_getPpdStorages()->size);

		while(commons_iterator_hasMoreElements(ite)){

			Job * job = commons_iterator_next(ite);

			PPDConnectionStorage * bestCandidate = praid_balancer_selectStorage();

			if(bestCandidate == NULL){
				puts("[ No se encuentran PPDs conectados ]");
				break;
			}

			praid_balancer_assignJob(bestCandidate , job);

		}

		free(ite);

		commons_misc_unlockThreadMutex(&redistributionMutex);
	}



	void praid_balancer_assignJob(PPDConnectionStorage * storage , Job * job){

		if(commons_console_logging_isAll()){
			printf("[ Asignando trabajo de SectorId %i al PPD-%i ]\n" , job->sectorId , storage->id);
		}else{
			printf("[ Asignando trabajo al PPD-%i ]\n" , storage->id);
		}

		commons_queue_put(storage->pendingJobs , job);
	}
