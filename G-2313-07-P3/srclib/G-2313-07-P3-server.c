/**
* @brief Funciones necesarias para el inicio y funcionamiento del server
* @file G-2313-07-P3-server.c
*
* @authors Alfonso Bonilla (alfonso.bonilla@estudiante.uam.es)
* @authors Monica de la Iglesia (monica.delaiglesia@estudiante.uam.es)
* @version 2.0
* @date 10-04-2017
*/

 /*! @page irc_server Servidor IRC
 *
 * <p>Esta sección incluye las funciones de conexión y de utilización del Servidor IRC.<br>
 * Este módulo hace uso de las funciones contenidas en la librería <b>G-2313-07-P1-utilities.h</b></p>
 *
 * <h2>Cabeceras</h2>
 * <code>
 * \b #include \b <G-2313-07-P3-server.h>
 * </code>
 *
 * <hr>
 * <hr>
 *
 * <h2>Funciones implementadas</h2>
 * <p>Se incluyen las siguientes funciones de conexión y uso del servidor IRC:
 * <ul>
 * <li>@subpage IRC_Initiate_Server</li>
 * <li>@subpage IRC_Accept_Connection</li>
 * <li>@subpage IRC_New_Client</li>
 * <li>@subpage IRC_Server_Parser</li>
 * <li>@subpage IRC_Ping_Pong</li>
 * <li>@subpage IRC_End_Server</li>
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

#include "../includes/G-2313-07-P3-server.h"

int sockval = 0; /**< @brief Valor del descriptor del socket del Servidor */
int in_register = 0;
long ssl_flag = FALSE;
SSL* ssl = NULL;

/**
 * @page IRC_Initiate_Server IRC_Initiate_Server
 * @brief Inicializa el servidor abriendo el socket
 * <h2>Synopsis</h2>
 *
 * @code
 * #include "includes/G-2313-07-P1-server.h"
 *
 * int IRC_Initiate_Server(int port)
 * @endcode
 *
 * <h2>Descripción</h2>
 * Abre el socket y comprobando que no haya error, a continuacion hace bind y lo prepara para escuchar.
 * Escucha un máximo de conexiones. En caso de que alguno de estos pasos falle se reflejará en el log del
 * sistema.
 *
 * Además establece el manejador para la señal SIGINT
 *
 * @param[in] port Numero del puerto en el que se va a establecer el Servidor IRC.
 *
 * @retval int El descriptor del socket que acaba de abrir y preparar para escuchar concexiones.
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
int IRC_Initiate_Server(int port)
{
	struct sockaddr_in Direccion;

	syslog(LOG_INFO, "Creating socket");
	if ( (sockval = socket(AF_INET, SOCK_STREAM, 0)) < 0 ){
		syslog(LOG_ERR, "Error creating socket");
		exit(EXIT_FAILURE);
	}

	Direccion.sin_family = AF_INET;              /* TCP/IP family */
	Direccion.sin_port = htons(port);            /* Asigning port */
	Direccion.sin_addr.s_addr=htonl(INADDR_ANY); /* Accept all adresses */
	bzero((void *)&(Direccion.sin_zero), 8);

	syslog (LOG_INFO, "Binding socket");
	if (bind (sockval, (struct sockaddr *)&Direccion, sizeof(Direccion))<0){
		syslog(LOG_ERR, "Error binding socket");
		exit(EXIT_FAILURE);
	}

	syslog (LOG_INFO, "Listening connections");
	if (listen (sockval, MAX_CONNECTIONS)<0){
		syslog(LOG_ERR, "Error listenining");
		exit(EXIT_FAILURE);
	}

	/*Manejadores de señales*/
	signal(SIGINT, IRC_End_Server);
	signal(SIGALRM, IRC_Ping_Pong);

	return sockval;
}

/**
 * @page IRC_End_Server IRC_End_Server
 * @brief Finaliza y libera todos los recuersos del servidor.
 * <h2>Synopsis</h2>
 *
 * @code
 * #include "includes/G-2313-07-P1-server.h"
 *
 * void IRC_End_Server(int sig)
 * @endcode
 *
 * <h2>Descripción</h2>
 *
 * Esta función se encarga de finalizar el servidor, por lo tanto se ocupa de liberar todos los recursos
 * que este usando el servidor en el momento de ser la lanzada la señal sig.
 * Ademas de hacer estas liberaciones cierra el socket en el que estaba abierto el servidor para dejarlo
 * libre para futuros usos.
 *
 * @param[in] sig Valor de la señal a la que va a estar asociada esta funcion.
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
void IRC_End_Server(int sig)
{
	char ** users = NULL, **nicks = NULL, **realnames = NULL, **passwords = NULL, **hosts = NULL, **IPs = NULL;
	long *creationTSs = NULL, *actionTSs = NULL, *ids = NULL, *modes = NULL;
	int *sockets = NULL;
	long nelements;

	IRCTADUser_GetAllLists(&nelements,&ids, &users, &nicks, &realnames, &passwords, &hosts, &IPs, &sockets, &modes, &creationTSs, &actionTSs);
	IRCTADUser_FreeAllLists(nelements,ids,users, nicks, realnames, passwords, hosts, IPs, sockets, modes, creationTSs, actionTSs);

	close(sockval);
	syslog (LOG_INFO, "Exiting service");
}

/**
 * @page IRC_Accept_Connection IRC_Accept_Connection
 * @brief Acepta conexiones y crea un hilo cuando llega un cliente nuevo.
 * <h2>Synopsis</h2>
 *
 * @code
 * #include "includes/G-2313-07-P1-server.h"
 *
 * void IRC_Accept_Connection(int sockval)
 * @endcode
 *
 * <h2>Descripción</h2>
 *
 * Esta función esta permanentemente a la espera de aceptar conexiones, cunado recibe una
 * nueva conexion crea un hilo para el cliente que acaba de llegar y manda a este hilo a la funcion
 * launch_service.
 *
 * @param[in] sockval Valor del descriptor del servidor obtenido al crear el socket.
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
void IRC_Accept_Connection(int sockval)
{
	int desc;
	struct sockaddr Conexion;
	pthread_t hilo;
	socklen_t len;
	len = sizeof(Conexion);

	while(1){
		if ((desc = accept(sockval, &Conexion, &len))<0){
			syslog(LOG_ERR, "Error accepting connection");
			exit(EXIT_FAILURE);
		}
		syslog(LOG_INFO, "%d descriptor de usuario\n", desc);

		/*Creamos el hilo para atender al cliente*/
		pthread_create(&hilo, NULL, IRC_New_Client, (void*) &desc);
	}
	return;
}


/**
 * @page IRC_New_Client IRC_New_Client
 * @brief Se encarga de esperar mensajes del cliente
 * <h2>Synopsis</h2>
 *
 * @code
 * #include "includes/G-2313-07-P1-server.h"
 *
 * void* IRC_New_Client(void* valor)
 * @endcode
 *
 * <h2>Descripción</h2>
 *
 * Esta función esta siempre esperando mensajes del cliente, los cuales dividira por si ha recibido
 * mas de un comando en el mismo mensaje, puesto que el maximo de IRC es 512 por cada mensaje.
 * A continuacion enviara este mensaje a parsear y ejecutar.
 * Adicionalmente esta función hace uso de otras para comprobar si el usuario ha cerrado conexión sin hacer QUIT
 *
 * @param[in] valor puntero void al descriptor del usuario
 *
 * @note Esta función se encarga de liberar la memoria reservada por el cliente.
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
void *IRC_New_Client(void* valor)
{
	int connval = *((int *) valor);
	char *str;
	char mensaje[MAX_BUFFER];
	char *command;
	char *nick = NULL;
	char *prefix_user = NULL;

	while(1){
		bzero(mensaje, MAX_BUFFER);
		syslog (LOG_INFO, "Newping_pong access");

		if(isClosed(connval) == TRUE){
			IRCTAD_Quit (nick);
			close(connval);
			free(nick);
			free(prefix_user);
			pthread_exit(NULL);
		}

		recv(connval, (void*) mensaje, MAX_BUFFER, 0);

		if(mensaje[0] == '\0'){
			IRCTAD_Quit (nick);
			close(connval);
			free(nick);
			free(prefix_user);
			pthread_exit(NULL);
		}

		str = IRC_UnPipelineCommands(mensaje, &command);
		IRC_Server_Parser(command, connval, &nick, &prefix_user);
		free(command);
		while (str != NULL){
			str = IRC_UnPipelineCommands(str, &command);
			IRC_Server_Parser(command, connval, &nick, &prefix_user);
			free(command);
		}
	}
}

/**
 * @page IRC_Ping_Pong IRC_Ping_Pong
 * @brief Se encarga de realizar el protocolo PING PONG
 * <h2>Synopsis</h2>
 * @code
 * #include "includes/G-2313-07-P1-server.h"
 *
 * void IRC_Ping_Pong(int sig)
 * @endcode
 *
 * <h2>Descripción</h2>
 *
 * Esta funcion realiza el protocolo PING PONG, es decir, envía a cada usuario del servidor
 * un PING cada 30 segundos, y espera recibir de este un PONG, si no lo recibe es que el usuario ya no
 * esta conectado y por tanto lo elimina del servidor. De esta manera se optimiza el uso del servidor y de nicks
 * dejando solo conectados a aquellos que estan interactuando en el servidor.
 *
 * @param[in] sig Valor de la señal a la que va a estar asociada esta función.
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
void IRC_Ping_Pong (int sig)
{
	/**/
}

/**
 * @page IRC_Server_Parser IRC_Server_Parser
 * @brief Parsea y ejecuta los comandos IRC recibidos
 * <h2>Synopsis</h2>
 *
 * @code
 * #include "includes/G-2313-07-P1-server.h"
 *
 * void IRC_Server_Parser(char* command, int desc, char** nick, char** prefix_user)
 * @endcode
 *
 * <h2>Descripción</h2>
 *
 * Recibe una cadena con el comando que se quiere ejecutar, esta función se encarga de parsear este
 * comando y comprobar que esta correctamente formado. Si el comando pertenece a una caracteristica
 * no implementada en nuestro servidor se enviará un mensaje al descriptor de usuario recibido por parametro
 * indicando que dicho comando no esta implementado y es desconocido.
 *
 * En caso de ser un comando conocido este se parseara, y según la información de este comando y el estado
 * del usario se procedera a ejecutar el comando y enviar al usuario un mensaje, indicando que se ha realizado
 * correctamente o incorrectamente.
 *
 * En algún caso el mensaje sera enviado a todos los usuarios del canal, como puede ser el caso de JOIN KICK PART
 * para informar de lo ocurrido a todos.
 *
 *
 * @param[in] command puntero a char con el comando recibido y que se va a parsear y ejecutar
 * @param[in] desc entero descriptor del usuario
 * @param[in,out] nick doble puntero char al nick del ususario
 * @param[in,out] prefix_user doble puntero char al prefix del usuario
 *
 *
 * @warning Esta función realiza reservas en nick y prefix_user. Libera la memoria solo si se hace QUIT, si el cliente
 * se desconecta es el usuario quien tiene que liberar la memoria.
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
void IRC_Server_Parser(char* command, int desc, char** nick, char** prefix_user)
{
	char *prefix = NULL, *realname = NULL, *server = NULL, *modehost = NULL, *user = NULL, *target = NULL, *maskarray = NULL, *channel = NULL, *key = NULL;
	char *msg = NULL, *password = NULL, *serverPing = NULL, *serverPong = NULL, *topic = NULL, *comment = NULL, *nick_pars = NULL, *topic_actual = NULL;
	char **list = NULL;
	char aux[MAX_BUFFER], mode[2], whoname[MAX_NICKNAME+2];
	long nelements, creationTS, actionTS;
	char *unknown_real = NULL, *unknown_nick = NULL, *unknown_user = NULL, *modo = NULL;
	long unknown_id = 0;
	char *host = NULL, *IP = NULL, *away = NULL, *names = NULL, *setpass = NULL, *mask = NULL, *oppar = NULL;
	int i;
	int sock = 0;

	/*Indexamos con el tipo de comando*/
	switch(IRC_CommandQuery(command)){

/************************************ PASS ****************************************************/
		case PASS:
			syslog(LOG_INFO, "CASE PASSS\n");
			if(IRCParse_Pass (command, &prefix, &password) == IRC_OK){
				syslog(LOG_INFO, "PARSEO PASS CORRECTO\n");
			}
			free(prefix);
			free(password);
			break;

/************************************ NICK ****************************************************/
		case NICK:
			if(IRCParse_Nick(command, &prefix, &nick_pars, &msg) != IRC_OK){
				syslog(LOG_INFO, "NICK => ERROR %s", nick_pars);
				if(IRCMsg_ErrWasNoSuchNick (&msg, prefix, nick_pars, nick_pars) == IRC_OK){
					if(ssl_flag == FALSE){
					  send(desc, msg, strlen(msg), 0);
					}else{
					  enviar_datos_SSL(ssl, msg);
					}
					free(msg);
				}
			}else if(strlen(nick_pars) > MAX_NICKNAME+1){
				syslog(LOG_INFO, "Longitud maxima superada\n");
				if(IRCMsg_ErrErroneusNickName(&msg, prefix, nick_pars, nick_pars) == IRC_OK){
					if(ssl_flag == FALSE){
					  send(desc, msg, strlen(msg), 0);
					}else{
					  enviar_datos_SSL(ssl, msg);
					}
					free(msg);
				}
			}else if(exist_User(nick_pars) == TRUE){
				syslog(LOG_INFO, "NICK %s => EN USO\n", *nick);
				if(IRCMsg_ErrNickNameInUse(&msg, prefix , nick_pars, nick_pars) == IRC_OK){
					send(desc,msg, strlen(msg), 0);
					free(msg);
				}

			}else{
				syslog(LOG_INFO, "CASE NICK => CORRECTO\n");
				in_register = 1;

				/*Regeneramos el prefix*/
				if(*nick != NULL){

					/* Obtiene todos los parámetros a partir del nick*/
					switch (IRCTADUser_Set(unknown_id, unknown_user, *nick, unknown_real, unknown_user, nick_pars, unknown_real)) {

						case IRCERR_NOENOUGHMEMORY:
							syslog(LOG_INFO, "CASE NICK => IRCERR_NOENOUGHMEMORY\n");
							break;

						case IRCERR_NICKUSED:
							syslog(LOG_INFO, "CASE NICK => IRCERR_NICKUSED\n");
							break;

						case IRCERR_INVALIDNICK:
							syslog(LOG_INFO, "CASE NICK => IRCERR_INVALIDNICK\n");
							break;

						case IRCERR_INVALIDREALNAME:
							syslog(LOG_INFO, "CASE NICK => IRCERR_INVALIDREALNAME\n");
							break;

						case IRCERR_INVALIDUSER:
							syslog(LOG_INFO, "CASE NICK => IRCERR_INVALIDUSER\n");
							break;

						case IRC_OK:
							user = NULL;

							IRCTADUser_GetData (&unknown_id, &user, &unknown_nick, &unknown_real, &host, &IP, &desc, &creationTS, &actionTS, &away);

							/*if(IRCMsg_Nick (&msg, *prefix_user+1, NULL, nick_pars) == IRC_OK){
								send(desc,msg, strlen(msg), 0);
								free(msg);
							}*/

							free(*prefix_user);
							IRC_Prefix (&(*prefix_user), nick_pars, user, NULL, "LOCALHOST");
							syslog(LOG_INFO, "PREFIX NUEVO %s", *prefix_user);

							/*Avisar en todos los canales*/

							free(*nick);
							free(user);
							free(unknown_nick);
							free(unknown_real);
							free(host);
							free(IP);
							free(away);
							break;
					}
				}
				*nick = (char *) malloc(strlen(nick_pars)+1); /*MIrando lo de eloy en metis*/
				strcpy(*nick, nick_pars);

			}
			free(prefix);
			free(nick_pars);

			break;

/************************************ USER ****************************************************/
		case USER:
			syslog(LOG_INFO, "CASE USER\n");
			if (in_register == 1){
				if(IRCParse_User (command, &prefix, &user, &modehost, &server, &realname) == IRC_OK){
					syslog(LOG_INFO, "BIEN PARSEADO USER\n");

					syslog(LOG_INFO, "sacados datos de conexcion\n");

					if(IRCTADUser_New(user, *nick, realname, NULL, "alfon", "127.0.0.1", desc) == IRC_OK){

						syslog(LOG_INFO, "ANADIDO AL TAD\n");
						free(prefix);
						IP = NULL;

						IRCTADUser_GetData (&unknown_id, &user, &unknown_nick, &unknown_real, &host, &IP, &desc, &creationTS, &actionTS, &away);

						IRC_Prefix (&(*prefix_user), *nick, user, NULL, "LOCALHOST");
						syslog(LOG_INFO, "PREFIX NUEVO EN USER %s", *prefix_user);

						free(unknown_nick);
						free(unknown_real);
						free(host);
						free(IP);
						free(away);
					}

					/* Mensaje de Bienvenida*/
					if(IRCMsg_RplWelcome(&msg, *prefix_user+1, *nick, *nick, user, server) == IRC_OK){
						if(ssl_flag == FALSE){
							if(ssl_flag == FALSE){
							  send(desc, msg, strlen(msg), 0);
							}else{
							  enviar_datos_SSL(ssl, msg);
							}
						}else{
							enviar_datos_SSL(ssl, msg);
						}
						free(msg);
					}
					in_register = 0;

					free(prefix);
					free(user);
					free(modehost);
					free(server);
					free(realname);
				}
			}
			break;

/************************************ JOIN ****************************************************/
		case JOIN:
			syslog(LOG_INFO, "CASE JOIN\n");
			if(IRCParse_Join(command, &prefix, &channel, &key, &msg) == IRC_OK){
				/*Comprobamos que empiece por #*/
				if(channel[0] != '#') {
          syslog(LOG_INFO, "CANAL INCORRECTO: %s", channel);
					free(msg);
					if(IRCMsg_ErrNoSuchChannel (&msg, *prefix_user+1, *nick, channel) == IRC_OK){
						if(ssl_flag == FALSE){
  send(desc, msg, strlen(msg), 0);
}else{
  enviar_datos_SSL(ssl, msg);
}
						free(msg);
					}
					free(prefix);
					free(channel);
					free(key);
					return;
       	}

				/*Si el canal no existe establecemos al usuario como operador*/
				if(IRCTAD_TestUserOnChannel (channel, *nick) == IRCERR_NOVALIDCHANNEL){
					strcpy(mode, "o");
				}else{
					strcpy(mode, "");
				}

				if((IRCTADChan_GetModeInt (channel) & IRCMODE_CHANNELPASSWORD ) == IRCMODE_CHANNELPASSWORD){
					if(IRCTADChan_TestPassword (channel, key) != IRC_OK || key == NULL){
						syslog(LOG_INFO, "PASS dont match\n");
						free(msg);
						if(IRCMsg_ErrBadChannelKey(&msg, *prefix_user+1, *nick, channel) == IRC_OK){
							if(ssl_flag == FALSE){
							  send(desc, msg, strlen(msg), 0);
							}else{
							  enviar_datos_SSL(ssl, msg);
							}
							free(msg);
						}

						free(unknown_real);
						free(host);
						free(IP);
						free(away);
						free(user);
						free(prefix);
						free(channel);
						free(key);
						return;
					}
				}

				free(prefix);

				switch (IRCTAD_Join (channel, *nick, mode, key)) {

					case IRCERR_NOVALIDUSER: /*no existe el usuario indicado*/
						if(IRCMsg_ErrNoLogin(&msg, *prefix_user+1, *nick, user) == IRC_OK){
							if(ssl_flag == FALSE){
							  send(desc, msg, strlen(msg), 0);
							}else{
							  enviar_datos_SSL(ssl, msg);
							}
							free(msg);
						}
						break;

					case IRCERR_NOVALIDCHANNEL: /*el canal indicado no es valido*/
						if(IRCMsg_ErrNoSuchChannel(&msg, *prefix_user+1, *nick, channel) ==  IRC_OK){
							if(ssl_flag == FALSE){
							  send(desc, msg, strlen(msg), 0);
							}else{
							  enviar_datos_SSL(ssl, msg);
							}
							free(msg);
						}
						break;

					case IRCERR_USERSLIMITEXCEEDED: /*no se admiten mas usuarios en el canal*/
						if(IRCMsg_ErrChannelIsFull(&msg, *prefix_user+1, *nick, channel) == IRC_OK){
							if(ssl_flag == FALSE){
							  send(desc, msg, strlen(msg), 0);
							}else{
							  enviar_datos_SSL(ssl, msg);
							}
							free(msg);
						}
						break;

					case IRCERR_NOENOUGHMEMORY: /*no hay suficiente memoria para crear el canal*/
						break;

					case IRCERR_BANEDUSERONCHANNEL: /*no puede unirse por estar baneado*/
						if(IRCMsg_ErrBannedFromChan(&msg, *prefix_user+1, *nick, channel) == IRC_OK){
							if(ssl_flag == FALSE){
							  send(desc, msg, strlen(msg), 0);
							}else{
							  enviar_datos_SSL(ssl, msg);
							}
							free(msg);
						}
						break;

					case IRCERR_NOINVITEDUSER: /*canal con invitacion y no ha sido invitado*/
						if(IRCMsg_ErrInviteOnlyChan(&msg, *prefix_user+1, *nick, channel) == IRC_OK){
							if(ssl_flag == FALSE){
							  send(desc, msg, strlen(msg), 0);
							}else{
							  enviar_datos_SSL(ssl, msg);
							}
							free(msg);
						}
						break;

					case IRCERR_YETINCHANNEL: /*user distinto, mismo nick*/
						break;

					case IRC_OK: /*se ha anadido el usuario al canal*/
						syslog(LOG_INFO, "JOIN CORRECTO");

						free(prefix);

						IRCTAD_ListNicksOnChannelArray(channel, &list, &nelements);
						for(i=0; i<nelements; i++){
							if(IRCTADUser_GetData (&unknown_id, &user, &list[i], &unknown_real, &host, &IP, &sock, &creationTS, &actionTS, &away) == IRC_OK){
								if(IRCMsg_Join (&msg, *prefix_user+1, NULL, NULL, channel) == IRC_OK){
									send(sock, msg, strlen(msg), 0);
									free(msg);
								}
								free(unknown_real);
								free(host);
								free(IP);
								free(away);
								free(user);
								unknown_real = host = IP = away = user = NULL;
								unknown_id=0;
								sock=0;
							}
						}
						IRCTADUser_FreeList (list, nelements);

						break;
				}
			}else{ /*Parseo no fue IRC_OK*/
				free(msg);
				if(IRCMsg_ErrNeedMoreParams(&msg, *prefix_user+1 , *nick, command) == IRC_OK){
					if(ssl_flag == FALSE){
					  send(desc, msg, strlen(msg), 0);
					}else{
					  enviar_datos_SSL(ssl, msg);
					}
					free(msg);
				}
			}
			free(prefix);
			free(channel);
			free(key);
			break;

/************************************ LIST ****************************************************/
		case LIST:
			syslog(LOG_INFO, "CASE LIST\n");
			if(IRCParse_List (command, &prefix, &channel, &target) == IRC_OK){

				if(IRCMsg_RplListStart(&msg, *prefix_user+1, *nick) == IRC_OK){
					if(ssl_flag == FALSE){
					  send(desc, msg, strlen(msg), 0);
					}else{
					  enviar_datos_SSL(ssl, msg);
					}
					free(msg);
				}

				if(IRCTADChan_GetList(&list, &nelements, NULL) == IRC_OK){
					for(i=0; i < nelements; i++){
						if(IRCTADChan_GetModeInt(list[i]) != IRCMODE_SECRET){
							 if(IRCTAD_GetTopic(list[i], &topic) == IRC_OK){
								 sprintf(aux, "%ld", IRCTADChan_GetNumberOfUsers(list[i]));
								 syslog(LOG_INFO, "%s aux", aux);
								 if(IRCMsg_RplList(&msg, *prefix_user+1, *nick, list[i], aux, topic) == IRC_OK){
									 if(ssl_flag == FALSE){
										  send(desc, msg, strlen(msg), 0);
										}else{
										  enviar_datos_SSL(ssl, msg);
										}
									 free(msg);
								 }
							 }
						 }
					 }
				 }

				if(IRCMsg_RplListEnd(&msg, *prefix_user+1, *nick) == IRC_OK){
					if(ssl_flag == FALSE){
					  send(desc, msg, strlen(msg), 0);
					}else{
					  enviar_datos_SSL(ssl, msg);
					}
					free(msg);
				}

				IRCTADChan_FreeList (list, nelements);
				free(channel);
				free(prefix);
				free(target);
			}
			break;

/************************************ NAMES ***************************************************/
		case NAMES:
			syslog(LOG_INFO, "CASE NAMES\n");
			if(IRCParse_Names (command, &prefix, &channel, &target) == IRC_OK){

				free(target);
				/*Names del canal indicado*/
				if(channel != NULL){

					if(IRCTAD_ListNicksOnChannelArray(channel, &list, &nelements) == IRC_OK){
						names = (char *) malloc(nelements*(MAX_NICKNAME+1) * sizeof(char));
						strcpy(names, "");

						for(i = 0; i<nelements; i++){
							if(i > 0){
								strcat(names, " ");
							}
							if((IRCTAD_GetUserModeOnChannel (channel, list[i]) & IRCUMODE_OPERATOR) == IRCUMODE_OPERATOR){
								strcat(names, "@");
							}
							strcat(names, list[i]);
						}

						IRCTADChan_FreeList (list, nelements);

						if(IRCMsg_RplNamReply (&msg, *prefix_user+1, *nick, "=", channel, names) == IRC_OK){
							if(ssl_flag == FALSE){
							  send(desc, msg, strlen(msg), 0);
							}else{
							  enviar_datos_SSL(ssl, msg);
							}
							free(msg);
						}

						free(names);
					}


				}else{/*Names de todos los canles del servidor*/

				}

				if(IRCMsg_RplEndOfNames (&msg, *prefix_user+1, *nick, channel) == IRC_OK){
					if(ssl_flag == FALSE){
					  send(desc, msg, strlen(msg), 0);
					}else{
					  enviar_datos_SSL(ssl, msg);
					}
					free(msg);
				}

				free(prefix);
				free(channel);
				free(target);
			}

			break;

/************************************* WHO ****************************************************/
		case WHO:
			syslog(LOG_INFO, "CASE WHO\n");
			if(IRCParse_Who (command, &prefix, &mask, &oppar) == IRC_OK){

				if(mask == NULL){
					free(prefix);
					free(oppar);
				}else{
					if(IRCTAD_ListNicksOnChannelArray(mask, &list, &nelements) == IRC_OK){
						for(i=0; i<nelements; i++){
							if(IRCTADUser_GetData (&unknown_id, &user, &list[i], &unknown_real, &host, &IP, &sock, &creationTS, &actionTS, &away) == IRC_OK){
								sprintf(whoname, "~%s", user);
								if(IRCMsg_RplWhoReply (&msg, SERVER, *nick, mask, whoname, IP, SERVER, list[i] , "H", 0, unknown_real) == IRC_OK){
										if(ssl_flag == FALSE){
										  send(desc, msg, strlen(msg), 0);
										}else{
										  enviar_datos_SSL(ssl, msg);
										}
										free(msg);
								}
								free(unknown_real);
								free(host);
								free(IP);
								free(away);
								free(user);
								unknown_real = host = IP = away = user = NULL;
								unknown_id=0;
								sock=0;
							}
						}
						IRCTADUser_FreeList (list, nelements);
					}


					if(IRCMsg_RplEndOfWho (&msg, SERVER, *nick, mask) == IRC_OK){
						if(ssl_flag == FALSE){
						  send(desc, msg, strlen(msg), 0);
						}else{
						  enviar_datos_SSL(ssl, msg);
						}
						free(msg);
					}

					free(prefix);
					free(mask);
					free(oppar);
				}
			}

			break;

/************************************ WHOIS ***************************************************/
		case WHOIS:
			syslog(LOG_INFO, "CASE WHOIS\n");
			if(IRCParse_Whois(command, &prefix, &target, &maskarray) == IRC_OK){

				if(IRCTADUser_GetData (&unknown_id, &user, nick, &unknown_real, &host, &IP, &sock, &creationTS, &actionTS, &away) == IRC_OK){

					if(away != NULL){
						if(IRCMsg_RplAway (&msg, *prefix_user+1, *nick, *nick, away) == IRC_OK){
							if(ssl_flag == FALSE){
							  send(desc, msg, strlen(msg), 0);
							}else{
							  enviar_datos_SSL(ssl, msg);
							}
							free(msg);
						}
					}else	if(IRCTAD_ListChannelsOfUserArray (user, *nick, &list, &nelements) == IRC_OK){
						names = (char*)malloc(nelements * MAX_CHANNELNAME * sizeof(char));
						strcpy(names, "");

						for(i=0; i<nelements; i++){
							if(i > 0){
								strcat(names, " ");
							}
							if((IRCTAD_GetUserModeOnChannel (list[i], *nick) & IRCUMODE_OPERATOR) == IRCUMODE_OPERATOR){
								strcat(names, "@");
							}
							strcat(names, list[i]);
						}

						if(IRCMsg_RplWhoIsChannels (&msg, *prefix_user+1, *nick, *nick, names) == IRC_OK){
							if(ssl_flag == FALSE){
							  send(desc, msg, strlen(msg), 0);
							}else{
							  enviar_datos_SSL(ssl, msg);
							}
							free(msg);
						}

						if(IRCMsg_RplEndOfWhoIs (&msg, *prefix_user+1, *nick, *nick) == IRC_OK){
							if(ssl_flag == FALSE){
							  send(desc, msg, strlen(msg), 0);
							}else{
							  enviar_datos_SSL(ssl, msg);
							}
							free(msg);
						}

						free(names);
						IRCTADChan_FreeList (list, nelements);
					}
					free(user);
					free(unknown_real);
					free(host);
					free(IP);
					free(away);
				}

			}else{
				if(IRCMsg_ErrNoNickNameGiven(&msg, *prefix_user+1, *nick) == IRC_OK){
					if(ssl_flag == FALSE){
					  send(desc, msg, strlen(msg), 0);
					}else{
					  enviar_datos_SSL(ssl, msg);
					}
					free(msg);
				}
			}
			free(prefix);
			free(target);
			free(maskarray);

			break;

/************************************ PING ****************************************************/
		case PING:
			syslog(LOG_INFO, "CASE PING\n");
			if(IRCParse_Ping (command, &prefix, &serverPing, &serverPong, &msg) == IRC_OK){
				if(IRCMsg_Pong (&msg, *prefix_user+1, serverPing, serverPong, serverPing) == IRC_OK){
					if(ssl_flag == FALSE){
					  send(desc, msg, strlen(msg), 0);
					}else{
					  enviar_datos_SSL(ssl, msg);
					}
					free(msg);
				}
			}
			free(prefix);
			free(serverPing);
			free(serverPong);
			break;

/************************************ PRIVMSG **************************************************/
		case PRIVMSG: /*Segundo*/
			syslog(LOG_INFO, "CASE PRIVMSG\n");
			if(IRCParse_Privmsg (command, &prefix, &target, &msg) == IRC_OK){

				/*CASO MENSAJE EN CANAL*/
				if(target[0] == '#') {
					switch (IRCTAD_ListNicksOnChannelArray(target, &list, &nelements)) {

						case IRCERR_NOENOUGHMEMORY:
							break;

						case IRCERR_NOVALIDCHANNEL:
							free(msg);
							if(IRCMsg_ErrNoSuchChannel(&msg, *prefix_user+1, *nick, channel) ==  IRC_OK){
								if(ssl_flag == FALSE){
								  send(desc, msg, strlen(msg), 0);
								}else{
								  enviar_datos_SSL(ssl, msg);
								}
								free(msg);
							}
							break;

						case IRC_OK:
							for(i=0; i<nelements; i++){
								if(strcmp((*nick), list[i]) != 0){
									if(IRCTADUser_GetData (&unknown_id, &unknown_user, &list[i], &unknown_real, &host, &IP, &sock, &creationTS, &actionTS, &away) == IRC_OK){
										if(away == NULL){
											if(IRCMsg_Privmsg (&comment, *prefix_user+1, target, msg) ==  IRC_OK){
												send(sock, comment, strlen(comment), 0);
												free(comment);
											}
										}
										free(unknown_user);
										free(unknown_real);
										free(host);
										free(IP);
										free(away);
										unknown_id = 0;
										sock = 0;
										unknown_user = unknown_real = host = IP = away = NULL;
									}
								}
							}
							free(msg);
							break;

					}/*Fin del siwtch listnicksonchanel array*/
					IRCTADUser_FreeList (list, nelements);

        }else{
					/*CASO MENSAJE PRIVADO A USUARIO*/
					if(exist_User(target) == FALSE){
						free(msg);
						if(IRCMsg_ErrNoSuchNick(&msg, *prefix_user+1, *nick, target) == IRC_OK){
							if(ssl_flag == FALSE){
							  send(desc, msg, strlen(msg), 0);
							}else{
							  enviar_datos_SSL(ssl, msg);
							}
							free(msg);
						}
					}else{ /*caso el usuario existe*/
						if(IRCTADUser_GetData (&unknown_id, &unknown_user, &target, &unknown_real, &host, &IP, &sock, &creationTS, &actionTS, &away) == IRC_OK){
							if(away != NULL){
								free(msg);
								if(IRCMsg_RplAway (&msg, *prefix_user+1, *nick, *nick, away) == IRC_OK){
									if(ssl_flag == FALSE){
									  send(desc, msg, strlen(msg), 0);
									}else{
									  enviar_datos_SSL(ssl, msg);
									}
									free(msg);
								}
							}else if(IRCMsg_Privmsg (&comment, *prefix_user+1, target, msg) ==  IRC_OK){
								send(sock, comment, strlen(comment), 0);
								free(comment);
								free(msg);
							}
							free(unknown_user);
							free(unknown_real);
							free(host);
							free(IP);
							free(away);
						}
					}
				}
			}

			free(target);
			free(prefix);


			break;

/************************************ PART ****************************************************/
		case PART: /*Abandonar canal*/
			syslog(LOG_INFO, "CASE PART\n");
			if(IRCParse_Part (command, &prefix, &channel, &msg) == IRC_OK){
				free(msg);
				switch (IRCTAD_Part (channel, *nick)) {

					case IRCERR_NOVALIDUSER: /*No existe el usuario en el canal*/
						if(IRCMsg_ErrNoLogin(&msg, *prefix_user+1, *nick, user) == IRC_OK){
							if(ssl_flag == FALSE){
							  send(desc, msg, strlen(msg), 0);
							}else{
							  enviar_datos_SSL(ssl, msg);
							}
							free(msg);
						}
						break;

					case IRCERR_NOVALIDCHANNEL: /*No existe el canal indicado*/
						if(IRCMsg_ErrNoSuchChannel(&msg, *prefix_user+1, *nick, channel) ==  IRC_OK){
							if(ssl_flag == FALSE){
							  send(desc, msg, strlen(msg), 0);
							}else{
							  enviar_datos_SSL(ssl, msg);
							}
							free(msg);
						}
						break;

					case IRCERR_UNDELETABLECHANNEL: /*No se puede eliminar el canal porque es permanente*/
						break;

					case IRC_OK:
						if(IRCTAD_ListNicksOnChannelArray(channel, &list, &nelements) == IRC_OK){
							for(i=0; i<nelements; i++){
								if(IRCTADUser_GetData (&unknown_id, &user, &list[i], &unknown_real, &host, &IP, &sock, &creationTS, &actionTS, &away) == IRC_OK){
									if(IRCMsg_Part(&msg, *prefix_user+1, channel, "Hasta Nunki") ==  IRC_OK){
										send(sock, msg, strlen(msg), 0);
										free(msg);
									}
									free(unknown_real);
									free(host);
									free(IP);
									free(away);
									free(user);
									unknown_real = host = IP = away = user = NULL;
									unknown_id=0;
									sock=0;
								}
							}
							IRCTADUser_FreeList (list, nelements);
						}

						if(IRCMsg_Part(&msg, *prefix_user+1, channel, "Hasta Nunki") ==  IRC_OK){
							if(ssl_flag == FALSE){
							  send(desc, msg, strlen(msg), 0);
							}else{
							  enviar_datos_SSL(ssl, msg);
							}
							free(msg);
						}

						break;
				}
			}
			free(prefix);
			free(channel);
			break;

/************************************ TOPIC ***************************************************/
		case TOPIC:
			syslog(LOG_INFO, "TOPIC\n");
			if(IRCParse_Topic (command, &prefix, &channel, &topic) == IRC_OK){

				/*Comprobamos si el canal tiene topic*/
				if(IRCTAD_GetTopic (channel, &topic_actual) ==  IRC_OK){

					if(topic == NULL && topic_actual == NULL){
						if(IRCMsg_RplNoTopic(&msg, *prefix_user+1, *nick, channel) == IRC_OK){
							if(ssl_flag == FALSE){
							  send(desc, msg, strlen(msg), 0);
							}else{
							  enviar_datos_SSL(ssl, msg);
							}
							free(msg);
						}
					}else if (topic == NULL && topic_actual != NULL){
						if(IRCMsg_RplTopic(&msg, *prefix_user+1, *nick, channel, topic_actual) == IRC_OK){
							if(ssl_flag == FALSE){
							  send(desc, msg, strlen(msg), 0);
							}else{
							  enviar_datos_SSL(ssl, msg);
							}
							free(msg);
						}
					}else{
						/*Primero comprobamos el modo que tiene el canal*/
						if((IRCTADChan_GetModeInt (channel) & IRCMODE_TOPICOP) == IRCMODE_TOPICOP){
							if(IRCMsg_ErrChanOPrivsNeeded(&msg, *prefix_user+1, *nick, channel) == IRC_OK){
								if(ssl_flag == FALSE){
								  send(desc, msg, strlen(msg), 0);
								}else{
								  enviar_datos_SSL(ssl, msg);
								}
								free(msg);
							}
						}else{
							if(IRCTAD_SetTopic (channel, *nick, topic) == IRC_OK){
								if(IRCMsg_Topic (&msg, *prefix_user+1, channel, topic) == IRC_OK){
									if(ssl_flag == FALSE){
									  send(desc, msg, strlen(msg), 0);
									}else{
									  enviar_datos_SSL(ssl, msg);
									}
									free(msg);
								}
							}
						}
					}
					free(topic_actual);
				}
				free(channel);
				free(topic);
				free(prefix);
			}

			break;

/************************************ MODE ****************************************************/
		case MODE:
			syslog(LOG_INFO, "CASE MODE\n");
			if(IRCParse_Mode (command, &prefix, &channel, &modo, &user) == IRC_OK){

				if((IRCTAD_GetUserModeOnChannel (channel, *nick) & IRCUMODE_OPERATOR) != IRCUMODE_OPERATOR){
					if(IRCMsg_ErrChanOPrivsNeeded(&msg, *prefix_user+1, *nick, channel) == IRC_OK){
						if(ssl_flag == FALSE){
						  send(desc, msg, strlen(msg), 0);
						}else{
						  enviar_datos_SSL(ssl, msg);
						}
						free(msg);
					}
				}else{
					if(modo != NULL){
						if(strcmp(modo, "\\+k") == 0){
							setpass = (char*) malloc(sizeof(user) + 9);
							strcpy(setpass, "");
							strcat(setpass, "+k ");
							strcat(setpass, user);

							if(IRCTAD_Mode (channel, *nick, setpass) == IRC_OK){
								if(IRCMsg_Mode (&msg, *prefix_user+1, channel, setpass, user) == IRC_OK){
									if(ssl_flag == FALSE){
									  send(desc, msg, strlen(msg), 0);
									}else{
									  enviar_datos_SSL(ssl, msg);
									}
									free(msg);
								}
							}
							free(setpass);
						}else{
							if(IRCTAD_Mode (channel, *nick, modo) == IRC_OK){
								if(IRCMsg_Mode (&msg, *prefix_user+1, channel, modo, user) == IRC_OK){
									if(ssl_flag == FALSE){
									  send(desc, msg, strlen(msg), 0);
									}else{
									  enviar_datos_SSL(ssl, msg);
									}
									free(msg);
								}
							}
						}
					}
				}
				free(prefix);
				free(channel);
				free(modo);
				free(user);
			}

			break;

/************************************ KICK ****************************************************/
		case KICK:
			syslog(LOG_INFO, "CASE KICK\n");

			if(IRCParse_Kick (command, &prefix, &channel, &user, &comment) == IRC_OK){

				if((IRCTAD_GetUserModeOnChannel (channel, *nick) & IRCUMODE_OPERATOR) != IRCUMODE_OPERATOR){
					if(IRCMsg_ErrChanOPrivsNeeded(&msg, *prefix_user+1, *nick, channel) == IRC_OK){
						if(ssl_flag == FALSE){
						  send(desc, msg, strlen(msg), 0);
						}else{
						  enviar_datos_SSL(ssl, msg);
						}
						free(msg);
					}
				}else{
					switch (IRCTAD_KickUserFromChannel (channel, user)) {
						case IRCERR_NOVALIDUSER:
							if(IRCMsg_ErrNoLogin(&msg, *prefix_user+1, *nick, user) == IRC_OK){
								if(ssl_flag == FALSE){
								  send(desc, msg, strlen(msg), 0);
								}else{
								  enviar_datos_SSL(ssl, msg);
								}
								free(msg);
							}
							break;

						case IRCERR_NOVALIDCHANNEL:  /*el canal indicado no es valido*/
							if(IRCMsg_ErrNoSuchChannel(&msg, *prefix_user+1, *nick, channel) ==  IRC_OK){
								if(ssl_flag == FALSE){
								  send(desc, msg, strlen(msg), 0);
								}else{
								  enviar_datos_SSL(ssl, msg);
								}
								free(msg);
							}
							break;

						case IRCERR_UNDELETABLECHANNEL:
							break;

						case IRC_OK:
							/*Notificacamos a todos los usuarios del canal de quien fue expulsado*/
							if(IRCTAD_ListNicksOnChannelArray(channel, &list, &nelements) == IRC_OK){
								for(i=0; i<nelements; i++){
									if(IRCTADUser_GetData (&unknown_id, &unknown_user, &list[i], &unknown_real, &host, &IP, &sock, &creationTS, &actionTS, &away) == IRC_OK){
										if(IRCMsg_Kick (&msg, *prefix_user+1, channel, user, comment) == IRC_OK){
											send(sock, msg, strlen(msg), 0);
											free(msg);
										}
										free(unknown_real);
										free(host);
										free(IP);
										free(away);
										free(unknown_user);
										unknown_real = host = IP = away = unknown_user = NULL;
										unknown_id=0;
										sock=0;
									}
								}
								IRCTADUser_FreeList (list, nelements);
							}

							/*Notificacamos al usuario su expulsión*/
							IRCTADUser_GetData (&unknown_id, &unknown_user, &user, &unknown_real, &host, &IP, &sock, &creationTS, &actionTS, &away);
							if(IRCMsg_Kick (&msg, *prefix_user+1, channel, user, comment) == IRC_OK){
								send(sock, msg, strlen(msg), 0);
								free(msg);
							}
							free(unknown_user);
							free(unknown_real);
							free(host);
							free(IP);
							free(away);
							break;
					}
				}
				free(prefix);
				free(channel);
				free(user);
				free(comment);
			}
			break;

/************************************ AWAY ****************************************************/
		case AWAY:
			syslog(LOG_INFO, "CASE AWAY");
			if(IRCParse_Away (command, &prefix, &comment) == IRC_OK){
					if(IRCTADUser_GetData (&unknown_id, &unknown_user, nick, &unknown_real, &host, &IP, &sock, &creationTS, &actionTS, &away) == IRC_OK){
						if(IRCTADUser_SetAway (unknown_id, unknown_user, *nick, unknown_real, comment) == IRC_OK){
							if(comment != NULL){
								if(IRCMsg_RplNowAway (&msg, *prefix_user+1, *nick) == IRC_OK){
									if(ssl_flag == FALSE){
									  send(desc, msg, strlen(msg), 0);
									}else{
									  enviar_datos_SSL(ssl, msg);
									}
									free(msg);
								}
							}else{
								if(IRCMsg_RplUnaway (&msg, *prefix_user+1, *nick) == IRC_OK){
									if(ssl_flag == FALSE){
									  send(desc, msg, strlen(msg), 0);
									}else{
									  enviar_datos_SSL(ssl, msg);
									}
									free(msg);
								}
							}
						}
						free(unknown_user);
						free(unknown_real);
						free(host);
						free(IP);
						free(away);
					}
				free(prefix);
				free(comment);
			}
			break;

/************************************ QUIT ****************************************************/
		case QUIT:
			syslog(LOG_INFO, "CASE QUIT\n");
			if(IRCParse_Quit(command, &prefix, &comment) != IRC_OK){
				syslog(LOG_INFO, "Error en Quit\n");
				free(prefix);
				free(comment);
			}else{

				/*Si sale y no estaba resgistrado*/
				if(exist_descriptor(&desc) == FALSE)
					pthread_exit(NULL);

				IRCTAD_Quit (*nick);

				if(IRCMsg_Quit (&msg, *prefix_user+1, comment) == IRC_OK){
					if(ssl_flag == FALSE){
					  send(desc, msg, strlen(msg), 0);
					}else{
					  enviar_datos_SSL(ssl, msg);
					}
					free(msg);
				}

				close(desc);

				free(prefix);
				free(comment);
				free(*nick);
				free(*prefix_user);
				pthread_exit(NULL);
			}
			break;
/************************************ MOTD ****************************************************/
		case MOTD:
			syslog(LOG_INFO, "MOTD\n");
			if(IRCParse_Motd (command, &prefix, &target) == IRC_OK){

				if(IRCMsg_RplMotdStart(&msg, *prefix_user+1, *nick, SERVER) == IRC_OK){
					if(ssl_flag == FALSE){
					  send(desc, msg, strlen(msg), 0);
					}else{
					  enviar_datos_SSL(ssl, msg);
					}
					free(msg);
				}

				if(IRCMsg_RplMotd(&msg, *prefix_user+1, *nick, SERVER) == IRC_OK){
					if(ssl_flag == FALSE){
					  send(desc, msg, strlen(msg), 0);
					}else{
					  enviar_datos_SSL(ssl, msg);
					}
					free(msg);
				}

				if(IRCMsg_RplEndOfMotd(&msg, *prefix_user+1, *nick) == IRC_OK){
					if(ssl_flag == FALSE){
					  send(desc, msg, strlen(msg), 0);
					}else{
					  enviar_datos_SSL(ssl, msg);
					}
					free(msg);
				}

				free(prefix);
				free(target);
			}

			break;

		default:
			syslog(LOG_INFO, "OPCION NO IMPLEMENTADA %ld\n", IRC_CommandQuery(command));

			if(IRCMsg_ErrUnKnownCommand(&msg, *prefix_user+1, *nick, command) == IRC_OK){
				if(ssl_flag == FALSE){
				  send(desc, msg, strlen(msg), 0);
				}else{
				  enviar_datos_SSL(ssl, msg);
				}
				free(msg);
			}
			break;
	}
}
