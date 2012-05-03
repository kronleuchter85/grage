/*
 * pfs-fat32.c
 *
 *  Created on: 04/11/2011
 *      Author: gonzalo-joaco
 *				Garcas! Me dejaron afuera!
 *					Sos un lloron niño!!
 */

#include <stdlib.h>

#include "pfs-cache.h"
#include "pfs-fat32.h"
#include "pfs-state.h"

	FatFile * pfs_fat32_utils_openRootDirectory(Volume * v) {
		FatFile * fatFile = (FatFile *)calloc(1,sizeof(FatFile));

		if (pfs_cache_habilitada())
			fatFile->cache = pfs_cache_sectors_initialize();
		else
			fatFile->cache = NULL;

		uint32_t cluster = pfs_fat32_utils_getNextClusterInChain(v , v->root);

		fatFile->source = v->root;
		fatFile->sourceOffset = 0;

		if (FAT_32_ISEOC(cluster))
			fatFile->nextCluster = 0;
		else
			fatFile->nextCluster = cluster;

		fatFile->currentSector = v->fds;
		fatFile->dirEntryOffset = 0;
		fatFile->dirType = 0;

		pfs_state_addOpenFile(fatFile);

		return fatFile;
	}



	FatFile * pfs_fat32_utils_openNonRootDirectory(const char * path , Volume * v ){

		uint32_t next = v->root;
		FatFile * fatFile = (FatFile *)calloc(1,sizeof(FatFile));
		LongDirEntry longEntry;
		DirEntry sDirEntry;
		uint32_t offset;
		uint32_t blockNumber;
		char utf8name[14];

		if (pfs_cache_habilitada())
			fatFile->cache = pfs_cache_sectors_initialize();
		else
			fatFile->cache = NULL;


		Block block;
		block = pfs_fat32_utils_callGetBlock(v->root, fatFile);

		List directories = commons_list_tokenize((char *)path, '/');
		Iterator * ite = commons_iterator_buildIterator(directories);

		while (commons_iterator_hasMoreElements(ite)) {

			offset = 0;

			char * token = commons_iterator_next(ite);

			do{
				if (offset < v->bpc) {
					memcpy(&longEntry, block.content + offset, FAT_32_DIR_ENTRY_SIZE);
					if(FAT_32_LDIR_ISLONG(longEntry.LDIR_Attr)){
						pfs_fat_utils_getFileName(&longEntry , utf8name);
						offset += FAT_32_BLOCK_ENTRY_SIZE;
					}
					else{
						memcpy(&sDirEntry, block.content + offset, FAT_32_DIR_ENTRY_SIZE);
						pfs_fat32_utils_getShortName(&sDirEntry , utf8name);
						offset += FAT_32_DIR_ENTRY_SIZE;
					}
				} else if (offset >= v->bpc) {
					blockNumber = pfs_fat32_utils_getNextClusterInChain(v, block.id);
					block = pfs_fat32_utils_callGetBlock(blockNumber, fatFile);
					offset = 0;
				}

			}while (longEntry.LDIR_Ord != FAT_32_ENDOFDIR && !commons_string_equals(token, utf8name));

			if (longEntry.LDIR_Ord == FAT_32_ENDOFDIR) {
				return NULL;
			} else if (commons_string_equals(utf8name, token)) {

				if (offset >= v->bpc) {
					Block auxBlock;
					blockNumber = pfs_fat32_utils_getNextClusterInChain(v, block.id);
					auxBlock = pfs_fat32_utils_callGetBlock(blockNumber, fatFile);

					memcpy(&sDirEntry, auxBlock.content, FAT_32_DIR_ENTRY_SIZE);
					offset -= FAT_32_BLOCK_ENTRY_SIZE;
				}
				else{
					if(FAT_32_LDIR_ISLONG(longEntry.LDIR_Attr)){
						offset -= FAT_32_DIR_ENTRY_SIZE;
						memcpy(&sDirEntry, block.content + offset, FAT_32_DIR_ENTRY_SIZE);
						offset -= FAT_32_DIR_ENTRY_SIZE; //Esto es para el calculo del dirEntryOffset
					}
				}

				fatFile->longEntry = longEntry;
				fatFile->shortEntry = sDirEntry;

				fatFile->source = next;
				next = pfs_fat_getFirstClusterFromDirEntry(&sDirEntry);

				if ( commons_iterator_hasMoreElements(ite) ){
					block = pfs_fat32_utils_callGetBlock(next, fatFile);
				}
			}
		}

		fatFile->sourceOffset = offset;
		fatFile->nextCluster = pfs_fat_getFirstClusterFromDirEntry(&sDirEntry);
		fatFile->dirEntryOffset = 0;
		fatFile->dirType = 1;
		fatFile->EOC = 0;

		if(!FAT_32_LDIR_ISLONG(fatFile->longEntry.LDIR_Attr)){
			fatFile->sourceOffset -= FAT_32_DIR_ENTRY_SIZE;
		}

		pfs_state_addOpenFile(fatFile);

		return fatFile;
	}





	FatFile * pfs_fat32_open(const char * path) {

		Volume * v = pfs_state_getVolume();

		if (commons_string_equals(FAT_32_ROOT_FORWARD_SLASH, (char *)path) || path[1] == 0) {
			return pfs_fat32_utils_openRootDirectory(v);
		} else {
			return pfs_fat32_utils_openNonRootDirectory(path , v);
		}
	}




	int8_t pfs_fat32_readDirectory( struct dirent * direntry , FatFile * file , Volume * volume){
		LongDirEntry lfnentry;
		DirEntry  sfnentry;
		Block block;


		if ( file->dirType == 0) {
			if( file->dirEntryOffset >= volume->bpc ){
				file->nextCluster = pfs_fat32_utils_getNextClusterInChain(volume , file->nextCluster);
				file->dirEntryOffset = 0;
				block = pfs_fat32_utils_callGetBlock(file->nextCluster, file);
			} else {
				block = pfs_fat32_utils_callGetBlock(file->source, file);
			}
		} else {
			if( file->dirEntryOffset >= volume->bpc ){
				file->nextCluster = pfs_fat32_utils_getNextClusterInChain(volume , file->nextCluster);
				file->dirEntryOffset = 0;
				block = pfs_fat32_utils_callGetBlock(file->nextCluster, file);
			} else {
				block = pfs_fat32_utils_callGetBlock(file->nextCluster, file);
			}
		}

		do {
			memcpy(&lfnentry , block.content + file->dirEntryOffset , sizeof(LongDirEntry));
			file->dirEntryOffset += 32;

			if( file->dirEntryOffset >= volume->bpc ){
				if ( FAT_32_ISEOC(file->nextCluster) ){
					return EXIT_FAILURE;
				} else {
					file->nextCluster = pfs_fat32_utils_getNextClusterInChain(volume, file->nextCluster);
					file->dirEntryOffset = 0;
				}
			}

		} while ( lfnentry.LDIR_Ord == FAT_32_FREEENT );

		if( FAT_32_LDIR_ISLONG(lfnentry.LDIR_Attr) ){
			memcpy(&sfnentry , block.content + file->dirEntryOffset  , sizeof(DirEntry));
			file->dirEntryOffset += 32;

			pfs_fat32_utils_toDirent(direntry , &sfnentry , &lfnentry, volume);
			return EXIT_SUCCESS;

		} else if ( lfnentry.LDIR_Ord == FAT_32_ENDOFDIR ){

			return EXIT_FAILURE;

		} else { //La entrada es solo DirEntry ( . o ..)
			memcpy(&sfnentry , block.content + file->dirEntryOffset - 32 , sizeof(DirEntry));
			pfs_fat32_utils_toDirent(direntry , &sfnentry , NULL , volume);
			return EXIT_SUCCESS;

		}

		return EXIT_FAILURE;
	}


	void pfs_fat32_unlink(Volume * v , FatFile * fd){

		Block block = pfs_fat32_utils_callGetBlock(fd->source, fd);

		//Borrado de DirEntry
		pfs_fat32_unlink_dirEntry(v , fd , block);

		//Borrado de la chain en la FAT
		uint8_t result;
		do {
			result = pfs_fat32_unlink_FatEntryChain(v , fd);
		} while( result );

	}


	void pfs_fat32_rmdir(Volume * v , FatFile * fd){

		Block block = pfs_fat32_utils_callGetBlock(fd->source, fd);

		//Borrado de DirEntry
		pfs_fat32_unlink_dirEntry(v , fd , block);

		//Borrado de la chain en la FAT
		pfs_fat32_unlink_FatEntryChain(v , fd);
	}

	void pfs_fat32_unlink_dirEntry(Volume * v , FatFile * fd , Block block){
		DirEntry sDirEntry;
		LongDirEntry lDirEntry , auxLDirEntry;
		uint32_t next = fd->source;
		Block  auxBlock;
		uint8_t twoWritesFlag = 0;

		uint32_t clusterOffset = fd->sourceOffset;
		memcpy(&lDirEntry, block.content + clusterOffset, sizeof(LongDirEntry));
		if(FAT_32_LDIR_ISLONG(lDirEntry.LDIR_Attr)){
			if((clusterOffset += FAT_32_DIR_ENTRY_SIZE) < v->bpc){
				memcpy(&sDirEntry, block.content + clusterOffset, sizeof(DirEntry));
				if((clusterOffset += FAT_32_DIR_ENTRY_SIZE) < v->bpc){
					memcpy(&auxLDirEntry, block.content + clusterOffset, sizeof(LongDirEntry));
				}
				else{
					uint32_t clusterId = pfs_fat32_utils_getNextClusterInChain(v, block.id);
					auxBlock = pfs_fat32_utils_callGetBlock(clusterId , fd);
					memcpy(&auxLDirEntry, auxBlock.content, sizeof(LongDirEntry));
				}
			}
			else{
				twoWritesFlag = 1;
				uint32_t clusterId = pfs_fat32_utils_getNextClusterInChain(v, block.id);
				auxBlock = pfs_fat32_utils_callGetBlock(clusterId , fd);

				memcpy(&sDirEntry, auxBlock.content, sizeof(DirEntry));
				memcpy(&auxLDirEntry, auxBlock.content + FAT_32_DIR_ENTRY_SIZE, sizeof(LongDirEntry));
			}
		}
		else{
			memcpy(&sDirEntry, block.content + clusterOffset, sizeof(DirEntry));
			if((clusterOffset += FAT_32_DIR_ENTRY_SIZE) < v->bps){
				memcpy(&auxLDirEntry, block.content + clusterOffset, sizeof(LongDirEntry));
			}
			else{
				uint32_t clusterId = pfs_fat32_utils_getNextClusterInChain(v, block.id);
				auxBlock = pfs_fat32_utils_callGetBlock(clusterId , fd);
				memcpy(&auxLDirEntry, auxBlock.content, sizeof(LongDirEntry));
			}
		}
		if(twoWritesFlag){
			if(auxLDirEntry.LDIR_Ord != 0x00){
				uint8_t phantomValue = FAT_32_FREEENT;
				memcpy(block.content + v->bpc - FAT_32_DIR_ENTRY_SIZE, &phantomValue, sizeof(LongDirEntry));
				memcpy(auxBlock.content, &phantomValue, sizeof(DirEntry));
			}
			else{
				uint8_t phantomValue = FAT_32_ENDOFDIR;
				memcpy(block.content + v->bps - FAT_32_DIR_ENTRY_SIZE, &phantomValue, sizeof(LongDirEntry));
				memcpy(auxBlock.content, &phantomValue, sizeof(DirEntry));

			}
			pfs_fat32_utils_callPutBlock(block , NULL);
			pfs_fat32_utils_callPutBlock(auxBlock , NULL);;
		}
		else{
			clusterOffset = fd->sourceOffset;

			if(auxLDirEntry.LDIR_Ord != 0x00){
				uint8_t phantomValue = FAT_32_FREEENT;
				if(FAT_32_LDIR_ISLONG(lDirEntry.LDIR_Attr)){
					memcpy(block.content + clusterOffset, &phantomValue, sizeof(LongDirEntry));
					memcpy(block.content + clusterOffset + FAT_32_DIR_ENTRY_SIZE, &phantomValue, sizeof(DirEntry));
				}
				else{
					memcpy(block.content + clusterOffset, &phantomValue, sizeof(DirEntry));
				}
			}
			else{
				uint8_t phantomValue = FAT_32_ENDOFDIR;
				if(FAT_32_LDIR_ISLONG(lDirEntry.LDIR_Attr)){
					memcpy(block.content + clusterOffset, &phantomValue, sizeof(LongDirEntry));
					memcpy(block.content + clusterOffset + FAT_32_DIR_ENTRY_SIZE, &phantomValue, sizeof(DirEntry));
				}
				else{
					memcpy(block.content + clusterOffset, &phantomValue, sizeof(DirEntry));
				}
			}
			pfs_fat32_utils_callPutBlock(block , NULL);
		}
	}


	int8_t pfs_fat32_unlink_FatEntryChain(Volume * v , FatFile * fd){
		uint32_t nextCluster , offset , EntryValue , sector;
		uint32_t phantomValue = FAT_32_FAT_FREE_ENTRY;

		nextCluster = pfs_fat32_utils_getNextClusterInChain(v ,fd->nextCluster);
		sector = pfs_fat_utils_getFatEntrySector(v, fd->nextCluster);
		DiskSector diskSector = pfs_endpoint_callCachedGetSector(sector);
		offset = pfs_fat_utils_getFatEntryOffset(v, fd->nextCluster);

		memcpy(&EntryValue, diskSector.sectorContent + offset, sizeof(uint32_t));
		memcpy(diskSector.sectorContent + offset, &phantomValue, sizeof(uint32_t));
		pfs_endpoint_callCachedPutSector(diskSector);

		v->freeClusterCount++;

		if( FAT_32_ISEOC(EntryValue) )
			return 0;
		else{
			fd->nextCluster = nextCluster;
			return 1;
		}
	}

	uint32_t pfs_fat32_write(Volume * v , FatFile * f , const char * buf , size_t size){

		uint32_t bytesWritten = 0 ;
		uint32_t bytesLeft = size;
		uint32_t bytesToWrite;
		Block block;
		uint32_t current;

		if (f->fileAbsoluteClusterNumberWrite == 0){
			block = pfs_fat32_utils_getBlockFromNthClusterWrite(f);
		}
		else{
			current = f->fileAbsoluteClusterNumberWrite;
			current = pfs_fat32_utils_getNextClusterInChain(v,current);
			block = pfs_fat32_utils_callGetBlock(current, f);
		}

		if( size + f->fileClusterOffset <= v->bpc ){
			memcpy(block.content + f->fileClusterOffset , buf , size);
			pfs_fat32_utils_callPutBlock(block , f);
			return bytesWritten += size;
		} else {
			memcpy(block.content + f->fileClusterOffset , buf , v->bpc - f->fileClusterOffset);
			bytesWritten = v->bpc - f->fileClusterOffset;
			bytesLeft -= bytesWritten;
		}

		pfs_fat32_utils_callPutBlock(block , f);

		//current = f->fileAbsoluteClusterNumberWrite;

		while( bytesWritten < size ){

			if ( (bytesLeft <= v->bpc) || FAT_32_ISEOC(current) ){
				bytesToWrite = bytesLeft;
			} else {
				bytesToWrite = v->bpc;
			}

			memcpy(block.content , buf + bytesWritten , bytesToWrite);
			bytesWritten += bytesToWrite;
			bytesLeft -= bytesToWrite;
			pfs_fat32_utils_callPutBlock(block , f);

			if(size - bytesWritten > 0){
				current = pfs_fat32_utils_getNextClusterInChain(v, block.id);
				block = pfs_fat32_utils_callGetBlock(current , f);
			}
		}

		return bytesWritten;
	}

	uint32_t pfs_fat32_read(Volume * v , FatFile * f , char * buf , size_t size){
		uint32_t bytesRead = 0;
		uint32_t bytesLeft = size;
		uint32_t bytesToRead , current;
		Block block;

		if (f->fileAbsoluteClusterNumberRead == 0){
			block = pfs_fat32_utils_getBlockFromNthClusterRead(f);
			current = block.id;
		} else {
			current = f->fileAbsoluteClusterNumberRead;
			current = pfs_fat32_utils_getNextClusterInChain(v , current);
			if ( ! FAT_32_ISEOC(current) ){
				block = pfs_fat32_utils_callGetBlock(current , f);
			}
		}

		if ( f->fileClusterOffset + size <= v->bpc ){	// Lo que se pide para leer + el offset caen dentro de un cluster
			memcpy(buf , block.content + f->fileClusterOffset , size);
			return bytesRead += size;
		} else {										// El contenido de un cluster no alcanza; se lee lo necesario y se sigue
			memcpy(buf , block.content + f->fileClusterOffset , v->bpc - f->fileClusterOffset);
			bytesRead += v->bpc - f->fileClusterOffset;
			bytesLeft -= bytesRead;
		}

		current = f->fileAbsoluteClusterNumberRead;

		while( bytesRead < size ){

			if (FAT_32_ISEOC(current)){
				return bytesRead;
			} else {
				current = pfs_fat32_utils_getNextClusterInChain(v , current);
				block = pfs_fat32_utils_callGetBlock(current , f);
			}

			if ( (bytesLeft <= v->bpc)){
				bytesToRead = bytesLeft;
			} else {
				bytesToRead = v->bpc;
			}

			memcpy( buf + bytesRead , block.content , bytesToRead);
			bytesRead += bytesToRead;
			bytesLeft -= bytesToRead;
		}

		f->fileAbsoluteClusterNumberRead = current;

		return bytesRead;
	}

	int8_t pfs_fat32_mknod(Volume * v , FatFile * destination , char * filename){
		DirEntry shortEntry;
		LongDirEntry longEntry , auxEntry;
		uint32_t blockNumber;
		uint16_t offset = 0;
		uint8_t freeCount = 0;

		/* Completamos los campos del nombre */
		longEntry.LDIR_Ord = 0x41; 						//Ultimo (y unico) long entry
		pfs_fat32_utils_loadLongEntryFilename(&longEntry , filename);
		pfs_fat32_utils_loadEntryFilename(&shortEntry , filename);

		/* Completamos los campos de fecha y hora */
		time_t tim = time(NULL);
		pfs_fat32_utils_fillTime(&(shortEntry.DIR_WrtDate), &(shortEntry.DIR_WrtTime), tim);
		pfs_fat32_utils_fillTime(&(shortEntry.DIR_CrtDate), &(shortEntry.DIR_CrtTime), tim);
		shortEntry.DIR_LstAccDate = shortEntry.DIR_CrtDate;
		shortEntry.DIR_CrtTimeTenth = 0;

		/* Completamos el resto de los atributos */
		shortEntry.DIR_Attr = 0; 								 //Archivo comun y silvestre
		longEntry.LDIR_Attr = FAT_32_ATTR_LONG_NAME;			 //Archivo comun y silvestre, es long entry
		shortEntry.DIR_FileSize = 0; 							 //Por ahora esta vacio
		shortEntry.DIR_FstClusHI = shortEntry.DIR_FstClusLO = 0; //Como esta vacio, no tiene contenido

		/* Buscamos 64 bytes contiguos libres dentro del cluster
		 * 32 para el LongDirEntry y 32 para el DirEntry
		 */

		//DiskSector sector = pfs_endpoint_callCachedGetSector(destination->currentSector , destination);
		if (destination->dirType == 0)
			blockNumber = destination->source;
		else
			blockNumber = pfs_fat_getFirstClusterFromDirEntry(&(destination->shortEntry));

		Block block = pfs_fat32_utils_callGetBlock(blockNumber , destination);

		while( freeCount < 2 ) {
			do {
				memcpy(&auxEntry , block.content + offset , FAT_32_DIR_ENTRY_SIZE);
				offset += 32;

				if ( FAT_32_DIRENT_ISFREE(auxEntry.LDIR_Ord) ){
					freeCount++;
					break;
				}

				if( offset >= v->bpc ){
					blockNumber = pfs_fat32_utils_getNextClusterInChain(v, blockNumber);
					if ( FAT_32_ISEOC(blockNumber) ){
						//Hay que agregar un cluster nuevo!!!
					} else {
						block = pfs_fat32_utils_callGetBlock(blockNumber , destination);
						offset = 0;
					}
				}

			} while ( ! FAT_32_DIRENT_ISFREE(auxEntry.LDIR_Ord) ); //Sale cuando encontro 32 bytes libres

			memcpy(&auxEntry , block.content + offset , FAT_32_DIR_ENTRY_SIZE); //Se buscan los siguientes 32 bytes libres
			offset += 32;

			if ( FAT_32_DIRENT_ISFREE(auxEntry.LDIR_Ord) )
				freeCount++; //Listo, tenemos 64 bytes contiguos libres
			else
				freeCount = 0; // Solo se encontraron 32 bytes libres - hay que empezar de nuevo
		}

		/* Escribimos los entries en el cluster y lo mandamos a escribir en el disco */

		uint16_t longOffset = offset - FAT_32_BLOCK_ENTRY_SIZE;
		uint16_t shortOffset = offset - FAT_32_DIR_ENTRY_SIZE;

		memcpy(block.content + longOffset , &longEntry , FAT_32_DIR_ENTRY_SIZE);
		memcpy(block.content + shortOffset , &shortEntry , FAT_32_DIR_ENTRY_SIZE);

		pfs_fat32_utils_callPutBlock(block , NULL);

		return EXIT_SUCCESS;
	}

	int8_t pfs_fat32_mkdir(Volume * v , FatFile * destination , char * filename){
		DirEntry shortEntry;
		LongDirEntry longEntry , auxEntry;
		uint32_t blockNumber;
		uint16_t offset = 0;
		uint8_t freeCount = 0;

		/* Completamos los campos del nombre */
		longEntry.LDIR_Ord = 0x41; 						//Ultimo (y unico) long entry
		pfs_fat32_utils_loadLongEntryFilename(&longEntry , filename);
		pfs_fat32_utils_loadEntryFilename(&shortEntry , filename);


		/* Completamos los campos de fecha y hora */
		time_t tim = time(NULL);
		pfs_fat32_utils_fillTime(&(shortEntry.DIR_WrtDate), &(shortEntry.DIR_WrtTime), tim);
		pfs_fat32_utils_fillTime(&(shortEntry.DIR_CrtDate), &(shortEntry.DIR_CrtTime), tim);
		shortEntry.DIR_LstAccDate = shortEntry.DIR_CrtDate;
		shortEntry.DIR_CrtTimeTenth = 0;


		/* Completamos el resto de los atributos */
		uint32_t newCluster = pfs_fat32_utils_assignCluster(v);
		pfs_fat_setFirstClusterToDirEntry(&shortEntry , newCluster);	//Seteamos LO + HI
		shortEntry.DIR_Attr = FAT_32_ATTR_DIRECTORY; 			 		//Es un directorio
		longEntry.LDIR_Attr = FAT_32_ATTR_LONG_NAME;					//Es un directorio, es long entry
		shortEntry.DIR_FileSize = 0; 							 		//Los directorios siempre estan en 0


		/* Buscamos 64 bytes contiguos libres dentro del cluster
		 * 32 para el LongDirEntry y 32 para el DirEntry
		 */

		//DiskSector sector = pfs_endpoint_callCachedGetSector(destination->currentSector , destination);
		if (destination->dirType == 0)
			blockNumber = destination->source;
		else
			blockNumber = pfs_fat_getFirstClusterFromDirEntry(&(destination->shortEntry));

		Block block = pfs_fat32_utils_callGetBlock(blockNumber , destination);

		while( freeCount < 2 ) {
			do {
				memcpy(&auxEntry , block.content + offset , FAT_32_DIR_ENTRY_SIZE);
				offset += 32;

				if ( FAT_32_DIRENT_ISFREE(auxEntry.LDIR_Ord) ){
					freeCount++;
					break;
				}

				if( offset >= v->bpc ){
					blockNumber = pfs_fat32_utils_getNextClusterInChain(v, blockNumber);
					if ( FAT_32_ISEOC(blockNumber) ){
						//Hay que agregar un cluster nuevo!!!
					} else {
						block = pfs_fat32_utils_callGetBlock(blockNumber , destination);
						offset = 0;
					}
				}

			} while ( ! FAT_32_DIRENT_ISFREE(auxEntry.LDIR_Ord) ); //Sale cuando encontro 32 bytes libres

			memcpy(&auxEntry , block.content + offset , FAT_32_DIR_ENTRY_SIZE); //Se buscan los siguientes 32 bytes libres
			offset += 32;

			if ( FAT_32_DIRENT_ISFREE(auxEntry.LDIR_Ord) )
				freeCount++; //Listo, tenemos 64 bytes contiguos libres
			else
				freeCount = 0; // Solo se encontraron 32 bytes libres - hay que empezar de nuevo
		}

		/* Escribimos los entries en el cluster y lo mandamos a escribir en el disco */

		uint16_t longOffset = offset - FAT_32_BLOCK_ENTRY_SIZE;
		uint16_t shortOffset = offset - FAT_32_DIR_ENTRY_SIZE;

		memcpy(block.content + longOffset , &longEntry , FAT_32_DIR_ENTRY_SIZE);
		memcpy(block.content + shortOffset , &shortEntry , FAT_32_DIR_ENTRY_SIZE);

		pfs_fat32_utils_callPutBlock(block , destination);


		/* Por ultimo, creamos entradas . y .. */
		DirEntry punto , puntoPunto;
		block = pfs_fat32_utils_callGetBlock(newCluster , destination);

		pfs_fat32_utils_fillDotEntry(&punto , &shortEntry);
		memcpy(block.content ,  &punto , FAT_32_DIR_ENTRY_SIZE);

		if (destination->dirType == 0)
			pfs_fat32_utils_fillDotDotEntry(&puntoPunto , NULL);
		else
			pfs_fat32_utils_fillDotDotEntry(&puntoPunto , &(destination->shortEntry));
		memcpy( block.content + FAT_32_DIR_ENTRY_SIZE , &puntoPunto , FAT_32_DIR_ENTRY_SIZE);

		pfs_fat32_utils_callPutBlock(block , destination);

		return EXIT_SUCCESS;
	}


	uint8_t pfs_fat32_truncate(Volume * v, FatFile * f, off_t newsize){

		Block block;
		DirEntry sDirEntry;
		uint8_t setEndOfData = FAT_32_ENDOFDIR;
		uint32_t setFatEntryFree = FAT_32_FAT_FREE_ENTRY;
		uint32_t clusterCount = 0;
		uint32_t currentCluster = f->nextCluster;

		if(f->nextCluster == v->root) return -1; //No se puede truncar el root

		//Actualizar fecha y hora de modificacion
		time_t tim = time(NULL);
		pfs_fat32_utils_fillTime(&(f->shortEntry.DIR_WrtDate), &(f->shortEntry.DIR_WrtTime), tim);
		f->shortEntry.DIR_LstAccDate = f->shortEntry.DIR_WrtDate;

		//Extendiendo el archivo
		if(newsize > f->shortEntry.DIR_FileSize) {
			pfs_fat32_utils_extendFileTruncate( v , f , newsize);
			return 0;
		}

		//Actualizar tamanio de archivo y cluster asociados
		uint32_t clusterId = f->source;
		uint32_t clusterOffset = f->sourceOffset;

		if(FAT_32_LDIR_ISLONG(f->longEntry.LDIR_Attr)){
			if(clusterOffset + FAT_32_DIR_ENTRY_SIZE >= v->bpc){
				clusterId = pfs_fat32_utils_getNextClusterInChain(v, clusterId);
			}
			else{
				clusterOffset += FAT_32_DIR_ENTRY_SIZE;
			}
		}

		block = pfs_fat32_utils_callGetBlock(clusterId, f);
		memcpy(&sDirEntry, block.content + clusterOffset, sizeof(DirEntry));
		sDirEntry.DIR_FileSize = newsize;
		if(newsize == 0){
			sDirEntry.DIR_FstClusHI = 0x0000;
			sDirEntry.DIR_FstClusLO = 0x0000;
		}
		memcpy(block.content + clusterOffset, &sDirEntry, sizeof(DirEntry));
		pfs_fat32_utils_callPutBlock(block, NULL);

		//Obtenemos el cluster a partir del cual se va a truncar
		while(clusterCount * v->bpc < newsize){
			clusterCount++;
			currentCluster = pfs_fat32_utils_getNextClusterInChain(v, currentCluster);
		}

		uint32_t offsetInCluster = newsize % v->bpc;

		//Borrado del primer cluster considerando que sea a partir del medio de un sector
		block = pfs_fat32_utils_callGetBlock(currentCluster , f);
		if(v->bpc - offsetInCluster == v->bpc){
			memset(block.content + offsetInCluster, setEndOfData, v->bpc - offsetInCluster);
		}
		else{
			memset(block.content + offsetInCluster + 1, setEndOfData, v->bpc - offsetInCluster);
		}

		pfs_fat32_utils_callPutBlock(block , f);


		if( offsetInCluster == 0)
			v->freeClusterCount++;


		DiskSector diskSector;
		uint32_t sectorId;
		//Seteo de EOC a la EntryFat correspondiente al cluster obtenido y borrado de datos
		uint32_t nextCluster;
		nextCluster = pfs_fat32_utils_getNextClusterInChain(v, currentCluster);
		pfs_fat32_utils_markEndOfChain(v , currentCluster);

		if(FAT_32_ISEOC(nextCluster)){
			return EXIT_SUCCESS;
		}
		else{
			uint32_t fatEntryOffset;
			while(!FAT_32_ISEOC(nextCluster)){
				block = pfs_fat32_utils_callGetBlock(nextCluster, f);
				memset(block.content, setEndOfData, v->bpc);
				pfs_fat32_utils_callPutBlock(block, f);

				sectorId = pfs_fat_utils_getFatEntrySector(v, block.id);
				diskSector = pfs_endpoint_callCachedGetSector(sectorId);
				fatEntryOffset = pfs_fat_utils_getFatEntryOffset(v, nextCluster);

				memcpy(&nextCluster, diskSector.sectorContent + fatEntryOffset, sizeof(uint32_t));
				memcpy(diskSector.sectorContent + fatEntryOffset, &setFatEntryFree, sizeof(uint32_t));
				pfs_endpoint_callCachedPutSector(diskSector);

				v->freeClusterCount++;
			}
			return EXIT_SUCCESS;
		}

	return EXIT_SUCCESS;
	}


	void pfs_fat32_moveFile(Volume * v, FatFile * oldFatFile, const char * path, const char * newpath){
		uint8_t freeCount = 0;
		uint32_t offset = FAT_32_BLOCK_ENTRY_SIZE;
		uint32_t blockNumber;
		LongDirEntry auxEntry;

		char newDirName[66];
		char dest[14];
		pfs_fat32_utils_getDirNameFromPath(newpath, newDirName);
		FatFile * newFatFile = pfs_fat32_open(newDirName);

		uint32_t clusterIdToWrite = newFatFile->nextCluster;
		Block block = pfs_fat32_utils_callGetBlock(clusterIdToWrite , newFatFile);

		while( freeCount < 2 ) {
			do {
				memcpy(&auxEntry , block.content + offset , FAT_32_DIR_ENTRY_SIZE);
				offset += 32;

				if ( FAT_32_DIRENT_ISFREE(auxEntry.LDIR_Ord) ){
					freeCount++;
					break;
				}

				if( offset >= v->bpc ){
					blockNumber = pfs_fat32_utils_getNextClusterInChain(v, blockNumber);
					if ( FAT_32_ISEOC(blockNumber) ){
						//Hay que agregar un cluster nuevo!!!
					} else {
						block = pfs_fat32_utils_callGetBlock(blockNumber , newFatFile);
						offset = 0;
					}
				}

			} while ( ! FAT_32_DIRENT_ISFREE(auxEntry.LDIR_Ord) ); //Sale cuando encontro 32 bytes libres

			memcpy(&auxEntry , block.content + offset , FAT_32_DIR_ENTRY_SIZE); //Se buscan los siguientes 32 bytes libres
			offset += 32;

			if ( FAT_32_DIRENT_ISFREE(auxEntry.LDIR_Ord) )
				freeCount++; //Listo, tenemos 64 bytes contiguos libres
			else
				freeCount = 0; // Solo se encontraron 32 bytes libres - hay que empezar de nuevo
		}

		uint16_t longOffset = offset - FAT_32_BLOCK_ENTRY_SIZE;
		uint16_t shortOffset = offset - FAT_32_DIR_ENTRY_SIZE;

		pfs_fat32_utils_getFileNameFromPath(newpath, dest);
		pfs_fat32_utils_loadEntryFilename(&oldFatFile->shortEntry , dest);
		pfs_fat32_utils_loadLongEntryFilename(&oldFatFile->longEntry ,dest);

		if(blockNumber != clusterIdToWrite) block = pfs_fat32_utils_callGetBlock(clusterIdToWrite , newFatFile);

		if(FAT_32_LDIR_ISLONG(oldFatFile->longEntry.LDIR_Attr)){
			memcpy(block.content + longOffset , &oldFatFile->longEntry , FAT_32_DIR_ENTRY_SIZE);
			if(longOffset + FAT_32_DIR_ENTRY_SIZE >= v->bps){
				blockNumber = pfs_fat32_utils_getNextClusterInChain(v, oldFatFile->source);
				block = pfs_fat32_utils_callGetBlock(blockNumber , oldFatFile);
				shortOffset = 0;
			}

			memcpy(block.content + shortOffset , &oldFatFile->shortEntry , FAT_32_DIR_ENTRY_SIZE);
		}
		else{
			shortOffset -= FAT_32_DIR_ENTRY_SIZE;
			memcpy(block.content + shortOffset , &oldFatFile->shortEntry , FAT_32_DIR_ENTRY_SIZE);
		}
		pfs_fat32_utils_callPutBlock(block , NULL);

		uint32_t blockToDeleteEntry = oldFatFile->source;
		block = pfs_fat32_utils_callGetBlock(blockToDeleteEntry, oldFatFile);

		pfs_fat32_unlink_dirEntry(v, oldFatFile, block);
	}


	void pfs_fat32_rename(Volume * v, FatFile * fatFile, char * dest){

		uint32_t clusterId = fatFile->source;
		uint32_t clusterOffset = fatFile->sourceOffset;

		Block block = pfs_fat32_utils_callGetBlock(clusterId , fatFile);

		pfs_fat32_utils_loadEntryFilename(&fatFile->shortEntry , dest);
		if(FAT_32_LDIR_ISLONG(fatFile->longEntry.LDIR_Attr)){
			pfs_fat32_utils_loadLongEntryFilename(&fatFile->longEntry ,dest);
			memcpy(block.content + clusterOffset, &fatFile->longEntry, sizeof(LongDirEntry));
			if(clusterOffset + FAT_32_DIR_ENTRY_SIZE >= v->bpc){
				clusterId = pfs_fat32_utils_getNextClusterInChain(v, clusterId);
				Block auxBlock = pfs_fat32_utils_callGetBlock(clusterId , fatFile);

				memcpy(auxBlock.content, &fatFile->shortEntry, sizeof(DirEntry));
				pfs_fat32_utils_callPutBlock(auxBlock , NULL);
			}
			else{
				memcpy(block.content + clusterOffset + FAT_32_DIR_ENTRY_SIZE, &fatFile->shortEntry, sizeof(DirEntry));
			}
		}
		else{
			memcpy(block.content + clusterOffset, &fatFile->shortEntry, sizeof(DirEntry));
		}
		pfs_fat32_utils_callPutBlock(block , NULL);
	}


	void pfs_fat32_updateDiskInformation(Volume * v){

		DiskSector sector = pfs_endpoint_callCachedGetSector(1);
		uint32_t nextFree = v->nextFreeCluster - 1;
		memcpy(sector.sectorContent + 488 , &(v->freeClusterCount) , sizeof(uint32_t));
		memcpy(sector.sectorContent + 492 , &(nextFree) , sizeof(uint32_t));
		pfs_endpoint_callPutSector(sector);
	}

	void pfs_fat32_fatCacheFlush(){
		List fatCache = pfs_cache_getListaCacheFat();
		Iterator * ite = commons_iterator_buildIterator(fatCache);

		while( commons_iterator_hasMoreElements(ite) ){
			CacheSectorRecord * nodo = (CacheSectorRecord *)commons_iterator_next(ite);

			if (nodo->modificado == TRUE){
				pfs_endpoint_callPutSector(nodo->sector);
				nodo->modificado = FALSE;
			}
		}
		free(ite);
	}

	void pfs_fat32_fileCacheFlush(FatFile * f){

		List fileCache = f->cache;
		Iterator * ite = commons_iterator_buildIterator(fileCache);

		while( commons_iterator_hasMoreElements(ite) ){
			CacheSectorRecord * nodo = (CacheSectorRecord *)commons_iterator_next(ite);
			pfs_endpoint_callPutSector(nodo->sector);

			commons_list_removeNode(fileCache , nodo , free);
		}
		free(ite);
	}



