#include "../include/iniciar_planificador.h"
#include "../include/main.h"

extern t_config* config;
t_squeue *lista_procesos_new;
t_squeue *lista_procesos_ready;
t_squeue *lista_procesos_ready_plus;
t_squeue *lista_procesos_exec;
t_squeue *lista_procesos_exit;
t_slist *lista_procesos_blocked;
t_slist *lista_recursos_blocked;
t_sdictionary *instancias_utilizadas;

sem_t grado_de_multiprogramacion;
sem_t proceso_en_cola_new;
sem_t proceso_en_cola_ready;
sem_t ejecutar_proceso;
sem_t pasar_a_ejecucion;

sem_t planificacion_new_iniciada;
sem_t planificacion_ready_iniciada;
sem_t planificacion_exec_iniciada;
sem_t planificacion_blocked_iniciada;
sem_t hay_una_peticion_de_proceso;
sem_t planificacion_detenida;

pthread_mutex_t mutex_plani_iniciada;

bool planificacion_iniciada = false;
int multiprog;

//Hola, soy un comentario

void iniciar_cosas_necesarias_planificador(){

    multiprog = config_get_int_value(config, "GRADO_MULTIPROGRAMACION");
    //printf("multiprog, %d", multiprog);
    lista_recursos_blocked = slist_create();
    iniciar_recursos();
    lista_procesos_new = squeue_create();
    lista_procesos_ready = squeue_create();
    lista_procesos_ready_plus = squeue_create();
    lista_procesos_exec = squeue_create();
    lista_procesos_exit = squeue_create();
    lista_procesos_blocked = slist_create();
    instancias_utilizadas = sdictionary_create();
    pthread_mutex_init(&mutex_plani_iniciada, NULL);
    sem_init(&pasar_a_ejecucion,0,1);
    sem_init(&proceso_en_cola_new, 0, 0);
    sem_init(&proceso_en_cola_ready, 0, 0);
    sem_init(&ejecutar_proceso, 0, 0);
    sem_init(&planificacion_new_iniciada, 0, 0);
    sem_init(&planificacion_ready_iniciada, 0, 0);
    sem_init(&planificacion_exec_iniciada, 0, 0);
    sem_init(&planificacion_blocked_iniciada,0,0);
    sem_init(&grado_de_multiprogramacion, 0, multiprog);
    sem_init(&hay_una_peticion_de_proceso, 0, 0);
    sem_init(&planificacion_detenida, 0, 0);
    iniciar_PLP();
    iniciar_PCP();

    //squeue_destroy(lista_procesos_new);
    //squeue_destroy(lista_procesos_ready);
    //sem_destroy(&un_semaforo); //remover luego

}

