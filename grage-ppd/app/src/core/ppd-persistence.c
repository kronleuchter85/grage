/*
 * ppd-persistence.c
 *
 *  Created on: 26/09/2011
 *      Author: Fernando
 */

#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>

#include <linux-commons-strings.h>

#include <grage-commons.h>

#include "ppd-configuration.h"
#include "ppd-persistance.h"
#include "ppd-state.h"


	void ppd_persistence_writeSector(DiskSector aSector){

		uint32_t offset = aSector.sectorNumber * SECTOR_SIZE;

        if (memcpy(ppd_state_getDiskStartAddress() + offset , aSector.sectorContent , SECTOR_SIZE ) == NULL){
        	//TODO: logear aca
        }

        msync(ppd_state_getDiskStartAddress() , SECTOR_SIZE , MS_SYNC );
	}


	DiskSector ppd_persistence_readSector(uint32_t sectorId ){

		DiskSector disk = commons_buildDiskSector();

		uint32_t offset = sectorId * SECTOR_SIZE;

		if( memcpy(&disk.sectorContent , ppd_state_getDiskStartAddress() + offset , SECTOR_SIZE ) == NULL){
        	//TODO: logear aca
        }

		disk.sectorNumber = sectorId;

		return disk;
	}


	void ppd_persistance_mapDisk(char * diskId){

		int32_t fd =  open(diskId , O_RDWR);
		if (fd == -1){
			puts("[ Error en open del disco al tratar de mapearlo. Finalizando aplicacion. ]");
			exit(EXIT_FAILURE);
		}

		char * map = mmap((caddr_t)0 , ppd_state_getVolumeSize() , PROT_WRITE , MAP_SHARED , fd , 0);
		if( map == MAP_FAILED ){
			puts("[ Error al mapear el disco. Finalizando aplicacion. ]");
			exit(EXIT_FAILURE);
		}

		if( posix_madvise(map , ppd_state_getVolumeSize() , POSIX_MADV_SEQUENTIAL) ){
			puts("[ Error en posix_madvise. Finalizando aplicacion. ]");
			exit(EXIT_FAILURE);
		}

		close(fd);

		ppd_state_setDiskStartAddress(map);
	}



	void ppd_persistance_unmapDisk(char * diskId ){

		int32_t fd =  open(diskId , O_RDONLY);
		if (fd == -1){
			puts("[ Error en open del disco al tratar de desmapearlo. Finalizando aplicacion. ]");
			exit(EXIT_FAILURE);
		}

		msync(ppd_state_getDiskStartAddress() , ppd_state_getVolumeSize() , MS_SYNC );

		if( munmap(ppd_state_getDiskStartAddress() , ppd_state_getVolumeSize() ) ){
			puts("[ Error al desmapear el disco. Finalizando aplicacion. ]");
			exit(EXIT_FAILURE);
		}

		close(fd);
	}


	void ppd_persistence_mapDevice(){
		ppd_persistance_mapDisk(ppd_conf_getDiskPath());

	}

	void ppd_persistence_unmapDevice(){
		ppd_persistance_unmapDisk( ppd_conf_getDiskPath() );
	}

