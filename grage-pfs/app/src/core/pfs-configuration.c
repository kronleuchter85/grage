/*
 * pfs-configuration.c
 *
 *  Created on: 05/09/2011
 *      Author: gonzalo-joaco
 */

#include <string.h>
#include <stdlib.h>

#include "linux-commons-file.h"

#include "pfs-configuration.h"


	/*
	 * Configuracion
	 */

	char * deviceAddress = NULL;
	char * devicePort = NULL;
	uint32_t maxConnections;
	uint32 cacheSize = 0;
	char * configurationFile = NULL;

	char * pfs_configuration_getDeviceAddress(void){
		return deviceAddress;
	}

	void pfs_configuration_setDeviceAddress(char * devAddress){
		deviceAddress = devAddress;
	}

	char * pfs_configuration_getDevicePort(void){
		return devicePort;
	}

	void pfs_configuration_setDevicePort(char * devPort){
		devicePort = devPort;
	}

	uint32_t pfs_configuration_getMaxConnections(void){
		return maxConnections;
	}

	void pfs_configuration_setMaxConnections(uint32_t numberOfConnections){
		maxConnections = numberOfConnections;
	}

	uint32 pfs_configuration_getCacheSize(void){
		return cacheSize;
	}

	void pfs_configuration_setCacheSize(char * sizeOfCache){
		cacheSize = atoi(sizeOfCache);
	}

	/*
	 * Procesamiento de la configuracion
	 */

	void pfs_configuration_processEntries(char * key , char * value , void * object){

		if( !strcmp(key , PFS_DEVICE_ADDRESS)){
			pfs_configuration_setDeviceAddress(value);
        }else if(!strcmp(key , PFS_DEVICE_PORT)){
        	pfs_configuration_setDevicePort(value);
        }else if(!strcmp(key , PFS_MAX_CONNECTIONS)){
        	pfs_configuration_setMaxConnections((uint32_t)atoi(value));
        }else if(!strcmp(key , PFS_CACHE_SIZE)){
        	pfs_configuration_setCacheSize(value);
        }
	}


	/*
	 * Inicializacion de la configuracion
	 */

	void pfs_configuration_setConfigurationFile(char * s){
		configurationFile = s;
	}
	char * pfs_configuration_getConfigurationFile(){
		return configurationFile;
	}

	void pfs_configuration_initialize(){
		File * file = commons_file_openFile(configurationFile);
		commons_file_loadConfiguration(file , pfs_configuration_processEntries);
		commons_misc_doFreeNull((void**) &file);
	}
