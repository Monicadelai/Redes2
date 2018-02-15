/**
* @brief Cabeceras de los parseadores del Cliente
* @file G-2313-07-P2-ClientParser.h
*
* @authors Alfonso Bonilla Trueba (alfonso.bonilla@estudiante.uam.es)
* @authors Mónica de la Iglesia Martínez (monica.delaiglesia@estudiante.uam.es)
* @version 1.0
* @date 10-04-2017
*/

#ifndef CLIENTPARSER_H
#define CLIENTPARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <redes2/ircxchat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <syslog.h>
#include <syslog.h>
#include <netdb.h>


/**
* @brief Genera los mensajes bien formados para que el servidor sea capaz de interpretarlos
* según el RFC
*
* @param[in] command puntero a char con el comando recibido y que se va a parsear y ejecutar
* @param[in] option valor de la opción
*
* @retval char* puntero a la cadena resultante con el mensaje listo para enviar al servidor
*/
char* IRC_Client_Parser(char* command, long option);


/**
* @brief Parseador de los comandos recibidos del servidor para mostrar la información en el Cliente
*
* @param[in] command puntero a char con el comando recibido y que se va a parsear y ejecutar
* @param[in] desc valor del descriptor del servidor
*
*/
void IRC_Reply_Parser(char* command, int desc);


#endif
