/**
* @brief Servidor IRC demonizado
* @file G-2313-07-P1-ServerIRC.c
*
* @authors Alfonso Bonilla Trueba (alfonso.bonilla@estudiante.uam.es)
* @authors Mónica de la Iglesia Martínez (monica.delaiglesia@estudiante.uam.es)
* @version 1.0
* @date 20-02-2017
*/


#include <stdio.h>
#include <stdlib.h>
#include <redes2/irc.h>

#include "../includes/G-2313-07-P1-server.h"


int main(int argc, char *argv[]){
	int socket;
	daemonizar();
	socket = IRC_Initiate_Server();
	IRC_Accept_Connection(socket);

	return EXIT_SUCCESS;
}
