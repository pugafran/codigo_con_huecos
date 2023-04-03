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

	

	cola->tam_cola = tam_cola;
	cola->datos = malloc(sizeof(*cola->datos)*tam_cola);

	if(cola->datos == NULL){
		perror("Puntero a la cola nulo");
		exit(1);
	}

	cola->head = 0;
	cola->tail = 0;

	if(sem_init(&(cola->num_huecos),0,tam_cola)== -1){
		perror("Error al inicializar el semaforo num_huecos.");
		exit(1);
	}

	if(sem_init(&(cola->num_ocupados),0,0) == -1){
		perror("Error al inicializar el semaforo num_ocupados.");
		exit(1);
	}

	if( pthread_mutex_init(&(cola->mutex_head),NULL) == -1){
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

	if(sem_destroy(&(cola->num_huecos)) == -1){
		perror("Error al destruir el semaforo hay_espacio.");
		exit(1);
	}

	if(sem_destroy(&(cola->num_ocupados)) == -1){
		perror("Error al destruir el semaforo hay_algo.");
		exit(1);
	}
	if(pthread_mutex_destroy(&(cola->mutex_head)) == -1){
		perror("Error al destruir el mutex cabeza.");
		exit(1);
	}	

    	if(pthread_mutex_destroy(&(cola->mutex_tail)) == -1){
		perror("Error al destruir el mutex cola.");
		exit(1);
	}	
}

void insertar_dato_cola(Cola *cola, dato_cola * dato)
{
    // A RELLENAR
	printf("\nInsertar dato cola - 1\n");

    if (cola == NULL) {
        perror("Cola no inicializada");
        exit(EXIT_FAILURE);
    }

	printf("\nInsertar dato cola - 2\n");

    if (sem_wait(&(cola->num_huecos)) < 0) {
        perror("Al esperar semáforo");
        exit(EXIT_FAILURE);
    }

	printf("\nInsertar dato cola - 3\n");

    if (pthread_mutex_lock(&(cola->mutex_head)) != 0) {
        perror("Al bloquear mutex");
        exit(EXIT_FAILURE);
    }

	printf("\nInsertar dato cola - 4\n");
	
    cola -> datos[cola -> head] = dato;

	printf("\nInsertar dato cola - 5\n");
	

    cola -> head = (cola -> head + 1) % cola -> tam_cola;

	printf("\nInsertar dato cola - 6\n");

    if (pthread_mutex_unlock(&(cola->mutex_head)) != 0) {
        perror("Al desbloquear mutex");
        exit(EXIT_FAILURE);
    }
	
	printf("\nInsertar dato cola - 7\n");
	

    if (sem_post(&(cola->num_ocupados)) < 0) {
        perror("Al incrementar semáforo");
        exit(EXIT_FAILURE);
    }

	printf("\nInsertar dato cola - 8\n");
	
}


dato_cola * obtener_dato_cola(Cola *cola)
{
    // A RELLENAR

	printf("\nObtener dato cola - 1\n");

    dato_cola *datoExtraido = (dato_cola *)malloc(sizeof(dato_cola));

	printf("\nObtener dato cola - 2\n");

    if (cola == NULL) {
        perror("Cola no inicializada");
        exit(EXIT_FAILURE);
    }
	printf("\nObtener dato cola - 3\n");

    if (sem_wait(&(cola->num_ocupados)) < 0) {
        perror("Al esperar semáforo");
        exit(EXIT_FAILURE);
    }

	printf("\nObtener dato cola - 4\n");

    if (pthread_mutex_lock(&(cola->mutex_tail)) != 0) {
        perror("Al bloquear mutex");
        exit(EXIT_FAILURE);
    }

	printf("\nObtener dato cola - 5\n");

    //datoExtraido = cola->datos[cola->tail];
    memcpy(datoExtraido, cola->datos[cola->tail], sizeof(dato_cola));

	printf("\nObtener dato cola - 6\n");

	cola->tail = (cola->tail+1)%cola->tam_cola;

	printf("\nObtener dato cola - 7\n");

    if (pthread_mutex_unlock(&(cola->mutex_tail)) != 0) {
        perror("Al desbloquear mutex");
        exit(EXIT_FAILURE);
    }

	printf("\nObtener dato cola - 8\n");

    if (sem_post(&(cola->num_huecos)) < 0) {
        perror("Al incrementar semáforo");
        exit(EXIT_FAILURE);
    }

	printf("\nObtener dato cola - 9\n");

    return datoExtraido;
   
}
