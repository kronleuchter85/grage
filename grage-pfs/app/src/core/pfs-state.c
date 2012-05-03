/*
 * pfs-state.c
 *
 *  Created on: 15/09/2011
 *      Author: Fernando
 */


#include <stdlib.h>
#include <stdio.h>

#include "pfs-state.h"
#include "pfs-fat32.h"


	char * mountingPoint;
	ListenSocket dataSocket;
	BPB biosParameterBlock;
	Volume * volume;

	void pfs_state_setDataSocket(ListenSocket s){
		dataSocket = s;
	}
	ListenSocket pfs_state_getDataSocket(){
		return dataSocket;

	}


	void pfs_state_setBiosParameterBlock(BPB bpb){
		biosParameterBlock = bpb;
	}
	BPB pfs_state_getBiosParameterBlock(){
		return biosParameterBlock;
	}


	void pfs_state_setVolume(Volume * v){
		volume = v;
	}
	Volume * pfs_state_getVolume(){
		return volume;
	}

	List openFiles = NULL;

	void pfs_state_initializeOpenFiles(){
		Volume * v = pfs_state_getVolume();
		Boolean eq(void * s1 , void * s2){
			FatFile * p1 = (FatFile *) s1;
			FatFile * p2 = (FatFile *) s2;
			return ((p1->source == p2->source)&&(p1->sourceOffset == p2->sourceOffset));
	    }
		openFiles = commons_list_buildList(NULL, eq, commons_list_ORDER_ALWAYS_FIRST);

	}

	void pfs_state_addOpenFile(FatFile * fatFile){
		commons_list_addNode(openFiles,fatFile);
	}

	void pfs_state_removeOpenedFile(FatFile * fatFile){

		void pfs_state_clearFileCache(FatFile * f){
			commons_list_removeList(f->cache , free);
			//free(f); Con el free da "stack smashing error" !!!
		}

		commons_list_removeNode(openFiles,fatFile,pfs_state_clearFileCache);
	}

	char * pfs_state_getMountPath(){
		return mountingPoint;
	}

	void pfs_state_setMountPath(char * path){
		mountingPoint = path;
	}
