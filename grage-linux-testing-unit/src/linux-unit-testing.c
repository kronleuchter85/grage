/*
 * unit-testing-commons.c
 *
 *  Created on: 13/08/2011
 *      Author: gonzalo
 */

#include <stdlib.h>

#include "linux-testing-unit.h"


	TestError unit_testing_initialize(){
		return CU_initialize_registry();
	}


	void unit_testing_finalize(){
		CU_cleanup_registry();
	}


	TestSuite unit_testing_createSuite(char * suiteName ,
			TestCallBackInitialize pInit,
			TestCallBackFinalize pClean){

		TestSuite pSuite = CU_add_suite(suiteName, pInit, pClean);
		if (NULL == pSuite) {
			CU_cleanup_registry();
			return NULL;
		}

		return pSuite;

	}


	TestCase unit_testing_addToSuite(TestSuite pSuite,
			const char * testName,
			TestCallBackFunction pTestFunc){

		TestCase test = CU_add_test(pSuite, testName, pTestFunc);
		if(test == NULL){
			unit_testing_finalize();
			return NULL;
		}

		return test;
	}


	void unit_testing_runAll(){
		CU_basic_set_mode(CU_BRM_VERBOSE);
		CU_basic_run_tests();

		/*
		 * Las siguientes lineas determinan el archivo de salida.
		 * El metodo unit_testing_getProjectName es implementado en el projecto
		 * que implementa el metodo unit_testing_loadAll para determinar el nombre
		 * del archivo de resultados generado.
		 *
		 * pd: fabi se la come
		 *
		 */
		CU_set_output_filename( unit_testing_getProjectName() );
		CU_list_tests_to_file();
		CU_automated_run_tests();
	}


