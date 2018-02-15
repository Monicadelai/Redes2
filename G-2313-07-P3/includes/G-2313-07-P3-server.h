/**
* @brief Cabeceras de funciones necesarias para el inicio y funcionamiento del server
* @file G-2313-07-P1-server.h
*
* @authors Alfonso Bonilla Trueba (alfonso.bonilla@estudiante.uam.es)
* @authors Mónica de la Iglesia Martínez (monica.delaiglesia@estudiante.uam.es)
* @version 1.1
* @date 10-04-2017
*/

#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <string.h>
#include <signal.h>     /*manejadores senales*/
#include <redes2/irc.h> /*libreria redes*/
#include <pthread.h>    /*Para hlos*/
#include <errno.h>
#include <sys/time.h>
#include <netdb.h>
#include "G-2313-07-P3-utilities.h"
#include "../includes/G-2313-07-P3-ConnectionSSL.h"


#define MAX_CONNECTIONS 500                        /*!<Numero maximo de conexiones*/
#define NFC_SERVER_PORT 6667                       /*!<Puerto del Servidor*/
#define MAX_BUFFER 512                             /*!<Tamaño maximo de mensaje*/
#define MAX_NICKNAME 9                             /*!<Tamaño maximo para el nickname*/
#define MAX_CHANNELNAME 50                         /*!<Tamaño máximo para el nombre de un canal*/
#define MAX_CHANELS_USER 10                        /*!<Máximo de canales en los que puede estar un usario*/
#define SERVER "localhost"                         /*!<Nombre del servidor*/
#define PREFIX_PERSONAL "localhost_alfonso_monica" /*!<Prefijo predeterminado*/


/**
* @brief Iniciacion del Servidor
*
*
* @retval int El descriptor del socket que acaba de abrir y preparar para escuchar concexiones.
*/
int IRC_Initiate_Server(int port);


/**
* @brief Protocolo PING PONG
*
* @param sig valor de la señal asociada
*/
void IRC_Ping_Pong (int sig);


/**
* @brief Finalizacion del Servidor liberando recursos
*
*
* @param sig valor de la señal asociada
*/
void IRC_End_Server(int sig);


/**
* @brief Acepta una nueva conexion creando un hilo nuevo
*
* @param sockval valor del socket del servidor
*/
void IRC_Accept_Connection(int sockval);


/**
* @brief Recibe mensajes del cliente y comprueba si este se ha ido
*
* @param valor puntero void con el valor del descriptor de usuario
*/
void *IRC_New_Client(void* valor);


/**
* @brief Parseador de comandos IRC y ejecuta estos
*
*
* @param[in] command puntero a char con el comando recibido y que se va a parsear y ejecutar
* @param[in] desc entero descriptor del usuario
* @param[in,out] nick doble puntero char al nick del ususario
* @param[in,out] prefix_user doble puntero char al prefix del usuario
*/
void IRC_Server_Parser(char* command, int desc, char** nick, char** prefix_user);


#endif
