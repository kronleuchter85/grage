/*
 * praid-utils.c
 *
 *  Created on: 09/11/2011
 *      Author: gonzalo
 */

#include <linux-commons.h>
#include <linux-commons-console-logging.h>

#include "praid-utils.h"
#include "praid-queue.h"

	ThreadMutex listingMutex = PTHREAD_MUTEX_INITIALIZER;



	void praid_utils_printLines(){

		if(commons_console_logging_isDefault()){

			for(int i=0 ; i<65 ; i++){
				printf("-");
			}
			printf("\n");
		}
	}


	void praid_utils_printSynchingInformation(SyncProcessState state){
		if(commons_console_logging_isAll()){
			printf("[ Sincronizados %i bytes de un total de %i ]\n" ,
					state.bytesSynchronized, state.source->volumeSize);
		}
	}



	void praid_utils_printEndSynchingInformation(SyncProcessState state){

		if(commons_console_logging_isDefault()){

			commons_misc_lockThreadMutex(&listingMutex);

			puts("[ Replicacion finalizada ]");

			printf("[ Replicados %i sectores ]\n" , state.sectorsCounter);
			printf("[ Replicados %i de %i bytes ]\n" ,
					 state.bytesSynchronized , state.source->volumeSize);

			Time currentTime = commons_misc_getCurrentTime();
			Time diffTime = commons_misc_getTimeDiff( &currentTime , &state.startTime );

			printf("[ Tiempo insumido: %i minutos, %i segundos, %i milisegundos ]\n"
					, (int)(diffTime.tv_sec/60) , (int)(diffTime.tv_sec%60) , (int)(diffTime.tv_usec/1000) );

			commons_misc_unlockThreadMutex(&listingMutex);
		}
	}



	void praid_utils_printClusterInformation(){

		if(commons_console_logging_isDefault()){

			commons_misc_lockThreadMutex(&listingMutex);

			Iterator * ite = commons_iterator_buildIterator(praid_state_getPpdStorages());

			printf("[ Cluster formado por %i PPD'S: ]\n" , praid_state_getPpdStorages()->size);

			if(praid_state_getPpdStorages()->size > 0){
				printf("[ Miembros actuales: {");

				while( commons_iterator_hasMoreElements( ite)){

					PPDConnectionStorage * s = commons_iterator_next(ite);

					printf(" PPD-%i " , s->id);

					if( commons_iterator_hasMoreElements(ite))
						printf( ",");
				}

				printf("} ]\n");
			}

			commons_misc_unlockThreadMutex(&listingMutex);
		}
	}


	void praid_utils_printPendingJobs(Queue jobs){

		Iterator * ite = commons_iterator_buildIterator(jobs);

		while(commons_iterator_hasMoreElements(ite)){

			Job * job = commons_iterator_next(ite);

			printf("[ Trabajo pendiente - SectorId: %i ]\n" , job->sectorId);
		}

		free(ite);


	}

