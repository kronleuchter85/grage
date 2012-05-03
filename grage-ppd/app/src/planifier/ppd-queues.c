/*
 * ppd-queues.c
 *
 *  Created on: 26/09/2011
 *      Author: gonzalo
 */
#include <stdlib.h>
#include "grage-commons.h"

#include "linux-commons.h"
#include "linux-commons-queue.h"
#include "linux-commons-strings.h"

#include "ppd-configuration.h"
#include "ppd-queues.h"
#include "ppd-planifier.h"
#include "ppd-persistance.h"
#include "ppd-state.h"


	NipcMessage ppd_queues_buildNipcMessageFromJob(Job * theJob);


	Queue jobsQueue;
	uint32_t jobId;

	Queue ppd_queues_getJobsQueue(){
		return jobsQueue;
	}

	void ppd_queues_initialize(){
		if(commons_string_equals(getPpdAlgoritmo() , "look")){
			jobsQueue = commons_queue_buildQueueWithSortingCriteria(
					(Boolean (*)(void *, void *))ppd_queues_isTheSameJob ,
					(Boolean (*)(void *, void *))ppd_alg_planif_strategy_look);
		}
		if(commons_string_equals(getPpdAlgoritmo() , "sstf")){
				jobsQueue = commons_queue_buildQueueWithSortingCriteria(
						(Boolean (*)(void *, void *))ppd_queues_isTheSameJob ,
						(Boolean (*)(void *, void *))ppd_alg_planif_strategy_sstf);
			}
	}



	Job * ppd_queues_buildJob(NipcMessage mes){
		Job * theJob = (Job *) malloc(sizeof(Job));

		theJob->pfsSocket = mes.payload.pfsSocket;
		theJob->sectorId = mes.payload.diskSector.sectorNumber;
		theJob->payloadLength = mes.header.payloadLength;
		theJob->messageType = mes.header.messageType;
		theJob->operationId = mes.header.operationId;
		memcpy(theJob->sectorContent ,
				mes.payload.diskSector.sectorContent ,
				sizeof(theJob->sectorContent));

		return theJob;
	}




	/*
	 * Ante una invocacion con alguno o ambos de los Jobs
	 * en NULL el resultado de la invocacion sera FALSE
	 */
	Boolean ppd_queues_isTheSameJob(Job * j1 , Job * j2){
		if( j1 == NULL || j2 == NULL)
			return FALSE;
		return (j1->jobId == j2->jobId);
	}




	void ppd_planifier_worker_applyDelayForRead(){
		uint8_t delay = atoi( getPpdReadDelay() );
		if (delay != 0)
			sleep(delay);
	}

	void ppd_planifier_worker_applyDelayForWrite(){
		uint8_t delay = atoi( getPpdWriteDelay() );
		if (delay != 0)
			sleep(delay);
	}


	uint32_t ppd_queues_getFirstJobOperationIdFromQueue(Queue queue){

		if(queue == NULL || queue->elements == NULL)
			return 0;
		else
			return ((Job *)queue->elements->data)->operationId;
	}





	NipcMessage  ppd_queues_pickFromQueue(){

		uint32_t firstOperationId = 0;

		while(! (firstOperationId = ppd_queues_getFirstJobOperationIdFromQueue(jobsQueue)));

		if(firstOperationId == NIPC_OPERATION_ID_GET_SECTORS ){

			ppd_planifier_worker_applyDelayForRead();
		}else if(firstOperationId == NIPC_OPERATION_ID_PUT_SECTORS){

			ppd_planifier_worker_applyDelayForWrite();
		}

		Job * theJob = commons_queue_get(jobsQueue);

		NipcMessage mes = ppd_queues_buildNipcMessageFromJob(theJob);

		free(theJob);

		return mes;
	}


	void ppd_queues_putInQueue(NipcMessage mes){
		Job * theJob = ppd_queues_buildJob(mes);
		theJob->jobId = jobId++;
		commons_queue_put(jobsQueue , theJob);
	}

	NipcMessage ppd_queues_buildNipcMessageFromJob(Job * theJob){
		NipcMessage mes;
		mes.header.payloadLength = theJob->payloadLength;
		mes.payload.pfsSocket = theJob->pfsSocket;
		mes.payload.diskSector.sectorNumber = theJob->sectorId;
		memcpy(mes.payload.diskSector.sectorContent ,
				theJob->sectorContent , sizeof(theJob->sectorContent));
		mes.header.messageType = theJob->messageType;
		mes.header.operationId = theJob->operationId;

		return mes;
	}

