#include "../include/conexionesMemoria.h"
extern t_log* logger;
extern t_config* config;
typedef struct
{
    int fd_conexion_IO;
} t_datos_server_interfaces;

typedef struct main
{
    int fd;
}info_fd_hilos;

int iniciar_conexiones(int* server_fd,int* fd_cpu,int* fd_kernel)
{
    //FALTA IP, esta hardcodadeado
    char* puerto;
    //INICIA SERVIDOR
    puerto = config_get_string_value(config,"PUERTO_ESCUCHA");
    *server_fd = iniciar_servidor(logger,"0.0.0.0",puerto);
    log_info(logger,"Servidor listo para recibir cliente!");
    //ESPERAR CPU
    *fd_cpu = esperar_cliente(*server_fd,logger,"CPU");
    //ESPERAR KERNEL
    *fd_kernel = esperar_cliente(*server_fd,logger,"KERNEL");
    
    return *server_fd != 0 && *fd_cpu != 0 && *fd_kernel != 0;
}

void inicializar_hilos(int fd_cpu, int fd_kernel)
{
    //HILO PARA COMUNICACION CON CPU
    pthread_t hiloCPU;
    info_fd_hilos* info_fd_cpu = malloc(sizeof(info_fd_hilos));
    info_fd_cpu->fd = fd_cpu;
    pthread_create(&hiloCPU,NULL,(void*) conexionCPU,(void*) info_fd_cpu);
    pthread_detach(hiloCPU);
    //HILO PARA COMUNICACION CON KERNEL
    pthread_t hiloKernel;
    info_fd_hilos* info_fd_kernel = malloc(sizeof(info_fd_hilos));
    info_fd_kernel->fd = fd_kernel;
    pthread_create(&hiloKernel,NULL,(void*) conexionKernel,(void*) info_fd_kernel);
    pthread_detach(hiloKernel);
}

int escucharConexionesIO(int fd_escucha_interfaces){
    int fd_conexion_IO = esperar_cliente(fd_escucha_interfaces,logger,"INTERFAZ I/O");
    int err;
    pthread_t conexionesIO;
    t_datos_server_interfaces* datosServerInterfaces = malloc(sizeof(t_datos_server_interfaces));
    datosServerInterfaces->fd_conexion_IO = fd_conexion_IO;
    err = pthread_create(&conexionesIO,NULL,(void*) procesarConexionesIO,(void*) datosServerInterfaces);
    if(err != 0)
    {
        log_error(logger,"Error al crear el hilo de conexion IO");
        perror("\nError hilo");
        exit(1);
    }
    pthread_detach(conexionesIO);
    
    
    return 1;
}

void procesarConexionesIO(void* datosServerInterfaces){
    t_datos_server_interfaces* auxiliarDatosServer = (t_datos_server_interfaces*) datosServerInterfaces;
    int fd_conexion_IO = auxiliarDatosServer->fd_conexion_IO;
    free(auxiliarDatosServer);

    int codigoOperacion = recibir_operacion(fd_conexion_IO);

    if(codigoOperacion == -1)
    {
        log_error(logger,"Error al recibirOperacion");
        //return;
    }

    char* interfazConectada = recibir_mensaje(fd_conexion_IO,logger);
    enviar_handshake_ok(logger,fd_conexion_IO,interfazConectada);
    free(interfazConectada);
}

void conexionCPU(void* info_fd_cpu)
{
    info_fd_hilos* fd_recibido = info_fd_cpu;
    int fd_cpu = fd_recibido->fd;
    free(fd_recibido);

    //FILE* archivoPseudocodigo = fopen("codigoPrueba.txt","r+");
    char** instruccionesPrueba = pasarArchivoEstructura("codigoPrueba.txt");
    //fclose(archivoPseudocodigo);

    //string_array_destroy(instrucciones);

    int codigoOperacion;
    while(1)
    {
        codigoOperacion = recibir_operacion(fd_cpu);

        if(codigoOperacion == -1)
        {
            log_error(logger,"Error al recibirOperacion");
            return;
        }

		switch (codigoOperacion) {
		case HANDSHAKE:
            char* moduloConectado = recibir_mensaje(fd_cpu,logger);
			enviar_handshake_ok(logger,fd_cpu, moduloConectado);
            free(moduloConectado);
			break;
        case INSTRUCCION:
            char* valor_pc = recibir_mensaje(fd_cpu,logger);
            uint32_t pc_recibido = atoi(valor_pc);
            enviar_mensaje(instruccionesPrueba[pc_recibido],fd_cpu,INSTRUCCION);
            //enviar_mensaje("SET AX 3",fd_cpu,INSTRUCCION);
            free(valor_pc);
            break;
		case -1:
			log_error(logger, "el cliente se desconecto. Terminando servidor");
            return;
			//return EXIT_FAILURE;
		default:
			log_warning(logger,"Operacion desconocida. No quieras meter la pata");
			break;
		}
    }
}
void conexionKernel(void* info_fd_kernel)
{
    info_fd_hilos* fd_recibido = info_fd_kernel;
    int fd_kernel = fd_recibido->fd;
    free(fd_recibido);
   

    int codigoOperacion;
    while(1)
    {
        codigoOperacion = recibir_operacion(fd_kernel);

        if(codigoOperacion == -1)
        {
            log_error(logger,"Error al recibirOperacion");
            return;
        }

		switch (codigoOperacion) {
		case HANDSHAKE:
            char* moduloConectado = recibir_mensaje(fd_kernel,logger);
			enviar_handshake_ok(logger,fd_kernel, moduloConectado);
            free(moduloConectado);
			break;
        case INICIAR_PROCESO:
            /* RECIBE EL PID Y EL PATH AL PID (SUMARLO A PATH DEL CONFIG PARA HALLAR LA RUTA ABSOLUTA)
            int pid = recibe
            char* pathPseudocodigo = recibir_mensaje(fd_kernel,logger);
            //se recibe el path del kernel
            char* pathPseudocodigo = "codigoPrueba.txt";
            FILE* archivoPseudocodigo = fopen(pathPseudocodigo,"r+");

            instrucciones = pasarArchivoEstructura(archivoPseudocodigo);
            fclose(archivoPseudocodigo);
            string_array_destroy(instrucciones);
            */
            break;
		case -1:
			log_error(logger, "el cliente se desconecto. Terminando servidor");
            return;
			//return EXIT_FAILURE;
		default:
			log_warning(logger,"Operacion desconocida. No quieras meter la pata");
			break;
		}
    }
}

void terminar_programa(int* fd_cpu,int* fd_kernel){
    destruir_log_config(logger,config);
    close(*fd_cpu);
    close(*fd_kernel);
}

