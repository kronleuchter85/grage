/*
 * pfs-entrypoint.c
 *
 *  Created on: 06/10/2011
 *      Author: gonzalo
 */

#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#include <linux-commons-strings.h>
#include <linux-commons-logging.h>
#include <linux-commons.h>
#include <linux-commons-list.h>
#include <linux-commons-console-logging.h>

#include "nipc-messaging.h"

#include "praid-state.h"
#include "praid-queue.h"
#include "praid-utils.h"
#include "praid-configuration.h"




	Boolean praid_pfs_entrypoint_processRequest(ListenSocket * pfsSocket);
	void praid_pfs_entrypoint_receiveInvocation(ListenSocket * ls);
	void praid_pfs_entrypoint_executePutSector(NipcMessage message);
	void praid_pfs_entrypoint_executeGetSector(NipcMessage message);




	pthread_t pfsSlaveThread;



	void praid_pfs_launchNewSlaveThread(ListenSocket ls){

		ListenSocket * theSocket = malloc(sizeof(ListenSocket));
		*theSocket = ls;

		pthread_create(&pfsSlaveThread , NULL , (void * (*)(void *))praid_pfs_entrypoint_receiveInvocation , theSocket);
	}




	void praid_pfs_entrypoint_receiveInvocation(ListenSocket * ls){

		if(commons_console_logging_isAll())
			puts("[ Se abrio una nueva conexion ]");

		log_info_t("Se abrio una nueva conexion");

		if( praid_conf_isPooledConnections()){

			while(praid_pfs_entrypoint_processRequest(ls));
		}else{

			praid_pfs_entrypoint_processRequest(ls);
		}

		if(commons_console_logging_isAll())
			puts("[ Se cerro una conexion ]");

		log_info_t("Se cerro una conexion");

	}



	Boolean praid_pfs_entrypoint_processRequest(ListenSocket * pfsSocket){

		NipcMessage message = nipc_messaging_receive(*pfsSocket);
		message.payload.pfsSocket = *pfsSocket;

		if(message.header.operationId == NIPC_OPERATION_ID_GET_SECTORS){

			message.payload.pfsSocket = *pfsSocket;

			praid_pfs_entrypoint_executeGetSector(message);

		}else if(message.header.operationId == NIPC_OPERATION_ID_PUT_SECTORS){

			message.payload.pfsSocket = *pfsSocket;

			praid_pfs_entrypoint_executePutSector(message);

		}else if(nipc_mbuilder_isBlanckMessage(message)){

			return FALSE;
		}

		return TRUE;
	}





	void praid_pfs_entrypoint_executePutSector(NipcMessage message){


		praid_utils_printLines();

		if(commons_console_logging_isDefault()){
			puts("[ Ejecutando PUT ]");
			printf("[ Se escribira sobre los PPD's con los siguientes ID: ");
		}


		Iterator * storages = commons_iterator_buildIterator(praid_state_getPpdStorages());

		while( commons_iterator_hasMoreElements( storages ) ){

			PPDConnectionStorage * storage = commons_iterator_next(storages);

			praid_storage_queue_put(storage->pendingJobs , message);

			if(commons_console_logging_isDefault()){
				printf("%i " , storage->id);

			log_info_t( commons_string_concat("Ejecutando PUT sobre PPD-" , commons_misc_intToString(storage->id)));

				if(commons_iterator_hasMoreElements(storages))
					printf(" , ");
			}
		}

		if(commons_console_logging_isDefault())
			printf("\n");


		free(storages);
	}


	void praid_pfs_entrypoint_executeGetSector(NipcMessage message){

		praid_utils_printLines();

		PPDConnectionStorage * storage = praid_balancer_selectStorage();

		if(commons_console_logging_isDefault())
			printf("[ Se realizará un GET (PPD: %i , SectorId: %i) ]\n"
					, storage->id , message.payload.diskSector.sectorNumber);

		log_info_t( commons_string_concat("Ejecutando GET sobre " , commons_misc_intToString(storage->id)));


		praid_storage_queue_put(storage->pendingJobs , message);
	}

