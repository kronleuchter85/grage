/*
 *
 *  file-config.h
 *
 *
 */
/*
 char * getClientIp(void);
 int getClientPort(void);
 int getLoggingLevel(void);
 */
#include "linux-commons.h"
#include "linux-commons-list.h"

#ifndef _FILE_INFORMATION_
#define _FILE_INFORMATION_


	char * commons_file_getKey(char * line);
	char * commons_file_getValue(char * line);
	void commons_file_loadConfiguration(File * file , void (*processKeysAndValues)(char * , char * , void *));

	void commons_file_parseKeyAndValueFile(File * file ,
			Boolean (*anIgnoreLineCriteria)(char *),
				void (*processKeysAndValuesCriteria)(char * , char *, void* ),
				void * storageObject);

	void commons_file_parseFileAndModifyValueForKey(File * file , char * aKey , char * newValue ,
			Boolean (*anIgnoreLineCriteria)(char *),
				void (*modifyKeysAndValuesCriteria)(char * , char * , void * , File * ),
				void * storageObject);

	void commons_file_updateEntry(char * key , char * value ,
				void (*modifyKeysAndValuesCriteria)(char * , char * , void * , File * ) ,
				File * file);

	Boolean commons_file_existsFile(char * name);
	Boolean commons_file_createDirectory(char * name);

	File * commons_file_openFile(const char * );
	File * commons_file_openOrCreateFile(const char * filename);
	File * commons_file_createOrTruncateFile(const char * filepath);

	Boolean commons_file_removeDirectory(char * dirname);
	void commons_file_removeFile(const char * filename);
	void commons_file_closeFile(File * file);

	void commons_file_insertEntry(char * key , char * value , File * file);
	void commons_file_insertLine( const char * line , File * file);

	List commons_file_getDirectoryFiles(char * path ,
			Boolean (*anIncludeCriteria)(char *), Boolean (*sortingCriteria)(char * , char *));

	char * commons_file_getStartNameMatchingFile(char * path , char * pattern);

	Boolean commons_file_isValidConfValue(char * value);

	int commons_file_getFileSize(File * file);

#endif
