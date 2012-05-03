/*
 * pfs-connection-pool.c
 *
 *  Created on: 15/11/2011
 *      Author: gonzalo
 */

#include <stdlib.h>
#include <unistd.h>

#include <linux-commons.h>
#include <linux-commons-socket.h>
#include <linux-commons-list.h>

#include "pfs-connection-pool.h"
#include "pfs-configuration.h"
#include "pfs-endpoint.h"


	PooledConnection * pfs_pool_getPooledConnection();
	PooledConnection * pfs_pool_buildAndAddPooledConnection();




	ThreadMutex poolingConnectionsMutex = PTHREAD_MUTEX_INITIALIZER;




	List pooledConnections = NULL;

	List pfs_pool_getPooledConnections(){
		return pooledConnections;
	}
	void pfs_pool_addPooledConnection(PooledConnection * conn){
		commons_list_addNode(pooledConnections , conn);
	}




	int pooledConnectionsMaxCount;

	void pfs_pool_setPooledConnectionsMaxCount(int count){
		pooledConnectionsMaxCount = count;
	}
	int pfs_pool_getPooledConnectionsMaxCount(){
		return pooledConnectionsMaxCount;
	}



	Boolean pfs_pool_isActiveConnection(PooledConnection * p){
		return p->active;
	}
	void pfs_pool_setActiveConnection(PooledConnection * p){
		p->active = TRUE;
	}
	void pfs_pool_setInactiveConnection(PooledConnection * p){
		p->active = FALSE;
	}





	void pfs_pool_initialize(){
      	pfs_pool_setPooledConnectionsMaxCount(pfs_configuration_getMaxConnections());

      	Boolean eq(void * c1 , void * c2){
      		PooledConnection * p1 = (PooledConnection *) c1;
      		PooledConnection * p2 = (PooledConnection *) c2;
      		return p1->listenSocket == p2->listenSocket;
      	}


      	pooledConnections = commons_list_buildList(NULL , eq ,
      			commons_list_ORDER_ALWAYS_FIRST );
	}



	/*
	 * Se obtiene la conexion
	 */
	PooledConnection * pfs_pool_getConection(){


		if(pfs_pool_isPooledConnectionsEnabled()
				&& pfs_pool_getPooledConnections() == NULL){
			pfs_pool_initialize();
		}else if (pfs_configuration_getMaxConnections() == 0){
			puts("[ No se esta trabajando con un pool de conexiones habilitado pero aun asi se quiere acceder a una conexion pooleada ]");
			exit(EXIT_FAILURE);
			return NULL;
		}


		PooledConnection * conn = NULL;

		while(conn == NULL){
			conn = pfs_pool_getPooledConnection();

			if(conn == NULL)
				sleep(1);
		}

		return conn;
	}


	/*
	 * Se libera la conexion...
	 */
	void pfs_pool_releaseConnection(PooledConnection * conn){
		commons_misc_lockThreadMutex(&poolingConnectionsMutex);

		pfs_pool_setInactiveConnection(conn);

		commons_misc_unlockThreadMutex(&poolingConnectionsMutex);
	}





	PooledConnection * pfs_pool_getPooledConnection(){

		commons_misc_lockThreadMutex(&poolingConnectionsMutex);

		PooledConnection * conn = NULL;

		/*
		 * Pool vacio
		 */
		if(pooledConnections == NULL || pooledConnections->size == 0){

			PooledConnection * conn = pfs_pool_buildAndAddPooledConnection();

			commons_misc_unlockThreadMutex(&poolingConnectionsMutex);

			return conn;
		}


		/*
		 * Pool con conexiones existentes
		 */

		Iterator * conns = commons_iterator_buildIterator(pooledConnections);

		while(commons_iterator_hasMoreElements(conns)){

			conn = commons_iterator_next(conns);

			if( !pfs_pool_isActiveConnection(conn) ){

				pfs_pool_setActiveConnection(conn);

				free(conns);

				commons_misc_unlockThreadMutex(&poolingConnectionsMutex);

				return conn;
			}
		}


		if(pooledConnections->size < pooledConnectionsMaxCount)
			conn = pfs_pool_buildAndAddPooledConnection();
		else
			conn = NULL;

		free(conns);

		commons_misc_unlockThreadMutex(&poolingConnectionsMutex);

		return conn;
	}






	PooledConnection * pfs_pool_buildAndAddPooledConnection(){

		ListenSocket ls = pfs_endpoint_doHandshake();

		if(ls == 0){
			puts("[ Por alguna razon no se pudo establecer una coneccion desde el PFS desde el pool de conexiones ]");
			exit(EXIT_FAILURE);
		}

		PooledConnection * conn = (PooledConnection *) malloc(sizeof(PooledConnection));
		conn->listenSocket = ls;

		pfs_pool_setActiveConnection(conn);
		pfs_pool_addPooledConnection(conn);

		return conn;
	}






	Boolean pfs_pool_isPooledConnectionsEnabled(){
		return (pfs_configuration_getMaxConnections() > 0);
	}

