/*
 * ppd-configuration.h
 *
 *  Created on: 05/09/2011
 *      Author: gonzalo
 */

#ifndef PPD_CONFIGURATION_H_
#define PPD_CONFIGURATION_H_


#define PPD_CONFIGURATION_MODE_CONNECT		"connect"
#define PPD_CONFIGURATION_MODE_LISTEN		"listen"

//#define PPD_DEFAULT_CONFIGURATION_FILE	"../conf/grage-ppd.properties"
//#define PPD_DEFAULT_LOG_FILE				"../logs/ppd.log"

#define PPD_DEFAULT_LOG_FILE				"/opt/grage-repository/logs/ppd.log"
#define PPD_DEFAULT_CONFIGURATION_FILE		"/opt/grage-repository/conf/grage-ppd.properties"

#define PPD_CONFIGURATION_MODE				"ppd.configuration.mode"
#define PPD_CONFIGURATION_ALGORITMO			"ppd.configuration.algoritmo"
#define PPD_CONFIGURATION_LISTEN_PORT		"ppd.configuration.listen.port"
#define PPD_CONFIGURATION_LOGGING_LEVEL		"ppd.configuration.logging.level"
#define PPD_CONFIGURATION_ID_DISK			"ppd.configuration.id.disk"
#define PPD_CONFIGURATION_CILINDER 			"ppd.configuration.cilinder"
#define PPD_CONFIGURATION_HEAD				"ppd.configuration.head"
#define PPD_CONFIGURATION_SECTOR			"ppd.configuration.sector"
#define PPD_CONFIGURATION_READ_TIME			"ppd.configuration.read.time"
#define PPD_CONFIGURATION_WRITE_TIME		"ppd.configuration.write.time"
#define PPD_CONFIGURATION_RPM				"ppd.configuration.rpm"
#define PPD_CONFIGURATION_SALTO_PISTA		"ppd.configuration.salto.pista"
#define PPD_CONFIGURATION_WRITE_DELAY		"ppd.configuration.write.delay"
#define PPD_CONFIGURATION_READ_DELAY		"ppd.configuration.read.delay"

#define PPD_CONFIGURATION_PRAID_PORT		"ppd.configuration.praid.port"
#define PPD_CONFIGURATION_PRAID_HOST		"ppd.configuration.praid.host"
#define PPD_CONFIGURATION_DISK_PATH			"ppd.configuration.disk.volume.path"
#define PPD_CONFIGURATION_POOLED_CONNECTIONS_ENABLED			"ppd.configuration.pooled.connections.enabled"

#define PPD_CONFIGURATION_CONSOLE_ENABLED	"ppd.configuration.console.enabled"


	void ppd_configuration_setup();
	void ppd_configuration_setConfigurationFile(char * s);
	char * ppd_configuration_getConfigurationFile(void);

	void ppd_conf_setPpdMode(char * p);
	char * ppd_conf_getPpdMode();
	void ppd_conf_setPraidPort(char * p);
	char * ppd_conf_getPraidPort();
	void ppd_conf_setPraidAddress(char * p);
	char * ppd_conf_getPraidAddress();



	void setPpdAlgoritmo(char * p);
	void ppd_conf_setPpdPort(char * p);
	void ppd_conf_setPpdIdDisk(char * p);
	void setPpdDiskCilinder(char * p);
	void setPpdDiskHead(char * p);
	void setPpdDiskSector(char * p);
	void setPpdReadTimeMs(char * p);
	void setPpdWriteTimeMs(char * p);
	void setPpdRpm(char * p);
	void setPpdSaltoPistaMs(char * p);
	void setPpdWriteDelay(char *);
	void setPpdReadDelay(char *);

	void ppd_conf_setDiskPath(char * path);

	char * getPpdAlgoritmo();
	char * ppd_conf_getPpdPort();
	char * ppd_conf_getPpdIdDisk();
	char * getPpdDiskCilinder();
	char * getPpdDiskHead();
	char * getPpdDiskSector();
	char * getPpdReadTimeMs();
	char * getPpdWriteTimeMs();
	char * getPpdRpm();
	char * getPpdSaltoPistaMs();
	char * getPpdReadDelay();
	char * getPpdWriteDelay();

	char * ppd_conf_getDiskPath();



	void ppd_conf_setPooledConnections(Boolean s);
	Boolean ppd_conf_isPooledConnections();


	uint32_t ppd_conf_getLoggingLevel();
	void ppd_conf_setLoggingLevel(uint32_t l);


	Boolean ppd_conf_isConsoleEnabled();
	void ppd_conf_setConsoleEnabled(Boolean);


#endif /* PFS_CONFIGURATION_H_ */

