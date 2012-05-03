/*
 * ppd-entrypoint.c
 *
 *  Created on: 08/10/2011
 *      Author: gonzalo
 */

#include <unistd.h>
#include <pthread.h>

#include <linux-commons-logging.h>
#include <linux-commons-strings.h>
#include <grage-commons.h>
#include <linux-commons.h>
#include <linux-commons-socket.h>
#include <linux-commons-file.h>

#include "nipc-messaging.h"

#include "ppd-entrypoint.h"
#include "ppd-endpoint.h"
#include "ppd-persistance.h"
#include "ppd-queues.h"
#include "ppd-state.h"
#include "ppd-configuration.h"
#include "ppd-sync.h"



	void ppd_entrypoint_launch(){
		if( ppd_state_isListenMode() ){

			ppd_entrypoint_launchPfsPpdEntrypoint();
		}else{

			ppd_entrypoint_launchPraidPpdEntrypoint();
		}
	}


	void ppd_entrypoint_executePutSector(NipcMessage message){

		log_info_t("Recibiendo peticion PUT Sectores ");
		log_info_t("Solicitando sectorId: %i" , message.payload.diskSector.sectorNumber);

		ppd_queues_putInQueue(message);
	}

	void ppd_entrypoint_executeGetSector(NipcMessage message){

		log_info_t("Recibiendo peticion GET Sectores");
		log_info_t("Solicitando sectorId: %i" , message.payload.diskSector.sectorNumber);

		ppd_queues_putInQueue(message);
	}




