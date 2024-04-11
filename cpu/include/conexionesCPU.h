#ifndef CPU_CONEXIONESCPU_H_
#define CPU_CONEXIONESCPU_H_

#include <stdio.h>
#include <stdlib.h>
#include "../../utils/include/sockets.h"
#include "../../utils/include/logsConfigs.h"
#include "../../utils/include/protocolo.h"
#include <pthread.h>
#include<commons/string.h>

void iniciar_conexiones(t_log* logger,t_config* config,int* fd_conexion_memoria,int* server_fd_escucha_dispatch, int* server_fd_escucha_interrupt, int* cliente_fd_conexion_dispatch, int* cliente_fd_conexion_interrupt);
void terminar_programa(t_log* logger, t_config* config, int* fd_conexion_memoria,int* cliente_fd_conexion_dispatch,int*cliente_fd_conexion_interrupt);
//void* atender_cliente(void* args);

#endif 