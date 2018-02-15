/**
* @brief Servidor ECO seguro gracias al uso de la librería SSL
* @file G-2313-07-P3-EcoServerSSL.c
*
* @authors Alfonso Bonilla Trueba (alfonso.bonilla@estudiante.uam.es)
* @authors Mónica de la Iglesia Martínez (monica.delaiglesia@estudiante.uam.es)
* @version 1.0
* @date 14-04-2017
*/

#include "../includes/G-2313-07-P3-ConnectionSSL.h"
#include <stdio.h>
#include <stdlib.h>

int main()
{
  int socket, longitud;
  SSL_CTX *contexto = NULL;
  SSL* ssl = NULL;
  struct sockaddr datos_cliente;
  char strin[512]="";

  inicializar_nivel_SSL();

  contexto = fijar_contexto_SSL("./certs/ca.pem", "./certs/servidor.pem");
  if(contexto == NULL){
    fprintf(stderr, "[ERROR]: Inicializacion del contexto erronea\n");
    return EXIT_FAILURE;
  }

  ssl = aceptar_canal_seguro_SSL(contexto, &socket, &datos_cliente, 6696);
  if(ssl == NULL){
    fprintf(stderr, "[ERROR]: Fallo al aceptar conexiones seguras\n");
    return EXIT_FAILURE;
  }

  if(evaluar_post_connectar_SSL(ssl) == FALSE){
    fprintf(stderr, "[ERROR]: Fallo en la verificacion de nuevas conexiones\n");
    cerrar_canal_SSL(ssl, contexto, socket);
    return EXIT_FAILURE;
  }


  while(1){
    longitud=512;
    bzero(strin, sizeof(char)*512);

    if(recibir_datos_SSL(ssl, strin, longitud) == FALSE){
      return EXIT_FAILURE;
    }

    if(strcmp(strin,"exit") == 0)
      break;


    printf("%s\n", strin);
    syslog(LOG_INFO, "reciboSERVER: -%s-", strin);

    if(enviar_datos_SSL(ssl, strin) == FALSE){
      cerrar_canal_SSL(ssl, contexto, socket);
      return EXIT_FAILURE;
    }

    printf("%s\n", strin);
    syslog(LOG_INFO, "envioSERVER: -%s-", strin);

  }

  fprintf(stdout,"La conexión ha sido cerrada\n");
  cerrar_canal_SSL(ssl, contexto, socket);
  return EXIT_SUCCESS;
}
