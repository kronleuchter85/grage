/*
 * praid-pfs-endpoint.c
 *
 *  Created on: 06/10/2011
 *      Author: gonzalo
 */
#include <unistd.h>
#include <stdint.h>
#include <linux-commons.h>
#include <linux-commons-socket.h>

#include "nipc-messaging.h"
#include "grage-commons.h"


	void praid_endpoint_pfs_responseGetSectors(ListenSocket ls , NipcMessage m){
		nipc_messaging_send(ls , m);
	}

	void praid_endpoint_ppd_sendMessage(ListenSocket ppdSocket , NipcMessage m){
		nipc_messaging_send(ppdSocket , m);
	}

	void praid_endpoint_pfs_responseAndClose(ListenSocket ls , NipcMessage m){
		praid_endpoint_pfs_responseGetSectors(ls , m);
		//close(ls);
	}


	void praid_endpoint_ppd_callSyncGetSector(ListenSocket ls , uint32_t sectorId){
		NipcMessage message = nipc_mbuilder_buildNipcMessage();
		message = nipc_mbuilder_addMessageType(message , NIPC_MESSAGE_TYPE_SYNC_PROCESS);
		message = nipc_mbuilder_addOperationId(message, NIPC_OPERATION_ID_SYNC_GET_SECTOR);

		nipc_messaging_send(ls , message);
	}

	void praid_endpoint_ppd_callSyncPutSector(ListenSocket ls , DiskSector diskSector , uint16_t bytes){
		NipcMessage message = nipc_mbuilder_buildNipcMessage();
		message = nipc_mbuilder_addDiskSector(message , diskSector);
		message = nipc_mbuilder_addPayloadLength(message , bytes);
		message = nipc_mbuilder_addMessageType(message , NIPC_MESSAGE_TYPE_SYNC_PROCESS);
		message = nipc_mbuilder_addOperationId(message, NIPC_OPERATION_ID_SYNC_PUT_SECTOR);

		nipc_messaging_send(ls , message);
	}




	void praid_endpoint_ppd_callProcessJobs(ListenSocket ls){
		NipcMessage message = nipc_mbuilder_buildNipcMessage();
		message = nipc_mbuilder_addMessageType(message , NIPC_MESSAGE_TYPE_SYNC_PROCESS);
		message = nipc_mbuilder_addOperationId(message, NIPC_OPERATION_ID_SYNC_END);

		nipc_messaging_send(ls , message);

	}





