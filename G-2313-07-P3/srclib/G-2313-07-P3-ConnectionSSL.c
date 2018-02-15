/**
* @brief Funciones para el manejo de canales seguros
* @file G-2313-07-P3-ConnectionSSL.c
*
* @authors Alfonso Bonilla Trueba (alfonso.bonilla@estudiante.uam.es)
* @authors Mónica de la Iglesia Martínez (monica.delaiglesia@estudiante.uam.es)
* @version 1.0
* @date 11-04-2017
*/

#include "../includes/G-2313-07-P3-ConnectionSSL.h"

/*! \mainpage
 *
 * \section intro_sec Introducción
 *
 * Seguridad basada en SSL (<a href="https://en.wikipedia.org/wiki/Transport_Layer_Security"><b>Secure Socket Layer</b></a>)
 * realizada por <b>Alfonso Bonilla Trueba</b> y <b>Mónica de la Iglesia Martínez</b> <br/>
 * La librería SSL
 * realizada se ajusta a lo requerido en las prácticas de Redes de Comunicaciones 2
 * del Grado de Ingeniería Informática impartido en la EPS de la <a href="https://www.uam.es/UAM/Home.htm">
 * <b>Universidad Autónoma de Madrid</b></a>
 *
 * <hr>
 * <hr>
 * \section install_sec Instalación
 *
 * \subsection step1 Paso 1: Comando Make
 * Al hacer make sobre el directorio de la practica se generan:
 * <ul><li>Los certificados necesarios en la carpeta certs</li>
 * </ul>
 *
 * <hr>
 * <hr>
 * \section step4 Información
 * <ul>
 * <li>Al hacer <b>make</b> se generan los certificados y las claves para hacer la comunicación segura, de forma que
 * el Cliente pueda verificar la identidad del Servidor y viceversa</li>
 * <ul><li>Common Name del certificado de la CA => <b>Redes2 CA</b></li>
 * <li>Common Name del certificado del Cliente => <b>G-2313-07-P3-client</b></li>
 * <li>Common Name del certificado del Servidor => <b>G-2313-07-P3-server</b></li>
 * </ul>
 * <li>Los certificados han sido generados con <a href="https://www.openssl.org/"><b>OpenSSL</b></a></li>
 * <li>El corrector <b>C3PO</b> incluido en la librearia de Redes2 ha sido usado para testear esta práctica</li>
 * </ul>
 */

/*! @page ssl_connection Canáles Seguros SSL
*
* <p>Esta sección incluye las funciones encargadas de operar con canáles seguros basados en SSL.<br>
* Estas funciones son encargadas de realizar el handshake, de recibir los datos por el canal seguro
* de enviar daros por el canal seguro y de liberar los recursos utilizados para crear este canal
* seguro</p>
*
* <h2>Cabeceras</h2>
* <code>
* \b #include \b <G-2313-07-P3-ConnectionSSL.h>
* </code>
*
* <hr>
* <hr>
*
* <h2>Funciones implementadas</h2>
* <p>Se incluyen las siguientes funciones para crear, operar y finalizar conexiones SSL:
* <ul>
* <li>@subpage inicializar_nivel_SSL</li>
* <li>@subpage fijar_contexto_SSL</li>
* <li>@subpage conectar_canal_seguro_SSL</li>
* <li>@subpage aceptar_canal_seguro_SSL</li>
* <li>@subpage evaluar_post_connectar_SSL</li>
* <li>@subpage enviar_datos_SSL</li>
* <li>@subpage recibir_datos_SSL</li>
* <li>@subpage cerrar_canal_SSL</li>
* </ul></p>
*
* <hr>
* <hr>
*
* <h2>Información</h2>
* @authors Alfonso Bonilla Trueba (alfonso.bonilla@estudiante.uam.es)
* @authors Mónica de la Iglesia Martínez (monica.delaiglesia@estudiante.uam.es)
* @copyright Pareja 7 - Grupo 2313
*/


/**
 * @page inicializar_nivel_SSL inicializar_nivel_SSL
 * @brief Preparación para poder usar la capa SSL
 * <h2>Synopsis</h2>
 *
 * @code
 * #include "includes/G-2313-07-P3-ConnectionSSL.h"
 *
 * void inicializar_nivel_SSL()
 * @endcode
 *
 * <h2>Descripción</h2>
 *
 * Esta función se encargará de realizar todas las llamadas necesarias para que la aplicación
 * pueda usar la capa segura SSL.
 *
 * <hr>
 *
 * <h2>Información</h2>
 * @authors Alfonso Bonilla Trueba (alfonso.bonilla@estudiante.uam.es)
 * @authors Mónica de la Iglesia Martínez (monica.delaiglesia@estudiante.uam.es)
 * @copyright Pareja 7 - Grupo 2313
 *
 * <hr>
 *
 */
void inicializar_nivel_SSL()
{
  SSL_load_error_strings();
  SSL_library_init();
  ERR_print_errors_fp(stdout);
}


/**
 * @page fijar_contexto_SSL fijar_contexto_SSL
 * @brief Inicializa el conxtexito que va a ser usado en el canal seguro
 * <h2>Synopsis</h2>
 *
 * @code
 * #include "includes/G-2313-07-P3-ConnectionSSL.h"
 *
 * SSL_CTX* fijar_contexto_SSL(char *certificado, char *key)
 * @endcode
 *
 * <h2>Descripción</h2>
 * Esta función se encarga de inicializar correctamente el contexto que va a ser utilizado para
 * la creación de canales seguros mediante SSL. Recibe información sobre las rutas a los certificados y
 * claves con los que va a trabajar la aplicación
 *
 * @param[in] certificado Ruta del certificado que se va a usar en la conexión SSL.
 * @param[in] key Llave que se va a usar en la conexión SSL.
 *
 * <hr>
 *
 * <h2>Información</h2>
 * @authors Alfonso Bonilla Trueba (alfonso.bonilla@estudiante.uam.es)
 * @authors Mónica de la Iglesia Martínez (monica.delaiglesia@estudiante.uam.es)
 * @copyright Pareja 7 - Grupo 2313
 *
 * <hr>
 *
 */
SSL_CTX* fijar_contexto_SSL(char *ca_certificado, char *certificado)
{
  SSL_CTX* contexto;
  char CApath [1024];

  if(ca_certificado == NULL || certificado == NULL)
    return NULL;

  contexto = SSL_CTX_new(SSLv23_method());
  ERR_print_errors_fp(stdout);
  if(!contexto){
    ERR_print_errors_fp(stdout);
    return NULL;
  }

  SSL_CTX_set_verify(contexto, SSL_VERIFY_PEER, NULL);

  if(!SSL_CTX_load_verify_locations(contexto, ca_certificado, CApath)){
    ERR_print_errors_fp(stdout);
    return NULL;
  }

  SSL_CTX_set_default_verify_paths(contexto);

  if(!SSL_CTX_use_certificate_file(contexto, certificado, SSL_FILETYPE_PEM)){
    printf("PETA EL USE CER\n" );
    ERR_print_errors_fp(stdout);
    return NULL;
  }

  if(!SSL_CTX_use_PrivateKey_file(contexto, certificado, SSL_FILETYPE_PEM)){
    ERR_print_errors_fp(stdout);
    return NULL;
  }

  SSL_CTX_set_verify(contexto, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, NULL);

  if(!SSL_CTX_load_verify_locations(contexto, ca_certificado, CApath)){
    ERR_print_errors_fp(stdout);
    return NULL;
  }

  return contexto;
}


/**
 * @page conectar_canal_seguro_SSL conectar_canal_seguro_SSL
 * @brief Se encarga de iniciar el proceso de handshake
 * <h2>Synopsis</h2>
 *
 * @code
 * #include "includes/G-2313-07-P3-ConnectionSSL.h"
 *
 * SSL* conectar_canal_seguro_SSL()
 * @endcode
 *
 * <h2>Descripción</h2>
 *
 * Dado un contexto SSL y un descriptor de socket esta función se encarga de
 * obtener un canal seguro SSL iniciando el proceso de handshake con el otro extremo.
 *
 * @param[in] contexto Contexto de la conexión SSL
 * @param[in] desc Puntero al descriptor del socket
 * @param[in] puerto Número del puerto que se va a usar en la conexión
 * @param[in] host El host sobre el que se va a realizar la conexión SSL
 *
 * <hr>
 *
 * <h2>Información</h2>
 * @authors Alfonso Bonilla Trueba (alfonso.bonilla@estudiante.uam.es)
 * @authors Mónica de la Iglesia Martínez (monica.delaiglesia@estudiante.uam.es)
 * @copyright Pareja 7 - Grupo 2313
 *
 * <hr>
 *
 */
SSL* conectar_canal_seguro_SSL(SSL_CTX *contexto, int *desc, int puerto, char* host)
{
  SSL* ssl;
  socklen_t c = 1;
  struct hostent* hostname = gethostbyname(host);
  struct sockaddr_in datos_servidor;

  /*Creamos el socket TCP*/
  *desc = socket(AF_INET, SOCK_STREAM, 6); /*TCP 6*/ /*UDP 17*/
  setsockopt( *desc, SOL_SOCKET, SO_REUSEADDR, (char *)&c, sizeof(c));

	memcpy(&datos_servidor.sin_addr, hostname->h_addr_list[0], hostname->h_length);

	datos_servidor.sin_family = AF_INET;
	datos_servidor.sin_port = htons(puerto);
	connect(*desc, (struct sockaddr*) &datos_servidor, sizeof(datos_servidor));

  ssl = SSL_new(contexto);
  if(ssl == NULL){
    ERR_print_errors_fp(stdout);
    return NULL;
  }

  if(!SSL_set_fd(ssl, *desc)){
    ERR_print_errors_fp(stdout);
    return NULL;
  }

  if(!SSL_connect(ssl)){
    ERR_print_errors_fp(stdout);
    return NULL;
  }

  return ssl;
}


/**
 * @page aceptar_canal_seguro_SSL aceptar_canal_seguro_SSL
 * @brief Bloquea la apliación esperando el handshake del cliente
 * <h2>Synopsis</h2>
 *
 * @code
 * #include "includes/G-2313-07-P3-ConnectionSSL.h"
 *
 * SSL* aceptar_canal_seguro_SSL(SSL_CTX *contexto, int* desc, struct sockaddr* datos_cliente, int puerto)
 * @endcode
 *
 * <h2>Descripción</h2>
 *
 * Dado un contexto SSL y un descriptor de socket esta función se encarga de
 * bloquear la aplicación, que se quedará esperando hasta recibir un handshake por parte del cliente.
 *
 * @param[in] contexto Contexto de la conexión SSL
 * @param[in] desc Puntero al descriptor del socket
 * @param[in] datos_cliente Puntero a estructura sockaddr con los datos del cliente
 * @param[in] puerto El numero del puerto que se va a usar en la conexión
 *
 * @retval SSL* Puntero a la conexion SSL que se acapa de crear
 * @retval NULL En caso de error
 *
 * <hr>
 *
 * <h2>Información</h2>
 * @authors Alfonso Bonilla Trueba (alfonso.bonilla@estudiante.uam.es)
 * @authors Mónica de la Iglesia Martínez (monica.delaiglesia@estudiante.uam.es)
 * @copyright Pareja 7 - Grupo 2313
 *
 * <hr>
 *
 */
SSL* aceptar_canal_seguro_SSL(SSL_CTX *contexto, int* desc, struct sockaddr* datos_cliente, int puerto)
{
  int nuevo_socket;
  socklen_t c = 1;
  socklen_t len;
  struct sockaddr_in direccion;
  SSL* ssl;

	len = sizeof(*datos_cliente);

  if(contexto == NULL || desc == NULL || datos_cliente == NULL || puerto <0)
    return NULL;

  /*Creamos el socket TCP*/
  *desc = socket(AF_INET, SOCK_STREAM, 6); /*TCP 6*/ /*UDP 17*/
  setsockopt(*desc, SOL_SOCKET, SO_REUSEADDR, (char *)&c, sizeof(c));

  syslog(LOG_INFO, "SERVER SSL : He creado un nuevo socket wiii");

	direccion.sin_family = AF_INET;
	direccion.sin_port = htons(puerto);
	direccion.sin_addr.s_addr = INADDR_ANY;

	bind(*desc, (struct sockaddr*) &direccion, sizeof(direccion));

  syslog(LOG_INFO, "SERVER SSL : bind hecho");

  listen(*desc, TAM_COLA);

  syslog(LOG_INFO, "SERVER SSL : listen hecho");

  nuevo_socket = accept(*desc, datos_cliente, &len);

  syslog(LOG_INFO, "SERVER SSL : accept hecho");

	ssl = SSL_new(contexto);
	if(ssl == NULL){
    ERR_print_errors_fp(stdout);
    return NULL;
  }

  if (!SSL_set_fd(ssl, nuevo_socket)){
    ERR_print_errors_fp(stdout);
    return NULL;
  }

  if(!SSL_accept(ssl)){
    ERR_print_errors_fp(stdout);
    return NULL;
  }

  return ssl;
}


/**
 * @page evaluar_post_connectar_SSL evaluar_post_connectar_SSL
 * @brief Comprueba la seguridad despueés del handshake
 * <h2>Synopsis</h2>
 *
 * @code
 * #include "includes/G-2313-07-P3-ConnectionSSL.h"
 *
 * long evaluar_post_connectar_SSL(const SSL *ssl)
 * @endcode
 *
 * <h2>Descripción</h2>
 *
 * Esta función comprueba una vez realizado el handshake que el canal de comunicación
 * se puede considerar seguro.
 *
 * @param[in] ssl Constante de la conexión SSL, no se va a modificar en la función
 *
 * @retval TRUE la comunicación a través del canal es segura
 * @retval FALSE la comunicación a través del canal no es segura
 *
 * <hr>
 *
 * <h2>Información</h2>
 * @authors Alfonso Bonilla Trueba (alfonso.bonilla@estudiante.uam.es)
 * @authors Mónica de la Iglesia Martínez (monica.delaiglesia@estudiante.uam.es)
 * @copyright Pareja 7 - Grupo 2313
 *
 * <hr>
 *
 */
long evaluar_post_connectar_SSL(const SSL *ssl)
{
  if(ssl == NULL)
    return FALSE;

  if(!SSL_get_peer_certificate(ssl)){
    ERR_print_errors_fp(stdout);
    return FALSE;
  }

  if(SSL_get_verify_result(ssl) != X509_V_OK){
    ERR_print_errors_fp(stdout);
    return FALSE;
  }

  return TRUE;
}


/**
 * @page enviar_datos_SSL enviar_datos_SSL
 * @brief Envia cualquier tipo de datos por un canal seguro
 * <h2>Synopsis</h2>
 *
 * @code
 * #include "includes/G-2313-07-P3-ConnectionSSL.h"
 *
 * long enviar_datos_SSL(SSL *ssl, const void* data)
 * @endcode
 *
 * <h2>Descripción</h2>
 * Esta función es equivalente a la función de envío de mensajes realizada en la
 * práctica 1, pero esta es utilizada para enviar datos a través del canal seguro.
 * Esta función es genérica y puede ser utilizada independientemente de los datos
 * que se vayan a enviar.
 *
 * @param[in] ssl Conexión SSL sobre la que se van a enviar datos.
 * @param[in] data Puntero const a la variable donde se encuentran los datos a enviar,
 * lo que quiere decir que los datos no se van a modificar]
 *
 * @retval TRUE Si los datos se han enviado correctamente.
 * @retval FALSE En caso de que ocurra algún fallo en el envío de datos.
 *
 * <hr>
 *
 * <h2>Información</h2>
 * @authors Alfonso Bonilla Trueba (alfonso.bonilla@estudiante.uam.es)
 * @authors Mónica de la Iglesia Martínez (monica.delaiglesia@estudiante.uam.es)
 * @copyright Pareja 7 - Grupo 2313
 *
 * <hr>
 *
 */
long enviar_datos_SSL(SSL *ssl, const void* data)
{
  if(ssl == NULL || data == NULL)
    return FALSE;

  if(!SSL_write(ssl, data, strlen((char*)data))){
    ERR_print_errors_fp(stdout);
    return FALSE;
  }

  return TRUE;
}


/**
 * @page recibir_datos_SSL recibir_datos_SSL
 * @brief Recibe cualquier tipo de datos por un canal seguro
 * <h2>Synopsis</h2>
 *
 * @code
 * #include "includes/G-2313-07-P3-ConnectionSSL.h"
 *
 * long recibir_datos_SSL(SSL *ssl, void* buf, int longitud)
 * @endcode
 *
 * <h2>Descripción</h2>
 *
 * Esta función es quivalente a la función de lectura de mensajes que se realizó en la
 * práctica 1, pero esta es utilizada para enviar datos a través del canal seguro.
 * Esta función es genérica y puede ser utilizada independientemente de los datos
 * que se vayan a recibir.
 *
 * @param[in] ssl Conexión SSL sobre la que se van a recibir datos.
 * @param[in] data Puntero a la variable donde se quieren guardar los datos recibidos.
 * @param[in] longitud Tamaño máximo que se quiere recibir.
 *
 * @retval TRUE Si los datos se han recibido correctamente
 * @retval FALSE En caso de que ocurra algún fallo en la recepción de datos
 *
 * <hr>
 *
 * <h2>Información</h2>
 * @authors Alfonso Bonilla Trueba (alfonso.bonilla@estudiante.uam.es)
 * @authors Mónica de la Iglesia Martínez (monica.delaiglesia@estudiante.uam.es)
 * @copyright Pareja 7 - Grupo 2313
 *
 * <hr>
 *
 */
long recibir_datos_SSL(SSL *ssl, void* data, int longitud)
{
  int recibido;

  if(ssl == NULL || data == NULL || longitud < 0)
    return FALSE;

  recibido = SSL_read(ssl, data, longitud);

  if(recibido <= 0){
    ERR_print_errors_fp(stdout);
    return FALSE;
  }

  return TRUE;
}


/**
 * @page cerrar_canal_SSL cerrar_canal_SSL
 * @brief Libera y cierra canal seguro
 * <h2>Synopsis</h2>
 *
 * @code
 * #include "includes/G-2313-07-P3-ConnectionSSL.h"
 *
 * void cerrar_canal_SSL(SSL *ssl, SSL_CTX *contexto, int socket)
 * @endcode
 *
 * <h2>Descripción</h2>
 *
 * Esta función liberar todos los recursos y cerrará el canal de comunicación seguro creado
 * previamente.
 *
 * @param[in] ssl Conexión SSL que se va a cerrar.
 * @param[in] contexto Puntero al contexto de la conexión que se va a cerrar.
 * @param[in] socket Descriptor del socket de la conexión a cerrar.
 *
 * <hr>
 *
 * <h2>Información</h2>
 * @authors Alfonso Bonilla Trueba (alfonso.bonilla@estudiante.uam.es)
 * @authors Mónica de la Iglesia Martínez (monica.delaiglesia@estudiante.uam.es)
 * @copyright Pareja 7 - Grupo 2313
 *
 * <hr>
 *
 */
void cerrar_canal_SSL(SSL *ssl, SSL_CTX *contexto, int socket)
{
  if(ssl == NULL || contexto == NULL || socket < 0)
    return;

  if(SSL_shutdown(ssl)!=1){
    ERR_print_errors_fp(stdout);
  }
  SSL_free(ssl);
  SSL_CTX_free(contexto);
  close(socket);
}
