/*
 * praid-entrypoint-listener.c
 *
 *  Created on: 06/10/2011
 *      Author: gonzalo
 */
#include <stdlib.h>
#include <unistd.h>

#include <linux-commons-strings.h>
#include <linux-commons-logging.h>
#include <linux-commons-socket.h>
#include <linux-commons-console-logging.h>

#include <nipc-messaging.h>

#include "praid-configuration.h"
#include "praid-entrypoint.h"
#include "praid-endpoint.h"
#include "praid-state.h"
#include "praid-sync.h"
#include "praid-utils.h"



	pthread_t entrypointListenerThread;




	void praid_entry_processNewPpdConnection(NipcMessage message , ListenSocket listenSocket){

		uint8_t ppdId = message.header.operationId;

		PPDConnectionStorage * destiny = praid_state_buildPPDConnectionStorageFromId(
				listenSocket , ppdId ,  message.header.responseCode);

		PPDConnectionStorage * master = praid_balancer_selectStorage();

		praid_state_addPpdStorage(destiny);

		praid_utils_printLines();

		if(commons_console_logging_isDefault()){
			printf("[ Nuevo PPD en el cluster con Id: %i ]\n" , ppdId);
		}
		log_info_t( commons_string_concat("Nuevo PPD en el cluster con ID: " , commons_misc_intToString(ppdId)));

		praid_utils_printClusterInformation(ppdId);

		if(praid_ppd_sync_isValidReplication()) {


			if(commons_console_logging_isDefault()){
				puts("[ Comenzando la replicación de datos ]");
				printf("[ Volumen a replicar: %.3f MiB ]\n" , (float)((float) master->volumeSize /(1024*1024)));
			}
			log_info_t("Comenzando la replicación de datos ");

			sleep(3);

			if(commons_console_logging_isDefault()){
				puts("[ Replicación en proceso .... ]");
			}
			log_info_t("Replicacion en proceso....");

			praid_ppd_sync_fireSynchronization(master , destiny);

			praid_utils_printLines();

		} else {

			if(commons_console_logging_isDefault())
				puts("[ No se realiza replicación de datos ]");

			praid_endpoint_ppd_callProcessJobs(destiny->connection);

		}

		praid_ppd_thread_launchNewSlaveThread(destiny);
	}





	void praid_entry_denegateConnection(uint8_t processId , ListenSocket aListenSocket){

		NipcMessage message = nipc_mbuilder_buildNipcMessage();

		message = nipc_mbuilder_addResponseCode(message , NIPC_RESPONSE_CODE_ERROR);
		message = nipc_mbuilder_addMessageType(message , NIPC_MESSAGE_TYPE_HANDSHAKE);
		message = nipc_mbuilder_addProcessId(message , processId);

		nipc_messaging_send(aListenSocket , message);

		close(aListenSocket);
	}





	void praid_entry_startEntrypointListening(){

		if(commons_console_logging_isDefault())
			puts("[ Proceso RAID en escucha ]");

		ServerSocket * serverSocket = commons_socket_openServerConnection(praid_configuration_getDevicePort());

		while (TRUE){

			ListenSocket listenSocket = commons_socket_acceptConnection(serverSocket);

			NipcMessage handshake = nipc_receiveHandshake(listenSocket );

			if(handshake.header.messageType == NIPC_MESSAGE_TYPE_HANDSHAKE){

				/*
				 * Si la replicacion no esta en proceso se aceptan conecciones de nuevos ppds
				 * de lo contrario las mismas son denegadas para no generar inconsistencias				 *
				 */
				if(handshake.header.processHandshakeId == NIPC_PROCESS_ID_PPD
						&& praid_sync_isReplicationActive()){

					if(commons_console_logging_isDefault())
						puts("[ Se denega la conexion del proceso PPD para mantener la consistencia en la replicacion en curso ]");

					log_info_t("Se denega la conexion del proceso PPD para mantener la consistencia en la replicacion en curso");

					praid_entry_denegateConnection(handshake.header.processHandshakeId , listenSocket);

					continue;

				}else if (handshake.header.processHandshakeId == NIPC_PROCESS_ID_PFS
						&& !praid_state_isThereAnyPpdConnected()){

					if(commons_console_logging_isDefault())
						puts("[ Se denega la conexion del proceso PFS por ausencia de recursos PPD's conectados ]");

					log_info_t("Se denega la conexion del proceso PFS por ausencia de recursos PPD's conectados");

					praid_entry_denegateConnection(handshake.header.processHandshakeId , listenSocket);

					continue;

				}else{

					nipc_sendHandshake(listenSocket ,  handshake.header.processHandshakeId );

					if(handshake.header.processHandshakeId == NIPC_PROCESS_ID_PFS){

						praid_pfs_launchNewSlaveThread(listenSocket);

					}else if(handshake.header.processHandshakeId == NIPC_PROCESS_ID_PPD){

						praid_entry_processNewPpdConnection(handshake , listenSocket);
					}
				}
			}
		}
	}



	void praid_entrypoint_listener_launch(){
		pthread_create(&entrypointListenerThread , NULL , (void * (*)(void *))praid_entry_startEntrypointListening , NULL);
	}
