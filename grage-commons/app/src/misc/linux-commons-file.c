/*
 *  file-config.c
 *  
 *
 *  Created by Fernando Nino on 4/11/11.
 *
 *  
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <dirent.h>

#include "linux-commons-strings.h"
#include "linux-commons-file.h"
#include "linux-commons-logging.h"
#include "linux-commons-list.h"

	Boolean commons_file_isPropertiesCommentedOrBlanckLine(char * line);

	void commons_file_loadConfiguration(File * file , void (*processKeysAndValues)(char * , char * , void *)){
		commons_file_parseKeyAndValueFile(file , commons_file_isPropertiesCommentedOrBlanckLine , processKeysAndValues , NULL);
	}

	char * commons_file_getKey(char * line ){
		char * formated = commons_string_trim(line);
		int indexEqual = commons_string_getIndexOf(formated , '=');
		if(indexEqual > 0){
			return commons_string_subString(formated , 0 , indexEqual -1);
		}else{
			return NULL;
		}
	}

	char * commons_file_getValue(char * line){
		char * formated = commons_string_trim(line);
		int indexEqual = commons_string_getIndexOf(formated , '=');
		if(indexEqual > 0){
			return commons_string_subString(formated , indexEqual +1 , strlen(formated)-1);
		}else{
			return NULL;
		}
	}

	Boolean commons_file_existsFile(char * name){
		Boolean existence = FALSE;
		File * file = fopen(name , "r");
		if(file != NULL){
			existence = TRUE;
			fclose(file);
		}
		return existence;
	}

	Boolean commons_file_createDirectory(char * name){
		return !mkdir(name , S_IRWXU);
	}



	void commons_file_parseKeyAndValueFile(File * file ,
			Boolean (*anIgnoreLineCriteria)(char *),
			void (*processKeysAndValuesCriteria)(char * , char * , void *),
			void * storageObject){


		char lineBuffer[SYSTEM_FILE_LINE_LENGTH];
		bzero(lineBuffer,SYSTEM_FILE_LINE_LENGTH);
		while(fgets(lineBuffer , SYSTEM_FILE_LINE_LENGTH , file)){
			char * line = commons_string_trim(lineBuffer);

			if(anIgnoreLineCriteria == NULL || !anIgnoreLineCriteria(line)){
				char * key = commons_file_getKey(line);
				char * value = commons_file_getValue(line);

				if(key != NULL && value != NULL)
					processKeysAndValuesCriteria(key , value , storageObject);
			}
		}
		bzero(lineBuffer, SYSTEM_FILE_LINE_LENGTH);
	}


	void commons_file_parseFileAndModifyValueForKey(File * file , char * aKey , char * newValue ,
			Boolean (*anIgnoreLineCriteria)(char *),
			void (*modifyKeysAndValuesCriteria)(char * , char * , void * , File * ),
			void * storageObject){


		char lineBuffer[SYSTEM_FILE_LINE_LENGTH];
		bzero(lineBuffer,SYSTEM_FILE_LINE_LENGTH);
		while(fgets(lineBuffer , SYSTEM_FILE_LINE_LENGTH , file)){
			char * line = commons_string_trim(lineBuffer);

			if(anIgnoreLineCriteria == NULL || !anIgnoreLineCriteria(line)){
				char * key = commons_file_getKey(line);
				char * value = commons_file_getValue(line);

				if(key != NULL && value != NULL){

					if(commons_string_equals(key , aKey)){

						/*
						 * Inicializamos la posicion donde fue encontrada la linea en cuestion
						 * mediante la posicion actual del archivo menos la suma del largo de la linea mas
						 * un caracter '\n' de fin de linea.
						 */
						long int currentPossition = ftell(file);
						long int fileStartLinePossition = currentPossition - (strlen(value) + 1);

						/*
						 * Pocisionamos el cursor del archivo en la posicion correspondiente
						 * para realizar la escritura
						 */
						fseek(file , fileStartLinePossition , SEEK_SET);

						//escritura...
						modifyKeysAndValuesCriteria(value , newValue, storageObject , file);

						/*
						 * volvemos a posicionarnos donde estabamos
						 */
						fseek(file , currentPossition , SEEK_SET);
					}
				}
			}
		}
		bzero(lineBuffer, SYSTEM_FILE_LINE_LENGTH);
	}



	Boolean commons_file_isPropertiesCommentedOrBlanckLine(char * line){
		return commons_string_startsWith( line, "#") && !commons_string_equals(line , "");
	}

	File * commons_file_openFile(const char * filename){
		return fopen(filename , "r+");
	}

	File * commons_file_openOrCreateFile(const char * filename){
		return fopen(filename , "a");
	}

	void commons_file_removeFile(const char * filename){
		remove(filename);
	}

	Boolean commons_file_removeDirectory(char * dirname){
		//return !rmdir(dirname);

		char * cmd = commons_string_concatAll(2 , "rm -r " , dirname);
		log_debug_t("Ejecutando comando nativo del sistema: [%s]" , cmd);
		return !system(cmd);
	}

	void commons_file_closeFile(File * file){
		fclose(file);
	}

	void commons_file_insertEntry(char * key , char * value , File * file){
		const char * content = commons_string_concatAll(3 , key , "=" , value);
		commons_file_insertLine(content, file);
	}

	void commons_file_insertLine(const char * line , File * file){
		int inserted = fwrite(line , sizeof(char) , strlen(line) , file);
		log_debug_t("Se han insertado %d bytes en el archivo" , inserted);
		fputc('\n' , file);
	}


	void commons_file_updateEntry(char * key , char * value ,
			void (*modifyKeysAndValuesCriteria)(char * , char * , void * , File * ) ,
			File * file){

		commons_file_parseFileAndModifyValueForKey(file, key , value ,
				commons_file_isPropertiesCommentedOrBlanckLine ,
				modifyKeysAndValuesCriteria , NULL);
	}



	File * commons_file_createOrTruncateFile(const char * filepath){
		return fopen(filepath , "w+");
	}



	List commons_file_getDirectoryFiles(char * path ,
			Boolean (*anIncludeCriteria)(char *) ,
			Boolean (*sortingCriteria)(char * , char *) ){

		Boolean isSameFile(char * f1 , char * f2){
			return commons_string_equals(f1 , f2);
		}

		FilesIterator * dp;
		List fileNames = NULL;

		if(sortingCriteria == NULL){
			fileNames = commons_list_buildList(NULL,
				(Boolean (*)(void *, void *))isSameFile ,
				(Boolean (*)(void *, void *))commons_list_ORDER_ALWAYS_FIRST);
		}else{
			fileNames = commons_list_buildList(NULL ,
				(Boolean (*)(void *, void *))isSameFile ,
				(Boolean (*)(void *, void *))sortingCriteria);
		}

		Directory * myDir = opendir(path);

		while( (dp = readdir(myDir)) != NULL ) {
			char * fileName = dp->d_name;
			if(anIncludeCriteria == NULL || anIncludeCriteria(fileName)){
				commons_list_addNode(fileNames , commons_string_cloneString(fileName));
			}
		}

		closedir(myDir);

		return fileNames;
	}

	char * commons_file_getStartNameMatchingFile(char * path , char * pattern){
		FilesIterator * dp;
		Directory * myDir = opendir(path);

		while( (dp = readdir(myDir)) != NULL ) {
			if(commons_string_startsWith(dp->d_name , pattern)){
				closedir(myDir);
				return commons_string_cloneString(dp->d_name);
			}
		}

		closedir(myDir);
		return NULL;
	}


	Boolean commons_file_isValidConfValue(char * value){
		return (value != NULL && !commons_string_equals(value , ""));
	}



	int commons_file_getFileSize(File * file){
		struct stat fileInfo;
		fstat(fileno(file) , &fileInfo);
		return fileInfo.st_size;
	}

