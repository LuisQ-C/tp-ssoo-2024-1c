#include "../include/planificadorLP.h"
#include "../include/main.h"

extern t_log* logger;
extern t_log* logger_obligatorio;
extern t_config* config;

extern t_squeue *lista_procesos_new;
extern t_squeue *lista_procesos_ready;
extern t_squeue *lista_procesos_ready_plus;

extern sem_t grado_de_multiprogramacion;
extern sem_t proceso_en_cola_new;
extern sem_t proceso_en_cola_ready;

extern sem_t planificacion_new_iniciada;
extern sem_t planificacion_ready_iniciada;
extern sem_t planificacion_exec_iniciada;

extern bool planificacion_iniciada;

void atender_estados_new(){

    
    while(1){
        //int valor1, valor2;
        //sem_getvalue(&planificacion_new_iniciada,&valor2);
        //log_info(logger,"VALOR NEW INICIADA: %d",valor2);
        sem_wait(&proceso_en_cola_new);
        sem_wait(&grado_de_multiprogramacion);
        sem_wait(&planificacion_new_iniciada);
        //printf("PASE PLANI NEEW INICIADA");
        
        //printf("PASE PROCESO COLA NEW");
        /*
            printf("\nPLANIFICACION NEW INICIADA \n");
            int hola;
            sem_getvalue(&proceso_en_cola_new, &hola);
            printf("\n %d \n", hola);
            int hola2;
            sem_getvalue(&planificacion_new_iniciada, &hola2);
            printf("\n %d \n", hola2);*/

        /*
        if(squeue_is_empty(lista_procesos_new)){
            sem_post(&planificacion_new_iniciada);
            continue;
        }*/
        /*if(!planificacion_iniciada){
            break;
        }*/
        /*if(!planificacion_iniciada){
            printf("\nESTA VACIA ? \n");
            sem_post(&proceso_en_cola_new);
            continue;
        }*/
        
        if (squeue_is_empty(lista_procesos_new) == false){
            t_pcb* pcb_auxiliar = squeue_pop(lista_procesos_new);
            cambiar_a_ready(pcb_auxiliar);
            //sem_getvalue(&proceso_en_cola_ready,&valor1);
            //sem_getvalue(&planificacion_new_iniciada,&valor2);
            //printf("\nVALOR PROCESO READY: %d \n",valor1);
            //printf("\nVALOR PLANIFICACION NEW: %d \n",valor2);
            sem_post(&proceso_en_cola_ready);
            sem_post(&planificacion_new_iniciada);
            //Una vez que pasa los estados de new a ready
            log_info(logger_obligatorio, "PID: %d - Estado Anterior: NEW - Estado Actual: READY", pcb_auxiliar->pid);
        }
        else{
            sem_post(&grado_de_multiprogramacion);
            sem_post(&planificacion_new_iniciada);
        }
        
        

    }
}

void iniciar_PLP(){
    pthread_t hilo_plp; //hilo del planificador de largo plazo, para atender estados en new
    pthread_create(&hilo_plp, NULL, (void *) atender_estados_new, NULL);
    pthread_detach(hilo_plp);
}

void cambiar_a_ready(t_pcb* pcb){
    //uint32_t estado_anterior = pcb->estado;
    pcb->estado = READY;
    squeue_push(lista_procesos_ready, pcb);

    mostrar_cola_ready();
}


char* listar_pids(t_squeue* squeue){
    char* pids = string_new();    

    void obtener_string_pids(t_pcb* pcb){
        char* pid = string_itoa(pcb->pid);
        string_append_with_format(&pids, "%s, ", pid);
        free(pid);
    }

    //list_iterate(squeue->cola->elements, (void*) obtener_string_pids);

    squeue_iterate(squeue, (void*) obtener_string_pids);
    return pids;
}

void mostrar_cola_ready(){

    char* pids = string_new();
    char* lista_pids = listar_pids(lista_procesos_ready);
    string_n_append(&pids, lista_pids, string_length(lista_pids)-2);
    log_info(logger_obligatorio, "Cola ready: %s", pids);
    free(pids);
    free(lista_pids);

}

void cambiar_a_ready_plus(t_pcb* pcb){
    //uint32_t estado_anterior = pcb->estado;
    pcb->estado = READY;
    squeue_push(lista_procesos_ready_plus, pcb);

    mostrar_cola_ready_plus();
}

void mostrar_cola_ready_plus(){
    char* pids = string_new();
    char* lista_pids = listar_pids(lista_procesos_ready_plus);
    string_n_append(&pids, lista_pids, string_length(lista_pids)-2);
    log_info(logger_obligatorio, "Cola ready prioridad: %s", pids);
    free(pids);
    free(lista_pids);
}