/*
 * ppd-planifier.h
 *
 *  Created on: 09/10/2011
 *      Author: gonzalo
 */

#include <pthread.h>
#include "linux-commons.h"


	pthread_t ppd_planifier_worker_getReadingThread();
	pthread_t ppd_planifier_worker_getWritingThread();


	void ppd_planifier_worker_doJobs();


	Boolean ppd_alg_planif_strategy_look(Object , Object);
	Boolean ppd_alg_planif_strategy_sstf(Object , Object);



