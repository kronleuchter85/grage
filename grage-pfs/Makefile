#
# Linux Commons Makefile
#

## Parametros a customizar para cada aplicacion
GRAGE_PROJECT_NAME=grage-pfs
BUILD_SOURCE=app/src/core/* ./app/src/*.c
BUILD_SOURCE_CONSOLE=app/src/console/*
BUILD_SOURCE_SERVICES=app/src/services/*

##
##Compilacion de la aplicacin (app)
##


GRAGE_REPOSITORY_LIB=$(GRAGE_REPOSITORY)/lib
GRAGE_REPOSITORY_INCLUDES=$(GRAGE_REPOSITORY)/includes
GRAGE_REPOSITORY=/opt/grage-repository

BUILD_DIR=build
BUILD_TARGET = build/bin/$(GRAGE_PROJECT_NAME)
BUILD_INCLUDES = -I./app/includes -I$(GRAGE_REPOSITORY_INCLUDES) 
BUILD_DEPS= -L$(GRAGE_REPOSITORY_LIB) -lgrage-commons -lgrage-nipc -lm -lpthread `pkg-config fuse --cflags --libs` -DFUSE_USE_VERSION=22 -D_FILE_OFFSET_BITS=64 -licui18n -licuuc -licudata



.PHONY: build

build:
	if [ -d build ]; then rm -rf build; fi
	mkdir build
	mkdir build/bin
	mkdir build/conf
	mkdir build/logs
	gcc $(BUILD_SOURCE) $(BUILD_SOURCE_SERVICES) $(BUILD_SOURCE_CONSOLE) $(BUILD_INCLUDES) $(BUILD_DEPS) -g -o $(BUILD_TARGET)
	cp app/conf/* build/conf
	cp app/conf/* /opt/grage-repository/conf
	
	


##
## Clean de build, test y reportes
##
clean-build:
	if [ -d $(BUILD_DIR) ]; then rm -r $(BUILD_DIR); fi

clean-target:
	if [ -d target ]; then rm -rf target ; fi

clean-eclipse-Debug:
	if [ -d Debug ]; then rm -rf Debug ; fi

clean:
	make clean-build
	make clean-target
	make clean-eclipse-Debug

