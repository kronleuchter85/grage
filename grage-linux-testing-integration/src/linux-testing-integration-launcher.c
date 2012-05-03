/*
 * linux-integration-testing-launcher.c
 *
 *  Created on: 28/08/2011
 *      Author: gonzalo
 */

#include <stdlib.h>

#include "linux-testing-integration.h"

	int main(void){

		if( FAULT == commons_testing_integration_doInitialize()){
			return EXIT_FAILURE;
		}

		int status = commons_testing_integration_doRun();

		commons_testing_integration_doFinalize();

		if(status == FAULT){
			return EXIT_FAILURE;
		}else{
			return EXIT_SUCCESS;
		}
	}
