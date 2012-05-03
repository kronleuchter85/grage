/*
 * ppd-connection-launcher.h
 *
 *  Created on: 08/10/2011
 *      Author: gonzalo
 */
#include <linux-commons.h>

#ifndef PPD_CONNECTION_LAUNCHER_H_
#define PPD_CONNECTION_LAUNCHER_H_


	void ppd_connections_connectToPraid();
	void ppd_connections_waitForPfsConnections();
	void ppd_launcher_launchConnections();

	Boolean ppd_connections_handshake(ListenSocket);


#endif /* PPD_CONNECTION_LAUNCHER_H_ */
