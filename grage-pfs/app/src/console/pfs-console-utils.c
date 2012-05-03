/*
 * pfs-console-utils.c
 *
 *  Created on: 28/09/2011
 *      Author: joaquin
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux-commons.h>

String pfs_console_utils_parseCMD(char * comando){
	int i=0;
	int j=0;
	String cmd;
	cmd = (String)malloc(255);

	memset(cmd, '\0', 255);

	while(comando[i] != '(' && comando[i] != '\n' && comando[i] != ' ' && i <= 250){
		cmd[j] = comando[i];
		i++;
		j++;
	}

	return cmd;
}

String pfs_console_utils_get_cmd_parameter(char * comando, int cmd_length){
	int i=cmd_length + 1;
	int j=0;
	String parametro;
	parametro = (String)malloc(255);
	memset(parametro, '\0', 255);

	while(comando[i] != ')' && i < (250 - cmd_length)){
		parametro[j] = comando[i];
		i++;
		j++;
	}
	return parametro;
}

