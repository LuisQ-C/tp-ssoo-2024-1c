#ifndef INTERFAZ_GEN_H_
#define INTERFAZ_GEN_H_

#include<stdio.h>
#include<stdlib.h>
#include "../../utils/include/sockets.h"
#include "../../utils/include/logsConfigs.h"
#include "../../utils/include/protocolo.h"


void interfazGenerica(t_config* config, int fd_conexion_kernel);

#endif