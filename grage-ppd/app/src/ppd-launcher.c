/*
 * ppd-launcher.c
 *
 *  Created on: 05/09/2011
 *      Author: EL Seba
 */

#include <strings.h>
#include <sys/types.h>
#include <wchar.h>
#include <iconv.h>
#include <fcntl.h>
#include <dirent.h>


#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <linux-commons-logging.h>
#include <linux-commons-strings.h>
#include <linux-commons-file.h>
#include <nipc-messaging.h>
#include <unistd.h>

#include "ppd-configuration.h"
#include "ppd-queues.h"
#include "ppd-planifier.h"
#include "ppd-persistance.h"
#include "ppd-state.h"
#include "ppd-launchConsole.h"
#include "ppd-connection.h"
#include "ppd-entrypoint.h"


	void ppd_launcher_exit();

	void ppd_launcher_console(){
		ppd_launchConsole_initialize();
		ppd_launchConsole_launchSlaveThread();
	}





	void ppd_launcher_initialize(){

		ppd_configuration_setup();

		if(log_create("PPD", PPD_DEFAULT_LOG_FILE , ppd_conf_getLoggingLevel() , M_CONSOLE_DISABLE)){
			puts("[ Fallo la creacion del log. Finalizando aplicacion. ]");
			exit(EXIT_FAILURE);
		}

		if( !commons_file_isValidConfValue( ppd_conf_getDiskPath())){
			puts("[ No existe un path al archivo del volumen. Finalizando aplicacion. ]");
			exit(EXIT_FAILURE);
		}

		ppd_state_initializeVolumeSize();

		if(ppd_state_getVolumeSize() != 0 )
			ppd_persistence_mapDevice();

		ppd_queues_initialize();
	}




	void ppd_launcher_doLaunch(){

 		ppd_launcher_launchConnections();

 		ppd_planifier_worker_doJobs();

		if(ppd_conf_isConsoleEnabled())
			ppd_launcher_console();

		ppd_entrypoint_launch();
	}



	void ppd_launcher_exit(){
		puts("[ Finalizando todo ]");

		/*
		 * Desmapeamos el disco solo si fue mapeado
		 */
		if( ppd_state_getDiskStartAddress() != NULL)
			ppd_persistence_unmapDevice();

		log_destroy();
	}


	int main(){

		ppd_launcher_initialize();

		ppd_launcher_doLaunch();

		ppd_launcher_exit();

		return EXIT_SUCCESS;
	}
