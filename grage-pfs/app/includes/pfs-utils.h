/*
 * pfs-utils.h
 *
 *  Created on: 06/11/2011
 *      Author: utn_so
 */

#ifndef PFS_UTILS_H_
#define PFS_UTILS_H_

	#include <stddef.h>
	#include <stdint.h>

	/******************/
	/*	Bit Operation */
	/******************/

	//! Byte swap unsigned short
	uint16_t swap_uint16(uint16_t val);

	//! Byte swap signed short
	int16_t swap_int16(int16_t val);

	//! Byte swap unsigned int
	uint32_t swap_uint32(uint32_t val);

	//! Byte swap int
	int32_t swap_int32(int32_t val);


	/***********************************************************/
	/*	Unicode Conversion  include -licui18n -licuuc -licudata*/
	/***********************************************************/

	uint16_t *unicode_utf8_to_utf16(const char *src_utf8, const size_t src_utf8size, size_t *dest_utf16size);

	int unicode_utf8_to_utf16_inbuffer(const char *src_utf8, const size_t src_utf8size, uint16_t *dest_utf16, size_t *dest_utf16size);

	char *unicode_utf16_to_utf8(const uint16_t *src_utf16, const size_t src_utf16size, size_t *dest_utf8size);

	int unicode_utf16_to_utf8_inbuffer(const uint16_t *src_utf16, const size_t src_utf16size, char* dest_utf8, size_t *dest_utf8size);

	char** string_split2( char *str, char delimitor );

	void string_split3( char *str, char delimitor, char **result );

#endif /* PFS_UTILS_H_ */
