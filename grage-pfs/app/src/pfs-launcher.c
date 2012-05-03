/*
 * pfs-launcher.c
 *
 *  Created on: 05/09/2011
 *      Author: gonzalo-joaco
 */
#include <time.h>
#include <sys/timeb.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <linux-commons-logging.h>
#include <linux-commons-socket.h>
#include <linux-commons-errors.h>
#include <linux-commons-list.h>
#include <linux-commons.h>
#include <grage-commons.h>

#include "nipc-messaging.h"

#include "pfs-endpoint.h"
#include "pfs-configuration.h"
#include "pfs-console.h"
#include "pfs-state.h"
#include "pfs-fat32.h"
#include "pfs-fuse.h"
#include "pfs-cache.h"



	pthread_t fuseThread;



	extern pthread_t consoleThread;

	int mainParamsCount;
	char * mainParamsArgs[];


	void pfs_launcher_launchConnections(){

		char * host = pfs_configuration_getDeviceAddress();
		char * port = pfs_configuration_getDevicePort();

		ListenSocket dataSocket = commons_socket_openClientConnection(host , port);

		if( dataSocket < 0 ){
			perror("Error al iniciar conexion con el PRAID/PPD");
			exit(EXIT_FAILURE);
		}

		pfs_state_setDataSocket(dataSocket);

		nipc_sendHandshake(dataSocket , NIPC_PROCESS_ID_PFS);

		NipcMessage message =  nipc_receiveHandshake(pfs_state_getDataSocket());

		if(message.header.responseCode == NIPC_RESPONSE_CODE_ERROR){
			puts("Fallo el handshake");
			exit(1);
		}

	}



	void pfs_launcher_initialize() {
		log_create("pfs", "/opt/grage-repository/logs/grage-pfs.log",
				INFO , M_CONSOLE_DISABLE);
		pfs_configuration_setConfigurationFile(PFS_DEFAULT_CONFIGURATION_FILE);
		pfs_configuration_initialize();

		pfs_launcher_launchConnections();
		//pfs_console_initialize();

	}





	void pfs_launcher_initializeBPB(){
		DiskSector sector = pfs_endpoint_callGetSector(0);

		BPB biosParameterBlock;

		memcpy(&biosParameterBlock , sector.sectorContent , sizeof(BPB));

		pfs_state_setBiosParameterBlock(biosParameterBlock);

		Volume * volume = pfs_fat_utils_loadVolume(&biosParameterBlock);

		if(volume == NULL){
			puts("El volumen es invalido");
			exit(1);
		}

		pfs_state_setVolume(volume);
	}




	void pfs_launch_fuseThread(char *argv[]){
		pfs_fuse_launchFuse(mainParamsCount,argv);
	}


	void pfs_launcher_launch(int argc, char *argv[]) {

		mainParamsCount = argc;

		pfs_launcher_initializeBPB();

		pfs_cache_initialize();

		//pthread_create(&fuseThread , NULL , pfs_launch_fuseThread , argv);

		pfs_state_setMountPath(argv[1]);

		pfs_console_initialize();

		pfs_launch_fuseThread(argv);

		//pthread_join(consoleThread , NULL);
	}


	void pfs_launcher_exit() {
		Volume * v = pfs_state_getVolume();
		pfs_fat32_fatCacheFlush();
		pfs_fat32_updateDiskInformation(v);
		log_destroy();
	}


	int main(int argc, char *argv[]) {

		pfs_launcher_initialize();

		pfs_launcher_launch(argc , argv);

		pfs_launcher_exit();

		return EXIT_SUCCESS;
	}


