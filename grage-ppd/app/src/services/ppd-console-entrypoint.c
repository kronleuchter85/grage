/*
 * ppd-console-entrypoint.c
 *
 *  Created on: Nov 3, 2011
 *      Author: seba
 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "grage-commons.h"
#include "ppd-configuration.h"
#include "ppd-utils.h"
#include "ppd-state.h"



PistaSector posicionActual;
MessageConsolePPD messageConsolePPD;
PistaSector ppd_console_entrypoint_getPosicionCabezal(){
	return posicionActual;
}
void ppd_console_entrypoint_setearPosicionCabezal(uint32 pista, uint32 sector){
	posicionActual.pista=pista;
	posicionActual.sectorNumber=sector;
}
uint32 nuevoSectorActual(float sector, float ms, uint32 cantidadSectoresPista){
	uint32 sectorFinal;
	uint32 sectoresMovidos;
	float sectoresPostaMovidos;

	sectoresMovidos = (ms / atoi(getPpdReadTimeMs())) + 1;
	sectoresPostaMovidos = sectoresMovidos % cantidadSectoresPista;
	sectorFinal = sector + sectoresPostaMovidos;
	if (sectorFinal >= cantidadSectoresPista)
	{
		return sectorFinal - cantidadSectoresPista;
	}
	else
	{
		return sectorFinal;
	}

}
float ppd_console_entrypoint_TiempoConsumido(uint32 pistaSolicitada, uint32 sectorSolicitado){

	uint32 sectorActual = posicionActual.sectorNumber;
	float pistaActual=posicionActual.pista;
	float cantidadSectoresPista = atoi(getPpdDiskSector());
	float costoPista = atoi(getPpdSaltoPistaMs());
	float costoSector =60*1000/atoi(getPpdRpm())/cantidadSectoresPista;
	float tiempoPistas = 0;
	float tiempoSectores = 0;
	float tiempoTotal;
	MessageConsolePPD mensaje;
	mensaje.messageID=MESSAGE_ID_SECTORES_RECORRIDOS;
	mensaje.timeInMiliseconds=-1;
	ppd_utils_set_cantidadSectoresPorCilindro(atoi(getPpdDiskCilinder()));

	if(ppd_utils_get_sector_from_sectorofcilinder(sectorSolicitado , pistaSolicitada)
			> (cantidadSectoresPista * ppd_utils_get_cantidadSectoresPorCilindro() - 1)){
		return -1;
	}

	while (!(pistaActual == pistaSolicitada))
	{
		if (pistaActual < pistaSolicitada)
		{
			pistaActual++;
		}
		else
		{
			pistaActual--;
		}
		sectorActual = nuevoSectorActual(sectorActual,costoPista,cantidadSectoresPista);
		tiempoPistas = costoPista + tiempoPistas;
	}
	while (sectorActual != sectorSolicitado)
	{
		sectorActual = sectorActual + 1;
		if (sectorActual == cantidadSectoresPista)
		{
			sectorActual = 0;
		}
		mensaje.pistaSector.sectorNumber=sectorActual;
		mensaje.pistaSector.pista=pistaActual;
		commons_socket_sendBytes(ppd_state_getPpdConsoleSocket()  , &mensaje , sizeof mensaje);
		commons_socket_receiveBytes( ppd_state_getPpdConsoleSocket() , &mensaje , sizeof mensaje);
		tiempoSectores = tiempoSectores + costoSector;
	}
	ppd_console_entrypoint_setearPosicionCabezal(pistaActual,sectorActual);
	tiempoTotal = tiempoPistas + tiempoSectores;
	return tiempoTotal;
}


