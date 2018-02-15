/**
* @brief Servidor IRC demonizado con seguirdad SSL
* @file G-2313-07-P3-ServerIRC.c
*
* @authors Alfonso Bonilla Trueba (alfonso.bonilla@estudiante.uam.es)
* @authors Mónica de la Iglesia Martínez (monica.delaiglesia@estudiante.uam.es)
* @version 2.0
* @date 20-02-2017
*/


#include <stdio.h>
#include <stdlib.h>
#include <redes2/irc.h>

#include "../includes/G-2313-07-P3-server.h"
void *IRC_New_Client_SSL(void* valor);
extern SSL* ssl;
extern long ssl_flag;

int main(int argc, char *argv[]){
	int socket = 0;
	int port;
	int desc;
	pthread_t hilo;

	SSL_CTX *contexto = NULL;

  struct sockaddr datos_cliente;

	if(argc >= 3){
		if( strcmp(argv[1], "--port") == 0){
			port = atoi(argv[2]);
		}else{
			fprintf(stderr, "Para ejecutar con argumentos %s --port <numero puerto> --ssl\n",argv[0]);
			return EXIT_FAILURE;
		}
		if(argc == 4){
			if(strcmp(argv[3], "--ssl") == 0){
				ssl_flag = TRUE;
			}else{
				fprintf(stderr, "Para ejecutar con argumentos %s --port <numero puerto> --ssl\n",argv[0]);
				return EXIT_FAILURE;
			}
		}
		if(argc > 4){
			fprintf(stderr, "Para ejecutar con argumentos %s --port <numero puerto> --ssl\n",argv[0]);
			return EXIT_FAILURE;
		}
	}else{
		if(argc == 2){
			if(strcmp(argv[1], "--ssl") == 0){
				ssl_flag = TRUE;
			}else{
				fprintf(stderr, "Para ejecutar con argumentos %s --port <numero puerto> --ssl\n",argv[0]);
				return EXIT_FAILURE;
			}
		}

		if(argc > 2){
			fprintf(stderr, "Para ejecutar con argumentos %s --port <numero puerto> --ssl\n",argv[0]);
			return EXIT_FAILURE;
		}else{
			port = NFC_SERVER_PORT;
		}
	}

	/*printf("Puerto %d\n", port);
	if(ssl_flag == TRUE)
		printf("Con conexion SSL\n");*/

	daemonizar();


	if(ssl_flag == FALSE){
		socket = IRC_Initiate_Server(port);
		IRC_Accept_Connection(socket);
	}else{

		inicializar_nivel_SSL();

	  contexto = fijar_contexto_SSL("./certs/ca.pem", "./certs/servidor.pem");
	  if(contexto == NULL){
	    fprintf(stderr, "[ERROR]: Inicializacion del contexto erronea\n");
	    return EXIT_FAILURE;
	  }

		syslog(LOG_INFO, "SERVER SSL : Contexto OK");

		while(1){
			syslog(LOG_INFO, "SERVER SSL : accept del normal");
			/*desc = accept(socket, &Conexion, &len);*/

			syslog(LOG_INFO, "SERVER SSL : accept canal seguro");
			ssl = aceptar_canal_seguro_SSL(contexto, &desc, &datos_cliente, port);
		  if(ssl == NULL){
		    fprintf(stderr, "[ERROR]: Fallo al aceptar conexiones seguras\n");
		    return EXIT_FAILURE;
		  }

			syslog(LOG_INFO, "SERVER SSL : evaluar conexion ssl");
		  if(evaluar_post_connectar_SSL(ssl) == FALSE){
		    fprintf(stderr, "[ERROR]: Fallo en la verificacion de nuevas conexiones\n");
		    cerrar_canal_SSL(ssl, contexto, socket);
		    return EXIT_FAILURE;
		  }

			syslog(LOG_INFO, "SERVER SSL : creo el hilo");
			pthread_create(&hilo, NULL, IRC_New_Client_SSL, (void*) &desc);
		}

	}


	return EXIT_SUCCESS;

}

void *IRC_New_Client_SSL(void* valor)
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

		if(recibir_datos_SSL(ssl, mensaje, MAX_BUFFER) == FALSE){
      pthread_exit(NULL);
    }

		syslog(LOG_INFO,"RECIBIDO: %s", mensaje);

		if(mensaje[0] == '\0'){
			IRCTAD_Quit (nick);
			close(connval);
			free(nick);
			free(prefix_user);
			pthread_exit(NULL);
		}

		str = IRC_UnPipelineCommands(mensaje, &command);
		syslog(LOG_INFO, "comando %s", command);
		IRC_Server_Parser(command, connval, &nick, &prefix_user);
		free(command);
		while (str != NULL){
			str = IRC_UnPipelineCommands(str, &command);
			syslog(LOG_INFO, "comando %s", command);
			IRC_Server_Parser(command, connval, &nick, &prefix_user);
			free(command);
		}
	}
}
