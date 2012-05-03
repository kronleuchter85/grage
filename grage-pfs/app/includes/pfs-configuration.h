/*
 * pfs-configuration.h
 *
 *  Created on: 05/09/2011
 *      Author: gonzalo
 */

#ifndef PFS_CONFIGURATION_H_
#define PFS_CONFIGURATION_H_

#define PFS_DEFAULT_CONFIGURATION_FILE		"/opt/grage-repository/conf/grage-pfs.properties"

#define PFS_DEVICE_ADDRESS	"pfs.device.address"
#define PFS_DEVICE_PORT		"pfs.device.port"
#define	PFS_MAX_CONNECTIONS	"pfs.max.connections"
#define PFS_CACHE_SIZE		"pfs.cache.size"

	char * pfs_configuration_getDeviceAddress(void);
	uint32_t pfs_configuration_getMaxConnections(void);
	char * pfs_configuration_getDevicePort(void);
	uint32_t pfs_configuration_getCacheSize(void);

	void pfs_configuration_setDevicePort(char * devPort);
	void pfs_configuration_setDeviceAddress(char *);
	void pfs_configuration_setMaxConnections(uint32_t numberOfConnections);
	void pfs_configuration_setCacheSize(char * sizeOfCache);

	void pfs_configuration_processEntries(char * key , char * value , void * object);

	void pfs_configuration_setConfigurationFile(char * s);
	char * pfs_configuration_getConfigurationFile();

	void pfs_configuration_initialize();

#endif /* PFS_CONFIGURATION_H_ */
