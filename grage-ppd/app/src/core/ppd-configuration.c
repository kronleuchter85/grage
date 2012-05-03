/*
 * ppd-configuration.c
 *
 *  Created on: 05/09/2011
 *      Author: EL Seba
 */

#include <stdlib.h>
#include <linux-commons-logging.h>
#include "linux-commons-file.h"
#include "linux-commons-strings.h"
#include "ppd-configuration.h"
#include "ppd-utils.h"

	char * ppdMode = NULL;
	char * ppdAlgoritmo = NULL;
	char * ppdPort = NULL;
	char * ppdIdDisk = NULL;
	char * ppdDiskCilinder = NULL;
	char * ppdDiskHead = NULL;
	char * ppdDiskSector = NULL;
	char * ppdReadTimeMs = NULL;
	char * ppdWriteTimeMs = NULL;
	char * ppdRpm = NULL;
	char * ppdSaltoPistaMs = NULL;
	char * ppdWriteDelay = NULL;
	char * ppdReadDelay = NULL;

	char * praidAddress = NULL;
	char * praidPort = NULL;
	char * diskPath = NULL;



	uint32_t loggingLevel;

	uint32_t ppd_conf_getLoggingLevel(){
		return loggingLevel;
	}
	void ppd_conf_setLoggingLevel(uint32_t l){
		loggingLevel = l;
	}

	void ppd_conf_setPraidPort(char * p){
		praidPort = p;
	}
	char * ppd_conf_getPraidPort(){
		return praidPort;
	}

	void ppd_conf_setPraidAddress(char * p){
		praidAddress = p;
	}
	char * ppd_conf_getPraidAddress(){
		return praidAddress;
	}



	/*
	 * Configuracion
	 */
	void ppd_conf_setPpdMode(char * p){
		ppdMode = p;
	}
	void setPpdAlgoritmo(char * p){
		ppdAlgoritmo = p;
	}
	void ppd_conf_setPpdPort(char * p){
		ppdPort = p;
	}
	void ppd_conf_setPpdIdDisk(char * p){
		ppdIdDisk = p;
	}
	void setPpdDiskCilinder(char * p){
		ppdDiskCilinder = p;
	}
	void setPpdDiskHead(char * p){
		ppdDiskHead = p;
	}
	void setPpdDiskSector(char * p){
		ppdDiskSector = p;
	}
	void setPpdReadTimeMs(char * p){
		ppdReadTimeMs = p;
	}
	void setPpdWriteTimeMs(char * p){
		ppdWriteTimeMs = p;
	}
	void setPpdRpm(char * p){
		ppdRpm = p;
	}
	void setPpdSaltoPistaMs(char * p){
		ppdSaltoPistaMs = p;
	}
	void setPddWriteDelay(char * secs){
		ppdWriteDelay = secs;
	}
	void setPpdReadDelay(char * secs){
		ppdReadDelay = secs;
	}

	void ppd_conf_setDiskPath(char * path){
		diskPath = path;
	}



	char * ppd_conf_getPpdMode(){
		return ppdMode;
	}
	char * getPpdAlgoritmo(){
		return ppdAlgoritmo;
	}
	char * ppd_conf_getPpdPort(){
		return ppdPort;
	}
	char * ppd_conf_getPpdIdDisk(){
		return ppdIdDisk;
	}
	char * getPpdDiskCilinder(){
		return ppdDiskCilinder;
	}
	char * getPpdDiskHead(){
		return ppdDiskHead;
	}
	char * getPpdDiskSector(){
		return ppdDiskSector;
	}
	char * getPpdReadTimeMs(){
		return ppdReadTimeMs;
	}
	char * getPpdWriteTimeMs(){
		return ppdWriteTimeMs;
	}
	char * getPpdRpm(){
		return ppdRpm;
	}
	char * getPpdSaltoPistaMs(){
		return ppdSaltoPistaMs;
	}
	char * getPpdWriteDelay(){
		return ppdWriteDelay;
	}
	char * getPpdReadDelay(){
		return ppdReadDelay;
	}

	char * ppd_conf_getDiskPath(){
		return diskPath;
	}


	void ppd_configuration_processEntries(char * key , char * value , void * object){

		if(commons_string_equals(key , PPD_CONFIGURATION_LOGGING_LEVEL)){
			if(commons_string_equals(value , "info")){
				ppd_conf_setLoggingLevel(INFO);
			}else if(commons_string_equals(value , "debug")){
				ppd_conf_setLoggingLevel(DEBUG);
			}else if(commons_string_equals(value, "warning")){
				ppd_conf_setLoggingLevel(WARNING);
			}else if(commons_string_equals(value , "error")){
				ppd_conf_setLoggingLevel(ERROR);
			}
		}
		if(commons_string_equals(key , PPD_CONFIGURATION_MODE)){
			 ppd_conf_setPpdMode(value);
		}
		if(commons_string_equals(key,PPD_CONFIGURATION_ALGORITMO)){
			setPpdAlgoritmo(value);
		}
		if(commons_string_equals(key,PPD_CONFIGURATION_LISTEN_PORT)){
			 ppd_conf_setPpdPort(value);
		}
		if(commons_string_equals(key,PPD_CONFIGURATION_ID_DISK)){
			ppd_conf_setPpdIdDisk(value);
		}
		if(commons_string_equals(key,PPD_CONFIGURATION_CILINDER)){
			setPpdDiskCilinder(value);
			ppd_utils_set_cantidadSectoresPorCilindro(value);
		}
		if(commons_string_equals(key,PPD_CONFIGURATION_HEAD)){
			setPpdDiskHead(value);
		}
		if(commons_string_equals(key,PPD_CONFIGURATION_SECTOR)){
			setPpdDiskSector(value);
		}
		if(commons_string_equals(key,PPD_CONFIGURATION_READ_TIME)){
			setPpdReadTimeMs(value);
		}
		if(commons_string_equals(key,PPD_CONFIGURATION_WRITE_TIME)){
			setPpdWriteTimeMs(value);
		}
		if(commons_string_equals(key,PPD_CONFIGURATION_RPM)){
			setPpdRpm(value);
		}
		if(commons_string_equals(key,PPD_CONFIGURATION_SALTO_PISTA)){
			setPpdSaltoPistaMs(value);
		}
		if(commons_string_equals(key,PPD_CONFIGURATION_READ_DELAY)){
			setPpdReadDelay(value);
		}
		if(commons_string_equals(key , PPD_CONFIGURATION_CONSOLE_ENABLED)){

			if( commons_string_equals(value , "true")){
				ppd_conf_setConsoleEnabled(TRUE);
			}else if(commons_string_equals(value , "false")){
				ppd_conf_setConsoleEnabled(FALSE);
			}
		}
		if(commons_string_equals(key,PPD_CONFIGURATION_WRITE_DELAY)){
			setPddWriteDelay(value);
		}
		if(commons_string_equals(key , PPD_CONFIGURATION_PRAID_PORT)){
			ppd_conf_setPraidPort(value);
		}
		if(commons_string_equals(key , PPD_CONFIGURATION_PRAID_HOST)){
			ppd_conf_setPraidAddress(value);
		}
		if(commons_string_equals(key , PPD_CONFIGURATION_DISK_PATH)){
			ppd_conf_setDiskPath(value);
		}
		if(commons_string_equals(key , PPD_CONFIGURATION_POOLED_CONNECTIONS_ENABLED)){
			if( commons_string_equals(value , "true")){
				ppd_conf_setPooledConnections(TRUE);
			}else if(commons_string_equals(value , "false")){
				ppd_conf_setPooledConnections(FALSE);
			}
		}
	}

	/*
	 * Inicializacion de la configuracion
	 */

	char * configurationFile = PPD_DEFAULT_CONFIGURATION_FILE;

	void ppd_configuration_setConfigurationFile(char * s){
		configurationFile = s;
	}
	char * ppd_configuration_getConfigurationFile(){
		return configurationFile;
	}

	void ppd_configuration_setup(){
		File * file = commons_file_openFile(configurationFile);

		if(file == NULL){
			printf("No existe el archivo de configuracion\n");
			exit(EXIT_FAILURE);
		}

		commons_file_loadConfiguration(file , ppd_configuration_processEntries);
		commons_file_closeFile(file);
	}




	Boolean pooledConnections = FALSE;

	void ppd_conf_setPooledConnections(Boolean s){
		pooledConnections = s;
	}
	Boolean ppd_conf_isPooledConnections(){
		return pooledConnections;
	}



	Boolean consoleEnabled = TRUE;

	Boolean ppd_conf_isConsoleEnabled(){
		return consoleEnabled;
	}
	void ppd_conf_setConsoleEnabled(Boolean s){
		consoleEnabled = s;
	}

