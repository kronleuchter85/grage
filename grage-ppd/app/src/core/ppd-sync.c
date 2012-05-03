/*
 * ppd-sync.c
 *
 *  Created on: 08/11/2011
 *      Author: gonzalo
 */
#include <stdlib.h>
#include <linux-commons-file.h>
#include <grage-commons.h>

#include "ppd-configuration.h"
#include "ppd-sync.h"
#include "ppd-state.h"
#include "ppd-endpoint.h"
#include "ppd-planifier.h"
#include "ppd-persistance.h"

	uint32_t allBytesWritten = 0;
	uint32_t sectorsWrittenCount = 0;

	uint32_t allBytesReaded = 0;
	uint32_t sectorsReadedCount = 0;





	void ppd_sync_replicateGet(){
		File * fatFile = commons_file_openFile(ppd_conf_getDiskPath());

		if( fatFile == NULL){
			printf("No existe el archivo %s\n." , ppd_conf_getDiskPath());
			exit(1);
		}

		ppd_state_setReplicationDiskVolume(fatFile);

		ppd_sync_processFileSectors(fatFile);

		ppd_sync_finishReplication();

		ppd_endpoint_sendFinishReplication();
	}





	void ppd_sync_replicatePut(uint16_t bytesCount , DiskSector sector){

		if(ppd_state_getReplicationDiskVolume() == NULL){

			File * fatFile = fopen(ppd_conf_getDiskPath() , "w");

			if( fatFile == NULL){
				printf("No existe el archivo %s\n." , ppd_conf_getDiskPath());
				exit(1);
			}
			ppd_state_setReplicationDiskVolume(fatFile);
		}

		if(bytesCount > 0){

			if( bytesCount < sizeof(sector.sectorContent))
				log_info_t("Escribiendo el ultimo tramo");

			size_t bytesWritten = fwrite(sector.sectorContent ,	sizeof(char) ,
					bytesCount , ppd_state_getReplicationDiskVolume());

			allBytesWritten += bytesWritten;
			sectorsWrittenCount ++;

			if(bytesWritten < 0){
				puts("Error en la escritura del archivo");
				puts("No se pudo finalizar la replicacion");
				exit(1);
			}

			fflush(ppd_state_getReplicationDiskVolume());
		}
	}




	void ppd_sync_finishReplication(){

		/*
		 * Si esta activo el proceso de replicacion se finaliza
		 * cerrando los recursos ocupados por el mismo
		 */
		if(ppd_state_isReplicationProcessActive()){

			fclose(ppd_state_getReplicationDiskVolume());

			ppd_state_setReplicationDiskVolume(NULL);


			/*
			 * Inicializamos de nuevo el size del volumen
			 * utilizado durante el mapeo del disco y durante
			 * el proceso de persistencia
			 */
			ppd_state_initializeVolumeSize();

			/*
			 * Si se mapeó en algun momento el disco lo desmapeamos
			 * (esto puede deberse a que el ppd actual es un master en la replicacion
			 * o es un slave que termino de replicarse pero que al iniciarse tenia algún
			 * contenido en su volumen de datos)
			 */
			if(ppd_state_getDiskStartAddress() != NULL )
				ppd_persistence_unmapDevice();

			/*
			 * Mapeamos el disco
			 */
			ppd_persistence_mapDevice();


			ppd_state_setReplicationProcessActive(FALSE);
		}

	}




	void ppd_sync_processFileSectors(File * fatFile){

		DiskSector disk = commons_buildDiskSector();

		size_t bytesReaded = fread(disk.sectorContent , sizeof(char) , sizeof(disk.sectorContent) , fatFile);

		allBytesReaded += bytesReaded;
		sectorsReadedCount++;

		while( ! feof(fatFile) ){

			ppd_endpoint_sendSyncSector(bytesReaded , disk);

			disk = commons_buildDiskSector();

			bytesReaded = fread(disk.sectorContent , sizeof(char) , sizeof(disk.sectorContent) , fatFile);
			allBytesReaded += bytesReaded;
			sectorsReadedCount++;

		}

		if(bytesReaded > 0){
			ppd_endpoint_sendSyncSector(bytesReaded , disk);
		}
	}





