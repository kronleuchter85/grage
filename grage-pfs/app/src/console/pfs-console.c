/*
 * pfs-console.c
 *
 *  Created on: 15/09/2011
 *      Author: Fernando
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#include <linux-commons.h>
#include <grage-commons.h>
#include "pfs-console-utils.h"
#include "pfs-cache.h"
#include "pfs-state.h"
#include "pfs-fat32.h"

	pthread_t consoleThread;

	void pfs_console_fsinfo(){
		Volume * v = pfs_state_getVolume();

		printf("Cantidad de Clusters ocupados: %d \n",pfs_fat_utils_BusyClustersQuantity());
		printf("Cantidad de Clusters libres: %d\n",v->freeClusterCount);
		printf("Tamaño de un Sector: %db \n",v->bps);
		printf("Tamaño de un Cluster: %db \n",v->bpc);
		printf("Tamaño de la FAT: %d kb \n", pfs_fat_utils_FATsizeKilobytes());
	}

	void unmountVolume(void){
		char command[128];
		sprintf(command , "fusermount -u %s" , pfs_state_getMountPath());
		puts(command);
		system(command);
	}

	void pfs_console_finfo(String parameter){
		FatFile * fatFile = pfs_fat32_open(parameter);
		Volume * v = pfs_state_getVolume();
		uint32_t currentCluster = fatFile->nextCluster;

		int i = 0;
		int cantidadClusters;
		if (fatFile->shortEntry.DIR_FileSize%v->bpc == 0){
			cantidadClusters = fatFile->shortEntry.DIR_FileSize/v->bpc;
		}else{
			cantidadClusters = (fatFile->shortEntry.DIR_FileSize/v->bpc)+1;
		}
		printf("#Clusters: \n");
		while (i<(cantidadClusters) && i<20){
			printf("%d ",currentCluster);
			currentCluster=pfs_fat32_utils_getNextClusterInChain(v,currentCluster);
			i++;
		}
		printf("\n");
	}

	void pfs_console_thread(){
		char buffer[250];
		String cmd, parameter;

		sleep(1);

		puts("[ Inicializando consola ]");

		while(TRUE){
			bzero(buffer , 250);
			//!feof(stdin)

			printf("> ");

			if (signal(SIGINT, pfs_cache_sectores_dump) == SIG_ERR){
				perror ("No se puede cambiar signal");
			}

			fgets(buffer, 250, stdin);

			//printf(">%s\n", buffer);
			cmd = pfs_console_utils_parseCMD(buffer);

			if(!strcmp(cmd,"exit")) {unmountVolume(); exit(1);}
			if(!strcmp(cmd, "fsinfo")) pfs_console_fsinfo();
			if(!strcmp(cmd, "finfo")){
				parameter = pfs_console_utils_get_cmd_parameter(buffer, strlen(cmd));
				pfs_console_finfo(parameter);
			}
			if(strcmp(cmd, "fsinfo") && strcmp(cmd, "finfo")) puts("Comando incorrecto");
		}
	}

	int pfs_console_initialize(){
		//pthread_create(&consoleThread, NULL, pfs_console_thread, NULL);

		pthread_create(&consoleThread , NULL , pfs_console_thread , NULL);
		//pfs_console_thread();
	}

