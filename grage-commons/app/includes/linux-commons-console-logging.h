/*
 * linux-commons-console-logging.h
 *
 *  Created on: 10/11/2011
 *      Author: gonzalo
 */

#include <stdlib.h>

#include "linux-commons.h"


#ifndef LINUX_COMMONS_CONSOLE_LOGGING_H_
#define LINUX_COMMONS_CONSOLE_LOGGING_H_

#define PRAID_CONF_CONSOLE_LEVEL_DEFAULT			"default"
#define PRAID_CONF_CONSOLE_LEVEL_NONE				"none"
#define PRAID_CONF_CONSOLE_LEVEL_ALL				"all"


	Boolean commons_console_logging_isAll();
	Boolean commons_console_logging_isDefault();
	void commons_console_logging_setLevel(char * level);

#endif /* LINUX_COMMONS_CONSOLE_LOGGING_H_ */
