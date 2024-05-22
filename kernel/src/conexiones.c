#include "../include/conexiones.h"

extern t_log* logger;
extern sem_t planificacion_blocked_iniciada;
extern sem_t proceso_en_cola_ready;

extern t_squeue *lista_procesos_ready;
typedef struct
{
    int fd_conexion_IO;
    t_log* logger;
} t_datos_server_interfaces;

int iniciar_conexiones(t_config* config,t_log* logger,int* fd_memoria,int* fd_cpu_dispatch, int* fd_cpu_interrupt,int* fd_escucha_interfaces)
{
    char* ip;
    char* puerto;
    //CONECTARSE A MEMORIA
    ip = config_get_string_value(config,"IP_MEMORIA");
    puerto = config_get_string_value(config,"PUERTO_MEMORIA");
    *fd_memoria = crear_conexion(ip,puerto,logger,"MEMORIA");
    //CONECTARSE A CPU A TRAVES DE DISPATCH E INTERRUPT
    ip = config_get_string_value(config,"IP_CPU");
    puerto = config_get_string_value(config,"PUERTO_CPU_DISPATCH");
    *fd_cpu_dispatch = crear_conexion(ip,puerto,logger,"CPU-DISPATCH");
    puerto = config_get_string_value(config,"PUERTO_CPU_INTERRUPT");
    *fd_cpu_interrupt = crear_conexion(ip,puerto,logger,"CPU-INTERRUPT");
    //LEVANTAR SERVER PARA I/O CON UN HILO
    ip = config_get_string_value(config,"IP_KERNEL");
    puerto = config_get_string_value(config,"PUERTO_ESCUCHA");
    *fd_escucha_interfaces = iniciar_servidor(logger,ip,puerto);
    return *fd_memoria != 0 && *fd_cpu_dispatch != 0 && *fd_cpu_interrupt != 0 && *fd_escucha_interfaces != 0;
}

void realizar_handshakes_kernel(int fd_memoria,int fd_cpu_dispatch, int fd_cpu_interrupt)
{
    mandarHandshake(logger,fd_memoria,"MODULO MEMORIA","MODULO KERNEL");
    mandarHandshake(logger,fd_cpu_dispatch,"MODULO CPU DISPATCH","MODULO KERNEL-DISPATCH");
    mandarHandshake(logger,fd_cpu_interrupt,"MODULO CPU INTERRUPT","MODULO KERNEL-INTERRUPT");
}

int escucharConexionesIO(t_log* logger,int fd_escucha_interfaces){
    int err;
    int fd_conexion_IO = esperar_cliente(fd_escucha_interfaces,logger,"INTERFAZ I/O");
    pthread_t conexionesIO;
    t_datos_server_interfaces* datosServerInterfaces = malloc(sizeof(t_datos_server_interfaces));
    datosServerInterfaces->fd_conexion_IO= fd_conexion_IO;
    datosServerInterfaces->logger = logger;
    err = pthread_create(&conexionesIO,NULL,(void*) procesarConexionesIO,(void*) datosServerInterfaces);
    if(err != 0)
    {
        log_error(logger,"Error al crear el hilo de conexion IO");
        perror("\nError creando hilo IO");
        exit(1);
    }
    pthread_detach(conexionesIO);
    
    
    return 1;
}
/***************************

          HILOS IOS

******************************/
void procesarConexionesIO(void* datosServerInterfaces){
    t_datos_server_interfaces* auxiliarDatosServer = (t_datos_server_interfaces*) datosServerInterfaces;
    int fd_conexion_IO = auxiliarDatosServer->fd_conexion_IO;
    t_log* logger = auxiliarDatosServer->logger;
    free(auxiliarDatosServer);

    recibir_operacion(fd_conexion_IO);
    char* interfaz_conectada = recibir_mensaje(fd_conexion_IO,logger);
    enviar_handshake_ok(logger,fd_conexion_IO,interfaz_conectada);

    //int codigo_operacion;

    

    char** tipo_nombre_io = string_split(interfaz_conectada,"-");
    int tipo = string_to_type(tipo_nombre_io[0]);

    if (tipo==-1)
    {
        log_warning(logger,"Tipo Invalido, finalizando hilo");
        return;
    }
    
    

    t_list_io* interfaz_agregada = agregar_interfaz_lista(tipo_nombre_io[1],tipo,fd_conexion_IO);

    string_array_destroy(tipo_nombre_io);

    free(interfaz_conectada);


    /*******************
    DECIDIMOS A QUE FUNCION IR DEPENDIENDO DEL TIPO DE INTERFAZ
    **************/
    switch(tipo)
    {
        case IO_GEN_SLEEP:
            atender_interfaz_generica(interfaz_agregada);
            break;
        case IO_STDIN_READ:
            break;
        case IO_STDOUT_WRITE:
            break;
        case IO_FS:
            break;
    }

    
}

int string_to_type(char* tipo)
{
    if(!strcmp("GENERICA",tipo))
    {
        return IO_GEN_SLEEP;
        
    }
    else if(!strcmp("STDIN",tipo))
    {
        return IO_STDIN_READ;
    }
    else if(!strcmp("STDOUT",tipo))
    {
        return IO_STDOUT_WRITE;
    }
    else if(!strcmp("DIALFS",tipo))
    {
        return IO_FS;
    }
    else
    {
        return -1;
    }
}

void atender_interfaz_generica(t_list_io* interfaz)
{
    while(1)
    {
        /*
        int conexion;
        conexion = recv(fd_conexion_IO,&conexion,sizeof(int),MSG_PEEK);
        
        if(!conexion)
        {
            log_error(logger,"SE DESCONECTO LA INTERFAZ: %s", interfaz_agregada->nombre_interfaz);
            break;
        }*/
       

        sem_wait(interfaz->hay_proceso_cola);
        
        sem_wait(&planificacion_blocked_iniciada);          //no podrian trabajar 2 colas al mismo tiempo
        t_elemento_iogenerica* solicitud_io = pop_elemento_cola_io(interfaz);
        
        int respuesta;
        int tiempo_dormicion = solicitud_io->tiempo;

        send(interfaz->fd_interfaz,&tiempo_dormicion,sizeof(int),0); //mandarle el pid y el tiempo a la interfaz

        recv(interfaz->fd_interfaz,&respuesta,sizeof(int),MSG_WAITALL);

        if(respuesta==INTERFAZ_LISTA)
        {
            printf("\nTERMINO LA SOLICITUD CORRECTAMENTE\n");
        }
        
        cambiar_a_ready(solicitud_io->pcb);

        sem_post(&proceso_en_cola_ready);

        log_info(logger, "PID: %d - Estado Anterior: BLOCKED - Estado Actual: READY", solicitud_io->pcb->pid);

        free(solicitud_io);

        sem_post(&planificacion_blocked_iniciada);
        
    }
    //DESTRUIRSE A SI MISMO, SACARSE DE LA LISTA
}



void terminar_programa(t_log* logger,t_config* config,int* fd_memoria,int* fd_cpu_dispatch,int* fd_cpu_interrupt)
{
    destruir_log_config(logger,config);
    close(*fd_memoria);
    close(*fd_cpu_dispatch);
    close(*fd_cpu_interrupt);
}
