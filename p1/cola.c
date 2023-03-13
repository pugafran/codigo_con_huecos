#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cola.h"

// El contenido de este fichero implementa las funciones de la cola.
// Es prácticamente igual a la cola que tienes hecha de las prácticas
// de laboratorio pero adaptándola a la estructura de datos dato_cola
// usada en este ejercicio.
//
// Mira el fichero cola.h para ver las estructuras de datos a utilizar

void inicializar_cola(Cola *cola, int tam_cola)
{
    // A RELLENAR
	if(tam_cola <=0){
		perror("El tamaño de la cola debe de ser positivo");
		exit(1);
	}
	cola->datos = (int*)malloc(sizeof(int)*tam_cola);
	if(cola->datos == NULL){
		perror("Puntero a la cola nulo");
		exit(1);
	}
	cola->tam_cola = tam_cola;
	if(sem_init(&cola->num_huecos,0,tam_cola)== -1){
		perror("Error al inicializar el semaforo hay_espacio.");
		exit(1);
	}
	if(sem_init(&cola->num_ocupados,0,0) == -1){
		perror("Error al inicializar el semaforo hay_algo.");
		exit(1);
	}
	if( pthread_mutex_init(&cola->mutex_head,NULL) == -1){
		perror("Error al inicializar el mutex.");
		exit(1);


	}
}


void destruir_cola(Cola *cola)
{
    // A RELLENAR
	if(cola == NULL){
		perror("Puntero a la cola es nulo");
		exit(1);
	}
	
	if(cola->datos != NULL) free(cola->datos);
	if(sem_destroy(&cola->num_huecos) == -1){
		perror("Error al destruir el semaforo hay_espacio.");
		exit(1);
	}

	if(sem_destroy(&cola->num_ocupados) == -1){
		perror("Error al destruir el semaforo hay_algo.");
		exit(1);
	}
	if(pthread_mutex_destroy(&cola->mutex_head) == -1){
		perror("Error al destruir el mutex.");
		exit(1);
	}	

    	if(pthread_mutex_destroy(&cola->mutex_tail) == -1){
		perror("Error al destruir el mutex.");
		exit(1);
	}	
}

void insertar_dato_cola(Cola *cola, dato_cola * dato)
{
    // A RELLENAR
    |
    |
    |
    |
}


dato_cola * obtener_dato_cola(Cola *cola)
{
    dato_cola *p;
    // A RELLENAR
    |
    |
    |
    |

    return(p);
}
