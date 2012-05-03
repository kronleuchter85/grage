/*
 * ppd-connection-launcher.c
 *
 *  Created on: 08/10/2011
 *      Author: gonzalo
 */
#include <linux-commons-logging.h>
#include "linux-commons-strings.h"
#include "linux-commons.h"
#include "linux-commons-socket.h"
#include "grage-commons.h"

#include "nipc-messaging.h"

#include "ppd-state.h"
#include "ppd-configuration.h"
#include "ppd-entrypoint.h"
#include "ppd-connection.h"


	void ppd_launcher_launchConnections(){
		if( ppd_state_isListenMode()){
			ppd_connections_waitForPfsConnections();
		}else{
			ppd_connections_connectToPraid();
		}
	}



	void ppd_connections_connectToPraid(){

		ListenSocket praidSocket = commons_socket_openClientConnection(
				ppd_conf_getPraidAddress() ,
				ppd_conf_getPraidPort());

		if(praidSocket > 0){
			log_info_t("Se ha logrado conectar con el PRAID");

			log_info_t("Salvando el estado de la coneccion");
			ppd_state_setPraidSocket(praidSocket);

			log_info_t("Realizando handshake");
			Boolean status = ppd_connections_handshake(praidSocket);

			if(!status){
				puts("[ El proceso RAID denego la coneccion del proceso PPD, vuelva a intentarlo mas tarde ]");
				exit(1);
			}

		}else{
			puts("[ Fallo la coneccion con el proceso PRAID ]");
			puts("[ Finalizando ]");
			exit(EXIT_FAILURE);
		}
	}


	void ppd_connections_waitForPfsConnections(){

		ServerSocket * pfsConnection = commons_socket_openServerConnection(
				ppd_conf_getPpdPort());

		if(pfsConnection == NULL){

			puts("[ Puerto ocupado. Se finaliza la aplicacion ]");
			exit(EXIT_FAILURE);
		}

		ppd_state_setPfsConnection(pfsConnection);

	}


	Boolean ppd_connections_handshake(ListenSocket theSocket){

		NipcMessage message;

		if( ppd_state_isListenMode()){

			message = nipc_receiveHandshake(theSocket );

			if(message.header.processHandshakeId == NIPC_PROCESS_ID_PFS)
				nipc_sendHandshake(theSocket , NIPC_PROCESS_ID_PPD);

			return TRUE;

		}else{

			uint8_t ppdId = atoi(ppd_conf_getPpdIdDisk());

			nipc_sendPpdHandshake(theSocket, ppdId , ppd_state_getVolumeSize() );

			message = nipc_receiveHandshake(theSocket);

			if(message.header.responseCode == NIPC_RESPONSE_CODE_ERROR){
				return FALSE;
			}else{
				return TRUE;
			}

		}
	}

