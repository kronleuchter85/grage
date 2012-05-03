/*
 * pfs-state.h
 *
 *  Created on: 15/09/2011
 *      Author: Fernando
 */

#include "grage-commons.h"
#include <linux-commons-socket.h>


#ifndef PFS_STATE_H_
#define PFS_STATE_H_


	void pfs_state_setDataSocket(ListenSocket);
	ListenSocket pfs_state_getDataSocket();

	void pfs_state_setBiosParameterBlock(BPB bpb);
	BPB pfs_state_getBiosParameterBlock();


	void pfs_state_setVolume(Volume * v);
	Volume * pfs_state_getVolume();

	void pfs_state_initializeOpenFiles();

	char * pfs_state_getMountPath(void);
	void pfs_state_setMountPath(char *);


#endif /* PFS_STATE_H_ */
