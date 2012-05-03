/*
 * Copyright (c) 2011, C-Talks and/or its owners. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 only, as
 * published by the Free Software Foundation.
 *
 * This code is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * version 2 for more details (a copy is included in the LICENSE file that
 * accompanied this code).
 *
 * You should have received a copy of the GNU General Public License version
 * 2 along with this work; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Please contact C-Talks owners, Matias Dumrauf or Facundo Viale
 * or visit https://sites.google.com/site/utnfrbactalks/ if you need additional
 * information or have any questions.
 */

/*
 *	@FILE: 	log.c
 *	@AUTOR: Facundo Viale
 *	@VERSION:	2.0	(5/08/2011)
 *					- Improvements
 *				1.5	(29/08/2009)
 *					- Full Revision
 */

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/timeb.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/prctl.h>
#include "linux-commons-logging.h"

//Variable global de configuracion de log.
log_config_t * logConfig;

/*
 * Private Functions
 */
inline static int log_vwrite(log_config_t *log, const char *thread_name, e_message_level level, const char* format, va_list args_list);
static char *log_error_as_string(e_message_level type);

/*
 * Public Functions
 */

/*
 * @NAME: log_create
 * @DESC: Crea un log
 */
int log_create(const char *program_name, const char* file_name, unsigned char log_levels, e_console_mode console_mode) {
	pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
	log_config_t* logCfg = malloc(sizeof(log_config_t));

	logCfg->file = NULL;

	if (file_name != NULL && (logCfg->file = fopen(file_name, "a")) == NULL) {
		free(logCfg);
		return -1;
	}

	logCfg->mutex = mutex;

	logCfg->console_mode = console_mode;

	if( program_name != NULL ){
		logCfg->program_name = program_name;
	} else {
		logCfg->program_name = "";
	}

	logCfg->program_pid = getpid();

	logCfg->log_levels = log_levels;

	log_setLogConfig(logCfg);

	return 0;
}

int log_info_t(const char *format, ... ){
	va_list args_list;
	char * thread_name = calloc(20,sizeof(char));
	va_start(args_list, format);
	sprintf(thread_name,"th-%d",pthread_self());
	int ret = log_vwrite(log_getLogConfig(), thread_name, INFO, format, args_list);
	free(thread_name);
	return ret;
}

/*
 * @NAME: log_info
 * @DESC: Escribe en el log un mensaje con el nivel info
 */
int log_info(const char *thread_name, const char* format, ... ) {
	va_list args_list;
	va_start(args_list, format);
	return log_vwrite(log_getLogConfig(), thread_name, INFO, format, args_list);
}


int log_warning_t(const char *format, ... ){
	va_list args_list;
	char * thread_name = calloc(20,sizeof(char));
	va_start(args_list, format);
	sprintf(thread_name,"th-%d",pthread_self());
	int ret = log_vwrite(log_getLogConfig(), thread_name, WARNING, format, args_list);
	free(thread_name);
	return ret;
}
/*
 * @NAME: log_warning
 * @DESC: Escribe en el log un mensaje con el nivel warning
 */
int log_warning(const char *thread_name, const char* format, ... ) {
	va_list args_list;
	va_start(args_list, format);
	return log_vwrite(log_getLogConfig(), thread_name, WARNING, format, args_list);
}

int log_error_t(const char *format, ... ){
	va_list args_list;
	char * thread_name = calloc(20,sizeof(char));
	va_start(args_list, format);
	sprintf(thread_name,"th-%d",pthread_self());
	int ret = log_vwrite(log_getLogConfig(), thread_name, ERROR, format, args_list);
	free(thread_name);
	return ret;
}

/*
 * @NAME: log_error
 * @DESC: Escribe en el log un mensaje con el nivel error
 */
int log_error(const char *thread_name, const char* format, ... ) {
	va_list args_list;
	va_start(args_list, format);
	return log_vwrite(log_getLogConfig(), thread_name, ERROR, format, args_list);
}

int log_debug_t(const char *format, ... ){
	va_list args_list;
	char * thread_name = calloc(20,sizeof(char));
	va_start(args_list, format);
	sprintf(thread_name,"th-%d",pthread_self());
	int ret = log_vwrite(log_getLogConfig(), thread_name, DEBUG, format, args_list);
	free(thread_name);
	return ret;
}

/*
 * @NAME: log_debug
 * @DESC: Escribe en el log un mensaje con el nivel debug
 */
int log_debug(const char *thread_name, const char* format, ... ) {

	va_list args_list;
	va_start(args_list, format);
	return log_vwrite(log_getLogConfig(), thread_name, DEBUG, format, args_list);
}

/*
 * @NAME: log_vwrite
 * @DESC: Genera el log!
 */
inline static int log_vwrite(log_config_t *log, const char *thread_name, e_message_level level, const char* format, va_list args_list) {
	time_t log_time;
	struct tm *log_tm;
	struct timeb tmili;
	char str_time[128];
	unsigned int thread_id = pthread_self();
	char *str_type = log_error_as_string(level);
	char logbuff[LOG_MAX_MESSAGE_LENGTH + 100];char
	msgbuff[LOG_MAX_MESSAGE_LENGTH];

	if (!log_has_level(log, level)) {
		return 1;
	}

	pthread_mutex_lock(&log->mutex);

	vsprintf(msgbuff, format, args_list);

	if ((log_time = time(NULL)) == -1) {
		perror("[[CRITICAL]] :: Error getting date!");
		return 0;
	}

	log_tm = localtime(&log_time);
	strftime(str_time, 127, "%H:%M:%S", log_tm);

	if (ftime(&tmili)) {
		perror("[[CRITICAL]] :: Error getting time!");
		return 0;
	}

	sprintf(logbuff, "%s:%hu %s/%d %s/%u %s: %s\n", str_time, tmili.millitm, log->program_name, log->program_pid, thread_name, thread_id, str_type, msgbuff);

	if ( log->file != NULL ) {
		fprintf(log->file, "%s", logbuff);
		fflush(log->file);
	}

	if ( log->console_mode == M_CONSOLE_ENABLE ) {
		printf("%s", logbuff);
		fflush(stdout);
	}
	pthread_mutex_unlock(&log->mutex);

	return 1;
}

/*
 * @NAME: log_errorTypeAsString
 * @DESC: Convierte un tipo numerico de error a su version en string
 */
static char *log_error_as_string(e_message_level type) {
	if (type == ERROR)
		return "ERROR";
	if (type == WARNING)
		return "WARNING";
	if (type == INFO)
		return "INFO";
	if (type == DEBUG)
		return "DEBUG";
	return "UNKNOW_MSG_TYPE";
}

/*
 * @NAME: log_destroy
 * @DESC: Destruye el puntero al log y cierra el archivo
 */
void log_destroy(void) {
	log_config_t * logCfg = log_getLogConfig();
	if (logCfg != NULL) {
		fclose(logCfg->file);
		pthread_mutex_destroy(&(logCfg->mutex));
		free(logCfg);
	}
}

log_config_t * log_getLogConfig(void){
	if(logConfig == NULL){
		fprintf(stderr, "Debe inicializar el log antes de usarlo.");
	}
	return logConfig;
}

void log_setLogConfig(log_config_t *aLogConfig){
	logConfig = aLogConfig;
}
