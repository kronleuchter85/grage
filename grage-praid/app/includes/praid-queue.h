/*
 * praid-queue.h
 *
 *  Created on: 10/10/2011
 *      Author: gonzalo
 */

#include <stdlib.h>
#include "nipc-messaging.h"
#include "linux-commons-list.h"
#include "linux-commons-queue.h"
#include "linux-commons.h"

#ifndef PRAID_QUEUE_H_
#define PRAID_QUEUE_H_


	typedef struct {
		uint32_t jobId;

		uint32_t messageType;
		uint32_t operationId;
		char sectorContent[512];
		uint32_t sectorId;
		uint32_t pfsSocket;
		uint16_t payloadLength;
	} Job;


	void praid_storage_queue_put(Queue aQueue , NipcMessage aMessage);
	NipcMessage praid_storage_queue_get(Queue aQueue);
	Boolean praid_jobs_eq(Job * j1 , Job * j2);
	Job * praid_jobs_buildJob(NipcMessage mes);
	NipcMessage praid_jobs_buildNipcMessageFromJob(Job * theJob);


#endif /* PRAID_QUEUE_H_ */
