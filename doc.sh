#!/bin/sh
#Made by 93alfon

OPTION=$1


if [ "$OPTION" = "-c" ]; then
	rm -Rf G-2313-07-P1/doc/html
	rm -Rf G-2313-07-P2/doc/html
	rm -Rf G-2313-07-P3/doc/html

	rm G-2313-07-P1/man/*.3 > /dev/null 2>&1
	rm G-2313-07-P2/man/*.3 > /dev/null 2>&1
	rm G-2313-07-P3/man/*.3 > /dev/null 2>&1

	echo "Limpiada toda la documentación"
else
	doxygen doxygen/Doxyfile-P1 > /dev/null 2>&1
	doxygen doxygen/Doxyfile-P2 > /dev/null 2>&1
	doxygen doxygen/Doxyfile-P3 > /dev/null 2>&1

	cp doxygen/doxygen.png G-2313-07-P1/doc/html
	cp doxygen/doxygen.png G-2313-07-P2/doc/html
	cp doxygen/doxygen.png G-2313-07-P3/doc/html

	mv -f G-2313-07-P1/doc/man/man3/* G-2313-07-P1/man
	mv -f G-2313-07-P2/doc/man/man3/* G-2313-07-P2/man
	mv -f G-2313-07-P3/doc/man/man3/* G-2313-07-P3/man

	rm -Rf G-2313-07-P1/doc/man
	rm -Rf G-2313-07-P2/doc/man
	rm -Rf G-2313-07-P3/doc/man

	rm G-2313-07-P1/man/_*
	rm G-2313-07-P2/man/_*
	rm G-2313-07-P3/man/_*

	rm G-2313-07-P1/man/G-2313-07-P1-*
	rm G-2313-07-P2/man/G-2313-07-P2-*
	rm G-2313-07-P3/man/G-2313-07-P3-*

	rm G-2313-07-P1/man/auxiliar_functions.3
	rm G-2313-07-P1/man/irc_server.3

	rm G-2313-07-P2/man/irc_client_functions.3
	rm G-2313-07-P2/man/irc_client_interface.3
	rm G-2313-07-P2/man/irc_client_parsers.3

	rm G-2313-07-P3/man/ssl_connection.3

	echo "Generada y organizada toda la documentación"
	echo "   >> Usar -c para limpiar toda la documentación"
fi
