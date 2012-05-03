/*
 * linux-commons-console-logging.c
 *
 *  Created on: 10/11/2011
 *      Author: gonzalo
 */

#include <stdlib.h>

#include "linux-commons.h"


#define PRAID_CONF_CONSOLE_LEVEL_DEFAULT			"default"
#define PRAID_CONF_CONSOLE_LEVEL_NONE				"none"
#define PRAID_CONF_CONSOLE_LEVEL_ALL				"all"


	char * levelStatus = PRAID_CONF_CONSOLE_LEVEL_DEFAULT;

	Boolean commons_console_logging_isAll(){

		if(commons_string_equals(levelStatus ,  PRAID_CONF_CONSOLE_LEVEL_ALL))
			return TRUE;
		else
			return FALSE;
	}

	Boolean commons_console_logging_isDefault(){
		if( commons_string_equals(levelStatus , PRAID_CONF_CONSOLE_LEVEL_DEFAULT)
				|| commons_string_equals( levelStatus , PRAID_CONF_CONSOLE_LEVEL_ALL))
			return TRUE;
		else
			return FALSE;
	}

	void commons_console_logging_setLevel(char * level){
		levelStatus = level;
	}

