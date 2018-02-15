/**
* @brief Cabeceras funciones asociadas a hilos en el Cliente
* @file G-2313-07-P2-ClientFunctions.h
*
* @authors Alfonso Bonilla (alfonso.bonilla@estudiante.uam.es)
* @authors Monica de la Iglesia (monica.delaiglesia@estudiante.uam.es)
* @version 1.0
* @date 10-04-2017
*/

#ifndef CLIENTFUNCTIONS_H
#define CLIENTFUNCTIONS_H

#include <stdio.h>
#include <stdlib.h>
#include <redes2/ircxchat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <syslog.h>
#include <netdb.h>
#include <pthread.h>
#include <string.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <ifaddrs.h>

#include "G-2313-07-P2-ClientParser.h"

typedef struct conexion conexion;
typedef struct thread_info thread_info;
typedef struct thread_fich thread_fich;


/**
 * @brief Estructura con la informacion de la conexion
 */
 struct conexion{
 	char *nick;	/**< @brief Nick del cliente */
 	char *user;	/**< @brief Usuario del cliente */
 	char *realname;	/**< @brief Realnme del cliente */
 	char *password;	/**< @brief Password del cliente */
 	char *server;	/**< @brief Server del cliente */
 	int port;	/**< @brief Puerto al que se conecta el cliente */
 	int desc;	/**< @brief Descriptor del socket del cliente */
};

/**
 * @brief Estructura con información necesaria para las llamadas que hará el hilo
 */
struct thread_info {
	int sd;                     /**< @brief Descriptor del socket del cliente */
	struct sockaddr_in address; /**< */
	int addrlen;                /**< */
	char* data;                 /**< @brief Contenido a enviar */
	long datalen;               /**< @brief Longitud de los datos que se van a enviar (Fichero)*/
	char* nick;									/**< @brief Nick del cliente */
};

/**
 * @brief Estructura con información necesaria para el envio de ficheros
 */
struct thread_fich {
	char* filename;	/**< @brief Nombre del del fichero */
	char* data;	/**< @brief Contenido del fichero */
	long unsigned datalen;	/**< @brief Longitud del contenido del fichero */
	char* nick; /**< @brief Nick del  cliente */
};

/**
* @brief Recibe los mensajes del servidor y llama a el parseador de respuestas
* para mostrar la información en el cliente
*
*
* @param[in] thr_info puentero a la información del hilo
*/
void* IRC_Client_Recieve(void* thr_info);


/**
* @brief Envia un mensaje de PING al servidor cada 30 segundos
*
*
* @param[in] thr_info puntero a la información del hilo
*/
void* IRC_Client_Ping(void* thr_info);


/**
* @brief Envia un mensaje de WHO al servidor cada 30 segundos
*
*
* @param[in] thr_info puntero a la información del hilo
*/
void* IRC_Client_Who(void* thr_info);


/**
* @brief Envia un mensaje para el envio de información
*
* @param[in] thr_info puntero a la información del hilo
*/
void* IRC_Send_File(void* thr_fich_info);

/**
* @brief Accepta el recibimiento de información
*
* @param[in] fmsg puntero a la información
*/
void* IRC_Accept_File(void* fmsg);


#endif
