#include "../include/archivos.h"

extern t_list* instrucciones_procesos;


/*Pasa todo el contenido del archivo a un char**, se debe destruirlo luego de utilizar la funcion*/
char** pasarArchivoEstructura(char* path)
{   
    FILE* f = fopen(path,"r+");
    char** archivoInstrucciones = string_array_new();
    
    char* instruccionLeida = string_new();
    size_t longitud = 1;
    while(getline(&instruccionLeida, &longitud,f) != -1)
    {
        string_array_push(&archivoInstrucciones,instruccionLeida);
        instruccionLeida = string_new();
        longitud = 1;
    }
    free(instruccionLeida);
    fclose(f);
    return archivoInstrucciones;
}

/* Añade el pid  y las intrucciones del proceso a la lista instrucciones_procesos*/
void agregar_proceso_lista(int pid,char* path)
{
    t_proceso* proceso_creado = malloc(sizeof(t_proceso));
    proceso_creado->pid = pid;
    proceso_creado->instrucciones = pasarArchivoEstructura(path);
    list_add(instrucciones_procesos,proceso_creado);
}

/* Libera las estructuras asociadas al proceso */
void destruir_proceso_lista(t_proceso* proceso_a_destruir)
{
    string_array_destroy(proceso_a_destruir->instrucciones);
    free(proceso_a_destruir);
}

/*
void quitar_proceso_lista(int pid)
{
    t_proceso* proceso = malloc(sizeof(t_proceso));
    proceso->pid = pid;
    list_find(instrucciones_procesos,buscar_proceso_pid(proceso));
    list_remove_and_destroy_element(instrucciones_procesos,proceso->pid,destruir_proceso_lista);
}

bool buscar_proceso_pid(t_proceso* proceso)
{
    if(proceso-> pid == *pid)
    {
        return true;
    }
    return false;
}*/