/*
 * ppd_console_interpreter.c
 *
 *  Created on: 10/10/2011
 *      Author: joaquin
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux-commons.h>
#include <linux-commons-socket.h>
#include <linux-commons-strings.h>
#include <linux-commons-list.h>
#include "ppd_console_launcher.h"
#include <grage-commons.h>
#include "ppd-console-utils.h"
uint32 sectores[5];
int receivedCount;
String ppd_console_parseCMD(char * comando){
	uint32 i=0;
	uint32 j=0;
	String cmd;
	cmd = (String)malloc(255);

	memset(cmd, '\0', 255);

	while(comando[i] != '(' && comando[i] != '\n' && comando[i] != ' ' && i <= 250){
		cmd[j] = comando[i];
		i++;
		j++;
	}

	return cmd;
}

String ppd_console_get_cmd_parameter(char * comando, uint32 cmd_length){
	uint32 i=cmd_length + 1;
	uint32 j=0;
	String parametro;
	parametro = (String)malloc(255);
	memset(parametro, '\0', 255);

	while(comando[i] != ')' && comando[i] != ' ' && i < (250 - cmd_length))
	{
		parametro[j] = comando[i];
		i++;
		j++;
		if(comando[i] != ')' && comando[i] != ' ' && i < (250 - cmd_length) && (comando[i] > 57 || comando[i] < 48)){
			puts("Valor de Cluster ingresado incorrecto");
			return "-1";
		}
	}

	return parametro;
}

uint32 ppd_console_obtenerNroParametros(char * comando){
	uint32 cant = 0;
	uint32 i=0;

	while(comando[i] !=')' && comando[i] != '\n' && i < 250 && cant < 5){
		if(comando[i] == ' ') cant++;
		i++;
	}

	if(cant > 0 && cant < 5 &&comando[6] != ' ') {
		cant++;
		return cant;
	}
	if(cant == 0 && atoi(ppd_console_get_cmd_parameter(comando, strlen(ppd_console_parseCMD(comando)))) >= 0){
		return 1;
	}
	if(comando[6] == ' '){
		puts("El primer parametro no debe ser vacio");
		return 0;
	}
	if (cant == 0 && atoi(ppd_console_get_cmd_parameter(comando, strlen(ppd_console_parseCMD(comando)))) < 0){
		puts("trace debe recibir parametros");
		return 0;
	}
	if(cant >= 5){
		puts("El maximo de clusters es 5");
		return 0;
	}
	return 0;
}

void ppd_console_llenarVector(char * comando, uint32 nro_parametros){
	if(nro_parametros != 0){
		uint32 i = 6;
		uint32 j = 0;
		uint32 k = 0;
		char aux[6];
		while(comando[i] != ')' && i < 244 && j < nro_parametros){
			while(comando[i] >= 48 && comando[i] <= 57){
				aux[k] = comando[i];
				k++;
				i++;
			}
			k = 0;
			sectores[j] = atoi(aux);
			memset(aux, '\0', sizeof(aux));
			j++;
			i++;
		}
	}
}

void ppd_console_info(){
	puts("console info");
	imprimirPosicionActual();
}

void ppd_console_clean(uint32 clusterInicial, uint32 clusterFinal){
	puts("console clean");
	printf("clusterInicial %d\n", clusterInicial);
	printf("clusterFinal %d\n", clusterFinal);
}
void imprimirPosicionActual(){
	MessageConsolePPD mensaje = armarMensaje(MESSAGE_ID_POSICION_ACTUAL, -1,-1,-1);
	commons_socket_sendBytes(ppd_console_launcher_getSocketPPD(),&mensaje,sizeof mensaje);
	receivedCount = commons_socket_receiveBytes( ppd_console_launcher_getSocketPPD() ,&mensaje , sizeof mensaje);
	printf("Posición actual: %d:%d\n",mensaje.pistaSector.pista,mensaje.pistaSector.sectorNumber);
}
void imprimirSectoresRecorridos(uint32 pistaRequerida, uint32 sectorRequerido){
	MessageConsolePPD mensaje = armarMensaje(MESSAGE_ID_SECTORES_RECORRIDOS, pistaRequerida,sectorRequerido,-1);
	commons_socket_sendBytes(ppd_console_launcher_getSocketPPD(),&mensaje,sizeof mensaje);
	receivedCount = commons_socket_receiveBytes( ppd_console_launcher_getSocketPPD() ,&mensaje , sizeof mensaje);

	printf("Sectores Recorridos: ");
	while(mensaje.messageID == MESSAGE_ID_SECTORES_RECORRIDOS){

		printf("%d:%d ",mensaje.pistaSector.pista,mensaje.pistaSector.sectorNumber);
		commons_socket_sendBytes(ppd_console_launcher_getSocketPPD(),&mensaje,sizeof mensaje);
		receivedCount = commons_socket_receiveBytes( ppd_console_launcher_getSocketPPD() ,&mensaje , sizeof mensaje);


	}
	if(mensaje.messageID == MESSAGE_ID_ERROR){
		puts("El sector solicitado se encuentra fuera de rango.");
		return;
	}
	printf("\nTiempo consumido: %dms \n",mensaje.timeInMiliseconds);
}
void obtenerSectoresPorCilindro(){
	MessageConsolePPD mensaje = armarMensaje(MESSAGE_ID_SECTORES_POR_CILINDRO, -1, -1,-1);
	commons_socket_sendBytes(ppd_console_launcher_getSocketPPD(),&mensaje,sizeof mensaje);
	receivedCount = commons_socket_receiveBytes( ppd_console_launcher_getSocketPPD() ,&mensaje , sizeof mensaje);
	ppd_console_utils_set_cantidadSectoresPorCilindro(mensaje.pistaSector.sectorNumber);
}
void ppd_console_trace(uint32 sectorPedido){
	puts("console trace");
	imprimirPosicionActual();
	obtenerSectoresPorCilindro();
	printf("Sector Solicitado: %d:%d \n", ppd_console_utils_get_cilinder_from_sector(sectorPedido),ppd_console_utils_get_sectorofcilinder_from_sector(sectorPedido));

	imprimirSectoresRecorridos(ppd_console_utils_get_cilinder_from_sector(sectorPedido),ppd_console_utils_get_sectorofcilinder_from_sector(sectorPedido));
}

void ppd_console_interpreter(){

	char buffer[250];

	String cmd, parameter, parameter2;
	receivedCount = 1;
	sleep(2);
	printf(">");
	fgets(buffer, 250, stdin);
	while( strcmp(buffer, "exit") ){
		cmd = ppd_console_parseCMD(buffer);
		//INFO
		if(!strcmp(cmd, "info"))ppd_console_info();
		//CLEAN
		if(!strcmp(cmd, "clean") && buffer[5]!= ' '){
			if(buffer[5]=='(' && buffer[6]!=')'){
				parameter  = ppd_console_get_cmd_parameter(buffer, strlen(cmd));
				parameter2 = ppd_console_get_cmd_parameter(buffer, (strlen(cmd) + 1 + strlen(parameter)));
				if(atoi(parameter2)>0) ppd_console_clean(atoi(parameter), atoi(parameter2));
				else puts("Indique clusters a limpiar");
				bzero(parameter, sizeof(parameter));
				bzero(parameter2,sizeof(parameter2));
			}
			else puts("Indique clusters a limpiar");
		}
		//TR++ACE
		if(!strcmp(cmd, "trace") && buffer[5]!= ' '){
			uint32 nro_parametros;
			uint32 i = 1;
			nro_parametros = ppd_console_obtenerNroParametros(buffer);
			ppd_console_llenarVector(buffer, nro_parametros);
			while(i <= nro_parametros){
				ppd_console_trace(sectores[i - 1]);
				if (i+1>nro_parametros) puts("Próximo Sector: ..");
				else printf("Próximo Sector: %d:%d\n\n",ppd_console_utils_get_cilinder_from_sector(sectores[i]),ppd_console_utils_get_sectorofcilinder_from_sector(sectores[i]));
				i++;
			}
		}
		//EXIT
		if(!strcmp(cmd, "exit")) {
			char command[30];
			sprintf(command, "kill -9 %d", (int)getppid());
			system(command);
			exit(0);
		}

		if((strcmp(cmd, "info") && strcmp(cmd, "clean") && strcmp(cmd, "trace")) || buffer[5]== ' ' ){
			printf("CMD = %s",cmd);
			puts("Comando incorrecto");
			sleep(1);
		}
		memset(cmd,'\0',sizeof(cmd));
		memset(buffer,'\0',sizeof(buffer));

		printf(">");
		fgets(buffer, 250, stdin);

	}
}
