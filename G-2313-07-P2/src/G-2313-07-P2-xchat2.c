/**
* @brief Programa principal y funciones de interfaz del Cliente.
* @file G-2313-07-P2-xchat2.c
*
* @authors Alfonso Bonilla Trueba (alfonso.bonilla@estudiante.uam.es)
* @authors Mónica de la Iglesia Martínez (monica.delaiglesia@estudiante.uam.es)
* @version 1.0
* @date 10-04-2017
*/

#include "../includes/G-2313-07-P2-ClientParser.h"
#include "../includes/G-2313-07-P2-ClientFunctions.h"


int desc = 0;             /**< @brief Valor del descriotor del socket */
conexion conex;			  /**< @brief Estrucura en la que se almancenan los datos de la conexion */
int ping_flag = 0;        /**< @brief Flag que controla el ping */
int audio_flag = 0;       /**< @brief Flag que controla el audio */
pthread_t thread_recv;    /**< @brief Hilo encargado de recibir ficheros */
pthread_t thread_ping;    /**< @brief Hilo encargado del protocolo PING-PONG para comprobar el estado del Servidor */


/*! \mainpage
 *
 * \section intro_sec Introducción
 *
 * Cliente IRC realizado por <b>Alfonso Bonilla Trueba</b> y <b>Mónica de la Iglesia Martínez</b> <br/>
 * Este cliente se ajusta a lo requerido en las prácticas de Redes de Comunicaciones 2
 * del Grado de Ingeniería Informática impartido en la EPS de la <a href="https://www.uam.es/UAM/Home.htm">
 * <b>Universidad Autónoma de Madrid</b></a><br>
 * Los RFCs implicados en el protocolo IRC de este cliente son
 * <a href="https://tools.ietf.org/html/rfc1459"><b>RFC-1459</b></a>,
 * <a href="https://tools.ietf.org/html/rfc2810"><b>RFC-2810</b></a>,
 * <a href="https://tools.ietf.org/html/rfc2811"><b>RFC-2811</b></a>,
 * <a href="https://tools.ietf.org/html/rfc2812"><b>RFC-2812</b></a>,
 * y <a href="https://tools.ietf.org/html/rfc2813"><b>RFC-2813</b></a>
 *
 * <hr>
 * <hr>
 * \section install_sec Instalación
 *
 * \subsection step1 Paso 1: Comando Make
 * Al hacer make sobre el directorio de la practica se genera el ejecutable <b>G-2313-07-P2</b> dentro de la carpeta raiz
 * \subsection step2 Paso 2.a: Ejecución del cliente
 * El cliente se ejecuta <b>~$: ./G-2313-07-P2 </b> y se abrirá una ventana con la interfáz del cliente.<br>
 * También se puede ejecutar accediendo a la carpeta obj y haciendo doble click sobre el ejecutable <b>G-2313-07-P2</b>
 * \subsection step3 Paso 2.b: Ejecución del cliente en segundo plano
 * Si se quiere lanzar el cliente desde un terminal, pero poder seguir usando esta <b>~$: ./G-2313-07-P2 & </b>
 * \subsection step4 Paso 3: Conexión al servidor:
 * Una vez lanzado el progrma cliente, aparece una ventana solicitando los datos de conexión al servidor:
 * <ul><li><b>Apódo</b>: Nick de usuario con el que se quiere conectar al servidor</li>
 * <li><b>Nombre de usuario</b>: </li>
 * <li><b>Nombre Real</b>: Nombre completo real del usuario.</li>
 * <li><b>Clave</b>: Si se tiene reservado el nick en el servidor al que se va a conectar, será necesario introducir la contraseña,
 * en caso contrario, dejar vacío.</li>
 * <li><b>Servidor</b>: dirección del servidor al que se va a conectar</li>
 * <li><b>Puerto</b>: puerto del servidor al que se va a conectar</li>
 * <li><b>Seguridad SSL</b>: Si se activa el switch, la comuniación con el servidor se realizará mediante un canal seguro.</ul>
 *
 * <hr>
 * <hr>
 * \section step5 Información sobre el Cliente
 * <ul>
 * <li>Tipicamente el puerto al que conectar será el 6667, pero dependerá del servidor al que queramos conectarnos</li>
 * <li>Para finalizar el cliente basta con cerrar la ventana, aunque tambien se puede con el comando <b>~$: killall G-2313-07-P2</b></li>
 * <li>Para ver los mensajes que enviamos al servidor y los que recibimos, el cliente tiene una ventana de registro
 * plano donde se pueden ver todo el intercambio de mensajes.</li>
 * <li>Para ver el registro plano correctamente hay que pulsar el switch para que comience a registrar.</li>
 * <li>En el lateral derecho se puede ver el listado de todos los usuarios conectados.</li>
 * </ul>
 */

 /*! @page irc_client_interface Interface Cliente
 *
 * <p>Esta sección incluye las funciones encargadas de los parseos del Cliente.<br>Estas
 * generan los mensajes IRC para enviar al servidor y descomponen los mensajes recibidos
 * del Servidor IRC para mostrar su contenido en la interfaz del Cliente.</p>
 *
 * <hr>
 * <hr>
 *
 * <h2>Funciones implementadas</h2>
 * <p>Se incluyen las siguientes funciones para parsear los mensajes del Cliente IRC:
 * <ul>
 * <li>@subpage IRCInterface_ActivateChannelKey</li>
 * <li>@subpage IRCInterface_ActivateExternalMessages</li>
 * <li>@subpage IRCInterface_ActivateInvite</li>
 * <li>@subpage IRCInterface_ActivateModerated</li>
 * <li>@subpage IRCInterface_ActivateNicksLimit</li>
 * <li>@subpage IRCInterface_ActivatePrivate</li>
 * <li>@subpage IRCInterface_ActivateProtectTopic</li>
 * <li>@subpage IRCInterface_ActivateSecret</li>
 * <li>@subpage IRCInterface_BanNick</li>
 * <li>@subpage IRCInterface_Connect</li>
 * <li>@subpage IRCInterface_DeactivateChannelKey</li>
 * <li>@subpage IRCInterface_DeactivateExternalMessages</li>
 * <li>@subpage IRCInterface_DeactivateInvite</li>
 * <li>@subpage IRCInterface_DeactivateModerated</li>
 * <li>@subpage IRCInterface_DeactivateNicksLimit</li>
 * <li>@subpage IRCInterface_DeactivatePrivate</li>
 * <li>@subpage IRCInterface_DeactivateProtectTopic</li>
 * <li>@subpage IRCInterface_DeactivateSecret</li>
 * <li>@subpage IRCInterface_DisconnectServer</li>
 * <li>@subpage IRCInterface_ExitAudioChat</li>
 * <li>@subpage IRCInterface_GiveOp</li>
 * <li>@subpage IRCInterface_GiveVoice</li>
 * <li>@subpage IRCInterface_KickNick</li>
 * <li>@subpage IRCInterface_NewCommandText</li>
 * <li>@subpage IRCInterface_NewTopicEnter</li>
 * <li>@subpage IRCInterface_SendFile</li>
 * <li>@subpage IRCInterface_StartAudioChat</li>
 * <li>@subpage IRCInterface_StopAudioChat</li>
 * <li>@subpage IRCInterface_TakeOp</li>
 * <li>@subpage IRCInterface_TakeVoice</li>
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
 * @page IRCInterface_ActivateChannelKey IRCInterface_ActivateChannelKey
 *
 * @brief Llamada por el botón de activación de la clave del canal.
 *
 * <h2>Synopsis</h2>
 * @code
 *	#include <redes2/ircxchat.h>
 *
 * 	void IRCInterface_ActivateChannelKey (char *channel, char * key)
 * @endcode
 *
 * <h2>Descripción</h2>
 * Llamada por el botón de activación de la clave del canal. El segundo parámetro es
 * la clave del canal que se desea poner. Si es NULL deberá impedirse la activación
 * con la función implementada a tal efecto. En cualquier caso sólo se puede realizar si el servidor acepta la orden.
 * Las strings recibidas no deben ser manipuladas por el programador, sólo leídas.
 *
 * @param[in] channel canal sobre el que se va a activar la clave.
 * @param[in] key clave para el canal indicado.
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
void IRCInterface_ActivateChannelKey(char *channel, char *key)
{
	char *msg = NULL;

	if(channel == NULL || key == NULL)
		return;

	/*Generamos el mensaje para establecer la contrasena*/
	IRCMsg_Mode(&msg, NULL, channel, "+k", key);

	/*Enviamos el mensaje al registro plano*/
	IRCInterface_PlaneRegisterOutMessage(msg);

	/*Enviamos el mensaje al servidor*/
	send(desc, msg, strlen(msg), 0);

	/*Establecemos en la interfaz la pass*/
	IRCInterface_SetChannelKey(key);

	free(msg);
}


/**
 *
 * @page IRCInterface_ActivateExternalMessages IRCInterface_ActivateExternalMessages
 *
 * @brief Llamada por el botón de activación de la recepción de mensajes externos.
 *
 * <h2>Synopsis</h2>
 * @code
 *	#include <redes2/ircxchat.h>
 *
 * 	void IRCInterface_ActivateExternalMessages (char *channel)
 * @endcode
 *
 * <h2>Descripción</h2>
 * Llamada por el botón de activación de la recepción de mensajes externos.
 *
 * En cualquier caso sólo se puede realizar si el servidor acepta la orden.
 * La string recibida no debe ser manipulada por el programador, sólo leída.
 *
 * @param[in] channel canal sobre el que se activará la recepción de mensajes externos.
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
void IRCInterface_ActivateExternalMessages(char *channel)
{
	char *msg = NULL;

	if(channel == NULL)
		return;

	IRCMsg_Mode(&msg, NULL, channel, "+n", channel);

	/*Enviamos el mensaje al registro plano*/
	IRCInterface_PlaneRegisterOutMessage(msg);

	/*Enviamos el mensaje al servidor*/
	send(desc, msg, strlen(msg), 0);

	/*Establecemos en la interfaz la activacion de mensajes externos*/
	IRCInterface_SetExternalMessages();

	free(msg);
}


/**
 *
 * @page IRCInterface_ActivateInvite IRCInterface_ActivateInvite
 *
 * @brief Llamada por el botón de activación de canal de sólo invitación.
 *
 * <h2>Synopsis</h2>
 * @code
 *	#include <redes2/ircxchat.h>
 *
 * 	void IRCInterface_ActivateInvite (char *channel)
 * @endcode
 *
 * <h2>Descripción</h2>
 * Llamada por el botón de activación de canal de sólo invitación.
 *
 * En cualquier caso sólo se puede realizar si el servidor acepta la orden.
 * La string recibida no debe ser manipulada por el programador, sólo leída.
 *
 * @param[in] channel canal sobre el que se activará la invitación.
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
void IRCInterface_ActivateInvite(char *channel)
{
	char *msg = NULL;

	if(channel == NULL)
		return;

	IRCMsg_Mode(&msg, NULL, channel, "+i", channel);

	/*Enviamos el mensaje al registro plano*/
	IRCInterface_PlaneRegisterOutMessage(msg);

	/*Enviamos el mensaje al servidor*/
	send(desc, msg, strlen(msg), 0);

	IRCInterface_SetInvite();

	free(msg);
}


/**
 *
 * @page IRCInterface_ActivateModerated IRCInterface_ActivateModerated
 *
 * @brief Llamada por el botón de activación de la moderación del canal.
 *
 * <h2>Synopsis</h2>
 * @code
 *	#include <redes2/ircxchat.h>
 *
 * 	void IRCInterface_ActivateModerated (char *channel)
 * @endcode
 *
 * <h2>Descripción</h2>
 * Llamada por el botón de activación de la moderación del canal.
 *
 * En cualquier caso sólo se puede realizar si el servidor acepta la orden.
 * La string recibida no debe ser manipulada por el programador, sólo leída.
 *
 * @param[in] channel canal sobre el que se activará la moderación.
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
void IRCInterface_ActivateModerated(char *channel)
{
	char *msg = NULL;

	if(channel == NULL)
		return;

	IRCMsg_Mode(&msg, NULL, channel, "+m", channel);

	/*Enviamos el mensaje al registro plano*/
	IRCInterface_PlaneRegisterOutMessage(msg);

	/*Enviamos el mensaje al servidor*/
	send(desc, msg, strlen(msg), 0);

	IRCInterface_SetModerated();

	free(msg);
}


/**
 *
 * @page IRCInterface_ActivateNicksLimit IRCInterface_ActivateNicksLimit
 *
 * @brief Llamada por el botón de activación del límite de usuarios en el canal.
 *
 * <h2>Synopsis</h2>
 * @code
 *	#include <redes2/ircxchat.h>
 *
 * 	void IRCInterface_ActivateNicksLimit (char *channel, int * limit)
 * @endcode
 *
 * <h2>Descripción</h2>
 * Llamada por el botón de activación del límite de usuarios en el canal. El segundo es el
 * límite de usuarios que se desea poner. Si el valor es 0 se sobrentiende que se desea eliminar
 * este límite.
 *
 * En cualquier caso sólo se puede realizar si el servidor acepta la orden.
 * La string recibida no debe ser manipulada por el programador, sólo leída.
 *
 * @param[in] channel canal sobre el que se activará el límite de usuarios.
 * @param[in] limit límite de usuarios en el canal indicado.
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
void IRCInterface_ActivateNicksLimit(char *channel, int limit)
{
	char *msg = NULL;
	char limite[10];

	if(channel == NULL || limit < 0)
		return;

	strcpy(limite, "");
	sprintf(limite, "%d", limit);

	IRCMsg_Mode(&msg, NULL, channel, "+l", limite);

	/*Enviamos el mensaje al registro plano*/
	IRCInterface_PlaneRegisterOutMessage(msg);

	/*Enviamos el mensaje al servidor*/
	send(desc, msg, strlen(msg), 0);

	IRCInterface_SetNicksLimit(limit);

	free(msg);
}


/**
 *
 * @page IRCInterface_ActivatePrivate IRCInterface_ActivatePrivate
 *
 * @brief Llamada por el botón de activación del modo privado.
 *
 * <h2>Synopsis</h2>
 * @code
 *	#include <redes2/ircxchat.h>
 *
 * 	void IRCInterface_ActivatePrivate (char *channel)
 * @endcode
 *
 * <h2>Descripción</h2>
 * Llamada por el botón de activación del modo privado.
 *
 * En cualquier caso sólo se puede realizar si el servidor acepta la orden.
 * La string recibida no debe ser manipulada por el programador, sólo leída.
 *
 * @param[in] channel canal sobre el que se va a activar la privacidad.
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
void IRCInterface_ActivatePrivate(char *channel)
{
	char *msg = NULL;

	if(channel == NULL)
		return;

	IRCMsg_Mode(&msg, NULL, channel, "+p", channel);

	/*Enviamos el mensaje al registro plano*/
	IRCInterface_PlaneRegisterOutMessage(msg);

	/*Enviamos el mensaje al servidor*/
	send(desc, msg, strlen(msg), 0);

	IRCInterface_SetPrivate();

	free(msg);
}


/**
 *
 * @page IRCInterface_ActivateProtectTopic IRCInterface_ActivateProtectTopic
 *
 * @brief Llamada por el botón de activación de la protección de tópico.
 *
 * <h2>Synopsis</h2>
 * @code
 *	#include <redes2/ircxchat.h>
 *
 * 	void IRCInterface_ActivateProtectTopic (char *channel)
 * @endcode
 *
 * <h2>Descripción</h2>
 * Llamada por el botón de activación de la protección de tópico.
 *
 * En cualquier caso sólo se puede realizar si el servidor acepta la orden.
 * La string recibida no debe ser manipulada por el programador, sólo leída.
 *
 * @param[in] channel canal sobre el que se va a activar la protección de tópico.
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
void IRCInterface_ActivateProtectTopic(char *channel)
{
	char *msg = NULL;

	if(channel == NULL)
		return;

	IRCMsg_Mode(&msg, NULL, channel, "+t", NULL);

	if(msg != NULL){
		/*Enviamos el mensaje al registro plano*/
		IRCInterface_PlaneRegisterOutMessage(msg);

		/*Enviamos el mensaje al servidor*/
		send(desc, msg, strlen(msg), 0);

		/*Establecemos la proteccion de topic en la interfaz*/
		IRCInterface_SetProtectTopic();

		free(msg);
	}
}


/**
 *
 * @page IRCInterface_ActivateSecret IRCInterface_ActivateSecret
 *
 * @brief Llamada por el botón de activación de canal secreto.
 *
 * <h2>Synopsis</h2>
 * @code
 *	#include <redes2/ircxchat.h>
 *
 * 	void IRCInterface_ActivateSecret (char *channel)
 * @endcode
 *
 * <h2>Descripción</h2>
 * Llamada por el botón de activación de canal secreto.
 *
 * En cualquier caso sólo se puede realizar si el servidor acepta la orden.
 * La string recibida no debe ser manipulada por el programador, sólo leída.
 *
 * @param[in] channel canal sobre el que se va a activar el estado de secreto.
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
void IRCInterface_ActivateSecret(char *channel)
{
	char *msg = NULL;

	if(channel == NULL)
		return;

	IRCMsg_Mode(&msg, NULL, channel, "+s", NULL);

	/*Enviamos el mensaje al registro plano*/
	IRCInterface_PlaneRegisterOutMessage(msg);

	/*Enviamos el mensaje al servidor*/
	send(desc, msg, strlen(msg), 0);

	IRCInterface_SetSecret();

	free(msg);
}


/**
 *
 * @page IRCInterface_BanNick IRCInterface_BanNick
 *
 * @brief Llamada por el botón "Banear".
 *
 * <h2>Synopsis</h2>
 * @code
 *	#include <redes2/ircxchat.h>
 *
 * 	void IRCInterface_BanNick (char *channel, char *nick)
 * @endcode
 *
 * <h2>Descripción</h2>
 * Llamada por el botón "Banear". Previamente debe seleccionarse un nick del
 * canal para darle voz a dicho usuario.
 *
 * En cualquier caso sólo se puede realizar si el servidor acepta la orden.
 * Las strings recibidas no deben ser manipuladas por el programador, sólo leídas.
 *
 * @param[in] channel canal sobre el que se va a realizar el baneo. En principio es un valor innecesario.
 * @param[in] nick nick del usuario que va a ser baneado
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
void IRCInterface_BanNick(char *channel, char *nick)
{
	char *msg = NULL;

	if(channel == NULL || nick == NULL)
		return;

	IRCMsg_Mode(&msg, NULL, channel, "+b", nick);

	/*Enviamos el mensaje al registro plano*/
	IRCInterface_PlaneRegisterOutMessage(msg);

	/*Enviamos el mensaje al servidor*/
	send(desc, msg, strlen(msg), 0);

	free(msg);
}


/**
 *
 * @page IRCInterface_Connect IRCInterface_Connect
 *
 * @brief Llamada por los distintos botones de conexión.
 *
 * <h2>Synopsis</h2>
 * @code
 *	#include <redes2/ircxchat.h>
 *
 * 	long IRCInterface_Connect (char *nick, char * user, char * realname, char * password, char * server, int port, boolean ssl)
 * @endcode
 *
 * <h2>Descripción</h2>
 * Llamada por los distintos botones de conexión. Si implementará la comunicación completa, incluido
 * el registro del usuario en el servidor.
 *
 * En cualquier caso sólo se puede realizar si el servidor acepta la orden.
 * Las strings recibidas no deben ser manipuladas por el programador, sólo leída.
 *
 *
 * @param[in] nick nick con el que se va a realizar la conexíón.
 * @param[in] user usuario con el que se va a realizar la conexión.
 * @param[in] realname nombre real con el que se va a realizar la conexión.
 * @param[in] password password del usuario si es necesaria, puede valer NULL.
 * @param[in] server nombre o ip del servidor con el que se va a realizar la conexión.
 * @param[in] port puerto del servidor con el que se va a realizar la conexión.
 * @param[in] ssl puede ser TRUE si la conexión tiene que ser segura y FALSE si no es así.
 *
 * @retval IRC_OK si todo ha sido correcto (debe devolverlo).
 * @retval IRCERR_NOSSL si el valor de SSL es TRUE y no se puede activar la conexión SSL pero sí una
 * conexión no protegida (debe devolverlo).
 * @retval IRCERR_NOCONNECT en caso de que no se pueda realizar la comunicación (debe devolverlo).
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
long IRCInterface_Connect(char *nick, char *user, char *realname, char *password, char *server, int port, boolean ssl)
{


	struct sockaddr_in serveraddr;
	struct hostent *servidor;
	char *msgNick = NULL, *msgUser = NULL, *msgPass = NULL, *msg = NULL;
	thread_info* info = NULL;

	if(nick == NULL || user == NULL || realname == NULL || server == NULL)
		return IRCERR_NOCONNECT;

	/*Creamos el socket para el cliente*/
	desc = socket(AF_INET, SOCK_STREAM, 0);
	if (desc < 0) {
			syslog(LOG_INFO, "CLIENTE: Error creando el socket");
			return IRCERR_NOCONNECT;
	}

	/*Obtenemos la DNS del servidor*/
	servidor = gethostbyname(server);
	if (servidor == NULL) {
			syslog(LOG_INFO, "CLIENTE: No existe el servidor %s\n", server);
			return IRCERR_NOCONNECT;
	}

	/*Comenzamos la conexion*/
	bzero((char *) &serveraddr, sizeof (serveraddr));
	serveraddr.sin_family = AF_INET;
	bcopy((char *) servidor->h_addr, (char *) &serveraddr.sin_addr.s_addr, servidor->h_length);
	serveraddr.sin_port = htons(port);

	/*Conexion con el servidor*/
	if (connect(desc, (struct sockaddr*) &serveraddr, sizeof (serveraddr)) < 0) {
			syslog(LOG_INFO, "CLIENTE: Error conectando con el servidor %s", server);
			return IRCERR_NOCONNECT;
	}

	/*Generamos los mensajes para registrar al usuario*/
	IRCMsg_Nick(&msgNick, NULL, nick, NULL);
	IRCMsg_User(&msgUser, NULL, user, server, realname);
	IRCMsg_Pass(&msgPass, NULL, password);

	/*Juntamos todos los mensajes en uno solo y lo enviamos al servidor*/
	if(strcmp(password, "") != 0){
		IRC_PipelineCommands(&msg, msgNick, msgUser, msgPass, NULL);
	}else{
		IRC_PipelineCommands(&msg, msgNick, msgUser, NULL);
	}


	if (send(desc, msg, strlen(msg), 0) < 0) {
		syslog(LOG_INFO, "CLIENTE: Error al enviar el mensaje de registro al servidor");
		return IRCERR_NOCONNECT;
	}
	syslog(LOG_INFO, "CLIENTE: Mensaje para registro enviado al servidor %s", server);

	/*Enviamos al registro plano los mensajes que acabamos de enviar*/
	IRCInterface_PlaneRegisterOutMessage(msg);

	/*Liberamos memoria*/
	IRC_MFree(4, &msg, &msgNick, &msgUser, &msgPass);

	/*Creamos hilo encargado de PING*/
	info = (thread_info*) malloc(sizeof (thread_info));
	info->sd = desc;
	pthread_create(&thread_ping, NULL, IRC_Client_Ping, (void *) info);

	/*Creamos el hilo encargado de recibir*/
	info = (thread_info*) malloc(sizeof (thread_info));
	info->sd = desc;
	pthread_create(&thread_recv, NULL, IRC_Client_Recieve, (void *) info);

	conex.nick = nick;
	conex.user = user;
	conex.realname = realname;
	conex.password = password;
	conex.server = server;
	conex.port = port;
	conex.desc = desc;

	return IRC_OK;
}


/**
 *
 * @page IRCInterface_DeactivateChannelKey IRCInterface_DeactivateChannelKey
 *
 * @brief Llamada por el botón de desactivación de la clave del canal.
 *
 * <h2>Synopsis</h2>
 * @code
 *	#include <redes2/ircxchat.h>
 *
 * 	void IRCInterface_DeactivateChannelKey (char *channel)
 * @endcode
 *
 * <h2>Descripción</h2>
 * Llamada por el botón de desactivación de la clave del canal.
 *
 * En cualquier caso sólo se puede realizar si el servidor acepta la orden.
 * La string recibida no debe ser manipulada por el programador, sólo leída.
 *
 * @param[in] channel canal sobre el que se va a desactivar la clave.
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
void IRCInterface_DeactivateChannelKey(char *channel)
{
	char *msg = NULL;

	if(channel == NULL)
		return;

	/*Generamos el mensaje para desactivar la contrasena*/
	IRCMsg_Mode(&msg, NULL, channel, "-k", NULL);

	/*Enviamos al registro plano el mensaje*/
	IRCInterface_PlaneRegisterOutMessage(msg);

	/*Enviamos el mensaje al servidor*/
	send(desc, msg, strlen(msg), 0);

	/*Establecemos en la interfaz que no hay pass*/
	IRCInterface_UnsetChannelKey();

	free(msg);
}


/**
 *
 * @page IRCInterface_DeactivateExternalMessages IRCInterface_DeactivateExternalMessages
 *
 * @brief Llamada por el botón de desactivación de la recepción de mensajes externos.
 *
 * <h2>Synopsis</h2>
 * @code
 *	#include <redes2/ircxchat.h>
 *
 * 	void IRCInterface_DeactivateExternalMessages (char *channel)
 * @endcode
 *
 * <h2>Descripción</h2>
 * Llamada por el botón de desactivación de la recepción de mensajes externos.
 *
 * En cualquier caso sólo se puede realizar si el servidor acepta la orden.
 * La string recibida no debe ser manipulada por el programador, sólo leída.
 *
 * @param[in] channel canal sobre el que se va a deactivar la recepción de mensajes externos.
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
void IRCInterface_DeactivateExternalMessages(char *channel)
{
	char *msg = NULL;

	if(channel == NULL)
		return;

	IRCMsg_Mode(&msg, NULL, channel, "-n", NULL);

	/*Enviamos el mensaje al registro plano*/
	IRCInterface_PlaneRegisterOutMessage(msg);

	/*Enviamos el mensaje al servidor*/
	send(desc, msg, strlen(msg), 0);

	IRCInterface_UnsetExternalMessages();

	free(msg);
}


/**
 *
 * @page IRCInterface_DeactivateInvite IRCInterface_DeactivateInvite
 *
 * @brief Llamada por el botón de desactivación de canal de sólo invitación.
 *
 * <h2>Synopsis</h2>
 * @code
 *	#include <redes2/ircxchat.h>
 *
 * 	void IRCInterface_DeactivateInvite (char *channel)
 * @endcode
 *
 * <h2>Descripción</h2>
 * Llamada por el botón de desactivación de canal de sólo invitación.
 *
 * En cualquier caso sólo se puede realizar si el servidor acepta la orden.
 * La string recibida no debe ser manipulada por el programador, sólo leída.
 *
 * @param[in] channel canal sobre el que se va a desactivar la invitación.
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
void IRCInterface_DeactivateInvite(char *channel)
{
	char *msg = NULL;

	if(channel == NULL)
		return;

	IRCMsg_Mode(&msg, NULL, channel, "-i", NULL);

	/*Enviamos el mensaje al registro plano*/
	IRCInterface_PlaneRegisterOutMessage(msg);

	/*Enviamos el mensaje al servidor*/
	send(desc, msg, strlen(msg), 0);

	IRCInterface_UnsetInvite();

	free(msg);
}


/**
 *
 * @page IRCInterface_DeactivateModerated IRCInterface_DeactivateModerated
 *
 * @brief Llamada por el botón de desactivación  de la moderación del canal.
 *
 * <h2>Synopsis</h2>
 * @code
 *	#include <redes2/ircxchat.h>
 *
 * 	void IRCInterface_DeactivateModerated (char *channel)
 * @endcode
 *
 * <h2>Descripción</h2>
 * Llamada por el botón de desactivación  de la moderación del canal.
 *
 * En cualquier caso sólo se puede realizar si el servidor acepta la orden.
 * La string recibida no debe ser manipulada por el programador, sólo leída.
 *
 * @param[in] channel canal sobre el que se va a desactivar la moderación.
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
void IRCInterface_DeactivateModerated(char *channel)
{
	char *msg = NULL;

	if(channel == NULL)
		return;

	IRCMsg_Mode(&msg, NULL, channel, "-m", NULL);

	/*Enviamos el mensaje al registro plano*/
	IRCInterface_PlaneRegisterOutMessage(msg);

	/*Enviamos el mensaje al servidor*/
	send(desc, msg, strlen(msg), 0);

	IRCInterface_UnsetModerated();

	free(msg);
}


/**
 *
 * @page IRCInterface_DeactivateNicksLimit IRCInterface_DeactivateNicksLimit
 *
 * @brief Llamada por el botón de desactivación de la protección de tópico.
 *
 * <h2>Synopsis</h2>
 * @code
 *	#include <redes2/ircxchat.h>
 *
 * 	void IRCInterface_DeactivateNicksLimit (char *channel)
 * @endcode
 *
 * <h2>Descripción</h2>
 * Llamada por el botón de desactivación  del límite de usuarios en el canal.
 *
 * En cualquier caso sólo se puede realizar si el servidor acepta la orden.
 * La string recibida no debe ser manipulada por el programador, sólo leída.
 *
 * @param[in] channel canal sobre el que se va a desactivar el límite de usuarios.
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
void IRCInterface_DeactivateNicksLimit(char *channel)
{
	char *msg = NULL;

	if(channel == NULL)
		return;

	IRCMsg_Mode(&msg, NULL, channel, "-l", NULL);

	/*Enviamos el mensaje al registro plano*/
	IRCInterface_PlaneRegisterOutMessage(msg);

	/*Enviamos el mensaje al servidor*/
	send(desc, msg, strlen(msg), 0);

	/*Establecemos en la interfaz que ha sido quitado el limite*/
	IRCInterface_UnsetNicksLimit();

	free(msg);
}


/**
 *
 * @page IRCInterface_DeactivatePrivate IRCInterface_DeactivatePrivate
 *
 * @brief Llamada por el botón de desactivación del modo privado.
 *
 * <h2>Synopsis</h2>
 * @code
 *	#include <redes2/ircxchat.h>
 *
 * 	void IRCInterface_DeactivatePrivate (char *channel)
 * @endcode
 *
 * <h2>Descripción</h2>
 * Llamada por el botón de desactivación del modo privado.
 *
 * En cualquier caso sólo se puede realizar si el servidor acepta la orden.
 * La string recibida no debe ser manipulada por el programador, sólo leída.
 *
 * @param[in] channel canal sobre el que se va a desactivar la privacidad.
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
void IRCInterface_DeactivatePrivate(char *channel)
{
	char *msg = NULL;

	if(channel == NULL)
		return;

	IRCMsg_Mode(&msg, NULL, channel, "-p", NULL);

	/*Enviamos el mensaje al registro plano*/
	IRCInterface_PlaneRegisterOutMessage(msg);

	/*Enviamos el mensaje al servidor*/
	send(desc, msg, strlen(msg), 0);

	IRCInterface_UnsetPrivate();

	free(msg);
}


/**
 *
 * @page IRCInterface_DeactivateProtectTopic IRCInterface_DeactivateProtectTopic
 *
 * @brief Llamada por el botón de desactivación de la protección de tópico.
 *
 * <h2>Synopsis</h2>
 * @code
 *	#include <redes2/ircxchat.h>
 *
 * 	void IRCInterface_DeactivateProtectTopic (char *channel)
 * @endcode
 *
 * <h2>Descripción</h2>
 * Llamada por el botón de desactivación de la protección de tópico.
 *
 * En cualquier caso sólo se puede realizar si el servidor acepta la orden.
 * La string recibida no debe ser manipulada por el programador, sólo leída.
 *
 * @param[in] channel canal sobre el que se va a desactivar la protección de tópico.
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
void IRCInterface_DeactivateProtectTopic(char *channel)
{
	char *msg = NULL;

	if(channel == NULL)
		return;

	IRCMsg_Mode(&msg, NULL, channel, "-t", NULL);

	/*Enviamos el mensaje al registro plano*/
	IRCInterface_PlaneRegisterOutMessage(msg);

	/*Enviamos el mensaje al servidor*/
	send(desc, msg, strlen(msg), 0);

	/*Establecemos que ya no esta protegido por topic en la interfaz*/
	IRCInterface_UnsetProtectTopic();

	free(msg);
}


/**
 *
 * @page IRCInterface_DeactivateSecret IRCInterface_DeactivateSecret
 *
 * @brief Llamada por el botón de desactivación de canal secreto.
 *
 * <h2>Synopsis</h2>
 * @code
 *	#include <redes2/ircxchat.h>
 *
 * 	void IRCInterface_DeactivateSecret (char *channel)
 * @endcode
 *
 * <h2>Descripción</h2>
 * Llamada por el botón de desactivación de canal secreto.
 *
 * En cualquier caso sólo se puede realizar si el servidor acepta la orden.
 * La string recibida no debe ser manipulada por el programador, sólo leída.
 *
 * @param[in] channel canal sobre el que se va a desactivar la propiedad de canal secreto.
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
void IRCInterface_DeactivateSecret(char *channel)
{
	char *msg = NULL;

	if(channel == NULL)
		return;

	IRCMsg_Mode(&msg, NULL, channel, "-s", NULL);

	/*Enviamos el mensaje al registro plano*/
	IRCInterface_PlaneRegisterOutMessage(msg);

	/*Enviamos el mensaje al servidor*/
	send(desc, msg, strlen(msg), 0);

	IRCInterface_UnsetSecret();

	free(msg);
}


/**
 *
 * @page IRCInterface_DisconnectServer IRCInterface_DisconnectServer
 *
 * @brief Llamada por los distintos botones de desconexión.
 *
 * <h2>Synopsis</h2>
 * @code
 *	#include <redes2/ircxchat.h>
 *
 * 	void IRCInterface_DisconnectServer (char * server, int port)
 * @endcode
 *
 * <h2>Descripción</h2>
 * Llamada por los distintos botones de desconexión. Debe cerrar la conexión con el servidor.
 *
 * En cualquier caso sólo se puede realizar si el servidor acepta la orden.
 * La string recibida no debe ser manipulada por el programador, sólo leída.

 * @param[in] server nombre o ip del servidor del que se va a realizar la desconexión.
 * @param[in] port puerto sobre el que se va a realizar la desconexión.
 *
 * @retval TRUE si se ha cerrado la conexión (debe devolverlo).
 * @retval FALSE en caso contrario (debe devolverlo).
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
boolean IRCInterface_DisconnectServer(char *server, int port)
{
	char *msg;

	if(server == NULL || port < 0)
		return FALSE;

	IRCMsg_Quit(&msg, NULL, "Leaving");

	/*Enviamos el mensaje al servidor*/
	send(desc, msg, strlen(msg), 0);

	IRCInterface_PlaneRegisterOutMessage(msg);


	close(desc);
	free(msg);

	return TRUE;
}


/**
 *
 * @page IRCInterface_ExitAudioChat IRCInterface_ExitAudioChat
 *
 * @brief Llamada por el botón "Cancelar" del diálogo de chat de voz.
 *
 * <h2>Synopsis</h2>
 * @code
 *	#include <redes2/ircxchat.h>
 *
 * 	void IRCInterface_ExitAudioChat (char *nick)
 * @endcode
 *
 * <h2>Descripción</h2>
 * Llamada por el botón "Parar" del diálogo de chat de voz. Previamente debe seleccionarse un nick del
 * canal para darle voz a dicho usuario. Esta función cierrala comunicación. Evidentemente tiene que
 * actuar sobre el hilo de chat de voz.
 *
 * En cualquier caso sólo se puede realizar si el servidor acepta la orden.
 * La string recibida no debe ser manipulada por el programador, sólo leída.
 *
 * @param[in] nick nick del usuario que solicita la parada del chat de audio.
 *
 * @retval TRUE si se ha cerrado la comunicación (debe devolverlo).
 * @retval FALSE en caso contrario (debe devolverlo).
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
boolean IRCInterface_ExitAudioChat(char *nick)
{
	if(nick == NULL)
		return FALSE;

	return TRUE;
}


/**
 *
 * @page IRCInterface_GiveOp IRCInterface_GiveOp
 *
 * @brief Llamada por el botón "Op".
 *
 * <h2>Synopsis</h2>
 * @code
 *	#include <redes2/ircxchat.h>
 *
 * 	void IRCInterface_GiveOp (char *channel, char *nick)
 * @endcode
 *
 * <h2>Descripción</h2>
 * Llamada por el botón "Op". Previamente debe seleccionarse un nick del
 * canal para darle "op" a dicho usuario.
 *
 * En cualquier caso sólo se puede realizar si el servidor acepta la orden.
 * Las strings recibidas no deben ser manipuladas por el programador, sólo leídas.
 *
 * @param[in] channel canal sobre el que se va dar op al usuario.
 * @param[in] nick nick al que se le va a dar el nivel de op.
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
void IRCInterface_GiveOp(char *channel, char *nick)
{
	char *msg = NULL;

	if(channel == NULL || nick == NULL)
		return;

	/*Generamos el mansaje y lo enviamos al servidor*/
	IRCMsg_Mode(&msg, NULL, channel, "+o", nick);

	send(desc, msg, strlen(msg), 0);

	/*Enviamos el mensaje al registro plano*/
	IRCInterface_PlaneRegisterOutMessage(msg);

	free(msg);
}


/**
 *
 * @page IRCInterface_GiveVoice IRCInterface_GiveVoice
 *
 * @brief Llamada por el botón "Dar voz".
 *
 * <h2>Synopsis</h2>
 * @code
 *	#include <redes2/ircxchat.h>
 *
 * 	void IRCInterface_GiveVoice (char *channel, char *nick)
 * @endcode
 *
 * <h2>Descripción</h2>
 * Llamada por el botón "Dar voz". Previamente debe seleccionarse un nick del
 * canal para darle voz a dicho usuario.
 *
 * En cualquier caso sólo se puede realizar si el servidor acepta la orden.
 * Las strings recibidas no deben ser manipuladas por el programador, sólo leídas.
 *
 * @param[in] channel canal sobre el que se va dar voz al usuario.
 * @param[in] nick nick al que se le va a dar voz.
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
void IRCInterface_GiveVoice(char *channel, char *nick)
{
	char *msg = NULL;

	if(channel == NULL || nick == NULL)
		return;

	/*Generamos el mensaje y lo enviamos al servidor*/
	IRCMsg_Mode(&msg, NULL, channel, "+v", nick);

	send(desc, msg, strlen(msg), 0);

	/*Enviamos el mensaje al registro plano*/
	IRCInterface_PlaneRegisterOutMessage(msg);

	free(msg);
}


/**
 *
 * @page IRCInterface_KickNick IRCInterface_KickNick
 *
 * @brief Llamada por el botón "Echar".
 *
 * <h2>Synopsis</h2>
 * @code
 *	#include <redes2/ircxchat.h>
 *
 * 	void IRCInterface_KickNick (char *channel, char *nick)
 * @endcode
 *
 * <h2>Descripción</h2>
 * Llamada por el botón "Echar". Previamente debe seleccionarse un nick del
 * canal para darle voz a dicho usuario.
 *
 * En cualquier caso sólo se puede realizar si el servidor acepta la orden.
 * Las strings recibidas no deben ser manipuladas por el programador, sólo leídas.
 *
 * @param[in] channel canal sobre el que se va a expulsar al usuario.
 * @param[in] nick nick del usuario que va a ser expulsado.
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
void IRCInterface_KickNick(char *channel, char *nick)
{
	char *msg = NULL;

	if(channel == NULL || nick == NULL)
		return;

	/*Generamos el mensaje para el comando KICK*/
	IRCMsg_Kick(&msg, NULL, channel, nick, NULL);

	/*Lo enviamos al registro plano*/
  IRCInterface_PlaneRegisterOutMessage(msg);

	/*Enviamos al servidor el comando */
  send(desc, msg, strlen(msg), 0);

	free(msg);
}


/**
 *
 * @page IRCInterface_NewCommandText IRCInterface_NewCommandText
 *
 * @brief Llamada la tecla ENTER en el campo de texto y comandos.
 *
 * <h2>Synopsis</h2>
 * @code
 *	#include <redes2/ircxchat.h>
 *
 * 	void IRCInterface_NewCommandText (char *command)
 * @endcode
 *
 * <h2>Descripción</h2>
 * Llamada de la tecla ENTER en el campo de texto y comandos. El texto deberá ser
 * enviado y el comando procesado por las funciones de "parseo" de comandos de
 * usuario.
 *
 * En cualquier caso sólo se puede realizar si el servidor acepta la orden.
 * La string recibida no debe ser manipulada por el programador, sólo leída.
 *
 * @param[in] comando introducido por el usuario.
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
void IRCInterface_NewCommandText(char *command)
{
	char *msg = NULL;
	long option = IRCUser_CommandQuery(command);

	if(command == NULL)
		return;

	/*Generamos el mensaje adecuado y lo enviamos al servidor*/
	if((msg = IRC_Client_Parser(command, option)) == NULL)
		return;

	send(desc, msg, strlen(msg), 0);

	/*Enviamos el mensaje al registro plano*/
	IRCInterface_PlaneRegisterOutMessage(msg);

	free(msg);
}


/**
 *
 * @page IRCInterface_NewTopicEnter IRCInterface_NewTopicEnter
 *
 * @brief Llamada cuando se pulsa la tecla ENTER en el campo de tópico.
 *
 * <h2>Synopsis</h2>
 * @code
 *	#include <redes2/ircxchat.h>
 *
 * 	void IRCInterface_NewTopicEnter (char * topicdata)
 * @endcode
 *
 * <h2>Descripción</h2>
 * Llamada cuando se pulsa la tecla ENTER en el campo de tópico.
 * Deberá intentarse cambiar el tópico del canal.
 *
 * En cualquier caso sólo se puede realizar si el servidor acepta la orden.
 * La string recibida no debe ser manipulada por el programador, sólo leída.
 *
 * param[in] topicdata string con el tópico que se desea poner en el canal.
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
void IRCInterface_NewTopicEnter(char *topicdata)
{
	char *msg = NULL;

	if(topicdata == NULL)
		return;

	/*Generamos el mensaje para enviar al servidor*/
	IRCMsg_Topic(&msg, NULL, IRCInterface_ActiveChannelName(), topicdata);

	send(desc, msg, strlen(msg), 0);

	/*Enviamos el mensaje al registro plano*/
	IRCInterface_PlaneRegisterOutMessage(msg);

	free(msg);
}



/**
 *
 * @page IRCInterface_SendFile IRCInterface_SendFile
 *
 * @brief Llamada por el botón "Enviar Archivo".
 *
 * <h2>Synopsis</h2>
 * @code
 *	#include <redes2/ircxchat.h>
 *
 * 	void IRCInterface_SendFile (char * filename, char *nick, char *data, long unsigned int length)
 * @endcode
 *
 * <h2>Descripción</h2>
 * Llamada por el botón "Enviar Archivo". Previamente debe seleccionarse un nick del
 * canal para darle voz a dicho usuario. Esta función como todos los demás callbacks bloquea el interface
 * y por tanto es el programador el que debe determinar si crea un nuevo hilo para enviar el archivo o
 * no lo hace.
 *
 * En cualquier caso sólo se puede realizar si el servidor acepta la orden.
 * Las strings recibidas no deben ser manipuladas por el programador, sólo leídas.
 *
 * @param[in] filename nombre del fichero a enviar.
 * @param[in] nick nick del usuario que enviará el fichero.
 * @param[in] data datos a ser enviados.
 * @param[in] length longitud de los datos a ser enviados.
 *
 * @retval TRUE si se ha establecido la comunicación (debe devolverlo).
 * @retval FALSE en caso contrario (debe devolverlo).
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
boolean IRCInterface_SendFile(char *filename, char *nick, char *data, long unsigned int length)
{
	thread_fich *fich_env = NULL;
	pthread_t id;

	fich_env = (thread_fich *) malloc (sizeof(thread_fich));
	fich_env->nick = nick;
	fich_env->filename = filename;
	fich_env->data = data;
	fich_env->datalen = length;

	if(pthread_create(&id, NULL, &IRC_Send_File, (void *)fich_env) != 0){
		IRCInterface_ErrorDialogThread ("Fallo al crear el hilo del fichero");
		return FALSE;
	}

	pthread_detach(id);
	return TRUE;
}


/**
 *
 * @page IRCInterface_StartAudioChat IRCInterface_StartAudioChat
 *
 * @brief Llamada por el botón "Iniciar" del diálogo de chat de voz.
 *
 * <h2>Synopsis</h2>
 * @code
 *	#include <redes2/ircxchat.h>
 *
 * 	void IRCInterface_StartAudioChat (char *nick)
 * @endcode
 *
 * <h2>Descripción</h2>
 * Llamada por el botón "Iniciar" del diálogo de chat de voz. Previamente debe seleccionarse un nick del
 * canal para darle voz a dicho usuario. Esta función como todos los demás callbacks bloquea el interface
 * y por tanto para mantener la funcionalidad del chat de voz es imprescindible crear un hilo a efectos
 * de comunicación de voz.
 *
 * En cualquier caso sólo se puede realizar si el servidor acepta la orden.
 * La string recibida no debe ser manipulada por el programador, sólo leída.
 *
 * @param[in] nick nick del usuario con el que se desea conectar.
 *
 * @retval TRUE si se ha establecido la comunicación (debe devolverlo).
 * @retval FALSE en caso contrario (debe devolverlo).
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
boolean IRCInterface_StartAudioChat(char *nick)
{
	if(nick == NULL)
		return FALSE;

	return TRUE;
}


/**
 *
 * @page IRCInterface_StopAudioChat IRCInterface_StopAudioChat
 *
 * @brief Llamada por el botón "Parar" del diálogo de chat de voz.
 *
 * <h2>Synopsis</h2>
 * @code
 *	#include <redes2/ircxchat.h>
 *
 * 	void IRCInterface_StopAudioChat (char *nick)
 * @endcode
 *
 * <h2>Descripción</h2>
 * Llamada por el botón "Parar" del diálogo de chat de voz. Previamente debe seleccionarse un nick del
 * canal para darle voz a dicho usuario. Esta función sólo para la comunicación que puede ser reiniciada.
 * Evidentemente tiene que actuar sobre el hilo de chat de voz.
 *
 * En cualquier caso sólo se puede realizar si el servidor acepta la orden.
 * La string recibida no debe ser manipulada por el programador, sólo leída.
 *
 * @param[in] nick nick del usuario con el que se quiere parar el chat de voz.
 *
 * @retval TRUE si se ha parado la comunicación (debe devolverlo).
 * @retval FALSE en caso contrario (debe devolverlo).
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
boolean IRCInterface_StopAudioChat(char *nick)
{
	if(nick == NULL)
		return FALSE;


	return TRUE;
}


/**
 *
 * @page IRCInterface_TakeOp IRCInterface_TakeOp
 *
 * @brief Llamada por el botón "Quitar Op".
 *
 * <h2>Synopsis</h2>
 * @code
 *	#include <redes2/ircxchat.h>
 *
 * 	void IRCInterface_TakeOp (char *channel, char *nick)
 * @endcode
 *
 * <h2>Descripción</h2>
 * Llamada por el botón "Quitar Op". Previamente debe seleccionarse un nick del
 * canal para quitarle "op" a dicho usuario.
 *
 * En cualquier caso sólo se puede realizar si el servidor acepta la orden.
 * Las strings recibidas no deben ser manipuladas por el programador, sólo leídas.
 *
 * @param[in] channel canal sobre el que se va a quitar op al usuario.
 * @param[in] nick nick del usuario al que se le va a quitar op.
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
void IRCInterface_TakeOp(char *channel, char *nick)
{
	char *msg = NULL;

	if(channel == NULL || nick == NULL)
		return;

	/*Generamos el mensaje para enviar al servidor*/
	IRCMsg_Mode(&msg, NULL, channel, "-o", nick);

	send(desc, msg, strlen(msg), 0);

	/*Enviamos el mensaje al registro plano*/
	IRCInterface_PlaneRegisterOutMessage(msg);

	free(msg);
}


/**
 *
 * @page IRCInterface_TakeVoice IRCInterface_TakeVoice
 *
 * @brief Llamada por el botón "Quitar voz".
 *
 * <h2>Synopsis</h2>
 * @code
 *	#include <redes2/ircxchat.h>
 *
 * 	void IRCInterface_TakeVoice (char *channel, char *nick)
 * @endcode
 *
 * <h2>Descripción</h2>
 * Llamada por el botón "Quitar voz". Previamente debe seleccionarse un nick del
 * canal para darle voz a dicho usuario.
 *
 * En cualquier caso sólo se puede realizar si el servidor acepta la orden.
 * Las strings recibidas no deben ser manipuladas por el programador, sólo leídas.
 *
 * @param[in] channel canal sobre el que se le va a quitar voz al usuario.
 * @param[in] nick nick del usuario al que se va a quitar la voz.
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
void IRCInterface_TakeVoice(char *channel, char *nick)
{
	char *msg = NULL;

	if(channel == NULL || nick == NULL)
		return;

	IRCMsg_Mode(&msg, NULL, channel, "-v", nick);

	/*Enviamos el mensaje al registro plano*/
	IRCInterface_PlaneRegisterOutMessage(msg);

	send(desc, msg, strlen(msg), 0);

	free(msg);
}

/*
Main
*/


int main (int argc, char *argv[])
{
	/* La función IRCInterface_Run debe ser llamada al final      */
	/* del main y es la que activa el interfaz gráfico quedándose */
	/* en esta función hasta que se pulsa alguna salida del       */
	/* interfaz gráfico.                                          */
	IRCInterface_Run(argc, argv);

	return 0;
}
