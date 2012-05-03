/*
 * pfs-console-utils.h
 *
 *  Created on: 28/09/2011
 *      Author: joaquin
 */
#define NUM_THREADS     1


#ifndef PFS_CONSOLE_UTILS_H_
#define PFS_CONSOLE_UTILS_H_

	String pfs_console_utils_parseCMD(char * comando);
	String pfs_console_utils_get_cmd_parameter(char * comando, int cmd_length);

#endif /* PFS_CONSOLE_UTILS_H_ */
