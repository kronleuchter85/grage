/*
 * linux-commons-strings.c
 *
 *  Created on: 17/08/2011
 *      Author: gonzalo
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdarg.h>
#include <strings.h>

#include "linux-commons-strings.h"


	Boolean commons_string_equals(String s1 , String s2){
		if(s1 == NULL || s2 == NULL)
			return FALSE;
		return !strcmp(s1 , s2);
	}


	String commons_string_cloneString(String string){
		if(string == NULL)
			return NULL;
		return strdup(string);
	}

	String commons_string_fillIntString(int v , int spacesToFix){
		String result = commons_misc_intToString(v);

		while(strlen(result) < spacesToFix)
			result = commons_string_concat("0" , result);

		return result;
	}


	int commons_string_getIndexOf(String string , char c){
		if(strchr(string , c) != NULL){
			int lenT = strlen(string);
			int len2 = strlen(strchr(string , c));
			return lenT - len2;
		}else {
			return -1;
		}
	}

	String commons_string_subString(String string , int startIndex , int endIndex){
		if(startIndex == 0)
			return strndup(string + startIndex , endIndex +1);
		else
			return strndup(string + startIndex , endIndex +1 -startIndex);
	}

	Boolean commos_string_isEmpty(String string){
		return commons_string_equals(commons_string_trim(string),"");
	}

	/*
	 * Funcion trim para cuando se pasa como parametro
	 * un string estatico (inmutable), crea memoria dinamica.
	 */
	String commons_string_trimd(String string){
		return commons_string_trim(strdup(string));
	}

	/*
	 * Realiza el trim de una cadena eliminando los espacios,
	 * un puntero con misma direccion al puntero enviado por parametro.
	 * Si se necesita usar con strings estaticos hay que hacer
	 * commons_string_trim(strdup("string estatico")) o usar la funcion commons_string_trimd
	 * sino da Segmentation fault.
	 */
	String commons_string_trim(String string){
		int start = 0;
		int end = strlen(string) -1;
		String aux = NULL;

		if(commons_string_equals(string , ""))
			return string;

		while(end > 0 && (string[end] == SP || string[end] == EOL)){
			string[end] = EOS;
			end--;
		}

		while(string[start] == SP)
			start++;

		aux = (String)malloc(strlen(&string[start]) + 1);

		strcpy(aux,&string[start]);

		strcpy(string,aux);

		free(aux);

		return string;
	}

	String commons_string_concat(String s1 , String s2){
		if(s1 == NULL || s2 == NULL)
			return NULL;
		String buffer = (String)malloc(strlen(s1) + strlen(s2) +1);
		sprintf(buffer, "%s%s" , s1 , s2);
		return commons_string_trim(buffer);
	}

	String commons_string_concatAll(int n , ...){
		String aux_buffer = (String) malloc(SYSTEM_STRING_CONCATENATION_SIZE*sizeof(char));
		String result = NULL;
		bzero(aux_buffer , SYSTEM_STRING_CONCATENATION_SIZE);

		va_list ap;
		va_start(ap, n);
		int i; for(i=0 ; i<n ; i++){
			strcat(aux_buffer, va_arg(ap, char*));
		}
		va_end(ap);

		commons_string_trim(aux_buffer);

		// utilizo la memoria suficiente par la cadena q arme.
		result = (String) malloc((strlen(aux_buffer)+1)*sizeof(char));
		bzero(result,strlen(aux_buffer)+1);
		strcpy(result,aux_buffer);

		free(aux_buffer);

		return result;
	}



	/*
	 * Determina si la primera cadena empieza con la segunda
	 */
	Boolean commons_string_startsWith(String s1 , String s2){
		int i=0; for( ; i<strlen(s2) ; i++){
			if(toupper(s1[i]) != toupper(s2[i]))
				return FALSE;
		}
		return TRUE;
	}

	/*
	 * Determina si la primera cadena termina con la segunda
	 */
	Boolean commons_string_endsWith(String s1 ,String s2){

		if(s1 == NULL || s2 == NULL)
			return FALSE;

		if(!strcmp(s1 , s2))
			return TRUE;

		if(strlen(s1) == 0 || strlen(s2) == 0)
			return FALSE;

		int i;
		for(i = 0 ; i<strlen(s2) ; i++){
			int index1 = strlen(s1)-(1+i);
			int index2 = strlen(s2)-(1+i);
			if(toupper(s1[index1]) != toupper(s2[index2]))
				return FALSE;
		}
		return TRUE;
	}

	/*
	 * s1 - string contenedor
	 * s2 - lo que hay q reemplazar
	 * s3 - con que hay q reemplazarlo
	 */
	String commons_string_replaceAll(String s1 , String s2 , String s3){

		int count = 0;
		String founds = s1;
		while((founds = strstr(founds , s2)) != NULL){
			founds +=1;
			count++;
		}

		if(count == 0)
			return s1;

		int size = 1 + strlen(s1) + count * (strlen(s3) - strlen(s2));
		String buffer = (String)malloc(size*sizeof(char));
		bzero(buffer, size);

		String found = s1;
		int pos = 0;
		String last;
		while( (found = strstr(found , s2)) != NULL ){
			int cal = strlen(s1) - strlen(found) - pos;
			strncat(buffer , s1 + pos , cal);
			strcat(buffer , s3);
			pos += cal + strlen(s2);
			found = found +strlen(s2);
			last = found;
		}

		if(strcmp(last , ""))
			strcat(buffer , last);

		return buffer;
	}
