/*
 * nipc-messaging.c
 *
 *  Created on: 01/10/2011
 *      Author: gonzalo
 */


#include "nipc-messaging.h"


	void nipc_messaging_send(ListenSocket aSocket , NipcMessage aMessage){
		commons_socket_sendBytes(aSocket , &aMessage , sizeof(NipcMessage));
	}

	NipcMessage nipc_messaging_receive(ListenSocket aSocket){

		NipcMessage message = nipc_mbuilder_buildNipcMessage();

		commons_socket_receiveBytes(aSocket , &message , sizeof(NipcMessage));

		return message;
	}
