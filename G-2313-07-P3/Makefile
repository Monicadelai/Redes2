#	ALFONSO BONILLA TRUEBA Y MONICA DE LA IGLESIA
#	REDES DE COMUNICACIONES II

CC=gcc
CCFLAGS=-g -Wall -pedantic -pthread
LIB= -lircredes -lirctad -lircinterface -lsoundredes
GTK_CONFIG=`pkg-config --cflags gtk+-3.0`
LIBRERIA_GTK= `pkg-config --cflags gtk+-3.0 --libs gtk+-3.0`
LIBRERIA_SSL= -lssl -lcrypto

HDIR=includes
OBJDIR=obj
SRCDIR=src
LIBSRCDIR=srclib
LIBOBJDIR=objlib
LIBDIR=lib
ECHODIR=echo
IRCDIR=cliente_servidor
CERTIFICADOS=certs

SOURCES= $(shell ls -1 $(SRCDIR)/*.c | xargs)
OBJS= $(shell echo $(SOURCES) | sed -e 's:\.c*:\.o:g' | sed -e 's:$(SRCDIR)/:$(OBJDIR)/:g')
LIBSOURCES= $(shell ls -1 $(LIBSRCDIR)/*.c | xargs)
LIBOBJS= $(shell echo $(LIBSOURCES) | sed -e 's:\.c*:\.o:g' | sed -e 's:$(LIBSRCDIR)/:$(LIBOBJDIR)/:g')
HEADERS= $(shell ls -1 $(HDIR)/*.h | xargs)

PREFIX= G-2313-07-P3

.PHONY: clean all clear help autores about


all:clean compress certificados titulo servidor_echo cliente_echo servidor_IRC libreria


$(OBJDIR)/$(PREFIX)-EcoServerSSL.o: $(SRCDIR)/$(PREFIX)-EcoServerSSL.c $(HEADERS)
	@echo -n compilando objeto \'$<\'...
	@echo -e $(CCFLAGS)
	@$(CC) $(CCFLAGS) -c $< -o $@  $(LIB)
	@echo -e '\e[1;36m[OK] \e[0m'

$(OBJDIR)/$(PREFIX)-EcoClientSSL.o: $(SRCDIR)/$(PREFIX)-EcoClientSSL.c $(HEADERS)
	@echo -n compilando objeto \'$<\'...
	@echo -e $(CCFLAGS)
	@$(CC) $(CCFLAGS) -c $< -o $@  $(LIB)
	@echo -e '\e[1;36m[OK] \e[0m'

$(OBJDIR)/$(PREFIX)-ServerIRC.o: $(SRCDIR)/$(PREFIX)-ServerIRC.c $(HEADERS)
	@echo -n compilando objeto \'$<\'...
	@echo -e $(CCFLAGS)
	@$(CC) $(CCFLAGS) -c $< -o $@  $(LIB)
	@echo -e '\e[1;36m[OK] \e[0m'

$(LIBOBJDIR)/$(PREFIX)-ConnectionSSL.o: $(LIBSRCDIR)/$(PREFIX)-ConnectionSSL.c $(HEADERS)
	@echo -n compilando objeto de librerias \'$<\'...
	@echo -e $(CCFLAGS)
	@$(CC) $(CCFLAGS) -c $< -o $@ $(LIB)
	@echo -e '\e[1;36m[OK] \e[0m'

$(LIBOBJDIR)/$(PREFIX)-server.o: $(LIBSRCDIR)/$(PREFIX)-server.c $(HEADERS)
	@echo -n compilando objeto de librerias \'$<\'...
	@echo -e $(CCFLAGS)
	@$(CC) $(CCFLAGS) -c $< -o $@ $(LIB)
	@echo -e '\e[1;36m[OK] \e[0m'

$(LIBOBJDIR)/$(PREFIX)-utilities.o: $(LIBSRCDIR)/$(PREFIX)-utilities.c $(HEADERS)
	@echo -n compilando objeto de librerias \'$<\'...
	@echo -e $(CCFLAGS)
	@$(CC) $(CCFLAGS) -c $< -o $@ $(LIB) $(LIBRERIA_GTK)
	@echo -e '\e[1;36m[OK] \e[0m'

servidor_echo: $(LIBOBJDIR)/$(PREFIX)-ConnectionSSL.o $(OBJDIR)/$(PREFIX)-EcoServerSSL.o
	@echo -e '\e[1;93m\t\n*** Generando Servidor ECO ***\n\e[0m'
	@echo -e compilando ejecutable \'$@\'...
	@$(CC) $(CCFLAGS) $^ -o $(ECHODIR)/$@ $(LIB) $(LIBRERIA_SSL)
	@echo -e '\e[1;36m[OK] \e[0m'

cliente_echo: $(LIBOBJDIR)/$(PREFIX)-ConnectionSSL.o $(OBJDIR)/$(PREFIX)-EcoClientSSL.o
	@echo -e '\e[1;93m\t\n*** Generando Cliente ECO ***\n\e[0m'
	@echo -e compilando ejecutable \'$@\'...
	@$(CC) $(CCFLAGS) $^ -o $(ECHODIR)/$@ $(LIB) $(LIBRERIA_SSL)
	@echo -e '\e[1;36m[OK] \e[0m'

servidor_IRC: $(LIBOBJDIR)/$(PREFIX)-ConnectionSSL.o $(LIBOBJDIR)/$(PREFIX)-server.o $(LIBOBJDIR)/$(PREFIX)-utilities.o $(OBJDIR)/$(PREFIX)-ServerIRC.o
	@echo -e '\e[1;93m\t\n*** Generando Servidor IRC ***\n\e[0m'
	@echo -e compilando ejecutable \'$@\'...
	@$(CC) $(CCFLAGS) $^ -o $(IRCDIR)/$@ $(LIB) $(LIBRERIA_SSL)
	@echo -e '\e[1;36m[OK] \e[0m'

libreria:
	@echo -e '\e[1;93m\t\n*** Generando Librerias ***\n\e[0m'
	@ar r lib/$(PREFIX)-ConnectionSSL.a $(LIBOBJDIR)/$(PREFIX)-ConnectionSSL.o
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
	@rm -f $(ECHODIR)/servidor_echo
	@rm -f $(ECHODIR)/cliente_echo
	@rm -f $(IRCDIR)/servidor_IRC
	@rm -f $(IRCDIR)/cliente_IRC
	@rm -f $(LIBDIR)/*.a
	@rm -f $(PREFIX).tar.gz
	@rm -f $(CERTIFICADOS)/*.pem
	@rm -f $(CERTIFICADOS)/*.srl
	@rm -f entrada.txt
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
	@echo -e '  >> all: Compila las librerías y el main y genera los ejecutables'
	@echo -e '  >> cliente_echo: Genera el Cliente ECHO con seguirdad SSL'
	@echo -e '  >> servidor_echo: Genera el Servidor ECHO con seguirdad SSL'
	@echo -e '  >> clear: limpia los directorios obj y los ejecutables'
	@echo -e '  >> autores: Muestra la informacion de los autores de esta practica'
	@echo -e '  >> compress: Limpia y comprime la practica'

certificados:
	@echo -e '\e[1;93m\t\n*** Generando Certificados ***\n\e[0m'
	@openssl genrsa -out $(CERTIFICADOS)/rootkey.pem 2048 > /dev/null 2>&1
	@openssl req -new -x509 -key $(CERTIFICADOS)/rootkey.pem -out $(CERTIFICADOS)/rootcert.pem -subj "/C=ES/ST=Madrid/L=Madrid/O=UAM/CN=Redes2 CA"
	@cat $(CERTIFICADOS)/rootkey.pem $(CERTIFICADOS)/rootcert.pem > $(CERTIFICADOS)/ca.pem

	@openssl req -nodes -newkey rsa:2048 -keyout $(CERTIFICADOS)/clientkey.pem -out $(CERTIFICADOS)/clientreq.pem -subj "/C=ES/ST=Madrid/L=Madrid/O=UAM/CN=G-2313-07-P3-client" > /dev/null 2>&1
	@openssl x509 -req -in $(CERTIFICADOS)/clientreq.pem  -CA $(CERTIFICADOS)/ca.pem -CAkey $(CERTIFICADOS)/ca.pem -CAcreateserial -out $(CERTIFICADOS)/clientcert.pem  > /dev/null 2>&1
	@cat $(CERTIFICADOS)/clientcert.pem $(CERTIFICADOS)/clientkey.pem $(CERTIFICADOS)/rootcert.pem > $(CERTIFICADOS)/cliente.pem

	@openssl req -nodes -newkey rsa:2048 -keyout $(CERTIFICADOS)/serverkey.pem -out $(CERTIFICADOS)/serverreq.pem -subj "/C=ES/ST=Madrid/L=Madrid/O=UAM/CN=G-2313-07-P3-server" > /dev/null 2>&1
	@openssl x509 -req -in $(CERTIFICADOS)/serverreq.pem -CA $(CERTIFICADOS)/ca.pem -CAkey $(CERTIFICADOS)/ca.pem -CAcreateserial -out $(CERTIFICADOS)/servercert.pem  > /dev/null 2>&1
	@cat $(CERTIFICADOS)/servercert.pem $(CERTIFICADOS)/serverkey.pem $(CERTIFICADOS)/rootcert.pem > $(CERTIFICADOS)/servidor.pem
	@echo Generando ....
	@echo -e '\e[1;36m[OK] \e[0m'

h: help

titulo:
	@echo -e '\e[1;93m\t\n*** Generando Ejecutables ***\n\e[0m'
