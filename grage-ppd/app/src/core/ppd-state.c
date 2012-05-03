/*
 * ppd-state.c
 *
 *  Created on: 08/10/2011
 *      Author: gonzalo
 */

#include <stdlib.h>

#include <linux-commons.h>
#include <linux-commons-socket.h>
#include <linux-commons-strings.h>
#include <linux-commons-file.h>

#include <grage-commons.h>

#include "ppd-configuration.h"
#include "ppd-state.h"
#include "ppd-persistance.h"



	Boolean connectionStatusActive;
	ListenSocket praidSocket;

	void ppd_state_setPraidSocket(ListenSocket ls){
		praidSocket = ls;
		connectionStatusActive = TRUE;
	}
	ListenSocket ppd_state_getPraidSocket(){
		return praidSocket;
	}



	ServerSocket * pfsConnection;

	void ppd_state_setPfsConnection(ServerSocket * p){
		pfsConnection = p;
		connectionStatusActive = TRUE;
	}
	ServerSocket * ppd_state_getPfsConnection(){
		return pfsConnection;
	}


	Boolean ppd_state_isListenMode(){
		return ( commons_string_equals(
				ppd_conf_getPpdMode() ,
				PPD_CONFIGURATION_MODE_LISTEN));
	}

	Boolean ppd_state_isConnectMode(){
		return !ppd_state_isListenMode();
	}





	char * diskStartAddress = NULL;

	char * ppd_state_getDiskStartAddress(){
		return diskStartAddress;
	}
	void ppd_state_setDiskStartAddress(char * anAddress){
		diskStartAddress = anAddress;
	}




	ListenSocket ppdConsoleSocket;

	void ppd_state_setPpdConsoleSocket(ListenSocket ls){
		ppdConsoleSocket = ls;
	}
	ListenSocket ppd_state_getPpdConsoleSocket(){
		return ppdConsoleSocket;
	}



	File * replicationDiskVolume = NULL;

	void ppd_state_setReplicationDiskVolume(File * f){
		replicationDiskVolume = f;
	}
	File * ppd_state_getReplicationDiskVolume(){
		return replicationDiskVolume;
	}




	uint32_t volumeSize;

	void ppd_state_setVolumeSize(uint32_t v){
		volumeSize = v;
	}
	uint32_t ppd_state_getVolumeSize(){
		return volumeSize;
	}

	void ppd_state_initializeVolumeSize(){

		File * volumeFile = commons_file_openFile(ppd_conf_getDiskPath());

		if(volumeFile != NULL){
			ppd_state_setVolumeSize(commons_file_getFileSize(volumeFile));

			commons_file_closeFile(volumeFile);

		}else if(ppd_state_isListenMode()){

			puts("[ En modo PPD Server debe existir el archivo de datos. Finalizando aplicacion. ]");
			exit(EXIT_FAILURE);
		}
	}





	Boolean replicationProcessActive = FALSE;

	Boolean ppd_state_isReplicationProcessActive(){
		return replicationProcessActive;
	}
	void ppd_state_setReplicationProcessActive(Boolean s){
		replicationProcessActive = s;
	}
