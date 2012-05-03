/*
 * unit-test-commons.h
 *
 *  Created on: 13/08/2011
 *      Author: gonzalo
 */

#include <CUnit/Basic.h>
#include <CUnit/Automated.h>


#ifndef UNIT_TEST_COMMONS_H_
#define UNIT_TEST_COMMONS_H_

	typedef CU_pSuite TestSuite;
	typedef CU_pTest TestCase;
	typedef CU_ErrorCode TestError;
	typedef CU_TestFunc TestCallBackFunction;
	typedef CU_InitializeFunc TestCallBackInitialize;
	typedef CU_CleanupFunc TestCallBackFinalize;


	int unit_testing_loadAll();

	TestError unit_testing_initialize();

	void unit_testing_finalize();

	TestSuite unit_testing_createSuite(char * suiteName ,
				TestCallBackInitialize pInit,
				TestCallBackFinalize pClean);

	TestCase unit_testing_addToSuite(TestSuite pSuite,
				const char * testName,
				TestCallBackFunction pTestFunc);

	void unit_testing_runAll();



	char * unit_testing_getProjectName();

	void suite_setup();

	void suite_tearDown();

#endif /* UNIT_TEST_COMMONS_H_ */
