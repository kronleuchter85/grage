/*
 * linux-commons-socket.h
 *
 *  Created on: 30/05/2011
 *      Author: gonzalo
 */
#include <netinet/in.h>

#include "linux-commons.h"

#ifndef LINUX_COMMONS_SOCKET_H_
#define LINUX_COMMONS_SOCKET_H_

#define INVALID_SOCKET -1

	typedef int ListenSocket;

	/**
	 * Sirve como connector del lado servidor
	 */
	typedef struct {
		ListenSocket listenSocket;
		struct sockaddr_in address;
	} ServerSocket;

	typedef struct {
		ListenSocket clientSocket;
		Thread servingThread;
	} ClientConnection;


	ClientConnection * commons_socket_buildClientConnection(ListenSocket l);
	ServerSocket * commons_socket_openServerConnection(char * port);
	ListenSocket commons_socket_openClientConnection(char * host , char * port );
	ListenSocket commons_socket_acceptConnection(ServerSocket * serverSocket);


	int commons_socket_sendBytes(ListenSocket ls , void * bytesToSend , int bytesCount);
	int commons_socket_receiveBytes(ListenSocket ls , void * buffer , int bytesCount);

	Boolean commons_socket_setSocketTimeOut(ListenSocket listenSocket , __time_t seconds , __suseconds_t microseconds);

#endif /* LINUX_COMMONS_SOCKET_H_ */
