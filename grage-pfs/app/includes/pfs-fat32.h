/*
 * pfs-fat32.h
 *
 *  Created on: 04/11/2011
 *      Author: gonzalo
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <stdint.h>
#include <time.h>

#include "linux-commons.h"
#include "linux-commons-strings.h"
#include "linux-commons-list.h"
#include "grage-commons.h"
#include "nipc-messaging.h"
#include "pfs-endpoint.h"
#include "pfs-state.h"
#include "pfs-utils.h"

#ifndef PFS_FAT32_H_
#define PFS_FAT32_H_


	#define FAT_32_ROOT_FORWARD_SLASH					"/"
	#define FAT_32_DIR_ENTRY_SIZE						32
	#define FAT_32_BLOCK_ENTRY_SIZE						64
	#define FAT_32_SECTOR_SIZE 							512
	#define FAT_32_FATENTRY_SIZE						4
	#define FAT_32_FREEENT  							0xE5 /* The directory entry is free             */
	#define FAT_32_ENDOFDIR 							0x00 /* This and the following entries are free */

	#define FAT_32_ATTR_READ_ONLY 						0x1
	#define FAT_32_ATTR_HIDDEN 							0x2
	#define FAT_32_ATTR_SYSTEM 							0x4
	#define FAT_32_ATTR_VOLUME_ID 						0x8
	#define FAT_32_ATTR_DIRECTORY 						0x10
	#define FAT_32_ATTR_ARCHIVE 						0x20
	#define FAT_32_ATTR_LONG_NAME 						( FAT_32_ATTR_READ_ONLY | FAT_32_ATTR_HIDDEN | FAT_32_ATTR_SYSTEM | FAT_32_ATTR_VOLUME_ID )
	#define FAT_32_FAT_FREE_ENTRY						0x00000000
	#define FAT_32_FAT_EOC								0x0FFFFFF8

	#define FAT_32_ISEOC(FatEntryValue)  (((FatEntryValue) & 0x0FFFFFFF) >= 0x0FFFFFF8)
	#define FAT_32_ISFREE(FatEntryValue) (((FatEntryValue) & 0x0FFFFFFF) == 0x00000000)
	#define FAT_32_DIRENT_ISFREE(D) 	 (((D) == FAT_32_FREEENT) || ((D) == FAT_32_ENDOFDIR))
	#define FAT_32_DIRENT_ISLAST(D) 	 (D == ENDOFDIR)
	#define FAT_32_LDIR_ISLONG(Attr) 	 (Attr == FAT_32_ATTR_LONG_NAME)
	#define FAT_32_LDIR_ISLAST(Ord)		 ((Ord & 0x40) == 0x40)
	#define FAT_32_LFN_ISNULL(character) (character == 0x00)

	typedef struct {
		uint32_t id;
		char content[4096];
	} Block;

	typedef struct {
		uint8_t DIR_Name[11];
		uint8_t DIR_Attr;
		uint8_t DIR_NTRes;
		uint8_t DIR_CrtTimeTenth;
		uint16_t DIR_CrtTime;
		uint16_t DIR_CrtDate;
		uint16_t DIR_LstAccDate;
		uint16_t DIR_FstClusHI;
		uint16_t DIR_WrtTime;
		uint16_t DIR_WrtDate;
		uint16_t DIR_FstClusLO;
		uint32_t DIR_FileSize;
	}  __attribute__((packed)) DirEntry;



	typedef struct {
		uint8_t LDIR_Ord;
		uint16_t LDIR_Name1[5];
		uint8_t LDIR_Attr;
		uint8_t LDIR_Type;
		uint8_t LDIR_Chksum;
		uint16_t LDIR_Name2[6];
		uint16_t LDIR_FstClusLO;
		uint16_t LDIR_Name3[2];
	}  __attribute__((packed)) LongDirEntry;



	typedef struct {
		uint32_t source; 			// Cluster donde se encuentran los (L)DirEntries del archivo/directorio
		uint32_t sourceOffset; 		// Offset dentro del cluster
		uint32_t content;			// Cluster donde comienza el contenido
		uint32_t nextCluster;		// LO + HI del DirEntry
		uint32_t currentSector;		// Cluster actual para el recorrido en el readdir
		DirEntry shortEntry;		// LDirEntry del archivo/directorio
		LongDirEntry longEntry;		// DirEntry del archivo/directorio
		uint32_t dirEntryOffset;	// cluster offset para los direntries del archivo
		uint8_t	dirType;			// Define si es root o subdir: 0 = root ; 1 = subdir
		uint32_t EOC;				// Guarda el EOC, sirve solo para el trunate y el write

		//pfs_fat32_read
		uint32_t fileClusterNumber;
		uint32_t fileAbsoluteClusterNumberWrite;
		uint32_t fileAbsoluteClusterNumberRead;
		uint32_t fileSectorNumberOfCluster;
		uint16_t fileClusterOffset;

		List cache;

	} FatFile;

//FAT32
	FatFile * pfs_fat32_utils_openRootDirectory(Volume * v);
	FatFile * pfs_fat32_utils_openNonRootDirectory(const char * path , Volume * v );
	FatFile * pfs_fat32_open(const char * path);
	int8_t pfs_fat32_readDirectory( struct dirent * direntry , FatFile * file , Volume * volume);
	void pfs_fat32_unlink_dirEntry(Volume *  , FatFile *  , Block );
	int8_t pfs_fat32_unlink_FatEntryChain(Volume * v , FatFile * fd);
	void pfs_fat32_unlink(Volume * v , FatFile * fd);
	void pfs_fat32_rmdir(Volume * v , FatFile * fd);
	uint32_t pfs_fat32_read(Volume * , FatFile * , char * , size_t);
	int8_t pfs_fat32_mkdir(Volume *  , FatFile *  , char * );
	int8_t pfs_fat32_mknod(Volume *  , FatFile *  , char * );
	uint8_t pfs_fat32_truncate(Volume * , FatFile * , off_t );
	void pfs_fat32_rename(Volume * , FatFile * , char * );
	void pfs_fat32_moveFile(Volume * , FatFile * , const char * , const char * );
	uint32_t pfs_fat32_write(Volume * , FatFile * , const char * , size_t);
	void pfs_fat32_updateDiskInformation(Volume *);
	void pfs_fat32_fatCacheFlush();
	void pfs_fat32_fileCacheFlush(FatFile *);

//UTILS
	Volume * pfs_fat_utils_loadVolume(BPB * b);

	uint32_t pfs_fat32_utils_fetchChar(LongDirEntry *D, int8_t n);
	int8_t pfs_fat32_utils_getNameLength(LongDirEntry * ldirentry);
	void pfs_fat32_utils_extractName( LongDirEntry * d, uint16_t * dest, int8_t length);
	void pfs_fat_utils_getFileName(LongDirEntry * l , char *);
	void pfs_fat32_utils_getDirNameFromPath(const char *path, char *dest);
	void pfs_fat32_utils_getFileNameFromPath(const char *path, char *dest);
	void pfs_fat32_utils_loadLongEntryFilename(LongDirEntry * , char * );
	uint8_t pfs_fat32_utils_loadEntryFilename(DirEntry * , char *);
	uint8_t pfs_fat_utils_hasLFN(uint32_t , DiskSector);

	uint32_t pfs_fat_utils_getFatEntrySector(Volume * v , uint32_t cluster);
	uint32_t pfs_fat_utils_getFatEntryOffset(Volume * v , uint32_t cluster);
	uint32_t pfs_fat_utils_getFirstSectorOfCluster(Volume * v , uint32_t cluster);
	uint32_t pfs_fat_utils_getClusterBySector(Volume * , uint32_t);
	uint32_t pfs_fat32_utils_getNextClusterInChain(Volume * v , uint32_t clusterId);
	uint32_t pfs_fat32_utils_getFirstSectorFromNextClusterInChain(Volume * v , uint32_t clusterId);
	Boolean pfs_fat32_utils_isLastSectorFromCluster(Volume * v , uint32_t sectorId);

	uint32_t pfs_fat_getFirstClusterFromDirEntry(DirEntry * D);
	uint8_t pfs_fat_setFirstClusterToDirEntry(DirEntry * , uint32_t);
	uint32_t pfs_fat32_utils_getDirEntryOffset(uint32_t sectorId , uint32_t os , uint32_t offset);
	void pfs_fat32_utils_toDirent(struct dirent * de , DirEntry * , LongDirEntry * , Volume * v);
	uint8_t pfs_fat32_isDirectoryEmpty(Volume * v, FatFile * fd);
	void pfs_fat32_utils_fileStat(Volume * , FatFile * , struct stat *);

	time_t pfs_fat32_utils_processTime(int s, int m, int h, int d, int mo, int y);
	time_t pfs_fat32_utils_getTime(DirEntry *D);
	uint8_t pfs_fat32_utils_fillTime(uint16_t * , uint16_t * , time_t);

	int8_t pfs_fat32_utils_seek(Volume * , FatFile * , off_t , uint32_t);
	Block pfs_fat32_utils_getBlockFromNthClusterWrite(FatFile *);
	Block pfs_fat32_utils_getBlockFromNthClusterRead(FatFile *);

	uint32_t pfs_fat32_utils_getNextFreeCluster(void); // Esta funcion se deberia poder borrar
	void pfs_fat32_utils_setNextFreeCluster(uint32_t);
	uint32_t pfs_fat32_utils_allocateNewCluster(Volume * , uint32_t);
	uint32_t pfs_fat32_utils_assignCluster(Volume * v);
	void pfs_fat32_utils_markEndOfChain(Volume * , uint32_t);
	uint32_t pfs_fat32_utils_findEOC(Volume * , uint32_t );
	void pfs_fat32_utils_expandChain(Volume * , uint32_t , uint32_t);

	void pfs_fat32_utils_fillDotEntry(DirEntry * , DirEntry *);
	void pfs_fat32_utils_fillDotDotEntry(DirEntry * , DirEntry *);
	uint32_t pfs_fat_utils_FreeClustersQuantity();

	void pfs_fat32_utils_updateFilesize(Volume * , FatFile * , uint32_t);
	void pfs_fat32_utils_extendFileTruncate(Volume * , FatFile * , off_t);
	void pfs_fat32_utils_extendFileWrite(Volume * , FatFile * , off_t);
	int8_t pfs_fat32_utils_seekWrite(Volume * , FatFile * , off_t , uint32_t);
	void pfs_fat32_utils_getShortName(DirEntry * , char *);

	void pfs_endpoint_callPutSector(DiskSector);

	Block pfs_endpoint_blocks_callGetBlock(uint32_t , FatFile *);
	Block pfs_fat32_utils_callGetBlock(uint32_t , FatFile *);
	void pfs_fat32_utils_callPutBlock(Block , FatFile * );

#endif /* PFS_FAT32_H_ */


