/*
 * praid-endpoint.h
 *
 *  Created on: 10/10/2011
 *      Author: gonzalo
 */
#include <linux-commons.h>
#include <linux-commons-socket.h>
#include <grage-commons.h>

#ifndef PRAID_ENDPOINT_H_
#define PRAID_ENDPOINT_H_

	void praid_endpoint_pfs_responseGetSectors(ListenSocket ls , NipcMessage m);
	void praid_endpoint_pfs_responseAndClose(ListenSocket ls , NipcMessage m);
	void praid_endpoint_ppd_sendMessage(ListenSocket ppdSocket , NipcMessage m);

	void praid_endpoint_ppd_callSyncGetSector(ListenSocket , uint32_t);
	void praid_endpoint_ppd_callSyncPutSector(ListenSocket , DiskSector , uint16_t bytes);
	void praid_endpoint_ppd_callFinishReplication(ListenSocket);

	void praid_endpoint_ppd_callProcessJobs(ListenSocket ls);

#endif /* PRAID_ENDPOINT_H_ */
