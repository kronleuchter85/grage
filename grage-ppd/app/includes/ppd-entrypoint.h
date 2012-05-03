/*
 * ppd-entrypoint.h
 *
 *  Created on: 09/10/2011
 *      Author: gonzalo
 */


#include <pthread.h>

#include <linux-commons.h>

#ifndef PPD_ENTRYPOINT_H_
#define PPD_ENTRYPOINT_H_


	void ppd_entrypoint_launch();
	pthread_t ppd_entrypoint_getEntrypointThread();
	void ppd_entrypoint_executePutSector(NipcMessage message);
	void ppd_entrypoint_executeGetSector(NipcMessage message);
	void ppd_entrypoint_doLunch();

	void ppd_entrypoint_endReplicationProcess();
	void ppd_pfs_entrypoint_serviceThread(ListenSocket * );


	void ppd_entrypoint_launchPfsPpdEntrypoint();
	void ppd_entrypoint_launchPraidPpdEntrypoint();

#endif /* PPD_ENTRYPOINT_H_ */
