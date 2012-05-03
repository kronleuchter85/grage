/*
 * nipc-protocol.h
 *
 *  Created on: 01/10/2011
 *      Author: gonzalo
 */


#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "grage-commons.h"
#include "linux-commons-socket.h"
#include "linux-commons-errors.h"

#ifndef NIPC_PROTOCOL_H_
#define NIPC_PROTOCOL_H_

#define NIPC_MESSAGE_PAYLOAD_BUFFER_SIZE		1024

#define NIPC_MESSAGE_TYPE_HANDSHAKE 			0
#define NIPC_MESSAGE_TYPE_SECTOR_ID				'I'
#define NIPC_MESSAGE_TYPE_SECTOR_DISK			'D'
#define NIPC_MESSAGE_TYPE_SYNC_PROCESS			'S'

#define NIPC_RESPONSE_CODE_SUCCESS				2
#define NIPC_RESPONSE_CODE_ERROR				1
#define NIPC_RESPONSE_CODE_NO_CODE				0

#define NIPC_OPERATION_ID_PUT_SECTORS			'P'
#define NIPC_OPERATION_ID_GET_SECTORS			'G'
#define NIPC_OPERATION_ID_DISCONNECT			'D'
#define NIPC_OPERATION_ID_SYNC_PUT_SECTOR		'p'
#define NIPC_OPERATION_ID_SYNC_GET_SECTOR		'g'
#define NIPC_OPERATION_ID_SYNC_END				'e'

#define NIPC_PROCESS_ID_PFS						4
#define NIPC_PROCESS_ID_PPD						3

#define NIPC_FIELD_BLANK						11


	typedef struct {

		uint32_t offset;
		char data[NIPC_MESSAGE_PAYLOAD_BUFFER_SIZE];

	} NipcStream;



	typedef struct {

		uint8_t messageType;
		uint16_t payloadLength;

		uint8_t operationId;
		uint32_t responseCode;

		uint8_t processHandshakeId;

	} __attribute__((packed)) NipcHeader;


	typedef struct {

		DiskSector diskSector;
		uint32_t pfsSocket;

	} __attribute__((packed)) NipcPayload;


	typedef struct {

		NipcHeader header;
		NipcPayload payload;

	} __attribute__((packed)) NipcMessage;




	void nipc_messaging_send(ListenSocket aSocket , NipcMessage aMessage);
	NipcMessage nipc_messaging_receive(ListenSocket aSocket);

	NipcMessage nipc_mbuilder_buildNipcMessage();
	NipcMessage nipc_mbuilder_buildNipcMessageFromHeader(NipcHeader header);
	NipcMessage nipc_mbuilder_buildNipcMessageFromHeaderAndPayload(NipcHeader header , NipcPayload payload);
	NipcMessage nipc_mbuilder_addMessageType(NipcMessage aMessage , uint8_t aType);
	NipcMessage nipc_mbuilder_addResponseCode(NipcMessage aMessage , uint32_t aCode);
	NipcMessage nipc_mbuilder_addPayloadLength(NipcMessage , uint16_t lenth);
	NipcMessage nipc_mbuilder_addOperationId(NipcMessage , uint8_t);
	NipcMessage nipc_mbuilder_addDiskSector(NipcMessage aMessage , DiskSector aSector);
	NipcMessage nipc_mbuilder_addDiskSectorId(NipcMessage aMessage , uint32_t sectorId);
	NipcMessage nipc_mbuilder_addDiskSectorContent(NipcMessage aMessage , char * aContent , uint16_t aLength);
	NipcMessage nipc_mbuilder_addProcessId(NipcMessage , uint8_t);

	Boolean nipc_mbuilder_isBlanckMessage(NipcMessage message);

	void nipc_sendHandshake(ListenSocket lSocket , uint8_t processId );
	void nipc_sendPpdHandshake(ListenSocket lSocket , uint8_t ppdId , uint32_t sectorsCount);
	NipcMessage nipc_receiveHandshake(ListenSocket lSocket );

#endif /* NIPC_PROTOCOL_H_ */
