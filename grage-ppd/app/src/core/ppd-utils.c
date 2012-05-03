/*
 * ppd-utils.c
 *
 *  Created on: 19/11/2011
 *      Author: gonzalo
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <grage-commons.h>


	int cantidadSectoresPorCilindro;
	void ppd_utils_set_cantidadSectoresPorCilindro(int cant){
		cantidadSectoresPorCilindro=cant;
	}
	int ppd_utils_get_cantidadSectoresPorCilindro(){
		return cantidadSectoresPorCilindro;
	}

	int ppd_utils_get_cilinder_from_sector(int sector){
		return (sector/cantidadSectoresPorCilindro);
	}
	int ppd_utils_get_sectorofcilinder_from_sector(int sector){
		return (sector-(sector/cantidadSectoresPorCilindro)*cantidadSectoresPorCilindro);
	}
	int ppd_utils_get_sector_from_sectorofcilinder(int sector, int cilinder){
		return (cilinder*cantidadSectoresPorCilindro)+sector;
	}
