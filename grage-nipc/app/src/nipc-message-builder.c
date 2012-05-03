/*
 * nipc-handshake.c
 *
 *  Created on: 03/10/2011
 *      Author: gonzalo
 */

#include "nipc-messaging.h"

	NipcMessage nipc_mbuilder_buildNipcMessage(){
		NipcMessage builder ;
		builder = nipc_mbuilder_addResponseCode(builder , NIPC_FIELD_BLANK);
		builder = nipc_mbuilder_addMessageType(builder , NIPC_FIELD_BLANK);
		builder = nipc_mbuilder_addPayloadLength(builder , NIPC_FIELD_BLANK);
		builder = nipc_mbuilder_addProcessId(builder , NIPC_FIELD_BLANK);
		builder = nipc_mbuilder_addOperationId(builder , NIPC_FIELD_BLANK);
		return builder;
	}

	NipcMessage nipc_mbuilder_buildNipcMessageFromHeader(NipcHeader header){
		NipcMessage builder ;
		builder.header = header;
		return builder;
	}

	NipcMessage nipc_mbuilder_buildNipcMessageFromHeaderAndPayload(NipcHeader header , NipcPayload payload){
		NipcMessage builder ;
		builder.payload = payload;
		builder.header = header;
		return builder;
	}

	NipcMessage nipc_mbuilder_addMessageType(NipcMessage aMessage , uint8_t aType){
		aMessage.header.messageType = aType;
		return aMessage;
	}

	NipcMessage nipc_mbuilder_addResponseCode(NipcMessage aMessage , uint32_t aCode){
		aMessage.header.responseCode = aCode;
		return aMessage;
	}

	NipcMessage nipc_mbuilder_addPayloadLength(NipcMessage aMessage , uint16_t lenth){
		aMessage.header.payloadLength = lenth;
		return aMessage;
	}

	NipcMessage nipc_mbuilder_addOperationId(NipcMessage aMessage , uint8_t operationId){
		aMessage.header.operationId = operationId;
		return aMessage;
	}

	NipcMessage nipc_mbuilder_addDiskSector(NipcMessage aMessage , DiskSector aSector){
		aMessage.payload.diskSector = aSector;
		return aMessage;
	}

	NipcMessage nipc_mbuilder_addDiskSectorId(NipcMessage aMessage , uint32_t sectorId){
		aMessage.payload.diskSector.sectorNumber = sectorId;
		return aMessage;
	}

	NipcMessage nipc_mbuilder_addDiskSectorContent(NipcMessage aMessage , char * aContent , uint16_t aLength){
		memcpy( aMessage.payload.diskSector.sectorContent , aContent , aLength);
		return aMessage;
	}

	NipcMessage nipc_mbuilder_addProcessId(NipcMessage message , uint8_t processId){
		message.header.processHandshakeId = processId;
		return message;
	}

	Boolean nipc_mbuilder_isBlanckMessage(NipcMessage message){

		return (message.header.messageType == NIPC_FIELD_BLANK
				&& message.header.operationId == NIPC_FIELD_BLANK
				&& message.header.processHandshakeId == NIPC_FIELD_BLANK
				&& message.header.responseCode == NIPC_FIELD_BLANK);
	}

