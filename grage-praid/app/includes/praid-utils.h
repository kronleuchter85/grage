/*
 * praid-utils.h
 *
 *  Created on: 09/11/2011
 *      Author: gonzalo
 */


#include "praid-sync.h"

#ifndef PRAID_UTILS_H_
#define PRAID_UTILS_H_

	void praid_utils_printClusterInformation();

	void praid_utils_printSynchingInformation(SyncProcessState state);

	void praid_utils_printEndSynchingInformation(SyncProcessState state);

	Queue praid_utils_getReadingJobs(PPDConnectionStorage * storage);

	void praid_utils_printLines();

	void praid_utils_printPendingJobs(Queue jobs);

#endif /* PRAID_UTILS_H_ */
