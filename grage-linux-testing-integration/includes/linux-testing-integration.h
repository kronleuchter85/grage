/*
 * linux-integration-testing.h
 *
 *  Created on: 28/08/2011
 *      Author: gonzalo
 */

#ifndef LINUX_INTEGRATION_TESTING_H_
#define LINUX_INTEGRATION_TESTING_H_

	enum IntegrationTestStatus{
		SUCCESS , FAULT
	};


	int commons_testing_integration_doInitialize();
	void commons_testing_integration_doFinalize();
	int commons_testing_integration_doRun();

#endif /* LINUX_INTEGRATION_TESTING_H_ */
