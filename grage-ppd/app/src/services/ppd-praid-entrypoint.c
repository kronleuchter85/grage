/*
 * praid-entrypoint.c
 *
 *  Created on: 10/11/2011
 *      Author: gonzalo
 */

#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <nipc-messaging.h>

#include "ppd-entrypoint.h"
#include "ppd-state.h"
#include "ppd-sync.h"
#include "ppd-launchConsole.h"

	void ppd_entrypoint_executeSyncPutSector(NipcMessage message);
	void ppd_entrypoint_executeSyncGetSector();
	void ppd_entrypoint_endReplicationProcess();



	void ppd_entrypoint_launchPraidPpdEntrypoint(){

		while(TRUE){

			NipcMessage m = nipc_messaging_receive(ppd_state_getPraidSocket());

			if(m.header.operationId == NIPC_FIELD_BLANK)
				break;

			if(m.header.operationId == NIPC_OPERATION_ID_GET_SECTORS){

				ppd_entrypoint_executeGetSector(m);

			}else if (m.header.operationId == NIPC_OPERATION_ID_PUT_SECTORS){

				ppd_entrypoint_executePutSector(m);

			}else if(m.header.messageType == NIPC_MESSAGE_TYPE_SYNC_PROCESS){

				if(m.header.operationId == NIPC_OPERATION_ID_SYNC_PUT_SECTOR){
					if(!ppd_state_isReplicationProcessActive())
						ppd_state_setReplicationProcessActive(TRUE);

					ppd_entrypoint_executeSyncPutSector(m);
				}else if(m.header.operationId == NIPC_OPERATION_ID_SYNC_GET_SECTOR){

					if(!ppd_state_isReplicationProcessActive())
						ppd_state_setReplicationProcessActive(TRUE);

					ppd_entrypoint_executeSyncGetSector();
				}else if(m.header.operationId == NIPC_OPERATION_ID_SYNC_END){

					ppd_entrypoint_endReplicationProcess();

				}
			}
		}

		puts("[ Finalizando la aplicacion por desconexion del PRAID ]");
		close(ppd_state_getPraidSocket());

		/*
		 * Matamos el proceso consola para q no quede pooleando
		 */
		if(ppd_launch_console_getChildProcessId() != -1)
			kill( ppd_launch_console_getChildProcessId() , SIGKILL);

		exit(1);
	}




	void ppd_entrypoint_executeSyncPutSector(NipcMessage message){
		ppd_sync_replicatePut(message.header.payloadLength , message.payload.diskSector);
	}

	void ppd_entrypoint_executeSyncGetSector(){
		ppd_sync_replicateGet();
	}
	void ppd_entrypoint_endReplicationProcess(){
		ppd_sync_finishReplication();
	}
