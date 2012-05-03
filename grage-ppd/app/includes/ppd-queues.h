/*
 * ppd-queues.h
 *
 *  Created on: 26/09/2011
 *      Author: gonzalo
 */

#include "linux-commons.h"
#include "nipc-messaging.h"
#include "linux-commons-queue.h"
#ifndef PPD_PLANIFIER_H_
#define PPD_PLANIFIER_H_

	typedef struct {
		uint32_t jobId;

		uint32_t messageType;
		uint32_t operationId;
		char sectorContent[512];
		uint32_t sectorId;
		uint32_t pfsSocket;
		uint16_t payloadLength;
	} Job;


	Job * ppd_queues_buildJob(NipcMessage mes);
	Boolean ppd_queues_isTheSameJob(Job * j1 , Job * j2);
	void ppd_queues_initialize();
	Queue ppd_queues_getJobsQueue();

	NipcMessage  ppd_queues_pickFromQueue();
	void ppd_queues_putInQueue(NipcMessage mes);


#endif /* PPD_PLANIFIER_H_ */
