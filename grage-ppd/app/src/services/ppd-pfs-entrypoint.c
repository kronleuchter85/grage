/*
 * ppd-pfs-entrypoint.c
 *
 *  Created on: 10/11/2011
 *      Author: gonzalo
 */
#include <signal.h>

#include <linux-commons.h>
#include <linux-commons-logging.h>
#include <linux-commons-socket.h>

#include <nipc-messaging.h>

#include "ppd-entrypoint.h"
#include "ppd-state.h"
#include "ppd-connection.h"
#include "ppd-configuration.h"
#include "ppd-launchConsole.h"


	void ppd_pfs_entrypoint_serviceThread(ListenSocket * pfsSocket);
	Boolean ppd_pfs_entrypoint_processRequest(ListenSocket * pfsSocket);



	void ppd_entrypoint_launchPfsPpdEntrypoint(){

		log_info_t("Quedando en escucha de conexiones PFS ");

		while(TRUE){

			ListenSocket pfsSocket = commons_socket_acceptConnection(ppd_state_getPfsConnection());

			if(pfsSocket == -1)
				break;

			Boolean status = ppd_connections_handshake(pfsSocket);

			pthread_t newPfsThread;

			if(status){

				ListenSocket * newPfsSocket = (ListenSocket *) malloc(sizeof(ListenSocket));
				*newPfsSocket = pfsSocket;

				pthread_create( &newPfsThread , NULL , (void* (*) (void*))ppd_pfs_entrypoint_serviceThread , newPfsSocket);

			}
		}
		/*
		* Matamos el proceso consola para q no quede pooleando
		*/
		if(ppd_launch_console_getChildProcessId() != -1)
			kill( ppd_launch_console_getChildProcessId() , SIGKILL);

		exit(1);
	}


	void ppd_pfs_entrypoint_serviceThread(ListenSocket * pfsSocket){

		log_info_t("Se abrio una conexion");

		if( ppd_conf_isPooledConnections()){

			while(ppd_pfs_entrypoint_processRequest(pfsSocket));

		}else{
			ppd_pfs_entrypoint_processRequest(pfsSocket);
		}

		log_info_t("Se cerro una conexion");
	}


	Boolean ppd_pfs_entrypoint_processRequest(ListenSocket * pfsSocket){

		NipcMessage message = nipc_messaging_receive(*pfsSocket);

		if(message.header.operationId == NIPC_OPERATION_ID_GET_SECTORS){

			message.payload.pfsSocket = *pfsSocket;

			ppd_entrypoint_executeGetSector(message);

		}else if(message.header.operationId == NIPC_OPERATION_ID_PUT_SECTORS){

			message.payload.pfsSocket = *pfsSocket;

			ppd_entrypoint_executePutSector(message);

		}else if(nipc_mbuilder_isBlanckMessage(message)){

			return FALSE;
		}

		return TRUE;
	}



