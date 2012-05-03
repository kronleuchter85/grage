/*
 * pfs-fat-utils.c
 *
 *  Created on: 04/11/2011
 *      Author: gonzalo-joaco
 */

#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <grage-commons.h>
#include "pfs-cache.h"

#include "pfs-connection-pool.h"
#include "pfs-endpoint.h"
#include "pfs-file-cache.h"
#include "pfs-fat32.h"


	Volume * pfs_fat_utils_loadVolume( BPB * b ){

		Volume * v = (Volume*) malloc(sizeof(Volume));

		v->bps = b->BPB_BytsPerSec;
		v->spc = b->BPB_SecPerClus;
		v->bpc = v->bps * v->spc;
		v->rsv = b->BPB_ResvdSecCnt;

		v->fatQty = b->BPB_NumFATs;
		v->fatSize = b->BPB_FATSz32;
		v->fatStartSector = b->BPB_ResvdSecCnt;

		v->root = b->BPB_RootClus;
		v->sectors = b->BPB_TotSec32;
		v->dataSectors = v->sectors - ( v->rsv + (v->fatQty * v->fatSize) );
		v->clusters = v->dataSectors / v->spc;
		v->fds = v->rsv + (v->fatQty * v->fatSize);

		printf("Clusters totales: %u.\n" , v->clusters);

		DiskSector sector = pfs_endpoint_callGetSector(b->BPB_FSInfo);
		memcpy( &(v->freeClusterCount) , sector.sectorContent + 488 , sizeof(uint32_t));
		memcpy( &(v->nextFreeCluster) , sector.sectorContent + 492 , sizeof(uint32_t));
		v->nextFreeCluster++; //No sé por qué, siempre atrasa uno!
		//En un disco en blanco, el nextFree es el 2. Si le creo un directorio adentro al dos,
		//y tengo que reservar un dir para su contendio, me devuelve el dos... error!

		pthread_mutex_init( &(v->fatLock) , NULL);

		if ( v->clusters < 65525 ) {
			return NULL;
		} else {
			return v;
		}

	}

	uint32_t pfs_fat32_utils_fetchChar(LongDirEntry *D, int8_t n) {
	    uint8_t i = (n % 13);

	    if ( i <= 4  ) return D->LDIR_Name1[i];
	    if ( i <= 10 ) return D->LDIR_Name2[i-5];
	    return D->LDIR_Name3[i-11];
	}

	int8_t pfs_fat32_utils_getNameLength(LongDirEntry * ldirentry){
		uint8_t i;
		uint16_t character;

	    for ( i=0 ; i<13 ; i++) {
	    	character = pfs_fat32_utils_fetchChar(ldirentry , i);
			if ( FAT_32_LFN_ISNULL(character) )
				return (i + 1);
	    }

	    return (i + 1);
	}

	void pfs_fat32_utils_extractName( LongDirEntry * d, uint16_t * dest, int8_t length) {
		int8_t i;

		for (i=0; i < (length - 1); i++) {
	    	dest[i] = pfs_fat32_utils_fetchChar(d , i);
	    }

	    dest[length - 1] = 0x00;

	    return;
	}

	void pfs_fat_utils_getFileName(LongDirEntry * l , char * utf8name){
		uint8_t nameLength = pfs_fat32_utils_getNameLength(l);

		uint16_t utf16name[13];
		pfs_fat32_utils_extractName(l , utf16name , nameLength);

		//char * utf8name = (char *)calloc(nameLength,sizeof(char));
		size_t utf8length = 0;

		unicode_utf16_to_utf8_inbuffer(utf16name , nameLength - 1 , utf8name , &utf8length);

		//return utf8name;
	}

	uint32_t pfs_fat_utils_getFatEntrySector(Volume * v , uint32_t cluster){
		uint32_t sectorNumberInFat = v->rsv + (cluster * 4 / v->bps);
		return sectorNumberInFat;
	}

	uint32_t pfs_fat_utils_getFatEntryOffset(Volume * v , uint32_t cluster){
		uint32_t fatEntryOffset = (cluster * 4) % v->bps;
		return fatEntryOffset;
	}



	uint32_t pfs_fat_utils_getFirstSectorOfCluster(Volume * v , uint32_t cluster){
		uint32_t firstSector = ((cluster - 2) * v->spc) + v->fds;
		return firstSector;
	}

	uint32_t pfs_fat_utils_getClusterBySector(Volume * v , uint32_t sector){
		uint32_t cluster = ((sector - v->fds) / v->spc) + 2;
		return cluster;
	}

	uint32_t pfs_fat32_utils_getNextClusterInChain(Volume * v , uint32_t clusterId){
		uint32_t nextCluster;

		uint32_t sector = pfs_fat_utils_getFatEntrySector(v, clusterId);
		uint32_t offset = pfs_fat_utils_getFatEntryOffset(v, clusterId);

		//DiskSector diskSector = pfs_endpoint_callGetSector(sector);
		DiskSector diskSector = pfs_endpoint_callCachedGetSector(sector);

		memcpy(&nextCluster, diskSector.sectorContent + offset, sizeof(uint32_t));

		return nextCluster;
	}


	uint32_t pfs_fat32_utils_getFirstSectorFromNextClusterInChain(Volume * v , uint32_t clusterId){

		uint32_t nextCluster = pfs_fat32_utils_getNextClusterInChain(v , clusterId);
		return pfs_fat_utils_getFirstSectorOfCluster(v , nextCluster);
	}

	Boolean pfs_fat32_utils_isLastSectorFromCluster(Volume * v , uint32_t sectorId){
		return ((v->bps * (1 + sectorId) % v->bpc) == 0);
	}

	uint8_t pfs_fat_setFirstClusterToDirEntry(DirEntry * d , uint32_t cluster) {
		if (d == NULL)
			return EXIT_FAILURE;

		char *src = (char *) &cluster;
		char *dst = (char *) &(d->DIR_FstClusLO);

		dst[0] = src[0];
		dst[1] = src[1];

		//if (V->FatType == FAT32) {
		dst = (char *) &(d->DIR_FstClusHI);
		dst[0] = src[2];
		dst[1] = src[3];

		return EXIT_SUCCESS;
	}

	uint32_t pfs_fat_getFirstClusterFromDirEntry(DirEntry * D) {
		if ( D == NULL )
			return EXIT_FAILURE;

		uint32_t val = 0;
		char *dst = (char *) &val;
		char *src = (char *) &(D->DIR_FstClusLO);

		dst[0] = src[0];
		dst[1] = src[1];

		src = (char *) &(D->DIR_FstClusHI);
		dst[2] = src[0];
		dst[3] = src[1];

		return val;
	}

	uint32_t pfs_fat32_utils_getDirEntryOffset(uint32_t sectorId , uint32_t os , uint32_t offset){
		return (sectorId - os) * 512 + offset;
	}

	void pfs_fat32_utils_getShortName(DirEntry * direntry , char * d_name){
		uint8_t i = 0;
		//char * d_name = (char *)malloc(12);

		while ( i < 8 ) {
			if ( direntry->DIR_Name[i] != 0x20 )
				memcpy(d_name + i, &(direntry->DIR_Name[i]) , sizeof(uint8_t));
			else
				break;
			i++;
		}

		if( direntry->DIR_Name[8] != 0x20 ){
			d_name[i] = 0x2E;
			i++;
			if(direntry->DIR_Name[0] != '.'){
				d_name[i] = direntry->DIR_Name[8];
				d_name[++i] = direntry->DIR_Name[9];
				d_name[++i] = direntry->DIR_Name[10];
				d_name[++i] = '\0';
			}
		}
		else d_name[i] = '\0';
		//return d_name;
	}

	void pfs_fat32_utils_toDirent(struct dirent * de , DirEntry  * direntry , LongDirEntry * ldirentry , Volume * v){

		if ( ldirentry != NULL ) {
			uint8_t length = pfs_fat32_utils_getNameLength(ldirentry);

			uint16_t utf16name[13];
			pfs_fat32_utils_extractName(ldirentry,utf16name,length);

			char * utf8name = (char *)calloc(length,sizeof(char));

			size_t utf8length = 0;
			unicode_utf16_to_utf8_inbuffer(utf16name , length - 1 , utf8name , &utf8length);

			strcpy(de->d_name,utf8name);
			free(utf8name);
		} else {
			uint8_t i = 0;

			while ( i < 8 ) {
				if ( direntry->DIR_Name[i] != 0x20 )
					memcpy(de->d_name + i, &(direntry->DIR_Name[i]) , sizeof(uint8_t));
				else
					break;
				i++;
			}

			if( direntry->DIR_Name[8] != 0x20 ){
				de->d_name[i] = 0x2E;
				i++;
				if(direntry->DIR_Name[0] != '.'){
					de->d_name[i] = direntry->DIR_Name[8];
					de->d_name[++i] = direntry->DIR_Name[9];
					de->d_name[++i] = direntry->DIR_Name[10];
					de->d_name[++i] = '\0';
				}
			}
			else de->d_name[i] = '\0';

/*
			memcpy(de->d_name , direntry->DIR_Name , sizeof(uint8_t));

			if( direntry->DIR_Name[1] == '.' ){
				memcpy(de->d_name + 1 , direntry->DIR_Name + 1 , sizeof(uint8_t));
				de->d_name[2] = '\0';
			} else {
				de->d_name[1] = '\0';
			}
*/
		}

		de->d_ino = pfs_fat_getFirstClusterFromDirEntry(direntry);

		if ( direntry->DIR_Attr == FAT_32_ATTR_DIRECTORY ){
			de->d_type = DT_DIR;
		}
		else
			de->d_type = DT_REG;
	}

	uint8_t pfs_fat32_isDirectoryEmpty(Volume * v, FatFile * fd){
		/*
		uint32_t sector = pfs_fat_utils_getFirstSectorOfCluster(v , fd->nextCluster);
		uint32_t offset = FAT_32_BLOCK_ENTRY_SIZE;
		uint32_t next = fd->nextCluster;
		DiskSector diskSector = pfs_endpoint_callCachedGetSector(sector);
		*/
		uint32_t offset = FAT_32_BLOCK_ENTRY_SIZE;
		Block block = pfs_fat32_utils_callGetBlock(fd->nextCluster, fd);
		LongDirEntry lDirEntry;

		/*
		memcpy(&lDirEntry, diskSector.sectorContent + offset, sizeof(LongDirEntry));

		while(lDirEntry.LDIR_Ord == FAT_32_FREEENT){
			offset += FAT_32_BLOCK_ENTRY_SIZE;
			memcpy(&lDirEntry, diskSector.sectorContent + offset, sizeof(LongDirEntry));

			if(offset == v->bps){
				if(pfs_fat32_utils_isLastSectorFromCluster(v , sector)){
					if(FAT_32_ISEOC(next)) return 1;
					sector = pfs_fat32_utils_getFirstSectorFromNextClusterInChain(v , next);
					diskSector = pfs_endpoint_callCachedGetSector(sector);
					next = pfs_fat32_utils_getNextClusterInChain(v , next);
				}
				else{
					diskSector = pfs_endpoint_callCachedGetSector(++sector);
				}
				offset = 0;
			}
		}
		*/
		memcpy(&lDirEntry, block.content + offset, sizeof(LongDirEntry));
		while(lDirEntry.LDIR_Ord == FAT_32_FREEENT){

			if(FAT_32_LDIR_ISLONG(lDirEntry.LDIR_Attr)) offset += FAT_32_BLOCK_ENTRY_SIZE;
			else offset += FAT_32_DIR_ENTRY_SIZE;

			if(offset >= v->bpc){
				block = pfs_fat32_utils_callGetBlock(pfs_fat32_utils_getNextClusterInChain(v, block.id), fd);
				offset = 0;
			}

			memcpy(&lDirEntry, block.content + offset, sizeof(LongDirEntry));

		}
		if(lDirEntry.LDIR_Ord == FAT_32_ENDOFDIR)
			return 1;
		else
			return 0;

	}

	time_t pfs_fat32_utils_processTime(int s, int m, int h, int d, int mo, int y) {
		struct tm t;
		memset((char *) &t, 0, sizeof(struct tm));

		t.tm_sec=s;         /* seconds */
	    t.tm_min=m;         /* minutes */
	    t.tm_hour=h;        /* hours */
	    t.tm_mday=d;        /* day of the month */
	    t.tm_mon=mo-1;         /* month */
	    t.tm_year=y;        /* year */

	 	return mktime(&t);
	}

	time_t pfs_fat32_utils_getTime(DirEntry *D) {
		int s=((((uint8_t *) &(D->DIR_WrtTime))[0] & 0x1f) * 2);
		int m=((((((uint8_t *) &(D->DIR_WrtTime))[1]&0x7) << 3) + (((uint8_t *) &(D->DIR_WrtTime))[0] >> 5)));
		int h=(((uint8_t *) &(D->DIR_WrtTime))[1] >> 3);
		int d=(((uint8_t *) &(D->DIR_WrtDate))[0] & 0x1f);
		int mo=((((((uint8_t *) &(D->DIR_WrtDate))[1]&0x1) << 3) + (((uint8_t *) &(D->DIR_WrtDate))[0] >> 5)));
		int y=(( ((uint8_t *) &(D->DIR_WrtDate))[1] >> 1) + 80);
	 	return pfs_fat32_utils_processTime(s,m,h,d,mo,y);
	}

	/*
	 *  pfs_fat32_utils_fillTime(uint16_t *Date, uint16_t *Time, time_t t);
	 *
	 * 	Uso:
	 *
	 * 	tim = time(NULL);
	 *  fat_fill_time(&(sDirEntry.DIR_CrtDate), &(sDirEntry.DIR_CrtTime), tim);
	 *  fat_fill_time(&(sDirEntry.DIR_WrtDate), &(sDirEntry.DIR_WrtTime), tim);
	 */
	uint8_t pfs_fat32_utils_fillTime(uint16_t * d , uint16_t * t , time_t actualTime) {
		struct tm time;
		uint16_t date=0;
		uint16_t tim=0;
		uint16_t bmask3=0x07FF;
		uint16_t bmask2=0x01FF;
		uint16_t bmask1=0x001F;

		localtime_r(&actualTime, &time);

		date = (uint16_t) time.tm_mday;
		date &= bmask1; // to set 0 first 11 bits;
		date |= ((uint16_t) time.tm_mon+1 ) << 5;
		date &= bmask2; // to set 0 first 6 bits;
		date |= (((uint16_t) ((time.tm_year + 1900) -1980)) << 9);

		tim = (uint16_t) (time.tm_sec / 2);
		tim &= bmask1;
		tim |= (((uint16_t) (time.tm_min)) << 5);
		tim &= bmask3;
		tim |= (((uint16_t) (time.tm_hour)) << 11);

		*d = date;
		*t = tim;
		return 0;
	}

	int8_t pfs_fat32_utils_seek(Volume * v , FatFile * f , off_t offset , uint32_t fsize){

		if ( offset > fsize || offset < 0 || f->shortEntry.DIR_Attr == FAT_32_ATTR_DIRECTORY ){
			return EXIT_FAILURE;
		}

		f->fileClusterNumber = offset / v->bpc;

		if ( (offset % v->bpc) != 0 )
			f->fileClusterNumber += 1;

		f->fileClusterOffset = offset % v->bpc;

		return EXIT_SUCCESS;
	}

	int8_t pfs_fat32_utils_seekWrite(Volume * v , FatFile * f , off_t offset , uint32_t fsize){

		if ( offset < 0 || f->shortEntry.DIR_Attr == FAT_32_ATTR_DIRECTORY ){
			return EXIT_FAILURE;
		}

		//Revisar en el caso que de 0
		f->fileClusterNumber = offset / v->bpc;
		if((offset % v->bpc) != 0)
			f->fileClusterNumber++;
		f->fileClusterOffset = offset % v->bpc;

		return EXIT_SUCCESS;
	}

	Block pfs_fat32_utils_getBlockFromNthClusterWrite(FatFile * f){
		Volume * v = pfs_state_getVolume();
		uint16_t clusterCount = 0;

		uint32_t c = pfs_fat_getFirstClusterFromDirEntry( &(f->shortEntry) );
		clusterCount++;
		while( clusterCount < f->fileClusterNumber ){
			c = pfs_fat32_utils_getNextClusterInChain(v,c);
			clusterCount++;
		}

		f->fileAbsoluteClusterNumberWrite = c;

		Block block = pfs_fat32_utils_callGetBlock(c, f);

		return block;
	}

	Block pfs_fat32_utils_getBlockFromNthClusterRead(FatFile * f){
		Volume * v = pfs_state_getVolume();
		uint16_t clusterCount = 0;

		uint32_t c = pfs_fat_getFirstClusterFromDirEntry( &(f->shortEntry) );
		clusterCount++;
		while( clusterCount < f->fileClusterNumber ){
			c = pfs_fat32_utils_getNextClusterInChain(v,c);
			clusterCount++;
		}

		f->fileAbsoluteClusterNumberRead = c;

		Block block = pfs_fat32_utils_callGetBlock(c , f);

		return block;
	}

	void pfs_fat32_utils_fileStat(Volume * v , FatFile * fatFile , struct stat * st) {
		memset((char *) st, 0, sizeof(struct stat));

		st->st_dev = (int)(v);
		st->st_nlink = 1;
		st->st_rdev = 0;
		st->st_blksize = v->bpc;

		if(fatFile->dirType == 0) {
			st->st_ino = v->root;
			st->st_mode = S_IFDIR | S_IRWXU | S_IRGRP | S_IROTH;
			st->st_size = 0;
			st->st_blocks=0;
			st->st_ctim.tv_sec = st->st_atim.tv_sec = st->st_mtim.tv_sec = 0;
		} else {
			st->st_ino = pfs_fat_getFirstClusterFromDirEntry(&(fatFile->shortEntry));

			if ( fatFile->shortEntry.DIR_Attr == FAT_32_ATTR_DIRECTORY ) {
				st->st_mode = S_IFDIR | S_IRWXU | S_IRGRP | S_IROTH;
			} else {
				st->st_mode = S_IFREG | S_IRWXU | S_IRGRP | S_IROTH;
			}
			st->st_size = fatFile->shortEntry.DIR_FileSize;
			if(st->st_size == 0) st->st_blocks = 0;
			else st->st_blocks = st->st_size / v->bps;
			if ( (st->st_size % v->bps) != 0 ) st->st_blocks++;
			st->st_ctim.tv_sec = st->st_atim.tv_sec = st->st_mtim.tv_sec = pfs_fat32_utils_getTime(&(fatFile->shortEntry));
		}
	}


	uint32_t pfs_fat32_utils_getNextFreeCluster(){
		Volume * v = pfs_state_getVolume();
		return v->nextFreeCluster;
	}

	uint8_t pfs_fat_utils_hasLFN(uint32_t offset, DiskSector diskSector){
		uint8_t attr;
		memcpy(&attr, diskSector.sectorContent + offset + 11, sizeof(attr));
		if(attr == FAT_32_ATTR_LONG_NAME) //Se suma 11 para verificar el Byte que define si es long o short entry
			return 1;
		else
			return 0;
	}

	void pfs_fat32_utils_setNextFreeCluster(uint32_t next){
		Volume * v = pfs_state_getVolume();
		v->nextFreeCluster = next;
	}

	void pfs_fat32_utils_getDirNameFromPath(const char *path, char *dest) {
		char *slash;
		strcpy(dest, path);
		slash = strrchr(dest, 0x2F); // 0x2F = "/"
		if (slash == &(dest[0]))
			dest[1] = 0;			 // root dir
		*slash  = 0;
	}

	void pfs_fat32_utils_getFileNameFromPath(const char *path, char *dest) {
		char *slash;
		slash = strrchr(path, 0x2F); // 0x2F = "/"
		slash++;
		strcpy(dest, slash);
	}

	/* Para nuevos directorios */
	uint32_t pfs_fat32_utils_assignCluster(Volume * v){
		uint32_t entry;
		uint8_t flag = 0; //Para verificar si dimos una vuelta entera a la fat

		pthread_mutex_lock( &(v->fatLock) );

		uint32_t currentFree = v->nextFreeCluster;
		uint32_t next = currentFree + 1;
		uint32_t sectorId = pfs_fat_utils_getFatEntrySector(v , next);
		uint32_t offset = pfs_fat_utils_getFatEntryOffset(v , next);

		DiskSector sector = pfs_endpoint_callCachedGetSector(sectorId);

		for( ; next < v->clusters; next++){
			memcpy(&entry , sector.sectorContent + offset , sizeof(uint32_t));
			offset += 4;

			if( FAT_32_ISFREE(entry) )
				break;

			if ( offset >= v->bps ){
				if( sector.sectorNumber < v->fatStartSector + v->fatSize ){
					if (flag == 1 && currentFree == entry) {
						return -EXIT_FAILURE; //No hay mas clusters disponibles... disco lleno!
					} else {
						offset = 0;
						sector.sectorNumber++;
					}
				} else {
					flag = 1;
					offset = 0;
					sector.sectorNumber = v->fatStartSector;
				}
				sector = pfs_endpoint_callCachedGetSector(sectorId);
			}
		}

		pfs_fat32_utils_markEndOfChain(v , currentFree);

		v->nextFreeCluster = next;

		pthread_mutex_unlock( &(v->fatLock) );

		v->freeClusterCount -= 1;

		return currentFree;
	}

	/* Para archivos o directorios que necesiten expandirse */
	uint32_t pfs_fat32_utils_allocateNewCluster(Volume * v , uint32_t currentLast){
		uint32_t entry;
		uint8_t flag = 0; //Para verificar si dimos una vuelta entera a la fat

		pthread_mutex_lock( &(v->fatLock) );

		uint32_t currentFree = v->nextFreeCluster;
		uint32_t next = currentFree + 1;
		uint32_t sectorId = pfs_fat_utils_getFatEntrySector(v , next);
		uint32_t offset = pfs_fat_utils_getFatEntryOffset(v , next);

		DiskSector sector = pfs_endpoint_callCachedGetSector(sectorId);

		for( ; next < v->clusters; next++){
			memcpy(&entry , sector.sectorContent + offset , sizeof(uint32_t));
			offset += 4;

			if( FAT_32_ISFREE(entry) )
				break;

			if ( offset >= v->bps ){
				if( sector.sectorNumber < v->fatStartSector + v->fatSize ){
					if (flag == 1 && currentFree == entry) {
						return -EXIT_FAILURE; //No hay mas clusters disponibles... disco lleno!
					} else {
						offset = 0;
						sector.sectorNumber++;
					}
				} else {
					flag = 1;
					offset = 0;
					sector.sectorNumber = v->fatStartSector;
				}
				sector = pfs_endpoint_callCachedGetSector(sectorId);
			}
		}

		pfs_fat32_utils_markEndOfChain(v , currentFree);

		pfs_fat32_utils_expandChain(v , currentLast , currentFree);

		v->nextFreeCluster = next;

		pthread_mutex_unlock( &(v->fatLock) );

		v->freeClusterCount -= 1;

		return currentFree;
	}

	void pfs_fat32_utils_markEndOfChain(Volume * v , uint32_t assignedCluser){
		uint32_t endOfChain = FAT_32_FAT_EOC;
		uint32_t sectorId = pfs_fat_utils_getFatEntrySector(v , assignedCluser);
		uint32_t offset = pfs_fat_utils_getFatEntryOffset(v , assignedCluser);
		DiskSector sector = pfs_endpoint_callCachedGetSector(sectorId);
		memcpy(sector.sectorContent + offset + 4, &endOfChain , sizeof(uint32_t));
		pfs_endpoint_callCachedPutSector(sector);
	}

	void pfs_fat32_utils_expandChain(Volume * v , uint32_t last , uint32_t free){
		uint32_t setChainToNewFreeCluster = free;

		uint32_t sectorId = pfs_fat_utils_getFatEntrySector(v , last);
		uint16_t   offset = pfs_fat_utils_getFatEntryOffset(v , last);
		DiskSector sector = pfs_endpoint_callCachedGetSector(sectorId);
		memcpy(sector.sectorContent + offset , &setChainToNewFreeCluster , sizeof(uint32_t));
		pfs_endpoint_callCachedPutSector(sector);
	}

	void pfs_fat32_utils_extendFileTruncate(Volume * v , FatFile * f , off_t newsize){

			uint32_t clusterCount;
			uint32_t lastCluster;
			uint32_t clusterAmountToReserve = newsize / v->bpc;
			if(newsize % v->bpc != 0) clusterAmountToReserve++;

			if(f->nextCluster == 0){

				lastCluster = pfs_fat32_utils_assignCluster(v);
				pfs_fat_setFirstClusterToDirEntry(&f->shortEntry , lastCluster);
				f->nextCluster = lastCluster;
				clusterCount = 1;
			}
			else{
				clusterCount = 0;
			}

			if(f->EOC == 0){ //El ECO aun no fue hallado. 0: valor por defecto asignado por el open
				lastCluster = pfs_fat32_utils_findEOC(v, f->nextCluster);
				f->EOC = lastCluster;
			}
			else{
				lastCluster = f->EOC;
			}

			for(; clusterCount < clusterAmountToReserve ; clusterCount++){
				lastCluster = pfs_fat32_utils_allocateNewCluster(v,lastCluster);
				f->EOC = lastCluster;
			}

			f->shortEntry.DIR_FileSize = newsize;
			Block block = pfs_fat32_utils_callGetBlock(f->source, f);
			uint32_t sectorOffset = f->sourceOffset;
			uint32_t blockId;

			if(FAT_32_LDIR_ISLONG(f->longEntry.LDIR_Attr)){
				if(sectorOffset + FAT_32_DIR_ENTRY_SIZE >= v->bpc){
					blockId = pfs_fat32_utils_getNextClusterInChain(v, f->source);
					sectorOffset = 0;
				}
				else{
					sectorOffset += FAT_32_DIR_ENTRY_SIZE;
					blockId = block.id;
				}
			}

			block = pfs_fat32_utils_callGetBlock(blockId , f);
			memcpy(block.content + sectorOffset, &(f->shortEntry), sizeof(DirEntry));
			pfs_fat32_utils_callPutBlock(block , f);
		}

	void pfs_fat32_utils_extendFileWrite(Volume * v , FatFile * f , off_t newsize){

		uint32_t clusterCount;
		uint32_t lastCluster;
		uint32_t clusterAmountToReserve = newsize / v->bpc;
		if(newsize % v->bpc != 0) clusterAmountToReserve++;

		if(f->nextCluster == 0){

			lastCluster = pfs_fat32_utils_assignCluster(v);
			pfs_fat_setFirstClusterToDirEntry(&f->shortEntry , lastCluster);
			f->nextCluster = lastCluster;
			clusterCount = 1;
		}
		else{
			clusterCount = 0;
		}

		if(f->EOC == 0){ //El ECO aun no fue hallado. 0: valor por defecto asignado por el open
			lastCluster = pfs_fat32_utils_findEOC(v, f->nextCluster);
			f->EOC = lastCluster;
		}
		else{
			lastCluster = f->EOC;
		}

		for(; clusterCount < clusterAmountToReserve ; clusterCount++){
			lastCluster = pfs_fat32_utils_allocateNewCluster(v,lastCluster);
			f->EOC = lastCluster;
		}

		f->shortEntry.DIR_FileSize += newsize;
		Block block = pfs_fat32_utils_callGetBlock(f->source, f);
		uint32_t sectorOffset = f->sourceOffset;
		uint32_t blockId;

		if(FAT_32_LDIR_ISLONG(f->longEntry.LDIR_Attr)){
			if(sectorOffset + FAT_32_DIR_ENTRY_SIZE >= v->bpc){
				blockId = pfs_fat32_utils_getNextClusterInChain(v, f->source);
				sectorOffset = 0;
			}
			else{
				sectorOffset += FAT_32_DIR_ENTRY_SIZE;
				blockId = block.id;
			}
		}

		block = pfs_fat32_utils_callGetBlock(blockId , f);
		memcpy(block.content + sectorOffset, &(f->shortEntry), sizeof(DirEntry));
		pfs_fat32_utils_callPutBlock(block , f);
	}


	uint32_t pfs_fat32_utils_findEOC(Volume * v, uint32_t firstCluster){

		uint32_t fatEntry;
		uint32_t aux;

		do{
			aux = firstCluster;
			uint32_t offset = pfs_fat_utils_getFatEntryOffset(v, firstCluster);
			uint32_t sector = pfs_fat_utils_getFatEntrySector(v, firstCluster);
			DiskSector diskSector = pfs_endpoint_callCachedGetSector(sector);
			memcpy(&fatEntry, diskSector.sectorContent + offset, sizeof(uint32_t));
			firstCluster = fatEntry;
		}while(!FAT_32_ISEOC(fatEntry));

		return aux;
	}
	void pfs_fat32_utils_loadLongEntryFilename(LongDirEntry * lde , char * utf8name){
		uint16_t utf16name[13];
		size_t utf16length = 0;
		uint8_t utf8length = strlen(utf8name);
		uint8_t i , j;

		// Carga:

		unicode_utf8_to_utf16_inbuffer(utf8name , utf8length , utf16name , &utf16length);

		for ( i = 0 ; i < utf8length ; i++){
		    j = (i % 13);

		    if ( j <= 4  ) lde->LDIR_Name1[j] = utf16name[i];
		    else if ( j <= 10 ) lde->LDIR_Name2[j-5] = utf16name[i];
		    else if ( j <= 12 ) lde->LDIR_Name3[j-11] = utf16name[i];
		}

		for( i = utf8length ; i < 13 ; i++ ){
			j = (i % 13);

		    if ( j <= 4  ) lde->LDIR_Name1[j] = 0;
		    else if ( j <= 10 ) lde->LDIR_Name2[j-5] = 0;
		    else if ( j <= 12 ) lde->LDIR_Name3[j-11] = 0;
		}
	}

	uint8_t pfs_fat32_utils_loadEntryFilename(DirEntry * de , char * utf8name){
		char utf8nameAux[14];
		strcpy(utf8nameAux, utf8name);
		char * name = strtok(utf8nameAux , ".");
		char * extension = strtok(NULL , ".");
		uint8_t i;

		//Falta hacer el chequeo de nombres (no puede comenzar con espacio, por ejemplo
		//Quizas no sea necesario segun pagina 28/29, titulo "Consideraciones"

		uint8_t length = strlen(name);
		if ( length == 8 ) {			//Cabe justo en el espacio

			for(i = 0 ; i < length ; i++ )
				de->DIR_Name[i] = toupper(utf8nameAux[i]);

		} else if (length < 8) {		//Rellenamos los espacios

			for(i = 0 ; i < length ; i++ )
				de->DIR_Name[i] = toupper(utf8nameAux[i]);
			for( i = length ; i < 8 ; i++ )
				de->DIR_Name[i] = 0x20;

		} else {						//Hay que crearle un nombre raro

			for(i = 0 ; i < 6 ; i++ )
				de->DIR_Name[i] = toupper(utf8nameAux[i]);
				de->DIR_Name[6] = '~';
				de->DIR_Name[7] = '1';
				//Hay que buscar si ya existe, y cambiar el 1 en caso afirmativo
				//pfs_fat32_utils_findEntryByShortEntry();
		}

		if ( extension == NULL ){
			for(i = 0 ; i < 3 ; i++ )
				de->DIR_Name[i+8] = 0x20;
		} else {
			for(i = 0 ; i < 3 ; i++ )
				de->DIR_Name[i+8] = toupper(extension[i]);
		}

		return EXIT_SUCCESS;
	}

	void pfs_fat32_utils_fillDotEntry(DirEntry * dot , DirEntry * actual){
		/* Fechas y horas */
		dot->DIR_CrtDate = actual->DIR_CrtDate;
		dot->DIR_CrtTime = actual->DIR_CrtTime;
		dot->DIR_WrtDate = actual->DIR_WrtDate;
		dot->DIR_WrtTime = actual->DIR_WrtTime;
		dot->DIR_LstAccDate = actual->DIR_LstAccDate;
		dot->DIR_CrtTimeTenth = actual->DIR_CrtTimeTenth;

		/* Comienzo de contenidos y atributos */
		dot->DIR_FstClusHI = actual->DIR_FstClusHI;
		dot->DIR_FstClusLO = actual->DIR_FstClusLO;
		dot->DIR_FileSize = actual->DIR_FileSize;
		dot->DIR_Attr = actual->DIR_Attr;

		/* Nombre */
		uint8_t i;
		dot->DIR_Name[0] = '.';
		for( i = 0 ; i < 10 ; i++ )
			dot->DIR_Name[i+1] = 0x20;
	}

	void pfs_fat32_utils_fillDotDotEntry(DirEntry * dotdot , DirEntry * parent){
		/* Fechas y horas */
		if (parent != NULL){
			dotdot->DIR_CrtDate = parent->DIR_CrtDate;
			dotdot->DIR_CrtTime = parent->DIR_CrtTime;
			dotdot->DIR_WrtDate = parent->DIR_WrtDate;
			dotdot->DIR_WrtTime = parent->DIR_WrtTime;
			dotdot->DIR_LstAccDate = parent->DIR_LstAccDate;
			dotdot->DIR_CrtTimeTenth = parent->DIR_CrtTimeTenth;

			/* Comienzo de contenidos y atributos */
			dotdot->DIR_FstClusHI = parent->DIR_FstClusHI;
			dotdot->DIR_FstClusLO = parent->DIR_FstClusLO;
			dotdot->DIR_FileSize = parent->DIR_FileSize;
			dotdot->DIR_Attr = parent->DIR_Attr;
		} else {
			dotdot->DIR_CrtDate = 0;
			dotdot->DIR_CrtTime = 0;
			dotdot->DIR_WrtDate = 0;
			dotdot->DIR_WrtTime = 0;
			dotdot->DIR_LstAccDate = 0;
			dotdot->DIR_CrtTimeTenth = 0;

			/* Comienzo de contenidos y atributos */
			dotdot->DIR_FstClusHI = 0;
			dotdot->DIR_FstClusLO = 0;
			dotdot->DIR_FileSize = 0;
			dotdot->DIR_Attr = 0x10;
		}

		/* Nombre */
		uint8_t i;
		dotdot->DIR_Name[0] = '.';
		dotdot->DIR_Name[1] = '.';
		for( i = 0 ; i < 9 ; i++ )
			dotdot->DIR_Name[i+2] = 0x20;
	}

	uint32_t pfs_fat_utils_BusyClustersQuantity(){
		Volume * v = pfs_state_getVolume();
		return v->clusters - v->freeClusterCount;
	}
	uint32_t pfs_fat_utils_FreeClustersQuantity(){
		Volume * v = pfs_state_getVolume();
		uint32_t sector = v->rsv;
		uint32_t clusterId = 2;
		uint32_t fatEntry;
		uint32_t offset;
		uint32_t result = 0;
		DiskSector diskSector = pfs_endpoint_callGetSector(sector);

		for(;clusterId <= v->clusters; sector++){
			offset = pfs_fat_utils_getFatEntryOffset(v, clusterId);
			if(offset == v->bps - FAT_32_FATENTRY_SIZE){
				diskSector = pfs_endpoint_callGetSector(++sector);
			}
			memcpy(&fatEntry, diskSector.sectorContent + offset, sizeof(uint32_t));
			if(fatEntry == FAT_32_FAT_FREE_ENTRY)
				result++;
			clusterId++;
		}
		return result;
	}

	void pfs_fat32_utils_updateFilesize(Volume * v , FatFile * f , uint32_t newsize){
		f->shortEntry.DIR_FileSize = newsize;
		//Block block = pfs_fat32_utils_callGetBlock(f->source, f);
		uint32_t clusterOffset = f->sourceOffset;
		uint32_t clusterId = f->source;

		if(FAT_32_LDIR_ISLONG(f->longEntry.LDIR_Attr)){
			if(clusterOffset + FAT_32_DIR_ENTRY_SIZE >= v->bpc){
				clusterId = pfs_fat32_utils_getNextClusterInChain(v, clusterId);
				clusterOffset = 0;
			}
			else{
				clusterOffset += FAT_32_DIR_ENTRY_SIZE;
			}
		}

		Block block = pfs_fat32_utils_callGetBlock(clusterId, f);
		memcpy(block.content + clusterOffset, &(f->shortEntry), sizeof(DirEntry));
		pfs_fat32_utils_callPutBlock(block , f);
	}

	uint32_t pfs_fat_utils_FATsizeKilobytes(){
		Volume * v = pfs_state_getVolume();
		return (v->fatSize*v->bps)/1024;
	}


	void pfs_fat32_utils_callPutBlock(Block block , FatFile * f){
		DiskSector diskSector;
		uint16_t offset = 0;

		Volume * v = pfs_state_getVolume();
		uint32_t firstSector = pfs_fat_utils_getFirstSectorOfCluster(v , block.id);
		uint32_t lastSector = firstSector + 8;

		if(pfs_cache_habilitada() && block.id != v->root){

			for(; firstSector < lastSector; firstSector++){
				diskSector.sectorNumber = firstSector;
				memcpy(diskSector.sectorContent, block.content, SECTOR_SIZE);
				offset += SECTOR_SIZE;
				pfs_endpoint_utils_putInFileCache(diskSector , f->cache);
			}
		}
		else{
			for( ; firstSector < lastSector ; firstSector++ ){
				diskSector.sectorNumber = firstSector;
				memcpy(diskSector.sectorContent , block.content + offset , SECTOR_SIZE);
				//pfs_endpoint_callCachedPutSector(diskSector);
				pfs_endpoint_callPutSector(diskSector);
				offset += SECTOR_SIZE;
			}
		}
	}

	Block pfs_fat32_utils_callGetBlock(uint32_t blockNumber , FatFile * f){
		Block block;
		Volume * v = pfs_state_getVolume();

		if(pfs_cache_habilitada() && blockNumber != v->root){
			Volume * v = pfs_state_getVolume();

			uint32_t firstSector = pfs_fat_utils_getFirstSectorOfCluster(v, blockNumber);
			uint32_t lastSector = firstSector + 8;
			uint32_t offset = 0;

			DiskSector diskSector;
			block.id = blockNumber;
			for(;firstSector < lastSector; firstSector++){
				diskSector = pfs_endpoint_callCachedGetFileSector(firstSector , f);
				memcpy(block.content + offset, diskSector.sectorContent, SECTOR_SIZE);
				offset += SECTOR_SIZE;
			}
		}
		else
			block = pfs_endpoint_blocks_callGetBlock(blockNumber, f);

		return block;
	}


	Block pfs_blocks_initializeBlock(uint32_t blockId){
		Block block;
		block.id = blockId;
		bzero(block.content , sizeof(block.content));
		return block;
	}

	Block pfs_endpoint_blocks_callGetBlock(uint32_t blockId , FatFile * file){

//		Block block = pfs_endpoint_utils_getFromFileCache(blockId , file);
//
//		if( block.id != 0 ){
//			return block;
//		}

		Block block = pfs_blocks_initializeBlock(blockId);

		PooledConnection * conns[8] = {
				pfs_pool_getConection(),
				pfs_pool_getConection(),
				pfs_pool_getConection(),
				pfs_pool_getConection(),
				pfs_pool_getConection(),
				pfs_pool_getConection(),
				pfs_pool_getConection(),
				pfs_pool_getConection()
		};

		DiskSector diskSectors[8];
		bzero(diskSectors , sizeof(diskSectors));

		uint32_t firstSectorId = pfs_fat_utils_getFirstSectorOfCluster(pfs_state_getVolume() , blockId);
		uint32_t index;
		for(index = 0 ; index < 8 ; index++){

			pfs_endpoint_blocks_callExecuteGetSector(conns[ index ]->listenSocket , firstSectorId + index);
		}


		for(index = 0; index < 8 ; index++){

			diskSectors[index] = pfs_endpoint_blocks_callReturnGetSector(conns[ index ]->listenSocket);
		}

		uint16_t offset = 0;
		for(index = 0 ; index < 8 ; index++){

			memcpy(block.content + offset, diskSectors[index].sectorContent ,
					sizeof(diskSectors[index].sectorContent ));

			offset += sizeof(diskSectors[index].sectorContent );
		}

		for(index = 0 ; index < 8 ; index++){
			pfs_pool_releaseConnection(conns[index]);
		}

		//pfs_endpoint_utils_putInFileCache(block , file);

		return block;
	}
