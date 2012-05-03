/*
 * ppd-console-launcher.c
 *
 *  Created on: 10/10/2011
 *      Author: joaquin
 */

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <linux-commons-logging.h>
#include "ppd_console_interpreter.h"
#define CONF_LOGS "/opt/grage-repository/logs/ppd-console.log"
#define SOCK_PATH "/opt/grage-repository/.echo_socket"

int socketPPD;
void ppd_console_launcher_setSocketPPD(int ls){
	socketPPD = ls;
}
int ppd_console_launcher_getSocketPPD(){
	return socketPPD;
}

int ppd_console_connect(){
	int s, len;
	char buffer[400];
	struct sockaddr_un remote;

	if ((s = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
	perror("socket");
		return 0;
    }

    //printf("Trying to connect...");

    remote.sun_family = AF_UNIX;
    strcpy(remote.sun_path, SOCK_PATH);
    len = strlen(remote.sun_path) + sizeof(remote.sun_family);

    if (connect(s, (struct sockaddr *)&remote, len) == -1) {
        perror("connect");
        return s;
    }
	return s;
}

	void ppd_console_launcher_initialize(){
		log_create("ppd-console",CONF_LOGS,INFO|WARNING|ERROR|DEBUG,M_CONSOLE_DISABLE);
		ppd_console_launcher_setSocketPPD(ppd_console_connect());

	}

	void ppd_console_launcher_exit() {
		log_destroy();
	}

	int main(int argc, char *args[]){
		puts("[ Consola iniciada ]");
		ppd_console_launcher_initialize();
		ppd_console_interpreter();
		ppd_console_launcher_exit();

		return EXIT_SUCCESS;
	}
