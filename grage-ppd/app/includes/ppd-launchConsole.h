/*
 * ppd-console.h
 *
 *  Created on: 10/10/2011
 *      Author: joaquin
 */

#include <unistd.h>

#include "linux-commons.h"

#ifndef PPD_CONSOLE_H_
#define PPD_CONSOLE_H_

	uint32 ppd_launchConsole_initialize();
	void ppd_launchConsole_startUNIX();


	void ppd_launchConsole_launchSlaveThread();

	void ppdConsoleServiceThread();

	pid_t ppd_launch_console_getChildProcessId();

#endif /* PPD_CONSOLE_H_ */
