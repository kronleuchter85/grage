/*
 * linux-commons-queue.h
 *
 *  Created on: 26/09/2011
 *      Author: gonzalo
 */

#include "linux-commons.h"
#include "linux-commons-list.h"

#ifndef LINUX_COMMONS_QUEUE_H_
#define LINUX_COMMONS_QUEUE_H_

	typedef List Queue;

	Queue commons_queue_buildQueue(Boolean (*aEqualityCriteria)(Object , Object));
	void commons_queue_put(Queue , Object);
	Object commons_queue_get(Queue);
	void commons_queue_destroy(Queue aQueue);

	Queue commons_queue_buildQueueWithSortingCriteria(
			Boolean (*aEqualityCriteria)(Object , Object),
			Boolean (*sortingCriteria)(Object , Object));

	Boolean commons_queue_isEmpty(Queue aQueue);

#endif /* LINUX_COMMONS_QUEUE_H_ */
