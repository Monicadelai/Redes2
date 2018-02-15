#	ALFONSO BONILLA TRUEBA Y MONICA DE LA IGLESIA
#	REDES DE COMUNICACIONES II

CC=gcc
CCFLAGS=-g -Wall -w -pedantic -pthread
LIB= -lircredes -lirctad -lircinterface -lsoundredes
LIBRERIA_SOUND= -lpulse -lpulse-simple
GTK_CONFIG=`pkg-config --cflags gtk+-3.0`
LIBRERIA_GTK= `pkg-config --cflags gtk+-3.0 --libs gtk+-3.0`

HDIR=includes
OBJDIR=obj
SRCDIR=src
LIBSRCDIR=srclib
LIBOBJDIR=objlib
LIBDIR=lib

SOURCES= $(shell ls -1 $(SRCDIR)/*.c | xargs)
OBJS= $(shell echo $(SOURCES) | sed -e 's:\.c*:\.o:g' | sed -e 's:$(SRCDIR)/:$(OBJDIR)/:g')
LIBSOURCES= $(shell ls -1 $(LIBSRCDIR)/*.c | xargs)
LIBOBJS= $(shell echo $(LIBSOURCES) | sed -e 's:\.c*:\.o:g' | sed -e 's:$(LIBSRCDIR)/:$(LIBOBJDIR)/:g')
HEADERS= $(shell ls -1 $(HDIR)/*.h | xargs)

PREFIX = G-2313-07-P2

.PHONY: clean all clear help autores about


all:clean compress $(PREFIX) libreria

$(LIBOBJDIR)/%.o: $(LIBSRCDIR)/%.c $(HEADERS)
	@echo -n compilando objeto de librerias \'$<\'...
	@echo -e $(CCFLAGS)
	@$(CC) $(CCFLAGS) -c $< -o $@ $(LIB) $(LIBRERIA_GTK)
	@echo -e '\e[1;36m[OK] \e[0m'

$(PREFIX): $(LIBOBJS) $(SRCDIR)/$(PREFIX)-xchat2.c
	@echo -e '\e[1;93m\t\n*** Generando Cliente IRC ***\n\e[0m'
	@echo -e compilando ejecutable \'$@\'...
	@$(CC) $(CCFLAGS) $(SRCDIR)/$(PREFIX)-xchat2.c $(LIBOBJS) $(LIB) $(LIBRERIA_GTK) -rdynamic -o $(PREFIX)
	@echo -e '\e[1;36m[OK] \e[0m'

cliente: $(PREFIX)

libreria:
	@echo -e '\e[1;93m\t\n*** Generando Librerias ***\n\e[0m'
	@ar r lib/$(PREFIX)-ClientParser.a $(LIBOBJDIR)/$(PREFIX)-ClientParser.o
	@ar r lib/$(PREFIX)-ClientFunctions.a $(LIBOBJDIR)/$(PREFIX)-ClientFunctions.o
	@echo -e '\e[1;36m[OK] \e[0m'

git: clean
	git add --all
	git commit -m "Commit desde el mejor make que existe"
	git push origin master
	@echo -e '\e[1;36m[OK] \e[0m'

clear: clean

clean:
	@rm -f $(OBJDIR)/*.o
	@rm -f $(LIBOBJDIR)/*.o
	@rm -f $(PREFIX)
	@rm -f $(LIBDIR)/*.a
	@rm -f $(PREFIX).tar.gz
	@echo -e '\e[1;33m\t[Eliminados ejecutables y ficheros objeto]\e[0m'

about: autores

compress: clean
	@tar -zcf ../$(PREFIX).tar.gz ../$(PREFIX) > /dev/null 2>&1
	@mv ../$(PREFIX).tar.gz $(PREFIX).tar.gz > /dev/null 2>&1
	@echo -e '\e[1;36m[OK] \e[0m'

autores:
	@echo -e '\e[1;32m\n\tPractica realizada por:\e[0m'
	@echo -e '\e[1;34m\t------------------------------\e[0m'
	@echo -e '\e[1;34m\t--  Alfonso Bonilla Trueba  --\e[0m'
	@echo -e '\e[1;34m\t--            Y             --\e[0m'
	@echo -e '\e[1;34m\t--   Monica de la Iglesia   --\e[0m'
	@echo -e '\e[1;34m\t------------------------------\e[0m'
	@echo -e '\e[1;32m\t      Grupo 2313 Pareja 7\e[0m\n'

help:
	@echo -e 'Se admiten las siguientes opciones:'
	@echo -e '  >> all: Compila las librerías y el main y genera el ejecutable'
	@echo -e '  >> cliente: Genera el Cliente IRC xchat2'
	@echo -e '  >> G-2313-07-P2: Genera el cliente IRC xchat2'
	@echo -e '  >> clear: limpia los directorios obj y los ejecutables'
	@echo -e '  >> autores: Muestra la informacion de los autores de esta practica'
	@echo -e '  >> compress: Limpia y comprime la practica'

h: help
