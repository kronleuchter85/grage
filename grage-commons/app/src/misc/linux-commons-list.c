/*
 * linux-commons-list.c
 *
 *	Esta lista tiene accesos sincronizados para
 *	agregar y eliminar elementos.
 *
 *  Created on: 02/06/2011
 *      Author: gonzalo
 */
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdarg.h>
#include <strings.h>

#include "linux-commons-list.h"
#include "linux-commons-strings.h"


	Boolean commons_list_ORDER_ALWAYS_FIRST(Object a, Object b){
		return FALSE;
	}
	Boolean commons_list_ORDER_ALWAYS_LAST(Object a, Object b){
		return TRUE;
	}

	List commons_list_buildList(
			void (*aListingCriteria)(Object) ,
			Boolean (*aEqualityCriteria)(Object , Object) ,
			Boolean (*aSortingCriteria)(Object , Object)){

		ThreadMutex m = PTHREAD_MUTEX_INITIALIZER;

		List myList = malloc(sizeof(L));
		myList->size = 0;
		myList->elements = NULL;
		myList->mutex = m;
		myList->listingCriteria = aListingCriteria;
		myList->equalityCriteria = aEqualityCriteria;
		myList->sortingCriteria = aSortingCriteria;
		return myList;
	}

	Node * commons_list_buildNode(Object data){
		Node * node = malloc(sizeof(Node));
		node->data = data;
		node->next = NULL;
		return node;
	}

	void commons_list_addNode(List l ,Object data){

		pthread_mutex_lock(&(l->mutex));

		Node * n = commons_list_buildNode(data);

		if(l->elements == NULL){
			l->elements = n;
		}else{
			Node * aux = l->elements;

			while(aux->next != NULL
					&& l->sortingCriteria( ((Node*)aux->next)->data , n->data)){
				aux = aux->next;
			}

//			if(aux->next == NULL){
//				aux->next = n;
//			}else{
//				n->next = aux->next;
//				aux->next = n;
//			}

			if(!l->sortingCriteria(aux->data , n->data)){
				n->next = aux;
				l->elements = n;
			}else{
				if( aux->next != NULL){
					n->next = aux->next;
				}
				aux->next = n;
			}
		}

		l->size++;

		pthread_mutex_unlock(&(l->mutex));
	}

	void commons_list_listAllNodes(List aList){
		Node * aux = aList->elements;
		while(aux != NULL){
			aList->listingCriteria(aux->data);
			aux = aux->next;
		}
	}

	void commons_list_removeNode(List aList, Object data , void (*removeOperation)(Object)){

		pthread_mutex_lock(&(aList->mutex));

		Node * prev = NULL;
		Node * act = aList->elements;

		while(act != NULL && !aList->equalityCriteria(act->data , data)){
			prev = act;
			act = act->next;
		}

		if(act != NULL){
			if(prev != NULL){
				prev->next = act->next;
			}else{
				aList->elements = act->next;
			}

			removeOperation(act->data);
			aList->size--;
		}

		pthread_mutex_unlock(&(aList->mutex));
	}

	Boolean commons_list_containsNode(List list, Object element ){
		Iterator * ite = commons_iterator_buildIterator(list);
		while(commons_iterator_hasMoreElements(ite)){
			Object elem = commons_iterator_next(ite);
			if(list->equalityCriteria(elem , element)){
				return TRUE;
			}
		}
		return FALSE;
	}


	Object commons_list_getNodeByCriteria(List list , Boolean (*selectionCriteria)(Object)){
		Iterator * ite = commons_iterator_buildIterator(list);
		while(commons_iterator_hasMoreElements(ite)){
			Object elem = commons_iterator_next(ite);
			if(selectionCriteria(elem)){
				return elem;
			}
		}
		return NULL;
	}


	List commons_list_buildStringsList(){

		void listing(String c){}
		Boolean equal(String c , String d){
			return commons_string_equals(c , d);
		}

		return commons_list_buildList(
				(void (*)(Object)) listing ,
				(Boolean (*)(Object, Object)) equal ,
				commons_list_ORDER_ALWAYS_LAST);
	}

	List commons_list_buildStringsListWithValues(int count , ...){

		List list = commons_list_buildStringsList();

		va_list ap;
		va_start(ap, count);
		int i; for(i=0 ; i<count ; i++){
			String currentElement = va_arg(ap, char*);
			if(currentElement != NULL)
				commons_list_addNode(list , currentElement );
		}
		va_end(ap);

		return list;
	}

	String commons_list_formatToPreetyString(List l){
		String formatedList = commons_list_serialize(l , ", ");
		if(formatedList == NULL)
			return "(no hay elementos)";
		else
			return formatedList;
	}

	String commons_list_serialize(List list , String separator){
		Iterator * ite = commons_iterator_buildIterator(list);
		String serialization = NULL;
		while(commons_iterator_hasMoreElements(ite)){
			String element = commons_iterator_next(ite);
			if(serialization == NULL){
				serialization = strdup(element);
			}else{
				serialization = commons_string_concatAll(3 , serialization , separator , strdup(element));
			}
		}
		return serialization;
	}

	List commons_list_tokenize(String tokens , char separator){

		if(tokens == NULL)
			return NULL;

		List list = commons_list_buildStringsList();

		int mark1=0;
		int i;
		String token;
		for(i=0 ; i<strlen(tokens) ; i++){

			if(tokens[i] == separator){
				token = strndup(tokens + mark1 , i-mark1);
				mark1 = i+1;

				if(!commons_string_equals(commons_string_trim(token) , "" ))
					commons_list_addNode(list , token);
			}

		}
		if(i==strlen(tokens)){
			token = strndup(tokens + mark1 , strlen(tokens) - mark1);

			if(!commons_string_equals(commons_string_trim(token) , "" ))
				commons_list_addNode(list , token);
		}

		return (list->size > 0) ? list : NULL;
	}
	List commons_list_deserialize(String listInString ){
		return commons_list_tokenize(listInString , ',');
	}


	int commons_list_getSize(List l){
		if(l == NULL)
			return -1;
		else return l->size;
	}



	Iterator * commons_iterator_buildIterator(List list){
		if(list == NULL)
			return NULL;

		Iterator * iterator = (Iterator *)malloc(sizeof(Iterator));
		iterator->elements = list->elements;
		return iterator;
	}

	Boolean commons_iterator_hasMoreElements(Iterator * i){
		if(i == NULL)
			return FALSE;
		return (i->elements != NULL);
	}

	Object commons_iterator_next(Iterator * i ){

		if( i == NULL || i->elements == NULL)
			return NULL;

		Object data = i->elements->data;
		i->elements = i->elements->next;
		return data;
	}




	void commons_list_removeList(List list , void (*removeOperation)(Object)){

		Iterator * ite = commons_iterator_buildIterator(list);

		while(commons_iterator_hasMoreElements(ite)){
			Object obj = commons_iterator_next(ite);
			commons_list_removeNode(list , obj , removeOperation);
		}

		free(ite);
		free(list);
	}


