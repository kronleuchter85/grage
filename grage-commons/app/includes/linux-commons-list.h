/*
 * linux-commons-list.h
 *
 *  Created on: 02/06/2011
 *      Author: gonzalo
 */
#include "linux-commons.h"


#ifndef LINUX_COMMONS_LIST_H_
#define LINUX_COMMONS_LIST_H_

	struct N{
		Object data;
		Object next;
	};

	typedef struct N Node;

	typedef struct {

		/*
		 * Campos...
		 */
		pthread_mutex_t mutex;
		Node * elements;
		int size;

		/*
		 * Funciones
		 */
		Boolean (*sortingCriteria)(Object , Object);
		Boolean (*equalityCriteria)(Object ,Object);
		void (*listingCriteria)(Object);

	}L;


	typedef struct {
		Node * elements;
	} Iterator;



	typedef L * List;

	Iterator * commons_iterator_buildIterator(List list);
	Boolean commons_iterator_hasMoreElements(Iterator * i);
	Object commons_iterator_next(Iterator * i );

	List commons_list_buildList(void (*aListingCriteria)(Object) ,
			Boolean (*aEqualityCriteria)(Object, Object) ,
			Boolean (*aSortingCriteria)(Object , Object));

	void commons_list_removeList(List list, void (*removeOperation)(Object));
	Node * commons_list_buildNode(Object data);
	void commons_list_addNode(List l ,Object n);
	void commons_list_listAllNodes(List aList);
	void commons_list_removeNode(List  aList,Object aNode , void (*removeOperation)(Object));

	List commons_list_buildStringsList();
	List commons_list_buildStringsListWithValues(int n , ...);

	String listToString(List l);

	String commons_list_formatToPreetyString(List l);
	String commons_list_serialize(List list , String separator);

	List commons_list_deserialize(String listInString );
	List commons_list_tokenize(String listInString , char separator);

	int commons_list_getSize(List l);

	Boolean commons_list_containsNode(List list, Object element );
	Object commons_list_getNodeByCriteria(List list , Boolean (*selectionCriteria)(Object));

	Boolean commons_list_ORDER_ALWAYS_FIRST(Object , Object);
	Boolean commons_list_ORDER_ALWAYS_LAST(Object , Object);

#endif /* LINUX_COMMONS_LIST_H_ */
