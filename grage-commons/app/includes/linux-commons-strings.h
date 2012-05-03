/*
 * linux-commons-strings.h
 *
 *  Created on: 17/08/2011
 *      Author: gonzalo
 */

#include "linux-commons.h"

#ifndef LINUX_COMMONS_STRINGS_H_
#define LINUX_COMMONS_STRINGS_H_

#define SP ' '
#define EOL '\n'
#define EOS '\0'
	/*
	 * Strings...
	 */

	String commons_string_subString(String string , int startIndex , int endIndex);
	String commons_string_trim(String string);
	String commons_string_concat(String s1 , String s2);
	String commons_string_concatAll(int n , ...);
	String commons_string_cloneString(String string);
	String commons_string_fillIntString(int v , int spacesToFix);
	String commons_string_replaceAll(String s1 , String s2 , String s3);
	String commons_string_trimd(String string);

	Boolean commons_string_equals(String s1 , String s2);
	Boolean commons_string_startsWith(String s1 , String s2);
	Boolean commons_string_endsWith(String s1 ,String s2);

	int commons_string_getIndexOf(String string , char c);


#endif /* LINUX_COMMONS_STRINGS_H_ */
