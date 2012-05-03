/*
 * ppd-utils.h
 *
 *  Created on: Nov 3, 2011
 *      Author: seba
 */

#ifndef PPD_UTILS_H_
#define PPD_UTILS_H_
void ppd_utils_set_cantidadSectoresPorCilindro(int cant);
int ppd_utils_get_cantidadSectoresPorCilindro();
int ppd_utils_get_cilinder_from_sector(int sector);
int ppd_utils_get_sectorofcilinder_from_sector(int sector);
int ppd_utils_get_sector_from_sectorofcilinder(int sector, int cilinder);
uint32_t ppd_utils_getSectorsCount();
#endif /* PPD_UTILS_H_ */
