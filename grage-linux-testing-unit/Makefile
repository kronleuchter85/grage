#
#
# Linux Commons Makefile
#
#

#
#       Macros
#

GRAGE_PROJECT_NAME=grage-linux-testing-unit
GRAGE_REPOSITORY=/opt/grage-repository
GRAGE_REPOSITORY_LIB=$(GRAGE_REPOSITORY)/lib
GRAGE_REPOSITORY_HEADERS=$(GRAGE_REPOSITORY)/includes

TARGET = build/lib$(GRAGE_PROJECT_NAME).a
INCLUDES = -I./includes
FLAGS = -w -g -c -o



#
# Instalacion de librerias y headers en un
# repositorio comun
#

install: build	
	if [ -d $(GRAGE_REPOSITORY_HEADERS) ]; then cp includes/*.h $(GRAGE_REPOSITORY_HEADERS); fi
	if [ -d $(GRAGE_REPOSITORY_LIB) ]; then cp $(TARGET) $(GRAGE_REPOSITORY_LIB); fi


#
#       Compilation
#

build: objdir *.o
	ar rcs $(TARGET) build/obj/*.o
	rm -rf build/obj
	
objdir:
	if [ -d build ]; then rm -rf build; fi
	mkdir -p build/obj

*.o:
	gcc $(INCLUDES) src/* -I /usr/local/include/CUnit -L /usr/local/lib -lcunit -static -w -g -c
	mv *.o build/obj

clean:
	rm -rf build
	make clean-eclipse-Debug

	
clean-eclipse-Debug:
	if [ -d Debug ]; then rm -r Debug ; fi