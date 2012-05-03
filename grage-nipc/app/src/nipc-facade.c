/*
 * nipc-facade.c
 *
 *  Created on: 08/10/2011
 *      Author: gonzalo
 */

#include "nipc-messaging.h"


	void nipc_sendHandshake(ListenSocket lSocket ,
			uint8_t processId){

		NipcMessage message = nipc_mbuilder_buildNipcMessage();

		message = nipc_mbuilder_addMessageType(message , NIPC_MESSAGE_TYPE_HANDSHAKE);
		message = nipc_mbuilder_addProcessId(message , processId);

		nipc_messaging_send(lSocket , message);

	}


	void nipc_sendPpdHandshake(ListenSocket lSocket ,
			uint8_t ppdId ,
			uint32_t sectorsCount){

		NipcMessage message = nipc_mbuilder_buildNipcMessage();

		message = nipc_mbuilder_addMessageType(message , NIPC_MESSAGE_TYPE_HANDSHAKE);
		message = nipc_mbuilder_addProcessId(message , NIPC_PROCESS_ID_PPD);
		message = nipc_mbuilder_addOperationId(message , ppdId);
		message = nipc_mbuilder_addResponseCode(message , sectorsCount);

		nipc_messaging_send(lSocket , message);

	}


	NipcMessage nipc_receiveHandshake(ListenSocket lSocket ){

		NipcMessage message = nipc_messaging_receive(lSocket);

		return message;
	}


