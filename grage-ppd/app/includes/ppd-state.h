/*
 * ppd-state.h
 *
 *  Created on: 08/10/2011
 *      Author: gonzalo
 */


#include "grage-commons.h"
#include "linux-commons.h"
#include "linux-commons-socket.h"


#ifndef PPD_STATE_H_
#define PPD_STATE_H_

	void ppd_state_setPraidSocket(ListenSocket ls);
	ListenSocket ppd_state_getPraidSocket();

	void ppd_state_setPfsConnection(ServerSocket * s);
	ServerSocket * ppd_state_getPfsConnection();

	char * ppd_state_getDiskStartAddress(void);
	void ppd_state_setDiskStartAddress(char *);

	void ppd_state_setReplicationDiskVolume(File * f);
	File * ppd_state_getReplicationDiskVolume();

	void ppd_state_setVolumeSize(uint32_t v);
	uint32_t ppd_state_getVolumeSize();

	Boolean ppd_state_isReplicationProcessActive();
	void ppd_state_setReplicationProcessActive(Boolean s);

	Boolean ppd_state_isListenMode();
	Boolean ppd_state_isConnectMode();

	void ppd_state_initializeVolumeSize();


	ListenSocket ppd_state_getPpdConsoleSocket();

#endif /* PPD_STATE_H_ */
