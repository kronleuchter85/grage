/*
 * linux-commons-socket.c
 *
 *  Created on: 29/05/2011
 *      Author: gonzalo
 */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "linux-commons-strings.h"
#include "linux-commons-socket.h"
#include "linux-commons-logging.h"


	ServerSocket * commons_socket_openServerConnection(char * port) {

		ServerSocket * serverSocket = (ServerSocket *) malloc(sizeof(ServerSocket));

		serverSocket->listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);

		if (serverSocket->listenSocket == SOCKET_ERROR) {
			log_error_t("No se pudo crear el socket con la llamada 'socket' fd %d",serverSocket->listenSocket);
			return NULL;
		}

		int on = 1;
		setsockopt(serverSocket->listenSocket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on) );

		bzero(&serverSocket->address, sizeof(struct sockaddr_in));
		serverSocket->address.sin_addr.s_addr = INADDR_ANY;
		serverSocket->address.sin_port = (in_port_t) htons(atoi(port));
		serverSocket->address.sin_family = AF_INET;

		if (bind(serverSocket->listenSocket,
				(struct sockaddr*) &serverSocket->address,
				sizeof(struct sockaddr_in)) == SOCKET_ERROR) {
			log_error_t("no se pudo bindear el socket %d ",serverSocket->listenSocket);
			return NULL;
		}

		if (listen(serverSocket->listenSocket, 5) == SOCKET_ERROR) {
			log_error_t("no se pudo poner el socket en escucha %d",serverSocket->listenSocket);
			return NULL;
		}

		return serverSocket;
	}


	ListenSocket commons_socket_acceptConnection(ServerSocket * serverSocket) {
		int nAddressSize = sizeof(struct sockaddr_in);
		return accept(serverSocket->listenSocket,
				(struct sockaddr*) &serverSocket->address,
				(socklen_t *) &nAddressSize);
	}


	ListenSocket commons_socket_openClientConnection(char * host , char * port ){

		if(host == NULL || port == NULL){
			log_error_t("invocando 'openClientConnection' con parametros {host} o {port} en NULL %s","");
			return INVALID_SOCKET;
		}

		struct hostent * serverHost;
		struct sockaddr_in clientSocket;
		bzero((char *) &clientSocket , sizeof(clientSocket));
		serverHost = gethostbyname(host);
		clientSocket.sin_port = htons(atoi(port));
		clientSocket.sin_family = AF_INET;
		clientSocket.sin_addr.s_addr = ((struct in_addr*)serverHost->h_addr_list[0])->s_addr;
		ListenSocket clientDescriptor = socket(AF_INET , SOCK_STREAM , IPPROTO_IP);
		int status = connect(clientDescriptor , (struct  sockaddr *)&clientSocket , sizeof(clientSocket));

		return (status == 0)? clientDescriptor : INVALID_SOCKET;
	}


	ClientConnection * commons_socket_buildClientConnection(ListenSocket l){
		ClientConnection * clientConnection = malloc(sizeof(ClientConnection));
		clientConnection->clientSocket = l;
		return clientConnection;
	}


	int commons_socket_sendBytes(ListenSocket ls , void * bytesToSend , int bytesCount){
		int allSsended = 0;
		int sended = 0;
		while(allSsended < bytesCount ){
			sended = send(ls , bytesToSend, bytesCount - allSsended , 0);
			allSsended += sended;

			if(sended <= 0)
				break;
		}

		log_debug_t("Se han enviado %d bytes" ,allSsended);
		return allSsended;
	}


	int commons_socket_receiveBytes(ListenSocket ls , void * bytesToReceive , int bytesCount){

		int allReceived = 0;
		int received = 0;
		while( allReceived < bytesCount ){
			received = recv(ls , bytesToReceive , bytesCount - allReceived  , MSG_WAITALL);
			allReceived += received;

			if(received <= 0)
				break;
		}

		log_debug_t("Se han recibido %d bytes" ,allReceived);
		return allReceived;
	}


	/*
	 * SI logra setear el timeout sin que ocurra ningun error devuelve TRUE
	 * De lo contrario devuevle FALSE
	 */
	Boolean commons_socket_setSocketTimeOut(ListenSocket listenSocket , __time_t seconds , __suseconds_t microseconds){

		struct timeval timeValue;

		timeValue.tv_sec = seconds;
		timeValue.tv_usec = microseconds;

		return (0 == setsockopt( listenSocket , SOL_SOCKET , SO_RCVTIMEO , (const void * ) &timeValue , sizeof(struct timeval)));
	}
