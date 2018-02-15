/**
* @brief Funciones de utilidad necesarias para el funcionamiento del servidor
* @file G-2313-07-P1-utilities.c
*
* @authors Alfonso Bonilla (alfonso.bonilla@estudiante.uam.es)
* @authors Monica de la Iglesia (monica.delaiglesia@estudiante.uam.es)
* @version 1.1
* @date 10-04-2017
*/
#include "../includes/G-2313-07-P1-utilities.h"

/*! @page auxiliar_functions Utilidades Servidor
*
* <p>Esta sección incluye las funciones auxiliares utilizadas por el servidor. estas
* potencian su funcionamineto y lo optimizan como las funciones que comprueban si existe un
* usuario o un descriptor.<br>Otras como daemonizar pueden ser usadas por cualquier programa, no es necesario
* que sea usada por el servidor.<br>
* Por esto en este módulo se incluyen funciones de gran utlidad para cualquier programa
* que haga uso de sockets y el TAD de usuarios incluido en la libreria de <b>irc-redes2</b> de la asignatura.</p>
*
* <h2>Cabeceras</h2>
* <code>
* \b #include \b <G-2313-07-P1-utilities.h>
* </code>
*
* <hr>
* <hr>
*
* <h2>Funciones implementadas</h2>
* <p>Se incluyen las siguientes funciones de apoyo al servidor, para hacer este mas eficiente:
* <ul>
* <li>@subpage daemonizar</li>
* <li>@subpage isClosed</li>
* <li>@subpage exist_User</li>
* <li>@subpage exist_descriptor</li>
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
 * @page daemonizar daemonizar
 * @brief Demoniza el programa y lo deja ejecutando en segundo plano
 * <h2>Synopsis</h2>
 *
 * @code
 * #include "includes/G-2313-07-P1-server.h"
 *
 * void daemonizar()
 * @endcode
 *
 * <h2>Descripción</h2>
 *
 * Demoniza el programa:\n
 * <ul>
 *   <li> Crea un proceso hijo, y hace terminar al padre.\n </li>
 *   <li> Crea una nueva sesión de tal forma que el proceso pase a ser el lider de la sesion.\n </li>
 *   <li> Cambia la mascara de modo de ficheros para que sean accesibles a cualquiera.\n </li>
 *   <li> Establece el directorio raiz / como directorio de trabajo\n </li>
 *   <li> Cierra todos los descriptores de fichero que pueda haber abiertos.\n </li>
 *   <li> Abre el log del sistema para su posterior uso.\n </li>
 * </ul>
 *
 * Se llama nada mas empezar el programa y todo se ejecutara en segundo plano en un proceso hijo que ha tomado el control.
 *
 * @warning Esta función deja al programa ejecuntado en segundo plano, por lo que se hay un error
 * habrá que consultar el log del sistema para saber donde ha fallado ya que prints no funcionaran.
 *
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
void daemonizar()
{
	pid_t pid;

	pid = fork(); /*Fork off the parent process*/
	if (pid < 0) exit(EXIT_FAILURE);
	if (pid > 0) exit(EXIT_SUCCESS); /*Exiting the parent process.*/

	umask(0); /*Change the file mode mask*/
	setlogmask (LOG_UPTO (LOG_INFO)); /*Open logs here*/
	openlog ("Server system messages:", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL3);
	syslog (LOG_ERR, "Initiating new server");

	if (setsid()< 0) { /*Create a new SID for the child process*/
		syslog (LOG_ERR, "Error creating a new SID for the child process.");
		exit(EXIT_FAILURE);
	}

	if ((chdir("/")) < 0) { /*Change the current working directory*/
		syslog (LOG_ERR, "Error changing the current working directory =\"/\"");
		exit(EXIT_FAILURE);
	}

	syslog (LOG_INFO, "Closing standard file descriptors");
	close(STDIN_FILENO); close(STDOUT_FILENO); close(STDERR_FILENO); /*Close out thestandard file descriptors*/
	return;
}


/**
 * @page isClosed isClosed
 * @brief Comprueba si un cliente se ha ido del servidor sin hacer quit
 * <h2>Synopsis</h2>
 *
 * @code
 * #include "includes/G-2313-07-P1-server.h"
 *
 * long isClosed(int desc)
 * @endcode
 *
 *
 * @param[in] desc Descriptor sobre el que se quiere comprobar si la conxion ya no esta abierta.
 *
 * @retval TRUE en caso de que el usuario se haya marchado sin hacer QUIT
 * @retval FALSE en caso de que el usuario siga en el servidor
 *
 * <h2>Descripción</h2>
 *
 * Esta funcion se encarga de comprobar si el socket sigue abierto, es decir, si el usuario se ha marchado pero no
 * ha cerrado la conexion.
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
long isClosed(int desc)
{
	int actividad;
	fd_set readfds;
	struct timeval timeout;

	FD_ZERO(&readfds);
	FD_SET(desc, &readfds);

	timeout.tv_sec = 0;
	timeout.tv_usec = 10000;

	actividad = select(desc, &readfds, NULL, NULL, &timeout);
	if((actividad < 0) && (errno != EINTR)){
		return TRUE;
	}else{
		return FALSE;
	}
}


/**
 * @page exist_User exist_User
 * @brief Comprueba si existe un usuario con el nick dado.
 * <h2>Synopsis</h2>
 *
 * @code
 * #include "includes/G-2313-07-P1-server.h"
 *
 * long exist_User(char *nick)
 * @endcode
 *
 * @param[in] nick Puntero a char donde se ecuentra el nick que se quiere comprobar
 *
 * @retval TRUE en caso de encontrar un usuario con el nick asociado
 * @retval FALSE en caso de no encontrar un usuario con el nick dado
 *
 * <h2>Descripción</h2>
 *
 * Dado un nick comprueba si hay algun usuario que tenga asociado este nick.
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
long exist_User(char *nick)
{
	char *unknown_user = NULL, *unknown_real = NULL;
	long unknown_id = 0;
	char *host, *IP, *away;
	int sock = 0;
	long creationTS, actionTS;

	/* Obtiene todos los parámetros a partir del nick*/
	if(IRCTADUser_GetData (&unknown_id, &unknown_user, &nick, &unknown_real, &host, &IP, &sock, &creationTS, &actionTS, &away) == IRC_OK){
		/*Encontrado un usario con el nick dado*/
		free(unknown_user);
		free(unknown_real);
		free(host);
		free(IP);
		free(away);
		return TRUE;
	}
	return FALSE;
}


/**
 * @page exist_descriptor exist_descriptor
 * @brief Comprueba si existe un descriptor de usuario
 * <h2>Synopsis</h2>
 *
 * @code
 * #include "includes/G-2313-07-P1-server.h"
 *
 * long exist_descriptor(int* desc)
 * @endcode
 *
 *
 * @param[in] desc Puntero a entero del descriptor que se quiere comporbar si pertenece a algun usuario
 * registrado en el servidor.
 *
 * @retval TRUE en caso de encontrar un usuario con el descriptor asociado
 * @retval FALSE en caso de no encontrar un usuario con el descriptor dado
 *
 * <h2>Descripción</h2>
 *
 * Dado un descriptor busca si en el servidor hay algun usuario que tenga asociado este descriptor.
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
long exist_descriptor(int* desc)
{
	char *unknown_user = NULL, *unknown_real = NULL, *unknown_nick = NULL;
	long unknown_id = 0;
	char *host, *IP, *away;
	long creationTS, actionTS;

	/* Obtiene todos los parámetros a partir del socket*/
	if(IRCTADUser_GetData (&unknown_id, &unknown_user, &unknown_nick, &unknown_real, &host, &IP, desc, &creationTS, &actionTS, &away) == IRC_OK){
		free(unknown_user);
		free(unknown_nick);
		free(unknown_real);
		free(host);
		free(IP);
		free(away);
		return TRUE;
	}
	return FALSE;
}
