/*
 * test-linux-commons-errors.c
 *
 *  Created on: 08/08/2011
 *      Author: gonzalo
 */

#include <stdlib.h>

#include "linux-testing-unit.h"


	int main(){

		if (CUE_SUCCESS != unit_testing_initialize())
			return CU_get_error();
		suite_setup();

		if(unit_testing_loadAll() == EXIT_FAILURE){
			unit_testing_finalize();
			return CU_get_error();
		}

		unit_testing_runAll();

		suite_tearDown();
		unit_testing_finalize();

		return EXIT_SUCCESS;
	}
