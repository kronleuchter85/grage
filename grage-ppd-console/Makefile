#
# Linux ppd-console Makefile
#

## Parametros a customizar para cada aplicacion
GRAGE_PROJECT_NAME=grage-ppd-console
BUILD_SOURCE=app/src/*.c

GRAGE_REPOSITORY_LIB=$(GRAGE_REPOSITORY)/lib
GRAGE_REPOSITORY_INCLUDES=$(GRAGE_REPOSITORY)/includes
GRAGE_REPOSITORY=/opt/grage-repository
BUILD_DIR=build
BUILD_TARGET = build/bin/$(GRAGE_PROJECT_NAME)
BUILD_INCLUDES = -I./app/includes -I$(GRAGE_REPOSITORY_INCLUDES) 
BUILD_DEPS= -L$(GRAGE_REPOSITORY_LIB) -lgrage-commons -lm -lpthread 

.PHONY: build

build:
	if [ -d build ]; then rm -rf build; fi
	mkdir build
	mkdir build/bin
	mkdir build/conf
	mkdir build/logs
	gcc $(BUILD_SOURCE) $(BUILD_INCLUDES) $(BUILD_DEPS) -o $(BUILD_TARGET)

#
# Instalacion de librerias y headers en un
# repositorio comun
#
install: build
	if [ -d $(GRAGE_REPOSITORY_LIB) ]; then cp $(BUILD_TARGET) $(GRAGE_REPOSITORY_LIB); fi


##
## Clean de build, test y reportes
##
clean-build:
	if [ -d $(BUILD_DIR) ]; then rm -r $(BUILD_DIR); fi

	
clean-target:
	if [ -d target ]; then rm -r target ; fi


clean-eclipse-Debug:
	if [ -d Debug ]; then rm -r Debug ; fi


clean:
	make clean-build
	make clean-target
	make clean-eclipse-Debug
