#include "../include/consola.h"

extern t_log* logger;
extern t_config* config;

extern t_squeue *lista_procesos_new;
extern t_squeue *lista_procesos_ready;
extern t_squeue *lista_procesos_exec;
extern t_squeue *lista_procesos_exit;

extern sem_t grado_de_multiprogramacion;
extern sem_t proceso_en_cola_new;
extern sem_t proceso_en_cola_ready;
extern sem_t pasar_a_ejecucion;

extern sem_t planificacion_new_iniciada;
extern sem_t planificacion_ready_iniciada;
extern sem_t planificacion_exec_iniciada;

extern bool planificacion_iniciada;

char* opciones[] = {
    "EJECUTAR_SCRIPT",
    "INICIAR_PROCESO",
    "FINALIZAR_PROCESO",
    "DETENER_PLANIFICACION",
    "INICIAR_PLANIFICACION",
    "MULTIPROGRAMACION",
    "PROCESO_ESTADO",
    NULL
};
//Por parte de la documentación compartida en UTNSO
char** custom_completion(const char* text, int start, int end){
    char** matches = NULL;

    if (start == 0) {
        matches = rl_completion_matches(text, &custom_completion_generator);
    } else {
        rl_attempted_completion_over = 1;
    }

    return matches;
}

char* custom_completion_generator(const char* text, int state){
    static int list_index, len;
    const char* name;

    if (!state) {
        list_index = 0;
        len = strlen(text);
    }

    while ((name = opciones[list_index])) {
        list_index++;

        if (strncasecmp(name, text, len) == 0) {
            return strdup(name);
        }
    }
    return NULL;
}

pthread_mutex_t hilo_pid_mutex;
int pid_contador = 0;

int fd_dispatch;
int fd_interrupt;
int fd_mem;
int fd_IO;
//asdasd
void iniciar_consola(void* fd_info){
    //t_log* logger;
    //logger = iniciar_logger("kernel.log","Kernel",1,LOG_LEVEL_INFO);
    info_fd* auxiliar = fd_info;
    //les asigno un valor a las variables globales
    fd_dispatch = auxiliar->fd_cpu_dispatch;
    fd_interrupt = auxiliar->fd_cpu_interrupt;
    fd_mem = auxiliar->fd_memoria;
    fd_IO = auxiliar->fd_escucha_interfaces;

    rl_attempted_completion_function = custom_completion;

    char* leido;
    printf("EJECUTAR_SCRIPT [PATH] \n");
    printf("INICIAR_PROCESO [PATH] \n");
    printf("FINALIZAR_PROCESO [PID] \n");
    printf("DETENER_PLANIFICACION \n");
    printf("INICIAR_PLANIFICACION \n");
    printf("MULTIPROGRAMACION [VALOR]\n");
    printf("PROCESO_ESTADO \n");

    leido = readline("> ");
    bool validarComando;
    while(!string_is_empty(leido)){
        validarComando = validar_instrucciones_leidas(leido);
        if(!validarComando){
            log_error(logger, "Comando no reconocido");
            free(leido);
            leido = readline("> ");
            continue;
        }
        add_history(leido);
        instrucciones_consola(leido);
        free(leido);
        leido = readline("> ");
    }
    free(leido);
}

bool validar_instrucciones_leidas(char* leido){
    char** instruccion_leida = string_split(leido, " ");
    bool valido;
    //log_info(logger, "%s y %s y %s", instruccion_leida[0], instruccion_leida[1], instruccion_leida[2]);

    if(strcmp(instruccion_leida[0], "EJECUTAR_SCRIPT") == 0 && instruccion_leida[1] != NULL && strcmp(instruccion_leida[1], "") != 0)
        valido = true;
    else if (strcmp(instruccion_leida[0], "INICIAR_PROCESO") == 0 && instruccion_leida[1] != NULL && strcmp(instruccion_leida[1], "") != 0)
        valido = true;
    else if (strcmp(instruccion_leida[0], "FINALIZAR_PROCESO") == 0 && instruccion_leida[1] != NULL && strcmp(instruccion_leida[1], "") != 0)
        valido = true;
    else if (strcmp(instruccion_leida[0], "DETENER_PLANIFICACION") == 0)
        valido = true;
    else if (strcmp(instruccion_leida[0], "INICIAR_PLANIFICACION") == 0)
        valido = true;
    else if (strcmp(instruccion_leida[0], "MULTIPROGRAMACION") == 0 && instruccion_leida[1] != NULL && strcmp(instruccion_leida[1], "") != 0)
        valido = true;
    else if (strcmp(instruccion_leida[0], "PROCESO_ESTADO") == 0)
        valido = true;
    else 
        valido = false;

    string_array_destroy(instruccion_leida);

    return valido;
}

void instrucciones_consola(char* leido){
    char** instruccion_leida = string_split(leido, " ");

    if(strcmp(instruccion_leida[0], "EJECUTAR_SCRIPT") == 0)
        ejecutar_script(instruccion_leida[1]);
    else if (strcmp(instruccion_leida[0], "INICIAR_PROCESO") == 0)
        iniciar_proceso(instruccion_leida[1]);
    else if (strcmp(instruccion_leida[0], "FINALIZAR_PROCESO") == 0)
        finalizar_proceso(atoi(instruccion_leida[1]));
    else if (strcmp(instruccion_leida[0], "DETENER_PLANIFICACION") == 0)
        detener_planificacion();
    else if (strcmp(instruccion_leida[0], "INICIAR_PLANIFICACION") == 0)
        iniciar_planificacion();
    else if (strcmp(instruccion_leida[0], "MULTIPROGRAMACION") == 0)
        multiprogramacion(atoi(instruccion_leida[1]));
    else if (strcmp(instruccion_leida[0], "PROCESO_ESTADO") == 0)
        proceso_estado();

    string_array_destroy(instruccion_leida);
}

void ejecutar_script(char* path){
    printf("ejecutar_script \n");
}

void iniciar_proceso(char* path){
    //printf("iniciar_proceso \n");
    log_debug(logger,"PATH A MANDAR: %s",path);
    t_pcb *nuevo_pcb = crear_pcb();
    squeue_push(lista_procesos_new, nuevo_pcb);
    log_info(logger, "Se crea el proceso %d en NEW", nuevo_pcb->pid);
    
    //Le envio las instrucciones a memoria y espero respuesta
    enviar_nuevo_proceso(&nuevo_pcb->pid, path, fd_mem);
    //enviar_pcb(nuevo_pcb, fd_dispatch); esto es para enviar el pcb a cpu
    int ok;
    recv(fd_mem, &ok,sizeof(int), MSG_WAITALL);
    log_info(logger, "recibi esto: %d", ok);
    
    sem_post(&proceso_en_cola_new);

    //free(nuevo_pcb);
}

void finalizar_proceso(int pid){
    printf("finalizar_proceso \n");
}

//sem_post(&proceso_en_cola_new);
        //sem_post(&proceso_en_cola_ready);
        //sem_post(&pasar_a_ejecucion);
        
void detener_planificacion(){
    //printf("detener_planificador \n");
    if(planificacion_iniciada)
    {
        planificacion_iniciada = false;
        
        pthread_t detener_new, detener_ready, detener_exec;
        pthread_create(&detener_new,NULL,(void*) detener_cola_new,NULL);
        pthread_create(&detener_ready,NULL,(void*) detener_cola_ready,NULL);
        pthread_create(&detener_exec,NULL,(void*) detener_cola_exec,NULL);
        pthread_detach(detener_new);
        pthread_detach(detener_ready);
        pthread_detach(detener_exec);
        
        
        log_info(logger, "Se detuvo la planificacion");
    }else
    {
        log_info(logger,"la plani esta pausada ya");
    }
    
}
void detener_cola_new(void* arg)
{
    sem_wait(&planificacion_new_iniciada);
}
void detener_cola_ready(void* arg)
{
    sem_wait(&planificacion_ready_iniciada);
}
void detener_cola_exec(void* arg)
{
    sem_wait(&planificacion_exec_iniciada);
}

void iniciar_planificacion(){
    if(!planificacion_iniciada){
    //printf("iniciar_planificacion \n");
    planificacion_iniciada = true;
    
    sem_post(&planificacion_new_iniciada);
    sem_post(&planificacion_ready_iniciada);
    sem_post(&planificacion_exec_iniciada);
    log_info(logger, "Planificación iniciada");
    }
}

void multiprogramacion(int valor){
    printf("multiprogramacion \n");
}

void proceso_estado(){
    //printf("proceso_estado \n");
    if(!squeue_is_empty(lista_procesos_new))
    {
        char* pids_listar = listar_pids(lista_procesos_new);
        log_info(logger, "Procesos cola new: %s", pids_listar);
        free(pids_listar);
    }
    else
        log_info(logger, "La cola new esta vacia");


    if(!squeue_is_empty(lista_procesos_ready))
    {
        char* pids_listar = listar_pids(lista_procesos_ready);
        log_info(logger, "Procesos cola ready: %s", pids_listar);
        free(pids_listar);
    }
    else 
        log_info(logger, "La cola ready esta vacia");

    if(!squeue_is_empty(lista_procesos_exec))
    {
        char* pids_listar = listar_pids(lista_procesos_exec);
        log_info(logger, "Procesos cola exec: %s", pids_listar);
        free(pids_listar);
    }
    else
        log_info(logger, "La cola exec esta vacia");

    if(!squeue_is_empty(lista_procesos_exit))
    {
        char* pids_listar = listar_pids(lista_procesos_exit);
        log_info(logger, "Procesos cola exit: %s", pids_listar);
        free(pids_listar);
    }
    else
        log_info(logger, "La cola exit esta vacia");

    //FALTA IMPRIMIR BLOCKED, TODAS SUS COLAS

    
}




//////Funciones procesos
t_pcb* crear_pcb(){
    t_pcb* pcb_creado = malloc(sizeof(t_pcb));

    pcb_creado->pid = asignar_pid();
    pcb_creado->pc = 0;
    pcb_creado->quantum = config_get_int_value(config, "QUANTUM");
    pcb_creado->estado = NEW;
    pcb_creado->registros_CPU = iniciar_registros_vacios();
    return pcb_creado;
}

t_registros_generales iniciar_registros_vacios(){
    t_registros_generales registro_auxiliar;

    registro_auxiliar.AX = 0;
    registro_auxiliar.BX = 0;
    registro_auxiliar.CX = 0;
    registro_auxiliar.DX = 0;
    registro_auxiliar.EAX = 0;
    registro_auxiliar.EBX = 0;
    registro_auxiliar.ECX = 0;
    registro_auxiliar.EDX = 0;

    return registro_auxiliar;
}

int asignar_pid(){
    int pid_a_asignar;
    pthread_mutex_lock(&hilo_pid_mutex);
    pid_a_asignar = pid_contador++;
    pthread_mutex_unlock(&hilo_pid_mutex);
    return pid_a_asignar;
}