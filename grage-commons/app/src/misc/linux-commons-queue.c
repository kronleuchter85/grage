/*
 * linux-commons-queue.c
 *
 *  Created on: 26/09/2011
 *      Author: gonzalo
 */

#include <stdlib.h>
#include "linux-commons-queue.h"



	Object commons_queue_get(Queue aQueue){

		commons_misc_lockThreadMutex(&aQueue->mutex);

		if(aQueue == NULL || aQueue->elements == NULL){

			commons_misc_unlockThreadMutex(&aQueue->mutex);
			return NULL;
		}


		Node * theFirst = aQueue->elements;
		aQueue->elements = theFirst->next;
		--aQueue->size;

		Object theData = theFirst->data;
		free(theFirst);

		commons_misc_unlockThreadMutex(&aQueue->mutex);

		return theData;
	}


	void commons_queue_put( Queue aQueue , Object anElement){
		commons_list_addNode(aQueue , anElement);
	}



	void commons_queue_destroy(Queue aQueue){
		printf("fabi se la come!");
	}


	/*
	 * Crea una cola loca :D...
	 */
	Queue commons_queue_buildQueue(
			Boolean (*aEqualityCriteria)(Object , Object)){



		Queue theQueue = (Queue)commons_list_buildList(NULL , aEqualityCriteria , commons_list_ORDER_ALWAYS_LAST);
		/*
		ThreadMutex m = PTHREAD_MUTEX_INITIALIZER;

		Queue theQueue = malloc(sizeof(L));
		theQueue->size = 0;
		theQueue->elements = NULL;
		theQueue->mutex = m;
		theQueue->equalityCriteria = aEqualityCriteria;
		theQueue->sortingCriteria = commons_list_ORDER_ALWAYS_LAST;
		*/

		return theQueue;
	}

	Queue commons_queue_buildQueueWithSortingCriteria(
			Boolean (*aEqualityCriteria)(Object , Object),
			Boolean (*sortingCriteria)(Object , Object)){

		/*
		ThreadMutex m = PTHREAD_MUTEX_INITIALIZER;

		Queue theQueue = malloc(sizeof(L));
		theQueue->size = 0;
		theQueue->elements = NULL;
		theQueue->mutex = m;
		theQueue->equalityCriteria = aEqualityCriteria;
		theQueue->sortingCriteria = commons_list_ORDER_ALWAYS_LAST;
		*/

		Queue theQueue = (Queue)commons_list_buildList(NULL , aEqualityCriteria , sortingCriteria);

		return theQueue;
	}

	Boolean commons_queue_isEmpty(Queue aQueue){
		if(aQueue == NULL)
			return TRUE;
		return (aQueue->size == 0);
	}

