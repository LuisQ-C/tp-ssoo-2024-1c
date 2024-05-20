#include "../include/instrucciones.h"

extern t_registro_cpu registro;
extern t_log* logger;
extern t_config* config;

// INSTRUCCIONES 
// SET
/* Setea el valor en el registro*/
void set_8(uint8_t* reg,uint8_t valor)
{
    *reg = valor;
}

/* Setea el valor en el registro*/
void set_32(uint32_t* reg,uint32_t valor)
{
    *reg = valor;
}

/* Ejecuta instruccion SET */
void set(char** instruccion)
{
    if(instruccion[1][0] != 'E')
    {
        uint8_t* registroRecibido = string_to_register8(instruccion[1]);
        set_8(registroRecibido,atoi(instruccion[2]));
    }
    else{
        uint32_t* registroRecibido = string_to_register32(instruccion[1]);
        set_32(registroRecibido,atoi(instruccion[2]));
    }
}

// SUM
/* Suma los valores de los registros pasados, guardando el resultado en el primero */
void sum_8_8(uint8_t* registroDestino,uint8_t* registroOrigen)
{
    //uint8_t numero_a_sumar = *registroOrigen;
    *registroDestino += *registroOrigen;
}

/* Suma los valores de los registros pasados, guardando el resultado en el primero */
void sum_8_32(uint8_t* registroDestino,uint32_t* registroOrigen)
{
    *registroDestino += *registroOrigen;
}

/* Suma los valores de los registros pasados, guardando el resultado en el primero */
void sum_32_32(uint32_t* registroDestino,uint32_t* registroOrigen)
{
    *registroDestino += *registroOrigen;
}

/* Suma los valores de los registros pasados, guardando el resultado en el primero */
void sum_32_8(uint32_t* registroDestino,uint8_t* registroOrigen)
{
    *registroDestino += *registroOrigen;
}

/* Ejecuta instruccion SUM */
void sum(char** instruccion)
{
    if(instruccion[1][0] != 'E')
    {
        if(instruccion[2][0] != 'E')
        {
            uint8_t* registroDestino = string_to_register8(instruccion[1]);
            uint8_t* registroOrigen = string_to_register8(instruccion[2]);
            sum_8_8(registroDestino,registroOrigen);
        }
        else{
            uint8_t* registroDestino = string_to_register8(instruccion[1]);
            uint32_t* registroOrigen = string_to_register32(instruccion[2]);
            sum_8_32(registroDestino,registroOrigen);
        }
    }
    else{
        if(instruccion[2][0] != 'E')
        {
            uint32_t* registroDestino = string_to_register32(instruccion[1]);
            uint8_t* registroOrigen = string_to_register8(instruccion[2]);
            sum_32_8(registroDestino,registroOrigen);
        }
        else{
            uint32_t* registroDestino = string_to_register32(instruccion[1]);
            uint32_t* registroOrigen = string_to_register32(instruccion[2]);
            sum_32_32(registroDestino,registroOrigen);
            }
    }

}

// SUB
/* Resta los valores de los registros pasados, guardando el resultado en el primero */
void sub_8_8(uint8_t* registroDestino,uint8_t* registroOrigen) 
{
    *registroDestino -= *registroOrigen;
}

/* Resta los valores de los registros pasados, guardando el resultado en el primero */
void sub_8_32(uint8_t* registroDestino,uint32_t* registroOrigen) 
{
    *registroDestino -= *registroOrigen;
}

/* Resta los valores de los registros pasados, guardando el resultado en el primero */
void sub_32_32(uint32_t* registroDestino,uint32_t* registroOrigen)
{
    *registroDestino -= *registroOrigen;
}

/* Resta los valores de los registros pasados, guardando el resultado en el primero */
void sub_32_8(uint32_t* registroDestino,uint8_t* registroOrigen)
{
    *registroDestino -= *registroOrigen;
}

/* Ejecuta instruccion SUB */
void sub(char** instruccion)
{
    if(instruccion[1][0] != 'E')
    {
        if(instruccion[2][0] != 'E')
        {
            uint8_t* registroDestino = string_to_register8(instruccion[1]);
            uint8_t* registroOrigen = string_to_register8(instruccion[2]);
            sub_8_8(registroDestino,registroOrigen);
        }
        else{
            uint8_t* registroDestino = string_to_register8(instruccion[1]);
            uint32_t* registroOrigen = string_to_register32(instruccion[2]);
            sub_8_32(registroDestino,registroOrigen);
        }
    }
    else{
        if(instruccion[2][0] != 'E')
        {
            uint32_t* registroDestino = string_to_register32(instruccion[1]);
            uint8_t* registroOrigen = string_to_register8(instruccion[2]);
            sub_32_8(registroDestino,registroOrigen);
        }
        else{
            uint32_t* registroDestino = string_to_register32(instruccion[1]);
            uint32_t* registroOrigen = string_to_register32(instruccion[2]);
            sub_32_32(registroDestino,registroOrigen);
        }
    }
}

// JNZ
/* Compara si el registro pasado es igual a 0, de ser asi el PC salta a la instruccion pasada */
void jnz_8(uint8_t* reg,uint32_t instruccion_proxima){
    if(*reg != 0)
        registro.PC = instruccion_proxima;
    else
        registro.PC++;
}

/* Compara si el registro pasado es igual a 0, de ser asi el PC salta a la instruccion pasada */
void jnz_32(uint32_t* reg,uint32_t instruccion_proxima){
    if(*reg != 0)
        registro.PC = instruccion_proxima;
    else
        registro.PC++;
}

/* Ejecuta instruccion JNZ */
void jnz(char** instruccion)
{
    if(instruccion[1][0] != 'E')
    {
        uint8_t* registro_a_chequear = string_to_register8(instruccion[1]);
        uint32_t instruccion_a_saltar = atoi(instruccion[2]);
        jnz_8(registro_a_chequear,instruccion_a_saltar);
    }
    else{
        uint32_t* registro_a_chequear = string_to_register32(instruccion[1]);
        uint32_t instruccion_a_saltar = atoi(instruccion[2]);
        jnz_32(registro_a_chequear,instruccion_a_saltar);
    }
}

// IO_GEN_SLEEP
/* Enviar al kernel la solicitud de que duerma a un entrada salida */
void io_gen_sleep(t_pcb* pcb_a_enviar,char** instruccionDesarmada,int fd_dispatch){

    int motivo_desalojo = IO_GEN_SLEEP;

    t_paquete* paquete = armar_paquete_pcb(pcb_a_enviar);

    agregar_a_paquete(paquete,&motivo_desalojo,sizeof(int));
    agregar_a_paquete(paquete,instruccionDesarmada[1],strlen(instruccionDesarmada[1])+1);

    int tiempo_sleep = atoi(instruccionDesarmada[2]);

    agregar_a_paquete(paquete,&tiempo_sleep,sizeof(int));

    enviar_paquete(paquete,fd_dispatch);
    eliminar_paquete(paquete);
}

// EXIT
/* Envia al kernel la solicitud de pasar el pcb a exit (success) */
void instruccion_exit(t_pcb* pcb_a_enviar,int fd_dispatch)
{
    int motivo_desalojado = EXIT;

    t_paquete* paquete = armar_paquete_pcb(pcb_a_enviar);

    agregar_a_paquete(paquete,&motivo_desalojado,sizeof(int));

    enviar_paquete(paquete,fd_dispatch);
    eliminar_paquete(paquete);
}



