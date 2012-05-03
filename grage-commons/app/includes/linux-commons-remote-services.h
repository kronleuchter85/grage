/*
 * linux-commons-remote-services.h
 *
 *  Created on: 09/07/2011
 *      Author: gonzalo
 */

#ifndef LINUX_COMMONS_REMOTE_SERVICES_H_
#define LINUX_COMMONS_REMOTE_SERVICES_H_

#define KSS_SYS_CALL_OPEN 				"kss_op01"
#define KSS_SYS_CALL_CLOSE 				"kss_op02"
#define KSS_SYS_CALL_READ 				"kss_op03"
#define KSS_SYS_CALL_WRITE				"kss_op04"
#define KSS_SYS_CALL_FLIST				"kss_op05"


#define FSS_FILE_CHECK_EXISTENCE		"fss_op01"
#define FSS_FILE_INFORMATION			"fss_op02"
#define FSS_FILE_CREATION				"fss_op04"
#define FSS_SECTORS_TABLE_CREATION		"fss_op06"
#define FSS_FILE_DELETION				"fss_op03"
#define FSS_DIRECTORY_LISTING			"fss_op10"
#define FSS_FORMAT						"fss_op11"
#define FSS_SECTORS_ASSIGN_AND_GET		"fss_op12"
#define FSS_HAS_VALID_FORMAT			"fss_op13"
#define FSS_UPDATE_FILE_SIZE			"fss_op14"


#define VDA_SECTORS_GETTING				"vda_op01"
#define VDA_SECTORS_PUTTING				"vda_op02"
#define VDA_CHS_GETTING					"vda_op03"
#define VDA_IDENTIFY_NAME				"vda_op04"

#define SHELL_MOUNT					"shl_op01"
#define SHELL_UMOUNT				"shl_op02"
#define SHELL_FORMAT				"shl_op03"
#define SHELL_LS							"shl_op04"
#define SHELL_TDD_DUMP			"shl_op05"
//#define SHELL_MD5_SUM			"shl_op06" ---> Ya no se usa
#define SHELL_QUIT						"shl_op07"
#define SHELL_DISCONNECT		"shl_op08"
#define SHELL_SHOW_DEVICES	"shl_op09"


/*
 * Handshakes
 */
#define FSS_HANDSHAKE					'F'
#define VDA_HANDSHAKE					'V'
#define SHELL_HANDSHAKE					'S'
#define FTP_HANDSHAKE					'T'


#endif /* LINUX_COMMONS_REMOTE_SERVICES_H_ */
