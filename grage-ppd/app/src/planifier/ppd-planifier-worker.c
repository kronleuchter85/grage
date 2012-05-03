/*
 * ppd-planifier-worker.c
 *
 *  Created on: 09/10/2011
 *      Author: gonzalo
 */


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>

#include "linux-commons.h"
#include "grage-commons.h"
#include "nipc-messaging.h"

#include "ppd-queues.h"
#include "ppd-persistance.h"
#include "ppd-state.h"
#include "ppd-entrypoint.h"
#include "ppd-configuration.h"
#include "ppd-endpoint.h"
#include "ppd-console-entreypoint.h"
#include "ppd-utils.h"

	void ppd_planifier_worker_doJob(void * arg);

	pthread_mutex_t count_mutex     = PTHREAD_MUTEX_INITIALIZER;
	pthread_cond_t  condition_var   = PTHREAD_COND_INITIALIZER;

	pthread_t jobsThread;

	void ppd_planifier_worker_doJobs(){
		pthread_create(&jobsThread , NULL , (void * (*)(void *)) ppd_planifier_worker_doJob , NULL);
	}




	void ppd_planifier_worker_doJob(void * arg){

		while(TRUE){

			NipcMessage m = ppd_queues_pickFromQueue();

			if ( m.header.operationId == NIPC_OPERATION_ID_PUT_SECTORS ) {

				ppd_persistence_writeSector( m.payload.diskSector );

			} else if ( m.header.operationId == NIPC_OPERATION_ID_GET_SECTORS ) {

				DiskSector disk = ppd_persistence_readSector(m.payload.diskSector.sectorNumber);

				m = nipc_mbuilder_addDiskSector( m , disk);
				m = nipc_mbuilder_addResponseCode(m , NIPC_RESPONSE_CODE_SUCCESS);

				//printf("Enviando sectorId: %i\n" , disk.sectorNumber);

				ppd_endpoint_responseGetSector(m);

			}
			if (m.header.operationId == 69){
				Iterator * cola = commons_iterator_buildIterator(ppd_queues_getJobsQueue());
				while (commons_iterator_hasMoreElements(cola)){
					printf("SECTOR DE LA COLA ID: %d\n",((Job *) commons_iterator_next(cola))->sectorId);
				}
				MessageConsolePPD mensaje;
				mensaje.pistaSector.pista = ppd_utils_get_cilinder_from_sector(m.payload.diskSector.sectorNumber);
				mensaje.pistaSector.sectorNumber = ppd_utils_get_sectorofcilinder_from_sector(m.payload.diskSector.sectorNumber);
				if ((mensaje.timeInMiliseconds = ppd_console_entrypoint_TiempoConsumido(
								mensaje.pistaSector.pista,
								mensaje.pistaSector.sectorNumber))==-1){
					mensaje.messageID = MESSAGE_ID_ERROR;
				}else{
					mensaje.messageID = MESSAGE_ID_TIEMPO_CONSUMIDO;

				}
				pthread_mutex_lock(&count_mutex);
				pthread_cond_signal( &condition_var );
				pthread_mutex_unlock(&count_mutex);

				commons_socket_sendBytes(ppd_state_getPpdConsoleSocket(), &mensaje,
									sizeof mensaje);

			}
			ppd_console_entrypoint_setearPosicionCabezal(
					ppd_utils_get_cilinder_from_sector(m.payload.diskSector.sectorNumber),
					ppd_utils_get_sectorofcilinder_from_sector(m.payload.diskSector.sectorNumber));
			//ppd_alg_setCurrentPossition(m.payload.diskSector.sectorNumber);

		}
	}




