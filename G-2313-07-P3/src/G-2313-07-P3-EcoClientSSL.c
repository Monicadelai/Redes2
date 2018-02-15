/**
* @brief Cliente ECO seguro gracias al uso de la librería SSL
* @file G-2313-07-P3-EcoClientSSL.c
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
  char strout[512]="";
  SSL_CTX *contexto = NULL;
  SSL* ssl = NULL;

  inicializar_nivel_SSL();

  contexto = fijar_contexto_SSL("./certs/ca.pem", "./certs/cliente.pem");
  if(contexto == NULL){
    fprintf(stderr, "[ERROR]: Inicializacion del contexto erronea\n");
    return EXIT_FAILURE;
  }

  ssl = conectar_canal_seguro_SSL(contexto, &socket, 6696, "localhost");
  if(ssl == NULL){
    fprintf(stderr, "[ERROR]: Conexion Canal Seguro Erronea\n");
    return EXIT_FAILURE;
  }

  if(evaluar_post_connectar_SSL(ssl) == FALSE){
    fprintf(stderr, "[ERROR]: Evaluacion Fallida\n");
    cerrar_canal_SSL(ssl, contexto, socket);
    return EXIT_FAILURE;
  }

  while (1){
    longitud = 512;

    fscanf(stdin,"%s", strout);
    syslog(LOG_INFO, "envioCLIENT: -%s-", strout);


    if(enviar_datos_SSL(ssl,strout) == FALSE){
      fprintf(stderr, "[ERROR]: Fallo al enviar los datos, cerrando el canal seguro.\n");
      cerrar_canal_SSL(ssl, contexto, socket);
      return EXIT_FAILURE;
    }

    if(strcmp(strout,"exit") == 0)
      break;

    if(recibir_datos_SSL(ssl, strout, longitud) == FALSE){
      fprintf(stderr, "[ERROR]: Conexion con el servidor cerrada\n");
      cerrar_canal_SSL(ssl, contexto, socket);
      return EXIT_FAILURE;
    }
    syslog(LOG_INFO, "reciboCLIENT: -%s-", strout);
    printf("%s\n", strout);
  }

  printf("Conexion cerrada\n");
  cerrar_canal_SSL(ssl, contexto, socket);
  return EXIT_SUCCESS;

}
