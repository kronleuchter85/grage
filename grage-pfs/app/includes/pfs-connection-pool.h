/*
 * pfs-connection-pool.h
 *
 *  Created on: 15/11/2011
 *      Author: gonzalo
 */

#include <linux-commons.h>
#include <linux-commons-socket.h>

#ifndef PFS_CONNECTION_POOL_H_
#define PFS_CONNECTION_POOL_H_



	typedef struct {

		Boolean active;
		ListenSocket listenSocket;

	} PooledConnection;


	void pfs_pool_setPooledConnectionsMaxCount(int count);

	PooledConnection * pfs_pool_getConection();

	void pfs_pool_releaseConnection(PooledConnection * conn);

	Boolean pfs_pool_isPooledConnectionsEnabled();

#endif /* PFS_CONNECTION_POOL_H_ */
