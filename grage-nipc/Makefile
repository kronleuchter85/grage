#
# Linux Commons Makefile
#


## Rutas del repositorio comun
GRAGE_REPOSITORY=/opt/grage-repository
GRAGE_REPOSITORY_LIB=$(GRAGE_REPOSITORY)/lib
GRAGE_REPOSITORY_INCLUDES=$(GRAGE_REPOSITORY)/includes
## nombres generales del projecto
GRAGE_PROJECT_NAME=grage-nipc
##
##Compilacion de la aplicacin (app)
##
BUILD_DIR=build
BUILD_TARGET = build/lib$(GRAGE_PROJECT_NAME).a
BUILD_INCLUDES = ./app/includes
BUILD_SOURCE=app/src/*
BUILD_FLAGS = -w -g -c -o
BUILD_LIB_DEPS= -L$(GRAGE_REPOSITORY_LIB) -lgrage-commons

#
# Comportamiento comun
#

build: objdir 
	ar rcs $(BUILD_TARGET) build/obj/*.o
	rm -rf build/obj
	
objdir:
	if [ -d build ]; then rm -rf build; fi
	mkdir -p build/obj
	gcc -I$(BUILD_INCLUDES) -I$(GRAGE_REPOSITORY_INCLUDES) $(BUILD_LIB_DEPS) $(BUILD_SOURCE) -w -g -c -lm
	mv *.o build/obj


#
# Instalacion de librerias y headers en un
# repositorio comun
#
install: build
	if [ -d $(GRAGE_REPOSITORY_INCLUDES) ]; then cp $(BUILD_INCLUDES)/*.h $(GRAGE_REPOSITORY_INCLUDES); fi
	if [ -d $(GRAGE_REPOSITORY_LIB) ]; then cp $(BUILD_TARGET) $(GRAGE_REPOSITORY_LIB); fi


##
## Clean de test y reportes
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