/**
* @brief Parseadores de entrada y salida del cliente.
* @file G-2313-07-P2-ClientParser.c
*
* @authors Alfonso Bonilla Trueba (alfonso.bonilla@estudiante.uam.es)
* @authors Mónica de la Iglesia Martínez (monica.delaiglesia@estudiante.uam.es)
* @version 1.0
* @date 10-04-2017
*/

#include "../includes/G-2313-07-P2-ClientParser.h"
extern int ping_flag; /**< @brief Acceso al flag que controla el ping en el cliente */

/*! @page irc_client_parsers Parseadores Cliente
*
* <p>Esta sección incluye las funciones encargadas de los parseos del Cliente.<br>Estas
* generan los mensajes IRC para enviar al servidor y descomponen los mensajes recibidos
* del Servidor IRC para mostrar su contenido en la interfaz del Cliente.</p>
*
* <h2>Cabeceras</h2>
* <code>
* \b #include \b <G-2313-07-P2-ClientParser.h>
* </code>
*
* <hr>
* <hr>
*
* <h2>Funciones implementadas</h2>
* <p>Se incluyen las siguientes funciones para parsear los mensajes del Cliente IRC:
* <ul>
* <li>@subpage IRC_Client_Parser</li>
* <li>@subpage IRC_Reply_Parser</li>
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
 * @page IRC_Client_Parser IRC_Client_Parser
 * @brief Parsea y ejecuta los comandos IRC escritos por el cliente.
 * <h2>Synopsis</h2>
 *
 * @code
 * #include "includes/G-2313-07-P2-ClientParser.h"
 *
 * char* IRC_Client_Parser(char* command, long option)
 * @endcode
 *
 * <h2>Descripción</h2>
 *
 * Recibe una cadena con el comando que se quiere ejecutar, esta función se encarga de parsear este
 * para enviar el mensaje del usuario ya bien formado al servidor, de manera que este sea capaz de
 * entender el contenido y lo que el usuario quiere realizar.
 *
 * @param[in] command puntero a char con el comando recibido y que se va a parsear y ejecutar
 * @param[in] option valor de la opción
 *
 * @retval char* puntero a la cadena resultante con el mensaje listo para enviar al servidor
 *
 * @warning Esta función reserva memoria en el puntero que devuelve, en caso de que este puntero sea
 * distinto de NULL su liberación es responsabilidad del usuario.
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
char* IRC_Client_Parser(char* command, long option)
{
  char *msg = NULL, *mensaje = NULL, *nick = NULL, *server = NULL, *user = NULL, *topic = NULL;
  char *nick_new = NULL, *password = NULL, *filter = NULL, *mode = NULL;
  char *realname = NULL;
  char *channel = NULL, *targetserver = NULL, *target = NULL;
  int port, ssl;
  switch (option){

    case UNAMES:
      syslog(LOG_INFO, "CLIENT: Case -> UNAMES");
      if(IRCUserParse_Names(command, &channel, &targetserver) == IRC_OK){
        IRCMsg_Names(&msg, NULL, channel, targetserver);
      }
      break;

    case ULIST:
      syslog(LOG_INFO, "CLIENT: Case -> ULIST");
      if(IRCUserParse_List(command, &channel, &target) == IRC_OK){
        IRCMsg_List(&msg, NULL, channel, target);
      }
      break;

    case UJOIN:
      syslog(LOG_INFO, "CLIENT: Case -> UJOIN");
      if(IRCUserParse_Join(command, &channel, &password) == IRC_OK){
        IRC_ToLower (channel);
        IRCMsg_Join(&msg, NULL, channel, password, NULL);
      }
      break;

    case UPART:
      syslog(LOG_INFO, "CLIENT: Case -> UPART");
      channel = IRCInterface_ActiveChannelName(); /*EL programador no puede liberar este puntero*/
      if(IRCUserParse_Part(command, &mensaje) == IRC_OK){
        if(mensaje != NULL){
          if(IRCInterface_QueryChannelExist(mensaje) == FALSE){
            IRCInterface_WriteChannel(channel, NULL, " Uso: PART [<canal>], abandona el canal, por omisión el canal actual");
            return NULL;
          }else{
            IRC_ToLower (mensaje);
            IRCMsg_Part(&msg, NULL, mensaje, "Saliendo");
          }
        }else{
          IRCMsg_Part(&msg, NULL, channel, "Saliendo");
        }
      }
      break;

    case UQUIT:
      syslog(LOG_INFO, "CLIENT: Case -> UQUIT");
      if(IRCUserParse_Quit(command, &channel) == IRC_OK){
        IRCMsg_Quit(&msg, NULL, channel);
        IRCInterface_PlaneRegisterOutMessage(msg);
        IRCInterface_DisconnectServer(NULL, 0);
        IRCInterface_RemoveAllChannels();
        IRCInterface_WriteSystem("*","Disconected");
      }
      break;

    case UNICK:
      syslog(LOG_INFO, "CLIENT: Case -> UNICK");
      if(IRCUserParse_Nick(command, &nick_new) == IRC_OK){
        IRCMsg_Nick(&msg, NULL, nick_new, NULL);
      }
      break;

    case UAWAY:
      syslog(LOG_INFO, "CLIENT: Case -> UAWAY");
      if(IRCUserParse_Away(command, &mensaje) == IRC_OK){
        if(mensaje == NULL){
          IRCMsg_Away(&msg, NULL, "No estoy!");
        }else{
          IRCMsg_Away(&msg, NULL, mensaje);
        }
      }
      break;

    case UWHOIS:
      syslog(LOG_INFO, "CLIENT: Case -> UWHOIS");
      if(IRCUserParse_Whois(command, &mensaje) == IRC_OK){
        IRCMsg_Whois(&msg, NULL, NULL, mensaje);
      }
      break;

    case UWHO:
      syslog(LOG_INFO, "CLIENT: Case -> UWHO");
      if(IRCUserParse_Who(command, &mensaje) == IRC_OK){
        IRCMsg_Who(&msg, NULL, mensaje, NULL);
      }
      break;

    case UKICK:
      syslog(LOG_INFO, "CLIENT: Case -> UKICK");
      channel = IRCInterface_ActiveChannelName();
      if(IRCUserParse_Kick(command, &nick, &mensaje) == IRC_OK){
        IRCMsg_Kick(&msg, NULL, channel, nick, mensaje);
      }
      break;

    case UTOPIC:
      syslog(LOG_INFO, "CLIENT: Case -> UTOPIC");
      channel = IRCInterface_ActiveChannelName();
      if(IRCUserParse_Topic(command, &topic) == IRC_OK){
        IRCMsg_Topic(&msg, NULL, channel, topic);
      }
      break;

    case UMSG:
      syslog(LOG_INFO, "CLIENT: Case -> UMSG");
      if(IRCUserParse_Msg(command, &target, &mensaje) == IRC_OK){
        IRCMsg_Privmsg(&msg, NULL, target, mensaje);
        return msg;
      }
      break;

    case UNOTICE:
      syslog(LOG_INFO, "CLIENT: Case -> UNOTICE");
      if(IRCUserParse_Notice(command, &target, &mensaje) == IRC_OK){
        IRCMsg_Notice(&msg, NULL, target, mensaje);
      }
      break;

    case UPING:
      syslog(LOG_INFO, "CLIENT: Case -> UPING");
      if(IRCUserParse_Ping(command, &user) == IRC_OK){
        IRCMsg_Ping(&msg, NULL, user, NULL);
      }
      break;

    case UMOTD:
      syslog(LOG_INFO, "CLIENT: Case -> UMOTD");
      if(IRCUserParse_Motd(command, &server) == IRC_OK){
        IRCMsg_Motd(&msg, NULL, server);
      }
      break;

    case UMODE:
      syslog(LOG_INFO, "CLIENT: Case -> UMODE");
      channel = IRCInterface_ActiveChannelName();
      if(IRCUserParse_Mode(command, &mode, &filter) == IRC_OK){
        IRCMsg_Mode(&msg, NULL, channel, mode, NULL);
      }
      break;

    default: /*Es que escribe por un canal*/
      syslog(LOG_INFO, "CLIENT: Case -> DEFAULT (mensaje) o comando desconocido");

      channel = IRCInterface_ActiveChannelName();
      IRCInterface_GetMyUserInfo(&nick, &user, &realname, &password, &server, &port, &ssl);

      if(command[0] == '/'){
        if(IRCInterface_QueryChannelExist(channel) == FALSE){
          IRCInterface_WriteSystem("*", "Comando desconocido");
        }else{
          IRCInterface_WriteChannel(channel, "*", "Comando desconocido");
        }
        break;
      }

      IRCMsg_Privmsg(&msg, NULL, channel, command);
      IRCInterface_WriteChannel(channel, nick, command);
      IRC_MFree(5, &nick, &user, &realname, &password, &server );
      break;
  }

  syslog(LOG_INFO, "Mensaje creado (CLIENT): %s", msg);
  return msg;
}


/**
 * @page IRC_Reply_Parser IRC_Reply_Parser
 * @brief Parsea los mensajes recibidos del servidor, y muestra su contenido al usuario
 * a través de la interfaz gráfica
 * <h2>Synopsis</h2>
 *
 * @code
 * #include "includes/G-2313-07-P2-ClientParser.h"
 *
 * void IRC_Reply_Parser(char* command, int desc)
 * @endcode
 *
 * <h2>Descripción</h2>
 *
 * Recibe una cadena con el mensaje que se ha recibido del servidor, este se parsea
 * para mostrar la información que contiene de manera que el usuario sea capaz de entenderla
 * ademas de interferir en la interfaz por ejemplo en el caso de cambio de nick, que cambia el la
 * parte inferior izquierda el nombre una vez se recibe la confirmacion del servidor.
 *
 * Mensajes tipo error seran mostrados en la parte principal junto a los mensajes del chat.
 * De manera que el usuario sea capaz de entender la causa del error, puesto que el mensaje
 * recibido del servidor tal cual sería mas dificil de entender para un usuario medio
 *
 * @param[in] command puntero a char con el comando recibido y que se va a parsear y ejecutar
 * @param[in] desc valor del descriptor del servidor
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
void IRC_Reply_Parser(char* command, int desc)
{
  char *prefix = NULL, *nick =  NULL, *msg = NULL, *oldnick = NULL, *user = NULL;
  char *host = NULL, *server = NULL, *channel = NULL, *mode = NULL, *errnick = NULL, *name = NULL;
  char *key = NULL, *realname = NULL, *password = NULL, *olduser = NULL, *oldserver = NULL, *comando = NULL;
  char *user1 = NULL, *msgtarget = NULL, *target = NULL, *topic = NULL, *info = NULL, *visible = NULL;
  char *timedate = NULL, *servername = NULL, *versionname = NULL, *nick2 = NULL, *type = NULL, *modetxt = NULL;
  char *availableusermodes = NULL, *availablechannelmodes = NULL, *addedg = NULL, *version = NULL;
  char *servicename = NULL;
  char **list = NULL;
  pthread_t id;
  int port, ssl, num, i, nops, nchannels;
  int nusers, ninvisibles, nservers, nclients, hopcount, secs_idle, signon;
  long modelong;
  char notificacion[512] = "";


  switch (IRC_CommandQuery(command)) {

    case NICK:
      syslog(LOG_INFO, "CLIENT: He recibido comando NICK del server");
      IRCInterface_PlaneRegisterInMessageThread(command);

      if(IRCParse_Nick(command, &prefix, &nick, &msg) == IRC_OK){
        if(IRCParse_ComplexUser(prefix, &oldnick, &olduser, &host, &oldserver) == IRC_OK){
          sprintf(notificacion, "%s ahora es conocido como %s",oldnick, msg);
          IRCInterface_WriteSystemThread("*", notificacion);
          IRCInterface_ListAllChannelsThread (&list, &num);
          for(i=0; i < num; i++){
            IRCInterface_WriteChannelThread (list[i], "*", notificacion);
          }
          IRCInterface_ChangeNick(oldnick, msg);

          /*Liberacion de punteros*/
          IRCInterface_FreeListAllChannelsThread (list, num);
          IRC_MFree(8, &prefix, &nick, &msg, &oldnick, &user, &host, &server, &oldserver);
        }

      }
      break;

    case MODE:
      syslog(LOG_INFO, "CLIENT: He recibido comando MODE del server");
      IRCInterface_PlaneRegisterInMessageThread(command);
      if(IRCParse_Mode (command, &prefix, &channel, &mode, &target) == IRC_OK){
        if(prefix == NULL)
          break;

        if(IRCParse_ComplexUser(prefix, &nick, &user, &host, &server) == IRC_OK){
          if(strstr(mode, "+l")){
            sprintf(notificacion, "%s fija el limite del canal a %s", nick, target);
            IRCInterface_WriteChannelThread(channel, "*", notificacion);
          }
          else if(strstr(mode, "-l")){
            sprintf(notificacion, "%s quita el limite del canal", nick);
            IRCInterface_WriteChannelThread(channel, "*", notificacion);
          }
          else if(strstr(mode, "+k")){
            sprintf(notificacion, "%s fija la contraseña del canal a %s", nick, target);
            IRCInterface_WriteChannelThread(channel, "*", notificacion);
          }
          else if(strstr(mode, "-k")){
            sprintf(notificacion, "%s quita la contraseña del canal", nick);
            IRCInterface_WriteChannelThread(channel, "*", notificacion);
          }
          /*****modo m*****/
          else if(strstr(mode, "+m")){
            sprintf(notificacion, "%s fija el modo +m %s ", nick, channel);
            IRCInterface_WriteChannelThread(channel, "*", notificacion);
          }
          else if(strstr(mode, "-m")){
            sprintf(notificacion, "%s fija el modo -m %s", nick, channel);
            IRCInterface_WriteChannelThread(channel, "*", notificacion);
          }
          /*****modo i*****/
          else if(strstr(mode, "+i")){
            sprintf(notificacion, "%s fija el modo +i %s ", nick, channel);
            IRCInterface_WriteChannelThread(channel, "*", notificacion);
          }
          else if(strstr(mode, "-i")){
            sprintf(notificacion, "%s fija el modo -i %s", nick, channel);
            IRCInterface_WriteChannelThread(channel, "*", notificacion);
          }
          /*****modo s*****/
          else if(strstr(mode, "+s")){
            sprintf(notificacion, "%s fija el modo +s %s ", nick, channel);
            IRCInterface_WriteChannelThread(channel, "*", notificacion);
          }
          else if(strstr(mode, "-s")){
            sprintf(notificacion, "%s fija el modo -s %s", nick, channel);
            IRCInterface_WriteChannelThread(channel, "*", notificacion);
          }
          /*****modo n*****/
          else if(strstr(mode, "+n")){
            sprintf(notificacion, "%s fija el modo +n %s ", nick, channel);
            IRCInterface_WriteChannelThread(channel, "*", notificacion);
          }
          else if(strstr(mode, "-n")){
            sprintf(notificacion, "%s fija el modo -n %s", nick, channel);
            IRCInterface_WriteChannelThread(channel, "*", notificacion);
          }
          /*****modo t*****/
          else if(strstr(mode, "+t")){
            sprintf(notificacion, "%s fija el modo +t %s ", nick, channel);
            IRCInterface_WriteChannelThread(channel, "*", notificacion);
          }
          else if(strstr(mode, "-t")){
            sprintf(notificacion, "%s fija el modo -t %s", nick, channel);
            IRCInterface_WriteChannelThread(channel, "*", notificacion);
          }
          else if(strstr(mode, "+b")){
            sprintf(notificacion, "%s banea a %s", nick, target);
            IRCInterface_WriteChannelThread(channel, "*", notificacion);
          }
          else if(strstr(mode, "+v")){
            sprintf(notificacion, "%s da voz a %s", nick, target);
            IRCInterface_WriteChannelThread(channel, "*", notificacion);
            IRCInterface_ChangeNickStateChannelThread(channel, target, VOICE);
          }
          else if(strstr(mode, "-v")){
            sprintf(notificacion, "%s quita voz a %s", nick, target);
            IRCInterface_WriteChannelThread(channel, "*", notificacion);
            IRCInterface_ChangeNickStateChannelThread(channel, target, NONE);
          }
          else if(strstr(mode, "+o")){
            sprintf(notificacion, "%s da op a %s", nick, target);
            IRCInterface_WriteChannelThread(channel, "*", notificacion);
            IRCInterface_ChangeNickStateChannelThread(channel, target, OPERATOR);
          }
          else if(strstr(mode, "-o")){
            sprintf(notificacion, "%s quita op a %s", nick, target);
            IRCInterface_WriteChannelThread(channel, "*", notificacion);
            IRCInterface_ChangeNickStateChannelThread(channel, target, NONE);
          }
          /*Liberacion de punteros*/
          IRC_MFree(8, &prefix, &channel, &mode, &user, &nick, &host, &server, &target);
        }
      }
      break;

    case QUIT:
      syslog(LOG_INFO, "CLIENT: He recibido comando QUIT del server");
      IRCInterface_PlaneRegisterInMessageThread(command);
      break;

    case JOIN:
      syslog(LOG_INFO, "CLIENT: He recibido comando JOIN del server");
      IRCInterface_PlaneRegisterInMessageThread(command);
      if(IRCParse_Join(command, &prefix, &channel, &key, &msg) == IRC_OK){
        if(prefix == NULL)
          break;

        IRCParse_ComplexUser(prefix, &oldnick, &olduser, &host, &oldserver);
        IRCInterface_GetMyUserInfoThread(&nick, &user, &realname, &password, &server, &port, &ssl);

        if(strcmp(oldnick, nick) == 0){
          sprintf(notificacion, "Has entrado el canal %s", msg);
        }else{
          sprintf(notificacion, "%s (~%s@%s) ha entrado el canal %s", oldnick,oldnick,oldserver,msg);
        }

        if(IRCInterface_QueryChannelExistThread(msg) != 1){
          IRCInterface_AddNewChannelThread(msg, NONE);
        }

        IRCInterface_WriteChannelThread(msg, NULL, notificacion);
        if(IRCMsg_Who(&comando, NULL, msg, NULL) == IRC_OK) {
          IRCInterface_PlaneRegisterOutMessageThread(comando);
          send(desc, comando, strlen(comando), 0);
          }

        /*Liberacion de punteros*/
        IRC_MFree(14, &comando, &prefix, &channel, &key, &msg, &oldnick, &olduser, &host, &oldserver, &nick, &user, &realname, &password, &server);
      }
      break;

    case PART:
      syslog(LOG_INFO, "CLIENT: He recibido comando PART del server");
      IRCInterface_PlaneRegisterInMessageThread(command);
      if (IRCParse_Part(command, &prefix, &channel, &msg) == IRC_OK) {
        if(prefix == NULL)
          break;

        IRCParse_ComplexUser(prefix, &oldnick, &olduser, &host, &oldserver);
        IRCInterface_GetMyUserInfoThread(&nick, &user, &realname, &password, &server, &port, &ssl);

        if(strcmp(oldnick, nick) == 0){
          IRCInterface_DeleteNickChannelThread(channel, nick);
          IRCInterface_RemoveChannelThread(channel);
          sprintf(notificacion, "Ha salido del canal %s %s", channel, msg);
        }else{
          IRCInterface_DeleteNickChannelThread(channel, oldnick);
          sprintf(notificacion, "%s (~%s@%s) ha salido del canal %s (%s)", oldnick, oldnick, oldserver, channel, msg);
        }

        IRCInterface_WriteChannelThread(channel, "*", notificacion);

        /*Liberacion de punteros*/
        IRC_MFree(12, &prefix, &channel, &msg, &oldnick, &olduser, &host, &oldserver, &nick, &user, &realname, &password, &server);
      }
      break;

    case TOPIC:
      syslog(LOG_INFO, "CLIENT: He recibido comando TOPIC del server");
      IRCInterface_PlaneRegisterInMessageThread(command);
      if(IRCParse_Topic(command, &prefix, &channel, &msg) == IRC_OK){
        if(prefix == NULL)
          break;

        if(IRCParse_ComplexUser(prefix, &nick, &user, &host, &server) == IRC_OK){
          IRCInterface_SetTopicThread(msg);
          sprintf(notificacion, "%s ha cambiado el topic a %s", nick, msg);
          IRCInterface_WriteChannelThread(channel, "*", notificacion);
          /*Liberacion de punteros*/
          IRC_MFree(7, &prefix, &channel, &msg, &nick, &user, &host, &server);
        }
      }
      break;

    case NAMES:
      syslog(LOG_INFO, "CLIENT: He recibido comando NAMES del server");
      IRCInterface_PlaneRegisterInMessageThread(command);
      break;

    case LIST:
      syslog(LOG_INFO, "CLIENT: He recibido comando LIST del server");
      IRCInterface_PlaneRegisterInMessageThread(command);
      break;

    case KICK:
      syslog(LOG_INFO, "CLIENT: He recibido comando KICK del server");
      IRCInterface_PlaneRegisterInMessageThread(command);
      if(IRCParse_Kick(command, &prefix, &channel, &user, &msg) == IRC_OK){
        syslog(LOG_INFO, "HE PARSEADO KICK");
        if(prefix == NULL)
          break;

        IRCInterface_GetMyUserInfoThread(&nick, &user1, &realname, &password, &server, &port, &ssl);

        if(strcmp(nick, user) == 0){
          IRCInterface_DeleteNickChannelThread(channel, user);
          sprintf(notificacion, "Has sido expulsado de %s por %s", channel, msg);
          IRCInterface_RemoveChannelThread(channel);
          IRCInterface_WriteSystemThread("*", notificacion);
        }else{
          IRCInterface_DeleteNickChannelThread(channel, user);
          sprintf(notificacion, "Has expulsado a %s de %s", user, channel);
          IRCInterface_WriteChannelThread(channel, NULL, notificacion);
        }
       /*Liberacion de punteros*/
       IRC_MFree(9, &prefix, &channel, &user, &msg, &nick, &user1, &realname, &password, &server);
      }
      break;

    case PRIVMSG:
      syslog(LOG_INFO, "CLIENT: He recibido comando PRIVMSG del server");
      IRCInterface_PlaneRegisterInMessageThread(command);
      if(IRCParse_Privmsg(command, &prefix, &msgtarget, &msg) == IRC_OK){
        if (msg[0] == 1) {
          /*FICHEROS*/
          if(pthread_create(&id, NULL, &IRC_Accept_File, (void *)msg) != 0){
            return;
          }

          pthread_detach (id);
          return;
        }else{

          if(IRCParse_ComplexUser(prefix, &nick, &user, &host, &server) == IRC_OK){
            if(IRCInterface_QueryChannelExistThread (nick) == FALSE && msgtarget[0] != '#'){
              IRCInterface_AddNewChannelThread(nick, NONE);
            }

            if(IRCInterface_QueryChannelExistThread (nick) == TRUE && msgtarget[0] != '#'){
              IRCInterface_WriteChannelThread(nick, nick, msg);
            }else{
              IRCInterface_WriteChannelThread(msgtarget, nick, msg);
            }
          }
        }

        /*Liberacion de punteros*/
        IRC_MFree(7, &prefix, &msgtarget, &msg, &nick, &user, &host, &server);
      }
      break;

    case MOTD:
      syslog(LOG_INFO, "CLIENT: He recibido comando MOTD del server");
      IRCInterface_PlaneRegisterInMessageThread(command);
      break;

    case WHOIS:
      syslog(LOG_INFO, "CLIENT: He recibido comando WHOIS del server");
      IRCInterface_PlaneRegisterInMessageThread(command);
      break;

    case WHO:
      syslog(LOG_INFO, "CLIENT: He recibido comando WHO del server");
      IRCInterface_PlaneRegisterInMessageThread(command);
      if (IRCParse_RplWhoReply(command, &prefix, &nick, &channel, &user, &host, &server, &nick2, &type, &msg, &hopcount, &realname) == IRC_OK) {
          IRCInterface_AddNickChannelThread(channel, nick2, user, realname, host, NONE);
      }
      /*Liberacion de punteros*/
      IRC_MFree(8, &nick, &channel, &user, &host, &server, &nick2, &type, &msg, &realname);
      break;

    case PING:
      syslog(LOG_INFO, "CLIENT: He recibido comando PING del server");
      IRCInterface_PlaneRegisterInMessageThread(command);
      if(IRCParse_Ping(command, &prefix, &server, &host, &msg) == IRC_OK){

        if(IRCMsg_Pong (&comando, prefix, server, host, msg) == IRC_OK){
          sprintf(notificacion, "Envio Pong");
          IRCInterface_WriteSystemThread(NULL, notificacion);
        }


        IRC_MFree(4, &prefix, &server, &host, &msg);
      }
      break;

    case PONG:
      syslog(LOG_INFO, "CLIENT: He recibido comando PONG del server");
      ping_flag = 0;
      IRCInterface_PlaneRegisterInMessageThread(command);

      IRCParse_Pong (command, &prefix, &servername, &name, &msg);
      IRCInterface_GetMyUserInfo(&nick, &user, &realname, &password, &server, &port, &ssl);

      if(strcmp(msg, server) != 0){
        channel = IRCInterface_ActiveChannelName();
        sprintf(notificacion, "Recibo PONG de %s", msg);
        if(IRCInterface_QueryChannelExist(channel) == FALSE){
          IRCInterface_WriteSystemThread("*", notificacion);
        }else{
          IRCInterface_WriteChannel(channel, "*", notificacion);
        }
      }
      IRC_MFree(9, &prefix, &servername, &name, &msg, &nick, &user, &realname, &password, &server);
      break;

    case AWAY:
      syslog(LOG_INFO, "CLIENT: He recibido comando AWAY del server");
      IRCInterface_PlaneRegisterInMessageThread(command);
      break;

    case PASS:
      syslog(LOG_INFO, "CLIENT: He recibido comando PASS del server");
      IRCInterface_PlaneRegisterInMessageThread(command);
      IRCParse_Pass (command, &prefix, &password);
      channel = IRCInterface_ActiveChannelName();
      sprintf(notificacion, "Establecida password (%s)", password);
      if(IRCInterface_QueryChannelExist(channel) == FALSE){
        IRCInterface_WriteSystemThread("*", notificacion);
      }else{
        IRCInterface_WriteChannel(channel, "*", notificacion);
      }
      IRC_MFree(2, &prefix, &password);
      break;

    case SETNAME:
      syslog(LOG_INFO, "CLIENT: He recibido comando SETNAME del server");
      IRCInterface_PlaneRegisterInMessageThread(command);

      break;

    case RPL_WELCOME:
      syslog(LOG_INFO, "CLIENT: He recibido comando RPL_WELCOME del server");
      IRCInterface_PlaneRegisterInMessageThread(command);
      if(IRCParse_RplWelcome(command, &prefix, &nick, &msg) == IRC_OK){
        IRCInterface_WriteSystemThread(NULL, msg);
        /*Liberacion de punteros*/
        IRC_MFree(3, &prefix, &nick, &msg);
      }
      break;

    case RPL_CREATED:
      syslog(LOG_INFO, "CLIENT: He recibido comando RPL_CREATED del server");
      IRCInterface_PlaneRegisterInMessageThread(command);
      if(IRCParse_RplCreated(command, &prefix, &nick, &timedate, &msg) == IRC_OK){
        IRCInterface_WriteSystemThread(NULL, msg);
        /*Liberacion de punteros*/
        IRC_MFree(4, &prefix, &nick, &timedate, &msg);
      }
      break;

    case RPL_YOURHOST:
      syslog(LOG_INFO, "CLIENT: He recibido comando RPL_YOURHOST del server");
      IRCInterface_PlaneRegisterInMessageThread(command);
      if(IRCParse_RplYourHost (command, &prefix, &nick, &msg, &servername, &versionname) == IRC_OK){
        IRCInterface_WriteSystemThread(NULL, msg);
        /*Liberacion de punteros*/
        IRC_MFree(5, &prefix, &nick, &msg, &servername, &versionname);
      }
      break;

    case RPL_LUSERCLIENT:
      syslog(LOG_INFO, "CLIENT: He recibido comando RPL_LUSERCLIENT del server");
      IRCInterface_PlaneRegisterInMessageThread(command);
      IRCParse_RplLuserClient(command, &prefix, &nick, &msg, &nusers, &ninvisibles, &nservers);
      IRCInterface_WriteSystemThread(NULL, msg);
      /*Liberacion de punteros*/
      IRC_MFree(3, &prefix, &nick, &msg);

      break;

    case RPL_LUSERME:
      syslog(LOG_INFO, "CLIENT: He recibido comando RPL_LUSERME del server");
      IRCInterface_PlaneRegisterInMessageThread(command);
      IRCParse_RplLuserMe (command, &prefix, &nick, &msg, &nclients, &nservers);
      IRCInterface_WriteSystemThread(NULL, msg);
      syslog(LOG_INFO, "CLIENT:DENTRO comando RPL_LUSERME del server");
      /*Liberacion de punteros*/
      IRC_MFree(3, &prefix, &nick, &msg);

      break;

    case RPL_MOTDSTART:
      syslog(LOG_INFO, "CLIENT: He recibido comando RPL_MOTDSTART del server");
      IRCInterface_PlaneRegisterInMessageThread(command);
      break;

    case RPL_MOTD:
      syslog(LOG_INFO, "CLIENT: He recibido comando RPL_MOTD del server");
      IRCInterface_PlaneRegisterInMessageThread(command);
      if(IRCParse_RplMotd(command, &prefix, &nick, &msg) == IRC_OK){
        IRCInterface_WriteSystemThread(NULL, msg);
        IRC_MFree(3, &prefix, &nick, &msg);
      }
      break;

    case RPL_ENDOFMOTD:
      syslog(LOG_INFO, "CLIENT: He recibido comando RPL_ENDOFMOTD del server");
      IRCInterface_PlaneRegisterInMessageThread(command);
      IRCInterface_WriteSystemThread(NULL, "End of MOTD command");
      break;

    case RPL_WHOREPLY:
      syslog(LOG_INFO, "CLIENT: He recibido comando RPL_WHOREPLY del server");
      IRCInterface_PlaneRegisterInMessageThread(command);
      if(IRCParse_RplWhoReply(command, &prefix, &nick, &channel, &user, &host, &server, &nick2, &type, &msg, &hopcount, &realname) == IRC_OK){

        if(type[1] == '@'){
          syslog(LOG_INFO, "es op");
          IRCInterface_AddNickChannelThread(channel, nick2, user, realname, host, OPERATOR);
        }else{
          IRCInterface_AddNickChannelThread(channel, nick2, user, realname, host, NONE);
        }
        /*Liberacion de punteros*/
        IRC_MFree(10, &prefix, &nick, &channel, &user, &host, &server, &nick2, &type, &msg, &realname);
      }
      break;

    case RPL_CHANNELMODEIS:
      syslog(LOG_INFO, "CLIENT: He recibido comando RPL_CHANNELMODEIS del server");
      IRCInterface_PlaneRegisterInMessageThread(command);
      if(IRCParse_RplChannelModeIs(command, &prefix, &nick, &channel, &modetxt) == IRC_OK){
        if(prefix == NULL)
          break;

        modelong = IRCInterface_ModeToIntModeThread(modetxt);
        IRCInterface_SetModeChannelThread (channel, modelong);

        if((modelong & IRCMODE_TOPICOP) == IRCMODE_TOPICOP){
          IRCInterface_SetProtectTopicThread();
        }
        /*Liberacion de punteros*/
        IRC_MFree(4, &prefix, &nick2, &channel, &modetxt);
      }
      break;

    case ERR_CHANOPRIVSNEEDED:
      syslog(LOG_INFO, "CLIENT: He recibido comando ERR_CHANOPRIVSNEEDED del server");
      IRCInterface_PlaneRegisterInMessageThread(command);

      switch (IRCParse_ErrChanOPrivsNeeded(command, &prefix, &nick, &channel, &msg)) {
        case IRCERR_NOSTRING:
          syslog(LOG_INFO, "IRCERR_NOSTRING");
          break;

        case IRCERR_ERRONEUSCOMMAND:
          syslog(LOG_INFO, "IRCERR_ERRONEUSCOMMAND");
          break;

        default:
          sprintf(notificacion, "%s %s", nick, msg);
          IRCInterface_WriteChannelThread(channel, NULL, notificacion);
          IRCInterface_ErrorDialogThread (notificacion);
          /*Liberacion de punteros*/
          IRC_MFree(4, &prefix, &nick, &channel, &msg);
          break;
      }
      break;

    case ERR_UNKNOWNMODE:
      syslog(LOG_INFO, "CLIENT: He recibido comando ERR_UNKNOWNMODE del server");
      IRCInterface_PlaneRegisterInMessageThread(command);
      IRCParse_ErrUnknownMode(command, &prefix, &nick, &mode, &name, &msg);
      channel = IRCInterface_ActiveChannelName();
      if(IRCInterface_QueryChannelExist(channel) == FALSE){
        IRCInterface_WriteSystemThread("*", msg);
      }else{
        IRCInterface_WriteChannel(channel, "*", msg);
      }
      IRCInterface_ErrorDialogThread (msg);
      /*Liberacion de punteros*/
      IRC_MFree(5, &prefix, &nick, &mode, &name, &msg);
      break;

    case ERR_NOSUCHNICK:
      syslog(LOG_INFO, "CLIENT: He recibido comando ERR_NOSUCHNICK del server");
      IRCInterface_PlaneRegisterInMessageThread(command);
      IRCParse_ErrNoSuchNick(command, &prefix, &nick, &name, &msg);
      channel = IRCInterface_ActiveChannelName();
      if(IRCInterface_QueryChannelExist(channel) == FALSE){
        IRCInterface_WriteSystemThread("*", msg);
      }else{
        IRCInterface_WriteChannel(channel, "*", msg);
      }
      IRCInterface_ErrorDialogThread (msg);
      IRC_MFree(4, &prefix, &nick, &name, &msg);
      break;

    case ERR_CHANNELISFULL:
      syslog(LOG_INFO, "CLIENT: He recibido comando ERR_CHANNELISFULL del server");
      IRCInterface_PlaneRegisterInMessageThread(command);
      IRCParse_ErrChannelIsFull (command, &prefix, &nick, &target, &msg);
      channel = IRCInterface_ActiveChannelName();
      if(IRCInterface_QueryChannelExist(channel) == FALSE){
        IRCInterface_WriteSystemThread("*", msg);
      }else{
        IRCInterface_WriteChannel(channel, "*", msg);
      }
      IRCInterface_ErrorDialogThread (msg);
      IRC_MFree(4, &prefix, &nick, &target, &msg);
      break;

    case ERR_NICKNAMEINUSE:
      syslog(LOG_INFO, "CLIENT: He recibido comando ERR_NICKNAMEINUSE del server");
      IRCInterface_PlaneRegisterInMessageThread(command);
      IRCParse_ErrNickNameInUse (command, &prefix, &nick, &errnick, &msg);
      sprintf(notificacion, "El nick %s está en uso", errnick);
      channel = IRCInterface_ActiveChannelName();
      if(IRCInterface_QueryChannelExist(channel) == FALSE){
        IRCInterface_WriteSystemThread("*", notificacion);
      }else{
        IRCInterface_WriteChannel(channel, "*", notificacion);
      }
      IRCInterface_ErrorDialogThread (notificacion);
      IRC_MFree(4, &prefix, &nick, &errnick, &msg);
      break;

    case ERR_ERRONEUSNICKNAME:
      syslog(LOG_INFO, "CLIENT: He recibido comando ERR_ERRONEUSNICKNAME del server");
      IRCInterface_PlaneRegisterInMessageThread(command);
      IRCParse_ErrErroneusNickName (command, &prefix, &nick, &errnick, &msg);
      channel = IRCInterface_ActiveChannelName();
      if(IRCInterface_QueryChannelExist(channel) == FALSE){
        IRCInterface_WriteSystemThread("*", msg);
      }else{
        IRCInterface_WriteChannel(channel, "*", msg);
      }
      IRCInterface_ErrorDialogThread (msg);
      IRC_MFree(4, &prefix, &nick, &errnick, &msg);
      break;

    case ERR_NICKCOLLISION:
      syslog(LOG_INFO, "CLIENT: He recibido comando ERR_NICKCOLLISION del server");
      IRCInterface_PlaneRegisterInMessageThread(command);
      IRCParse_ErrNickCollision (command, &prefix, &nick, &name, &msg, &target);
      channel = IRCInterface_ActiveChannelName();
      if(IRCInterface_QueryChannelExist(channel) == FALSE){
        IRCInterface_WriteSystemThread("*", msg);
      }else{
        IRCInterface_WriteChannel(channel, "*", msg);
      }
      IRCInterface_ErrorDialogThread (msg);
      IRC_MFree(5, &prefix, &nick, &name, &msg, &target);
      break;

    case ERR_NOMOTD:
      syslog(LOG_INFO, "CLIENT: He recibido comando ERR_NOMOTD del server");
      IRCInterface_PlaneRegisterInMessageThread(command);
      IRCParse_ErrNoMotd (command, &prefix, &nick, &msg);
      channel = IRCInterface_ActiveChannelName();
      if(IRCInterface_QueryChannelExist(channel) == FALSE){
        IRCInterface_WriteSystemThread("*", msg);
      }else{
        IRCInterface_WriteChannel(channel, "*", msg);
      }
      IRCInterface_ErrorDialogThread (msg);
      IRC_MFree(3, &prefix, &nick, &msg);
      break;

    case ERR_NOSUCHCHANNEL:
      syslog(LOG_INFO, "CLIENT: He recibido comando ERR_NOSUCHCHANNEL del server");
      IRCInterface_PlaneRegisterInMessageThread(command);
      IRCParse_ErrNoSuchChannel (command, &prefix, &nick, &name, &msg);
      channel = IRCInterface_ActiveChannelName();
      sprintf(notificacion, "%s : %s", name, msg);
      if(IRCInterface_QueryChannelExist(channel) == FALSE){
        IRCInterface_WriteSystemThread("*", notificacion);
      }else{
        IRCInterface_WriteChannel(channel, "*", notificacion);
      }
      IRCInterface_ErrorDialogThread (notificacion);
      IRC_MFree(4, &prefix, &nick, &name, &msg);
      break;

    case ERR_PASSWDMISMATCH:
      syslog(LOG_INFO, "CLIENT: He recibido comando ERR_PASSWDMISMATCH del server");
      IRCInterface_PlaneRegisterInMessageThread(command);
      IRCParse_ErrPasswdMismatch (command, &prefix, &nick, &msg);
      channel = IRCInterface_ActiveChannelName();
      if(IRCInterface_QueryChannelExist(channel) == FALSE){
        IRCInterface_WriteSystemThread("*", msg);
      }else{
        IRCInterface_WriteChannel(channel, "*", msg);
      }
      IRCInterface_ErrorDialogThread (msg);
      IRC_MFree(3, &prefix, &nick, &msg);
      break;

    case ERR_INVITEONLYCHAN:
      syslog(LOG_INFO, "CLIENT: He recibido comando ERR_INVITEONLYCHAN del server");
      IRCInterface_PlaneRegisterInMessageThread(command);
      IRCParse_ErrInviteOnlyChan (command, &prefix, &nick, &name, &msg);
      channel = IRCInterface_ActiveChannelName();
      sprintf(notificacion, "%s : %s", name, msg);
      if(IRCInterface_QueryChannelExist(channel) == FALSE){
        IRCInterface_WriteSystemThread("*", notificacion);
      }else{
        IRCInterface_WriteChannel(channel, "*", notificacion);
      }
      IRCInterface_ErrorDialogThread (notificacion);
      IRC_MFree(4, &prefix, &nick, &name, &msg);
      break;

    case ERR_BANNEDFROMCHAN:
      syslog(LOG_INFO, "CLIENT: He recibido comando ERR_BANNEDFROMCHAN del server");
      IRCInterface_PlaneRegisterInMessageThread(command);
      IRCParse_ErrBannedFromChan (command, &prefix, &nick, &name, &msg);
      channel = IRCInterface_ActiveChannelName();
      sprintf(notificacion, "%s : %s", name, msg);
      if(IRCInterface_QueryChannelExist(channel) == FALSE){
        IRCInterface_WriteSystemThread("*", notificacion);
      }else{
        IRCInterface_WriteChannel(channel, "*", notificacion);
      }
      IRCInterface_ErrorDialogThread (notificacion);
      IRC_MFree(4, &prefix, &nick, &name, &msg);
      break;

    case ERR_NONICKNAMEGIVEN:
      syslog(LOG_INFO, "CLIENT: He recibido comando ERR_NONICKNAMEGIVEN del server");
      IRCInterface_PlaneRegisterInMessageThread(command);
      IRCParse_ErrNoNickNameGiven (command, &prefix, &nick, &msg);
      channel = IRCInterface_ActiveChannelName();
      if(IRCInterface_QueryChannelExist(channel) == FALSE){
        IRCInterface_WriteSystemThread("*", msg);
      }else{
        IRCInterface_WriteChannel(channel, "*", msg);
      }
      IRCInterface_ErrorDialogThread (msg);
      IRC_MFree(3, &prefix, &nick, &msg);
      break;

    case ERR_RESTRICTED:
      syslog(LOG_INFO, "CLIENT: He recibido comando ERR_RESTRICTED del server");
      IRCInterface_PlaneRegisterInMessageThread(command);
      IRCParse_ErrRestricted (command, &prefix, &nick, &msg);
      channel = IRCInterface_ActiveChannelName();
      if(IRCInterface_QueryChannelExist(channel) == FALSE){
        IRCInterface_WriteSystemThread("*", msg);
      }else{
        IRCInterface_WriteChannel(channel, "*", msg);
      }
      IRCInterface_ErrorDialogThread (msg);
      IRC_MFree(3, &prefix, &nick, &msg);
      break;

    case ERR_CANNOTSENDTOCHAN:
      syslog(LOG_INFO, "CLIENT: He recibido comando ERR_CANNOTSENDTOCHAN del server");
      IRCInterface_PlaneRegisterInMessageThread(command);
      IRCParse_ErrCanNotSendToChan (command, &prefix, &nick, &name, &msg);
      channel = IRCInterface_ActiveChannelName();
      sprintf(notificacion, "%s : %s", name, msg);
      if(IRCInterface_QueryChannelExist(channel) == FALSE){
        IRCInterface_WriteSystemThread("*", notificacion);
      }else{
        IRCInterface_WriteChannel(channel, "*", notificacion);
      }
      IRCInterface_ErrorDialogThread (notificacion);
      IRC_MFree(4, &prefix, &nick, &name, &msg);
      break;

    case ERR_NOCHANMODES:
      syslog(LOG_INFO, "CLIENT: He recibido comando ERR_NOCHANMODES del server");
      IRCInterface_PlaneRegisterInMessageThread(command);
      IRCParse_ErrNoChanModes (command, &prefix, &nick, &name, &msg);
      channel = IRCInterface_ActiveChannelName();
      sprintf(notificacion, "%s : %s", name, msg);
      if(IRCInterface_QueryChannelExist(channel) == FALSE){
        IRCInterface_WriteSystemThread("*", notificacion);
      }else{
        IRCInterface_WriteChannel(channel, "*", notificacion);
      }
      IRCInterface_ErrorDialogThread (notificacion);
      IRC_MFree(4, &prefix, &nick, &name, &msg);
      break;

    case ERR_UNAVAILRESOURCE:
      syslog(LOG_INFO, "CLIENT: He recibido comando ERR_UNAVAILRESOURCE del server");
      IRCInterface_PlaneRegisterInMessageThread(command);
      IRCParse_ErrUnavailResource (command, &prefix, &nick, &target);
      channel = IRCInterface_ActiveChannelName();
      sprintf(notificacion, "Unavail Resource : %s", target);
      if(IRCInterface_QueryChannelExist(channel) == FALSE){
        IRCInterface_WriteSystemThread("*", notificacion);
      }else{
        IRCInterface_WriteChannel(channel, "*", notificacion);
      }
      IRCInterface_ErrorDialogThread (notificacion);
      IRC_MFree(3, &prefix, &nick, &target);
      break;

    case ERR_ALREADYREGISTRED:
      syslog(LOG_INFO, "CLIENT: He recibido comando ERR_ALREADYREGISTRED del server");
      IRCInterface_PlaneRegisterInMessageThread(command);
      IRCParse_ErrAlreadyRegistred (command, &prefix, &nick, &msg);
      channel = IRCInterface_ActiveChannelName();
      sprintf(notificacion, "%s : %s", nick, msg);
      if(IRCInterface_QueryChannelExist(channel) == FALSE){
        IRCInterface_WriteSystemThread("*", notificacion);
      }else{
        IRCInterface_WriteChannel(channel, "*", notificacion);
      }
      IRCInterface_ErrorDialogThread (notificacion);
      IRC_MFree(3, &prefix, &nick, &msg);
      break;

    case RPL_WHOISUSER:
      syslog(LOG_INFO, "CLIENT: He recibido comando RPL_WHOISUSER del server");
      IRCInterface_PlaneRegisterInMessageThread(command);
      if(IRCParse_RplWhoIsUser(command, &prefix, &nick, &nick2, &name, &host, &realname) == IRC_OK){
        sprintf(notificacion, "[%s] (%s:%s): %s", nick, name, host, realname);
        channel = IRCInterface_ActiveChannelName();
        if(IRCInterface_QueryChannelExist(channel) == FALSE){
          IRCInterface_WriteSystemThread(NULL, notificacion);
        }else{
          IRCInterface_WriteChannel(channel, NULL, notificacion);
        }
        /*Liberacion de punteros*/
        IRC_MFree(6, &prefix, &nick, &nick2, &name, &host, &realname);
      }
      break;

    case RPL_WHOISSERVER:
      syslog(LOG_INFO, "CLIENT: He recibido comando RPL_WHOISSERVER del server");
      IRCInterface_PlaneRegisterInMessageThread(command);
      IRCParse_RplWhoIsServer(command, &prefix, &nick, &nick2, &server, &msg);
      sprintf(notificacion, "[%s] %s %s", nick, server, msg);
      IRCInterface_WriteSystemThread(NULL, notificacion);
      channel = IRCInterface_ActiveChannelName();
      if(IRCInterface_QueryChannelExist(channel) == FALSE){
        IRCInterface_WriteSystemThread(NULL, notificacion);
      }else{
        IRCInterface_WriteChannel(channel, NULL, notificacion);
      }
      /*Liberacion de punteros*/
      IRC_MFree(5, &prefix, &nick, &nick2, &server, &msg);
      break;

    case RPL_WHOISIDLE:
      syslog(LOG_INFO, "CLIENT: He recibido comando RPL_WHOISIDLE del server");
      IRCInterface_PlaneRegisterInMessageThread(command);
      IRCParse_RplWhoIsIdle(command, &prefix, &nick, &nick2, &secs_idle, &signon, &msg);
      sprintf(notificacion, "[%s] inactivo durante %d seg, entro: %d", nick, secs_idle, signon);
      channel = IRCInterface_ActiveChannelName();
      if(IRCInterface_QueryChannelExist(channel) == FALSE){
        IRCInterface_WriteSystemThread(NULL, notificacion);
      }else{
        IRCInterface_WriteChannel(channel, NULL, notificacion);
      }
      /*Liberacion de punteros*/
      IRC_MFree(4, &prefix, &nick, &nick2, &msg);
      break;

    case RPL_ENDOFWHOIS:
      syslog(LOG_INFO, "CLIENT: He recibido comando RPL_ENDOFWHOIS del server");
      IRCInterface_PlaneRegisterInMessageThread(command);
      break;

    case RPL_ENDOFWHO:
      syslog(LOG_INFO, "CLIENT: He recibido comando RPL_ENDOFWHO del server");
      IRCInterface_PlaneRegisterInMessageThread(command);
      break;

    case RPL_YOUREOPER:
      syslog(LOG_INFO, "CLIENT: He recibido comando RPL_YOUREOPER del server");
      IRCInterface_PlaneRegisterInMessageThread(command);
      IRCParse_RplYoureOper(command, &prefix, &nick, &msg);
        sprintf(notificacion, "%s", msg);
        channel = IRCInterface_ActiveChannelName();
        if(IRCInterface_QueryChannelExist(channel) == FALSE){
          IRCInterface_WriteSystemThread("*", notificacion);
        }else{
          IRCInterface_WriteChannel(channel, "*", notificacion);
        }

        IRCInterface_ErrorDialogThread (notificacion);
        IRC_MFree(4, &prefix, &nick, &name, &msg);
      break;

    case RPL_AWAY:
      syslog(LOG_INFO, "CLIENT: He recibido comando RPL_AWAY del server");
      IRCInterface_PlaneRegisterInMessageThread(command);
      if(IRCParse_RplAway(command, &prefix, &nick, &nick2, &msg) == IRC_OK){
        channel = IRCInterface_ActiveChannelName();
        sprintf(notificacion, "%s: %s", nick2, msg);
        if(IRCInterface_QueryChannelExist(channel) == FALSE){
          IRCInterface_WriteSystemThread(NULL, notificacion);
        }else{
          IRCInterface_WriteChannel(channel, "*", notificacion);
        }
        IRCInterface_ErrorDialogThread (notificacion);
        /*Liberacion de punteros*/
        IRC_MFree(4, &prefix, &nick, &nick2, &msg);
      }
      break;

    case RPL_NOWAWAY:
      syslog(LOG_INFO, "CLIENT: He recibido comando RPL_NOWAWAY del server");
      IRCInterface_PlaneRegisterInMessageThread(command);
      if(IRCParse_RplNowAway(command, &prefix, &nick, &msg) == IRC_OK){
        sprintf(notificacion, "%s", msg);
        IRCInterface_WriteSystemThread(NULL, notificacion);
        IRC_MFree(3, &prefix, &nick, &msg);
      }
      break;

    case RPL_TOPIC:
      syslog(LOG_INFO, "CLIENT: He recibido comando RPL_TOPIC del server");
      IRCInterface_PlaneRegisterInMessageThread(command);
      IRCParse_RplTopic (command, &prefix, &nick, &name, &topic);
      sprintf(notificacion, "Topic de %s: %s",name, topic);
      channel = IRCInterface_ActiveChannelName();
      if(IRCInterface_QueryChannelExist(channel) == FALSE){
        IRCInterface_WriteSystemThread("*", notificacion);
      }else{
        IRCInterface_WriteChannel(channel, "*", notificacion);
      }
      IRC_MFree(4, &prefix, &nick, &name, &topic);
      break;

    case RPL_NOTOPIC:
      syslog(LOG_INFO, "CLIENT: He recibido comando RPL_NOTOPIC del server");
      IRCInterface_PlaneRegisterInMessageThread(command);
      IRCParse_RplNoTopic (command, &prefix, &nick, &name, &topic);
      sprintf(notificacion, "El canal %s no tiene Topic establecido", name);
      channel = IRCInterface_ActiveChannelName();
      if(IRCInterface_QueryChannelExist(channel) == FALSE){
        IRCInterface_WriteSystemThread("*", notificacion);
      }else{
        IRCInterface_WriteChannel(channel, "*", notificacion);
      }
      IRC_MFree(4, &prefix, &nick, &name, &topic);
      break;

    case RPL_INFO:
      syslog(LOG_INFO, "CLIENT: He recibido comando RPL_INFO del server");
      IRCInterface_PlaneRegisterInMessageThread(command);
      IRCParse_RplInfo (command, &prefix, &nick, &info);
      sprintf(notificacion, "Info: %s", info);
      channel = IRCInterface_ActiveChannelName();
      if(IRCInterface_QueryChannelExist(channel) == FALSE){
        IRCInterface_WriteSystemThread(NULL, notificacion);
      }else{
        IRCInterface_WriteChannel(channel, NULL, notificacion);
      }
      IRC_MFree(3, &prefix, &nick, &info);
      break;

    case RPL_LIST:
      syslog(LOG_INFO, "CLIENT: He recibido comando RPL_LIST del server");
      IRCInterface_PlaneRegisterInMessageThread(command);
      IRCParse_RplList (command, &prefix, &nick, &name, &visible, &topic);
      sprintf(notificacion, "%s \t Tema: %s Usuarios: %s", name, topic, visible);
      channel = IRCInterface_ActiveChannelName();
      if(IRCInterface_QueryChannelExist(channel) == FALSE){
        IRCInterface_WriteSystemThread(NULL, notificacion);
      }else{
        IRCInterface_WriteChannel(channel, NULL, notificacion);
      }
      IRC_MFree(5, &prefix, &nick, &name, &visible, &topic);
      break;

    case RPL_LISTEND:
      syslog(LOG_INFO, "CLIENT: He recibido comando RPL_LISTEND del server");
      IRCInterface_PlaneRegisterInMessageThread(command);
      channel = IRCInterface_ActiveChannelName();
      if(IRCInterface_QueryChannelExist(channel) == FALSE){
        IRCInterface_WriteSystemThread(NULL,"End of LIST");
      }else{
        IRCInterface_WriteChannel(channel, NULL,"End of LIST");
      }
      break;

    case RPL_ENDOFNAMES:
      syslog(LOG_INFO, "CLIENT: He recibido comando RPL_ENDOFNAMES del server");
      IRCInterface_PlaneRegisterInMessageThread(command);
      break;

    case RPL_LUSEROP:
      syslog(LOG_INFO, "CLIENT: He recibido comando RPL_LUSEROP del server");
      IRCInterface_PlaneRegisterInMessageThread(command);
      IRCParse_RplLuserOp (command, &prefix, &nick, &nops, &msg);
      channel = IRCInterface_ActiveChannelName();
      sprintf(notificacion, "Nº de OPs conectados: %d", nops);
      if(IRCInterface_QueryChannelExist(channel) == FALSE){
        IRCInterface_WriteSystemThread(NULL,notificacion);
      }else{
        IRCInterface_WriteChannel(channel, NULL,notificacion);
      }
      IRC_MFree(3, &prefix, &nick, &msg);
      break;

    case RPL_MYINFO:
      syslog(LOG_INFO, "CLIENT: He recibido comando RPL_MYINFO del server");
      IRCInterface_PlaneRegisterInMessageThread(command);
      IRCParse_RplMyInfo (command, &prefix, &nick, &servername, &version, &availableusermodes, &availablechannelmodes, &addedg);
      IRC_MFree(7, &prefix, &nick, &servername, &version, &availableusermodes, &availablechannelmodes, &addedg);
      break;

    case RPL_WHOISOPERATOR:
      syslog(LOG_INFO, "CLIENT: He recibido comando RPL_WHOISOPERATOR del server");
      IRCInterface_PlaneRegisterInMessageThread(command);
      IRCParse_RplWhoIsOperator (command, &prefix, &nick, &nick2, &msg);
      channel = IRCInterface_ActiveChannelName();
      sprintf(notificacion, "%s: %s", nick2, msg);
      if(IRCInterface_QueryChannelExist(channel) == FALSE){
        IRCInterface_WriteSystemThread(NULL,notificacion);
      }else{
        IRCInterface_WriteChannel(channel, NULL,notificacion);
      }
      IRC_MFree(4, &prefix, &nick, &nick2, &msg);
      break;

    case RPL_WHOISCHANNELS:
      syslog(LOG_INFO, "CLIENT: He recibido comando RPL_WHOISCHANNELS del server");
      IRCInterface_PlaneRegisterInMessageThread(command);
      IRCParse_RplWhoIsChannels (command, &prefix, &nick, &nick2, &name);
      channel = IRCInterface_ActiveChannelName();
      sprintf(notificacion, "<%s> :{[@|+]<%s>}", nick2, name);
      if(IRCInterface_QueryChannelExist(channel) == FALSE){
        IRCInterface_WriteSystemThread(NULL,notificacion);
      }else{
        IRCInterface_WriteChannel(channel, NULL,notificacion);
      }
      IRC_MFree(4, &prefix, &nick, &nick2, &name);
      break;

    case RPL_YOURESERVICE:
      syslog(LOG_INFO, "CLIENT: He recibido comando RPL_YOURESERVICE del server");
      IRCInterface_PlaneRegisterInMessageThread(command);
      IRCParse_RplYoureService (command, &prefix, &nick, &msg, &servicename);
      IRC_MFree(4, &prefix, &nick, &msg, &servicename);
      break;

    case RPL_LUSERCHANNELS:
      syslog(LOG_INFO, "CLIENT: He recibido comando RPL_LUSERCHANNELS del server");
      IRCInterface_PlaneRegisterInMessageThread(command);
      IRCParse_RplLuserChannels (command, &prefix, &nick, &nchannels, &msg);
      channel = IRCInterface_ActiveChannelName();
      sprintf(notificacion, "Nº de canales: %d", nchannels);
      if(IRCInterface_QueryChannelExist(channel) == FALSE){
        IRCInterface_WriteSystemThread(NULL,notificacion);
      }else{
        IRCInterface_WriteChannel(channel, NULL,notificacion);
      }
      IRC_MFree(2, &prefix, &nick);
      break;

    case RPL_NAMREPLY:
      syslog(LOG_INFO, "CLIENT: He recibido comando RPL_NAMREPLY del server");
      IRCInterface_PlaneRegisterInMessageThread(command);
      IRCParse_RplNamReply (command, &prefix, &nick, &type, &name, &msg);
      IRC_MFree(5, &prefix, &nick, &type, &name, &msg);
      break;

    default:
      /*Comando no entendido por nuestro cliente*/
      break;
  }

}
