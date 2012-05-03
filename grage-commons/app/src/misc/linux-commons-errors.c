/*
 * linux-errors.c
 *
 *  Created on: 30/06/2011
 *      Author: gonzalo
 */
#include <stdlib.h>

#include "linux-commons.h"
#include "linux-commons-errors.h"

	Boolean commons_errors_hasError(RuntimeErrorValidator * r){
		return (r->runtimeStatus == RUNTIME_STATUS_FAULT);
	}


	RuntimeErrorValidator * commons_errors_buildSuccessValidator(){
		RuntimeErrorValidator * r = (RuntimeErrorValidator *) malloc(sizeof(RuntimeErrorValidator));
		r->runtimeStatus = RUNTIME_STATUS_SUCCESS;
		r->errorDescription = NULL;
		return r;
	}


	RuntimeErrorValidator * commons_errors_buildFaultValidator(char * d){
		RuntimeErrorValidator * r = (RuntimeErrorValidator *) malloc(sizeof(RuntimeErrorValidator));
		r->runtimeStatus = RUNTIME_STATUS_FAULT;
		r->errorDescription = d;
		return r;
	}


	void commons_errors_setError(RuntimeErrorValidator * v , char * m){
		if(v != NULL){
			v->runtimeStatus = RUNTIME_STATUS_FAULT;
			v->errorDescription = m;
		}
	}

