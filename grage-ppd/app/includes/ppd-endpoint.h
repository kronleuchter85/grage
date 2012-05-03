/*
 * ppd-endpoint.h
 *
 *  Created on: 08/11/2011
 *      Author: gonzalo
 */

#include <nipc-messaging.h>
#include <grage-commons.h>


#ifndef PPD_ENDPOINT_H_
#define PPD_ENDPOINT_H_

	void ppd_endpoint_responseGetSector( NipcMessage message);
	void ppd_endpoint_sendFinishReplication();
	void ppd_endpoint_sendSyncSector(uint16_t payLength , DiskSector diskSector);


#endif /* PPD_ENDPOINT_H_ */
