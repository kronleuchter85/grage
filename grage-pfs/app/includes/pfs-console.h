/*
 * pfs-console.h
 *
 *  Created on: 15/09/2011
 *      Author: Fernando
 */

#include "grage-commons.h"
#ifndef PFS_CONSOLE_H_
#define PFS_CONSOLE_H_

#define NUM_THREADS     1

	void pfs_console_fsinfo();
	void pfs_console_finfo(String parameter);
	void * 	pfs_console_thread(void *thread_init_parameter);
	int pfs_console_initialize();
	void unmountVolume(void);

#endif /* PFS_CONSOLE_H_ */
