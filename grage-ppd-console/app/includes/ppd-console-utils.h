/*
 * ppd-console-utils.h
 *
 *  Created on: Nov 12, 2011
 *      Author: seba
 */

#ifndef PPD_CONSOLE_UTILS_H_
#define PPD_CONSOLE_UTILS_H_

MessageConsolePPD armarMensaje(uint32 messageID, uint32 pista, uint32 sector, uint32 timeMiliSeconds);
void ppd_console_utils_set_cantidadSectoresPorCilindro(int cant);
int ppd_console_utils_get_cantidadSectoresPorCilindro();
int ppd_console_utils_get_cilinder_from_sector(int sector);
int ppd_console_utils_get_sectorofcilinder_from_sector(int sector);
int ppd_console_utils_get_sector_from_sectorofcilinder(int sector, int cilinder);
#endif /* PPD_CONSOLE_UTILS_H_ */
