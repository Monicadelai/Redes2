/**
* @brief Cabeceras funciones asociadas para el uso de canales seguros
* @file G-2313-07-P3-ConnectionSSL.h
*
* @authors Alfonso Bonilla Trueba (alfonso.bonilla@estudiante.uam.es)
* @authors Mónica de la Iglesia Martínez (monica.delaiglesia@estudiante.uam.es)
* @version 1.0
* @date 11-04-2017
*/

#ifndef CONNECTIONSSL_H
#define CONNECTIONSSL_H

#include <redes2/irc.h>   /*libreria redes*/
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <openssl/ssl.h>
#include <openssl/x509v3.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <linux/udp.h>
#include <linux/tcp.h>
#include <syslog.h>

#define TAM_COLA 5


/**
* @brief Preparación para poder usar la capa SSL
*
*
*/
void inicializar_nivel_SSL();

/**
* @brief Inicializa el conxtexito que va a ser usado en el canal seguro
*
*
*
*/
SSL_CTX* fijar_contexto_SSL(char *ca_certificado, char *certificado);

/**
* @brief Se encarga de iniciar el proceso de handshake
*
*
*
*/
SSL* conectar_canal_seguro_SSL(SSL_CTX *contexto, int *desc, int puerto, char* host);

/**
* @brief Bloquea la apliación esperando el handshake del cliente
*
* @param[in] contexto Contexto de la conexión SSL
* @param[in] desc Puntero al descriptor del socket
* @param[in] datos_cliente Puntero a estructura sockaddr con los datos del cliente
* @param[in] puerto El numero del puerto que se va a usar en la conexión
*
*/
SSL* aceptar_canal_seguro_SSL(SSL_CTX *contexto, int* desc, struct sockaddr* datos_cliente, int puerto);

/**
* @brief Comprueba la seguridad despueés del handshake
*
* @param[in] ssl Constante de la conexión SSL, no se va a modificar en la función
*
* @retval TRUE la comunicación a través del canal es segura
* @retval FALSE la comunicación a través del canal no es segura
*/
long evaluar_post_connectar_SSL(const SSL *ssl);

/**
* @brief Envia cualquier tipo de datos por un canal seguro
*
* @param[in] ssl Conexión SSL sobre la que se van a enviar datos.
* @param[in] data Puntero a la variable donde se encuentran los datos a enviar.
*
* @retval TRUE Si los datos se han enviado correctamente.
* @retval FALSE En caso de que ocurra algún fallo en el envío de datos.
*
*/
long enviar_datos_SSL(SSL *ssl, const void* data);

/**
* @brief Recibe cualquier tipo de datos por un canal seguro
*
* @param[in] ssl Conexión SSL sobre la que se van a recibir datos.
* @param[in] data Puntero a la variable donde se quieren guardar los datos recibidos.
* @param[in] longitud Tamaño máximo que se quiere recibir.
*
* @retval TRUE Si los datos se han recibido correctamente
* @retval FALSE En caso de que ocurra algún fallo en la recepción de datos
*/
long recibir_datos_SSL(SSL *ssl, void* data, int longitud);

/**
* @brief Libera y cierra canal seguro
*
* @param[in] ssl Conexión SSL que se va a cerrar.
* @param[in] contexto Puntero al contexto de la conexión que se va a cerrar.
* @param[in] socket Descriptor del socket de la conexión a cerrar.
*
*/
void cerrar_canal_SSL(SSL *ssl, SSL_CTX *contexto, int socket);

#endif
