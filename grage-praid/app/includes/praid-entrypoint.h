/*
 * praid-entrypoint.h
 *
 *  Created on: 10/10/2011
 *      Author: gonzalo
 */

#include "linux-commons-socket.h"
#include "praid-state.h"

#ifndef PRAID_ENTRYPOINT_H_
#define PRAID_ENTRYPOINT_H_

	void praid_entry_startEntrypointListening();

	void praid_pfs_launchNewSlaveThread(ListenSocket ls);
	void praid_ppd_thread_launchNewSlaveThread(PPDConnectionStorage * aStorage);

#endif /* PRAID_ENTRYPOINT_H_ */
