/*
 * pfs-console-cache.c
 *
 *  Created on: Dec 3, 2011
 *      Author: seba
 */
#include <time.h>
#include <sys/timeb.h>
#include <stdlib.h>
#include <stdio.h>
#include "linux-commons-list.h"
#include "grage-commons.h"
#include "linux-commons-file.h"
#include "linux-commons-strings.h"
#include "pfs-configuration.h"
#include "pfs-cache.h"
#include "pfs-state.h"
#include <string.h>

extern List openFiles;
	void pfs_cache_sectores_dump() {
		char linea[512];
		bzero(linea, 512);
		int i;

		uint32 sectorsMaxCount = pfs_cache_getFileCacheMaxCount();
		Iterator * fuckingIterator = commons_iterator_buildIterator(
				openFiles);
		time_t log_time;
		struct tm *log_tm;
		char str_time[128];
		FILE * cache_dump = commons_file_openOrCreateFile("cache_dump.txt");
		time(&log_time);
		log_tm = localtime(&log_time);
		strftime(str_time, 128, "%Y.%m.%d  %H:%M:%S", log_tm);
		sprintf(linea, "%s", str_time);
		//fwrite(linea , sizeof(char) , sizeof(linea) , cache_dump);
		commons_file_insertLine(linea, cache_dump);
		sprintf(linea, "Tamanio de Bloque de Cache: %d kb ",
				pfs_configuration_getCacheSize());
		//fwrite(linea , sizeof(char) , sizeof(linea) , cache_dump);
		commons_file_insertLine(linea, cache_dump);
		i = 0;
		//if (pfs_configuration_getCacheSize()!=NULL)

		sprintf(linea, "Cantidad de bloques de Cache: %d ",
				commons_list_getSize(openFiles));
		//fwrite(linea , sizeof(char) , sizeof(linea) , cache_dump);
		commons_file_insertLine(linea, cache_dump);
		i = 0;
		while (commons_iterator_hasMoreElements(fuckingIterator)) {
			sprintf(linea, "Contenido de Bloque de Cache %d:", i);
			//fwrite(linea , sizeof(char) , sizeof(linea) , cache_dump);
			commons_file_insertLine(linea, cache_dump);
			commons_file_insertLine(
					((CacheSectorRecord *) commons_iterator_next(fuckingIterator))->sector.sectorContent,
					cache_dump);
			//fwrite(((CacheSectorRecord *)commons_iterator_next(fuckingIterator))->sector.sectorContent ,
			//			sizeof(char) ,
			//			sizeof(((CacheSectorRecord *)commons_iterator_next(fuckingIterator))->sector.sectorContent) ,
			//			cache_dump);
			i++;
		}

		commons_file_closeFile(cache_dump);
	}
