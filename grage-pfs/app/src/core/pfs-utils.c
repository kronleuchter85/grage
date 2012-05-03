/*
 * pfs-utils.c
 *
 *  Created on: 06/11/2011
 *      Author: utn_so
 */


#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unicode/utypes.h>
#include <unicode/ustring.h>
#include "pfs-utils.h"

//! Byte swap unsigned short
uint16_t swap_uint16(uint16_t val) {
	return (val << 8) | (val >> 8);
}

//! Byte swap signed short
int16_t swap_int16(int16_t val) {
	return (val << 8) | ((val >> 8) & 0xFF);
}

//! Byte swap unsigned int
uint32_t swap_uint32(uint32_t val) {
	val = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0xFF00FF);
	return (val << 16) | (val >> 16);
}

//! Byte swap int
int32_t swap_int32(int32_t val) {
	val = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0xFF00FF);
	return (val << 16) | ((val >> 16) & 0xFFFF);
}

uint16_t *unicode_utf8_to_utf16(const char *src_utf8, const size_t src_utf8size, size_t *dest_utf16size) {
	UErrorCode error = U_ZERO_ERROR;

	uint16_t utf16_buff[(src_utf8size + 1) * 2];
	uint16_t *utf16 = NULL;

	u_strFromUTF8(utf16_buff, (src_utf8size + 1) * 2, dest_utf16size, src_utf8, src_utf8size, &error);

	if (error != U_ZERO_ERROR) {
		return NULL;
	}

	utf16 = malloc(*dest_utf16size);
	memcpy(utf16, utf16_buff, *dest_utf16size);

	return utf16;
}

int unicode_utf8_to_utf16_inbuffer(const char *src_utf8, const size_t src_utf8size, uint16_t *dest_utf16, size_t *dest_utf16size) {
	UErrorCode error = U_ZERO_ERROR;

	u_strFromUTF8(dest_utf16, (src_utf8size + 1) * 2, dest_utf16size, src_utf8, src_utf8size, &error);

	if (error != U_ZERO_ERROR) {
		return 0;
	}

	return 1;
}

char *unicode_utf16_to_utf8(const uint16_t *src_utf16, const size_t src_utf16size, size_t *dest_utf8size) {
	UErrorCode error = U_ZERO_ERROR;

	char utf8_buff[src_utf16size + 1];char
	*utf8 = NULL;

	u_strToUTF8(utf8_buff, src_utf16size + 1, dest_utf8size, src_utf16, src_utf16size, &error);

	if (error != U_ZERO_ERROR) {
		return NULL;
	}

	utf8 = malloc(*dest_utf8size);
	memcpy(utf8, utf8_buff, *dest_utf8size);

	return utf8;
}

int unicode_utf16_to_utf8_inbuffer(const uint16_t *src_utf16, const size_t src_utf16size, char* dest_utf8, size_t *dest_utf8size) {
	UErrorCode error = U_ZERO_ERROR;

	u_strToUTF8(dest_utf8, src_utf16size + 1, dest_utf8size, src_utf16, src_utf16size, &error);

	if (error != U_ZERO_ERROR) {
		return 0;
	}

	return 1;
}

char** string_split2( char *str, char delimitor ){
	char **array;
	int elementsAmount = 0;

	{
		int cont;
		for(cont=0; str[cont] != '\0'; cont++ ){
			if( str[cont] == delimitor)	elementsAmount++;
		}
		elementsAmount++;
	}

	{
		array = calloc(elementsAmount+1, sizeof(char*) );
	}

	{
		int pibot, length, cont, elementIndex;
		for(cont=0, pibot=0, length=0, elementIndex=0; elementIndex != elementsAmount ; cont++ ){
			if( str[cont] == delimitor || str[cont] == '\0' ){
				array[elementIndex] = malloc(length + 1);
				if( length > 0 ){
					strncpy(array[elementIndex], &str[pibot], length);
					array[elementIndex][length] = '\0';
				} else {
					array[elementIndex][0] = '\0';
				}
				pibot = pibot + length + 1;
				length = 0;
				elementIndex++;
			} else {
				length++;
			}
		}
		array[elementIndex] = NULL;
	}
	return array;
}

void string_split3( char *str, char delimitor, char **result ){
	int pibot, length, cont, elementIndex = 0;

	for(cont=0, pibot=0, length=0; str[cont] != '\0' ; cont++ ){

		if( str[cont] == delimitor ){
			if( length > 0 ){
				memcpy(result[elementIndex], &str[pibot], length);
				result[elementIndex][length] = '\0';
			} else {
				result[elementIndex][0] = '\0';
			}
			pibot = pibot + length + 1;
			length = 0;
			elementIndex++;
		} else {
			length++;
		}
	}

	memcpy(result[elementIndex], &str[pibot], length);

	result[elementIndex + 1] = NULL;
}
