/*
 * linux-commons.c
 *
 *  Created on: 05/06/2011
 *      Author: gonzalo
 */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdarg.h>
#include <strings.h>

#include <math.h>

#include "linux-commons.h"
#include "grage-commons.h"

	int commons_misc_digitsCount(int i){
		return (i == 0) ? 1 : log10(i) + 1;
	}


	String commons_misc_intToString(int i){
		/*char * content = malloc(SYSTEM_INT_CONVERSION_MAX_DIGITS_COUNT);
		bzero(content, SYSTEM_INT_CONVERSION_MAX_DIGITS_COUNT);
		sprintf(content , "%i" , i);
		content = strndup(content , strlen(content));
		return content;
		 */

		String content = malloc(commons_misc_digitsCount(i));
		sprintf(content , "%i" , i);
		return content;

	}


	String commons_misc_longToString(long int l){
		char * s = (char*)malloc(sizeof(long int));
		sprintf( s, "%lu" , l);
		return s;
	}


	String commons_misc_generateRandomKey(int size){

		int n;
		char * p;
		char * buf;
		if (size<=0) return NULL;
		buf = malloc(size * sizeof (char));
		p=buf;

		while (p < (buf + size -1)) {
			n = rand() % 127;
			while (!(((n>=65) && (n<=90)) || ((n>=97) && (n<=122)) || ((n>=48) && (n<=57))))
				n = rand() % 127;
			*p = (char) n;
			p++;
		}
		while (!(((n>=65) && (n<=90)) || ((n>=97) && (n<=122)) || ((n>=48) && (n<=57))))
			n = rand() % 127;
		*p = (char) n;

		return buf;
	}


	void commons_misc_lockThreadMutex(pthread_mutex_t * m){
		pthread_mutex_lock(m);
	}

	void commons_misc_unlockThreadMutex(pthread_mutex_t * m){
		pthread_mutex_unlock(m);
	}


	/*
	 * Libera y asigna null a un puntero.
	 */
	void commons_misc_doFreeNull(void **pointer){
		if(*pointer == NULL){
			free(*pointer);
			*pointer = NULL;
		}
	}



	Time commons_misc_getTimeDiff( Time *end_time, Time *start_time) {
		Time temp_diff;

		temp_diff.tv_sec = end_time->tv_sec - start_time->tv_sec;
		temp_diff.tv_usec = end_time->tv_usec - start_time->tv_usec;

		/* Using while instead of if below makes the code slightly more robust. */

		while (temp_diff.tv_usec < 0) {
			temp_diff.tv_usec += 1000000;
			temp_diff.tv_sec -= 1;
		}

		return temp_diff;
	}


	Time commons_misc_getCurrentTime(){
		Time time;
		gettimeofday(&time, NULL);
		return time;
	}



	DiskSector commons_buildDiskSector(){
		DiskSector disk;
		disk.sectorNumber = -1;
		bzero(disk.sectorContent , sizeof(disk.sectorContent));

		return disk;
	}
