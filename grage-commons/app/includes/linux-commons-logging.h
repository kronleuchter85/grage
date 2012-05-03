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
 *	@FILE: 	log.h
 *	@AUTOR: Facundo Viale
 *	@VERSION:	2.0	(5/08/2011)
 *					- Improvements
 *				1.5	(29/08/2009)
 *					- Full Revision
 */

#ifndef OLD_LOG_H_
#define OLD_LOG_H_

	#define _XOPEN_SOURCE 500

	#include <stdio.h>
	#include <pthread.h>
	#include <stdarg.h>

	#define LOG_MAX_MESSAGE_LENGTH 2048

	typedef enum {
		M_CONSOLE_DISABLE	= 1,
		M_CONSOLE_ENABLE 	= 2
	} e_console_mode;

	typedef enum {
		ERROR 		= 1,
		WARNING 	= 2,
		INFO 		= 4,
		DEBUG 		= 8
	} e_message_level;

	typedef struct {
		FILE* file;
		char *program_name;
		int program_pid;
		unsigned char log_levels;
		e_console_mode console_mode;
		pthread_mutex_t mutex;
	} log_config_t;

	int 	log_create(const char *program_name, const char* file_name, unsigned char log_levels, e_console_mode console_mode);
	int 	log_info_t(const char *format, ... );
	int 	log_info(const char *thread_name, const char *format, ... );
	int 	log_warning_t(const char *format, ... );
	int 	log_warning(const char *thread_name, const char *format, ... );
	int 	log_error_t(const char *format, ... );
	int 	log_error(const char *thread_name, const char *format, ... );
	int 	log_debug_t(const char *format, ... );
	int 	log_debug(const char *thread_name, const char *format, ... );
	#define log_has_level(log, level)	((log->log_levels & level) == level)
	void 	log_destroy(void);
	log_config_t * log_getLogConfig(void);
	void 	log_setLogConfig(log_config_t * logConfig);

#endif /*OLD_LOG_H_*/
