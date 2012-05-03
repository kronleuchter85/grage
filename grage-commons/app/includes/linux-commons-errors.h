/*
 * linux-commons-errors.h
 *
 *  Created on: 30/06/2011
 *      Author: gonzalo
 */
#include "linux-commons.h"

#ifndef LINUX_ERRORS_H_
#define LINUX_ERRORS_H_

	enum RuntimeStatus {
		RUNTIME_STATUS_SUCCESS , RUNTIME_STATUS_FAULT
	};

	typedef struct {
		int runtimeStatus;
		String errorDescription;
	} RuntimeErrorValidator;


	Boolean commons_errors_hasError(RuntimeErrorValidator * );
	RuntimeErrorValidator * commons_errors_buildSuccessValidator();
	RuntimeErrorValidator * commons_errors_buildFaultValidator(String);
	void commons_errors_setError(RuntimeErrorValidator * , String );

#endif /* LINUX_ERRORS_H_ */
