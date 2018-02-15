/**
* @brief Cabeceras de funciones utilidad para el funcionamiento del servidor
* @file G-2313-07-P1-utilities.h
*
* @authors Alfonso Bonilla Trueba (alfonso.bonilla@estudiante.uam.es)
* @authors Mónica de la Iglesia Martínez (monica.delaiglesia@estudiante.uam.es)
* @version 1.1
* @date 10-04-2017
*/

#ifndef UTILITIES_H
#define UTILITIES_H

#include <redes2/irc.h> /*libreria redes*/
#include <unistd.h>     /*Para pid*/
#include <sys/types.h>  /*Para pid y unmask*/
#include <arpa/inet.h>  /*Para htons*/
#include <strings.h>    /*Para bzero*/
#include <sys/stat.h>   /*para unmask*/
#include <syslog.h>

/**
* @brief Comprueba si un cliente se ha ido del servidor sin hacer quit
*
*
* @retval TRUE en caso de que el usuario se haya marchado sin hacer QUIT
* @retval FALSE en caso de que el usuario siga en el servidor
*/
long isClosed(int desc);

/**
* @brief Deja el programa ejecutandose en segundo plano
*
* toma el control de la ejecucion.
*
*/
void daemonizar();

/**
* @brief Comprueba si existe un usuario dado su nick
*
* @param nick puntero char al nick de usuario
* @retval TRUE en caso de encontrar un usuario con el nick asociado
* @retval FALSE en caso de no encontrar un usuario con el nick dado
*/
long exist_User(char *nick);


/**
* @brief Comprueba si existe un usuario dado su descriptor
*
*
* @param desc puntero al entero descriptor de usuario
* @retval TRUE en caso de encontrar un usuario con el descriptor asociado
* @retval FALSE en caso de no encontrar un usuario con el descriptor dado
*/
long exist_descriptor(int* desc);

#endif
