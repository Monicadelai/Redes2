/**
* @brief Funciones asociadas a hilos del cliente
* @file G-2313-07-P2-ClientFunctions.c
*
* @authors Alfonso Bonilla Trueba (alfonso.bonilla@estudiante.uam.es)
* @authors Mónica de la Iglesia Martínez (monica.delaiglesia@estudiante.uam.es)
* @version 1.0
* @date 10-04-2017
*/

#include "../includes/G-2313-07-P2-ClientFunctions.h"

extern conexion conex; /**< @brief Acceso a la estructura con los datos de la conexion */
extern int ping_flag; /**< @brief Acceso al flag que controla el ping en el cliente */

/*! @page irc_client_functions Funciones Cliente
*
* <p>Esta sección incluye las funciones </p>
*
* <h2>Cabeceras</h2>
* <code>
* \b #include \b <G-2313-07-P2-ClientFunctions.h>
* </code>
*
* <hr>
* <hr>
*
* <h2>Funciones implementadas</h2>
* <p>Se incluyen las siguientes funciones de conexión y uso del servidor IRC:
* <ul>
* <li>@subpage IRC_Client_Recieve</li>
* <li>@subpage IRC_Client_Ping</li>
* <li>@subpage IRC_Client_Who</li>
* <li>@subpage IRC_Send_File</li>
* <li>@subpage IRC_Accept_File</li>
* </ul></p>
*
* @note Todas estas funciones están diseñadas para ser ejecutadas por un hilo indepnediente.
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
 * @page IRC_Client_Recieve IRC_Client_Recieve
 * @brief Recibe las respuestas del servidor y las manda a parsear
 * <h2>Synopsis</h2>
 *
 * @code
 * #include "includes/G-2313-07-P2-ClientFunctions.h"
 *
 * void* IRC_Client_Recieve(void* thr_info)
 * @endcode
 *
 * <h2>Descripción</h2>
 *
 * Esta función se encarga de estar permanentemente recibiendo mensajes del servidor
 * mientras la conexión esté activa. Una vez recibido el mensaje,lo dividirá en comandos
 * puesto que en un mismo mensaje pueden venir mas de un comando.
 * Cada comando recibido se enviará a pasear para mostrar en el cliente la información
 * de manera entendible y no a modo de comando.
 *
 * En caso de que no reciva un mensaje vacio entenderá que el servidor ha cerrado la
 * conexión y el hilo encargado de esta función finalizará.
 *
 * @param[in] thr_info Puntero a la información del hilo que se va a hacer cargo de estar
 * ejecutando esta función hasta que se cierre la conexión con el servidor
 *
 * @note Cuando se dectecte que el servidor se ha desconectado la función se encarga de hacer
 * kill al hilo que ejecutaba.
 *
 * @warning Esta función está diseñada para que sea ejecutada por un hilo
 *
 * <h2>Información</h2>
 * @authors Alfonso Bonilla Trueba (alfonso.bonilla@estudiante.uam.es)
 * @authors Mónica de la Iglesia Martínez (monica.delaiglesia@estudiante.uam.es)
 * @copyright Pareja 7 - Grupo 2313
 *
 * <hr>
 *
 */
void* IRC_Client_Recieve(void* thr_info)
{
  thread_info* info = (thread_info*) thr_info;
  int socket = info->sd;
  char *command = NULL, *str = NULL;
  char mensaje[512] = "";

  while(1){
    /*Recibimos y comprobamos*/
    if(recv(socket, (void*) mensaje, 512, 0) < 0){
      pthread_exit(NULL);
    }

    syslog(LOG_INFO, "CLIENT: Recibo del servidor %s", mensaje);

    /*Dividimos el mensaje y parseamos*/
    str = IRC_UnPipelineCommands(mensaje, &command);
    IRC_Reply_Parser(command, socket);
    free(command);
    while (str != NULL){
      str = IRC_UnPipelineCommands(str, &command);
      IRC_Reply_Parser(command, socket);
      free(command);
    }
    memset(mensaje,0, sizeof(mensaje));
  }
}



/**
 * @page IRC_Client_Ping IRC_Client_Ping
 * @brief Envia un mensaje de PING al servidor cada 30 segundos
 * <h2>Synopsis</h2>
 *
 * @code
 * #include "includes/G-2313-07-P2-ClientFunctions.h"
 *
 * void* IRC_Client_Ping(void* thr_info)
 * @endcode
 *
 * <h2>Descripción</h2>
 *
 * Esta función esta diseñada para que la ejecute un hilo encargado de ella exclusivamente, por ello
 * recibe por parámetro un puntero a la estructura thread_info donde se encuentra la información necesaria
 * para el correcto funcionamiento de la función.<br>
 *
 * Esta función va a estar ejecutandose hasta que el se cierre el cliente, o el servidor cierre la conexión.
 * Su principal función es comprobar que el servidor no ha cerrado la conexión y sigue estando disponible. Cada 30
 * segundos envía un mensaje PING al servidor, de esta manera comprueba si se ha cerrado o no la conexión.
 *
 * @param[in] thr_info Puntero a la estructura thread_info con la información sobre el descriptor del socket del cliente.
 *
 * @warning Esta función está diseñada para que sea ejecutada por un hilo
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
void* IRC_Client_Ping(void* thr_info)
{
  thread_info* info = (thread_info*) thr_info;
  char* msg = NULL, *server = NULL, *nick = NULL, *user = NULL, *realname = NULL, *password = NULL;
  int port, ssl, num, i;
  int socket = info->sd;
  char notificacion[250];
  char **list = NULL;

  /*Obtenemos datos de conexion y cliente*/
  IRCInterface_GetMyUserInfoThread(&nick, &user, &realname, &password, &server, &port, &ssl);

  free(user);
  free(realname);
  free(info);

  while(1){
    if(ping_flag == 1){
      sprintf(notificacion, "Desconectado del servidor (%s)",server);
      IRCInterface_WriteSystemThread(NULL, notificacion);
      IRCInterface_ListAllChannelsThread(&list, &num);
      for(i=0; i < num; i++){
        IRCInterface_WriteChannelThread(list[i], nick, notificacion);
      }
      free(server);
      free(nick);
      IRCInterface_FreeListAllChannelsThread(list, num);
      pthread_exit(NULL);
    }
    /*Generamos el mensaje ping*/
    IRCMsg_Ping(&msg, NULL, server, NULL);
    ping_flag++;

    /*Enviamos el mensaje PING*/
    send(socket, msg, strlen(msg), 0);

    /*Mostramos en el registro plano el mensaje PING que acabamos de enviar*/
    IRCInterface_PlaneRegisterOutMessageThread(msg);

    free(msg);
    sleep(30); /*30 segundos*/
  }
}


/**
 *
 * @page IRC_Client_Who IRC_Client_Who
 *
 * @brief Envia un mensaje de WHO al servidor cada 20 segundos
 *
 * <h2>Synopsis</h2>
 * @code
 * #include "includes/G-2313-07-P2-ClientFunctions.h"
 *
 * void* IRC_Client_Who(void* thr_info)
 * @endcode
 *
 * <h2>Descripción</h2>
 *
 * Envia un mensaje WHO al servidor cada 20 segundos, el mensaje WHO no es especifico de
 * ningún canal, si no que es general de todo el servidor. Esto es util para mantener actualizada
 * la lista de usuarios que estan conectados al servidor casi en tiempo real.
 *
 * @param[in] thr_info Puntero a la estructura thread_info con la información sobre el descriptor del socket del cliente.
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
void* IRC_Client_Who(void* thr_info)
{
  thread_info* info = (thread_info*) thr_info;
  int socket = info->sd;
  char* msg = NULL;

  IRCMsg_Who(&msg, NULL, NULL, NULL);

  while(1){

    /*Enviamos el mensaje WHO*/
    send(socket, msg, strlen(msg), 0);

    /*Mostramos en el registro plano el mensaje PING que acabamos de enviar*/
    IRCInterface_PlaneRegisterOutMessageThread(msg);

    sleep(20); /*20 segundos*/
  }
}


/**
 * @page IRC_Send_File IRC_Send_File
 * @brief Envia un mensaje para el envio de información
 * <h2>Synopsis</h2>
 *
 * @code
 * #include "includes/G-2313-07-P2-ClientFunctions.h"
 *
 * void* IRC_Send_File(void* thr_info)
 * @endcode
 *
 * <h2>Descripción</h2>
 *
 * Esta función genera y envía al servidor el mensaje de envio de información.
 *
 * @param[in] thr_info Información para el envio
 *
 * @warning Esta función está diseñada para que sea ejecutada por un hilo
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
void* IRC_Send_File(void* thr_fich)
{

  thread_fich* fich = NULL;
  char ini_msg[512], aux[4096], ip[16];
  struct timeval tv;
  struct ifaddrs *addrs, *addrs_aux;
  int fd;
  char *comm, *msg, lengthstr[128], portstr[6];
  int desc, port, i;
  struct sockaddr_in sock_addr;



  fich = (thread_fich *)thr_fich;

  srand(time(NULL));

  for (i = 0; i < 10; ++i){
    port = rand() % 65000;
    if (port < 1024){
      port+=1024;
    }

    //LISTEN
    desc = socket(AF_INET, SOCK_STREAM, 0);

    bzero (&sock_addr, sizeof (struct sockaddr_in));
    sock_addr.sin_port = htons (port);
     sock_addr.sin_addr.s_addr = htonl (INADDR_ANY);
    sock_addr.sin_family = AF_INET;
   


    if (bind(desc, (struct sockaddr *) &sock_addr, sizeof (struct sockaddr_in)) < 0){
      return FALSE;
    }

    if (listen (desc, 1) < 0){
      return FALSE;
    }

    if (desc < 0){
      continue;
    }
    break;
  }

  if(desc < 0){
    return FALSE;
  }

  getifaddrs(&addrs);
  addrs_aux = addrs;
  while (addrs_aux)
  {
    if (addrs_aux->ifa_addr && addrs_aux->ifa_addr->sa_family == AF_INET)
    {
        struct sockaddr_in *pAddr = (struct sockaddr_in *)addrs_aux->ifa_addr;
        sprintf (ip, "%s", inet_ntoa(pAddr->sin_addr));
    }

    if (strcmp (ip, "0.0.0.0") != 0 && strcmp (ip, "127.0.0.1") != 0 && strcmp (ip, "") != 0 && strlen (ip) >= 7){
      break;
    }

    addrs_aux = addrs_aux->ifa_next;
  }


 ;

  sprintf (lengthstr, "%lu", fich->datalen);
  sprintf (portstr, "%d", port);

  msg = (char *) malloc(sizeof(char)*(strlen ("1FS   ")+strlen(conex.nick)+strlen(fich->filename)+strlen(lengthstr)+
    strlen(ip)+strlen(portstr)+1));
  sprintf (msg, "%cFS %s %s %s %s %s", 1, conex.nick, fich->filename, lengthstr, ip, portstr);

  if (IRCMsg_Privmsg (&comm, NULL, fich->nick, msg) != IRC_OK){
    IRCInterface_ErrorDialog ("Error al crear mensaje.");
    return FALSE;
  }

  send (conex.desc, comm, strlen(comm)*sizeof(char), 0);
  IRCInterface_PlaneRegisterOutMessage (comm);


  tv.tv_sec = 60; // Damos 1 minuto al otro usuario para decidir
  tv.tv_usec = 0;

  setsockopt(desc, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(struct timeval));

  fd = accept (desc, NULL, 0);
  if (fd < 0){

    IRCInterface_ErrorDialog ("Error al iniciar la conexion o archivo rechazado.");
    free (fich->data);
    free (fich);
    return NULL;
  }


  if (read (fd, ini_msg, 512*sizeof(char)) < 0) {
    free (fich->data);
    free (fich);
    return NULL;
  }


  for (i = 0; i < fich->datalen; i+=4096){
    bzero (aux, 4096);
    strncpy (aux, &(fich->data[i]), 4096*sizeof(char));
    if (send (fd, aux, strlen(aux)*sizeof(char), 0) < 0){
      IRCInterface_ErrorDialog ("Error durante el envio del archivo.");
      free (fich->data);
      free (fich);
      return NULL;
    }
  }


  IRCInterface_ErrorDialogThread ("El archivo se ha transmitido con exito.");

  close(fd);
  freeifaddrs(addrs);
  free(comm);
  free(msg);
  free(fich->data);
  free(fich);
}

/**
 * @page IRC_Accept_File IRC_Accept_File
 * @brief Acepta los ficheros que recibe un cliente y muestra en una ventana emergente un error si es que lo hay.
 * <h2>Synopsis</h2>
 *
 * @code
 * #include "includes/G-2313-07-P2-ClientParser.h"
 *
 * void* IRC_Accept_File(void* fmsg)
 * @endcode
 *
 * <h2>Descripción</h2>
 *
 * Recibe una cadena que contiene los datos necesarios para aceptar el envio de un fichero, esta se
 * desconmopone enotras mas pequeñas y se procede a la aceptacion del fichero
 *
 * Mensajes tipo error seran mostrados a traves de una ventana emergente
 *
 * @param[in] fmsg puntero a una cadena que contiene los datos necesarios para la aceptacion de un fichero
 *
 * @note Esta función libera toda la memoria que reserva, por tanto el usuario no debe preocuparse
 * por liberar memoria.
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
void* IRC_Accept_File(void* fmsg)
{
  char *msg, *nick = NULL, *filename = NULL, *server = NULL;
  int desc, i, rec = 0, length, port;
  struct hostent *host_addr;
  struct sockaddr_in sock_addr;
  char data[4096];
  FILE *file;

  msg = (char *) fmsg;

  sscanf (msg, "\001FS %ms %ms %d %ms %d", &nick, &filename, &length, &server, &port);


  if (IRCInterface_ReceiveDialogThread (nick, filename) == FALSE){
    free (nick);
    free (filename);
    free (server);
    free (fmsg);
    return NULL;
  }

  if (server == NULL || port < 0){
    return FALSE;
  }

  host_addr = gethostbyname(server);
  if (host_addr == NULL){
    return FALSE;
  }

  desc = socket (AF_INET, SOCK_STREAM, 0);
  if (desc < 0){
    return FALSE;
  }

  bzero (&sock_addr, sizeof (struct sockaddr_in));
  sock_addr.sin_port = htons (port);
  sock_addr.sin_family = AF_INET;
  memcpy (&(sock_addr.sin_addr), host_addr->h_addr, host_addr->h_length);

  if (connect (desc, (struct sockaddr *) &sock_addr, sizeof (struct sockaddr_in)) < 0){
    return FALSE;
  }

  if (desc < 0){
    IRCInterface_ErrorDialogThread ("Error recibiendo archivo.");
    free (nick);
    free (filename);
    free (server);
    free (fmsg);
    return NULL;
  }


  if (send (desc, "\002GO", strlen("\002GO")*sizeof(char), 0) < 0){
    IRCInterface_ErrorDialogThread ("Error recibiendo archivo.");
    free (nick);
    free (filename);
    free (server);
    free (fmsg);
    return NULL;
  }

  file = fopen (filename, "w");
  if (file == NULL){
    IRCInterface_ErrorDialogThread ("Error recibiendo archivo.");
    free (nick);
    free (filename);
    free (server);
    free (fmsg);
    return NULL;
  }

  for (i = 0; i < length; rec = 0){
    bzero (data, 4096);
    rec = read (desc, data, 4096*sizeof(char));
    if (rec < 1){
      continue;
    }
    i += rec;
    fprintf(file, "%.*s", recv, data);
  }

  fclose (file);

  IRCInterface_ErrorDialogThread ("Transferencia de fichero completada.");

  close(desc);

  free (nick);
  free (filename);
  free (server);
  free (fmsg);
}
