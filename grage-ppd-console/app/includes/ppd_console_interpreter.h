/*
 * ppd_console_interpreter.h
 *
 *  Created on: 10/10/2011
 *      Author: joaquin
 */

#include "linux-commons.h"

#ifndef PPD_CONSOLE_INTERPRETER_H_
#define PPD_CONSOLE_INTERPRETER_H_

	String ppd_console_parseCMD(char * comando);
	String ppd_console_get_cmd_parameter(char * comando, uint32 cmd_length);
	uint32 ppd_console_obtenerNroParametros(char * comando);
	void ppd_console_llenarVector(char * comando, uint32 nro_parametros);
	void ppd_console_info();
	void ppd_console_clean(uint32 clusterInicial, uint32 clusterFinal);
	void ppd_console_trace(uint32 sectorPedido);
	void ppd_console_interpreter();
#endif /* PPD_CONSOLE_INTERPRETER_H_ */
