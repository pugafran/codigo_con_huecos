#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <sched.h>

// Archivos de cabecera para manipulación de sockets
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <time.h>

#include "cola.h"
#include "util.h"

#define CIERTO 1
#define FALSO 0

#define NUMFACILITIES 10
#define NUMLEVELS 8

// Estructura de datos para pasar los parametros a los hilos de atencion
struct param_hilo_aten
{
    int num_hilo;
    int s;
};

typedef struct param_hilo_aten param_hilo_aten;

// ====================================================================
// PROTOTIPOS FUNCIONES
// ====================================================================
static void handler(int signum); // Manejador de señal SIGINT

// ====================================================================
// VARIABLES GLOBALES
// ====================================================================

// Cola para sincronizar los hilos de atencion de peticiones con los
// hilos trabajadores
Cola cola_eventos;

// Puerto en el que esperamos los mensajes
int puerto;

// Variable booleana que indica si el socket es orientado a conexión o no
unsigned char es_stream = CIERTO;

// Variable que almacena el numero de hilos de atencion de peticiones
int num_hilos_aten;

// Variable que almacena el numero de hilos trabajadores
int num_hilos_work;

// Puntero a la dirección de comienzo del array de datos de hilo
// de los hilos de atencion de peticiones
pthread_t *hilos_aten;

// Puntero a la dirección de comienzo del array de datos de hilo
// de los hilos trabajadores
pthread_t *hilos_work;

// Arrays para la traducción de nombres de niveles y de facilities
// y para obtener los nombres de los ficheros de registro
char *facilities_names[NUMFACILITIES] = {
    "kern",
    "user",
    "mail",
    "daemon",
    "auth",
    "syslog",
    "lpr",
    "news",
    "uucp",
    "cron"};

char *level_names[NUMLEVELS] = {
    "emerg",
    "alert",
    "crit",
    "err",
    "warning",
    "notice",
    "info",
    "debug"};

char *facilities_file_names[NUMFACILITIES] = {
    "fac00.dat",
    "fac01.dat",
    "fac02.dat",
    "fac03.dat",
    "fac04.dat",
    "fac05.dat",
    "fac06.dat",
    "fac07.dat",
    "fac08.dat",
    "fac09.dat"};

// mutex de exclusion a los ficheros de registro
pthread_mutex_t mfp[NUMFACILITIES];
// pthread_mutex_lock(3) nada mas que lo tienes tu y luego unlock para desbloquearlo
// pthread_mutex_destroy

// Tamaño de la cola circular
int tam_cola;

// ====================================================================
// FUNCION handler de las señales recibidas por el hilo buque
// ====================================================================
static void handler(int signum)
{
    register int i;

    switch (signum)
    {
    case SIGINT:
        // Si se recibe esta señal, hay que terminar "bien"
        // liberando todos los recursos en uso antes de finalizar
        // A RELLENAR
        destruir_cola(&cola_eventos);
        free(hilos_aten);
        free(hilos_work);

        for (i = 0; i < NUMFACILITIES; i++)
        {
            if (pthread_mutex_destroy(&(mfp[i])) != 0)
            {
                fprintf(stderr, "Error eliminando los mutex que regulan el acceso exclusivo a los ficheros de log.\n");
                exit(1);
            }
        }

    default:
        pthread_exit(NULL);
    }
}

void procesa_argumentos(int argc, char *argv[])
{

    if (argc != 6) {
        fprintf(stderr, "El número de argumentos es incorrecto.\n");
        fprintf(stderr, "Uso: %s <num_puerto> <t|u> <tam_cola> <num_hilos_aten> <num_hilos_work>\n", argv[0]);
        exit(1);
    }

    // Extraer los argumentos y validar sus valores
    
    if(valida_numero(argv[1])){
        puerto = atoi(argv[1]);
    }

    else{
        fprintf(stderr, "El número de puerto debe ser un número.\n");
        exit(1);
    }


    if (puerto < 1024 || puerto > 65535) {
        fprintf(stderr, "El número de puerto debe estar entre 1024 y 65535.\n");
        exit(1);
    }

    char tipo_socket = * argv[2];
    if (tipo_socket != 't' && tipo_socket != 'u') {
        fprintf(stderr, "El tipo de socket debe ser 't' o 'u'.\n");
        exit(1);
    }

    else if(tipo_socket == 't')
        es_stream = CIERTO;
    
    else if (tipo_socket == 'u')
        es_stream = FALSO;  

    if(valida_numero(argv[3])){
        tam_cola = atoi(argv[3]);
    }

    else{
        fprintf(stderr, "El tamaño de la cola debe ser un número.\n");
        exit(1);
    }

    if (tam_cola <= 0) {
        fprintf(stderr, "El tamaño de la cola debe ser un número positivo.\n");
        exit(1);
    }

    if(valida_numero(argv[4])){
        num_hilos_aten = atoi(argv[4]);
    }

    else{
        fprintf(stderr, "El número de hilos de atención debe ser un número.\n");
        exit(1);
    }

    if (num_hilos_aten <= 0) {
        fprintf(stderr, "El número de hilos de atención debe ser un número positivo.\n");
        exit(1);
    }
    if(valida_numero(argv[5])){
        num_hilos_work = atoi(argv[5]);
    }

    else{
        fprintf(stderr, "El número de hilos de atención debe ser un número.\n");
        exit(1);
    }
    
    if (num_hilos_work <= 0) {
        fprintf(stderr, "El número de hilos trabajadores debe ser un número positivo.\n");
        exit(1);
    }
}

// ====================================================================
// Implementación de los hilos
// ====================================================================

void *Worker(int *id)
{
    int id_worker;
    FILE *fp;
    dato_cola *evt;
    char msg[2048];
    char *fechahora;
    time_t timeraw;

    // Hacemos copia del parámetro recibido
    id_worker = *id;

    // y liberamos la memoria reservada para él
    free(id);

    // Mostramos información de depuración por pantalla
    sprintf(msg, "Comienza el Worker %d\n", id_worker);
    log_debug(msg);

    // Codigo del worker. Espera datos de la cola de sincronización,
    // genera en base a ellos la línea a escribir, y la escribe
    // en el fichero que corresponda. Mira "cola.h"
    // para recordar la estructura dato_cola que recibe de la cola
    while (1)
    {
        // A RELLENAR
        evt = (dato_cola *) obtener_dato_cola(&cola_eventos);
        
        timeraw = time(NULL);
        fechahora = ctime(&timeraw);

        fechahora[strlen(fechahora) - 1] = '\0';

        fp = fopen(facilities_file_names[evt->facilidad],"a");
        fprintf(fp,"%s:%s:%s:%s\n", facilities_names[evt->facilidad], level_names[evt->nivel], fechahora, evt->msg);
    }
}

void *AtencionPeticiones(param_hilo_aten *q)
{
    int sock_dat, recibidos;
    struct sockaddr_in d_cliente;
    socklen_t l_dir = sizeof(d_cliente);
    char msg[100];
    char buffer[TAMMSG];
    char *token;
    char *loc;
    dato_cola *p;
    int s;        // Variable local para almacenar el socket que se recibe como parámetro
    int num_hilo; // Variable local para almacenar el numero de hilo que se recibe como parámetro
                  // (no usada, pero puedes usarla para imprimir mensajes de depuración)

    // Información de depuración
    sprintf(msg, "Comienza el Hilo de Atencion de Peticiones %d\n", q->num_hilo);
    log_debug(msg);

    // Hacemos copia de los parámetros recibidos
    s = q->s;
    num_hilo = q->num_hilo;

    // y liberamos la memoria reservada para el parámetro
    free(q);

    while (1) // Bucle infinito de atencion de mensajes
    {
        // Primero, se recibe el mensaje del cliente. Cómo se haga depende
        // de si el socket es orientado a conexión o no
        if (es_stream) // TCP
        {
            // Aceptar el cliente, leer su mensaje hasta recibirlo entero, y cerrar la conexión
            // A RELLENAR
            sock_dat = accept(s, 0, 0);
            recibidos = recv(sock_dat, buffer, TAMMSG, 0);
            buffer[recibidos] = 0;
            close(sock_dat);
        }

        else // UDP
        {
            // Recibir el mensaje del datagrama
            // A RELLENAR
            recibidos = recvfrom(sock_dat, buffer, TAMMSG, 0, (struct sockaddr *) &d_cliente, &l_dir);
        }
        // Una vez recibido el mensaje, es necesario separar sus partes,
        // guardarlos en la estructura adecuada, y poner esa estructura en la cola
        // de sincronización.
        // A RELLENAR
        token = strtok_r(p, ":", &loc);

            token = strtok_r(NULL, ":", &loc);
            printf("\nFacilidad: %s\n", token);
            p->facilidad = token;

            token = strtok_r(NULL, ":", &loc);
            printf("\nNivel: %s\n", token);
            p->nivel = token;
        
            token = strtok_r(NULL, ":", &loc);
            printf("\nMsg: %s\n", token);
            strcpy(p->msg, token);

        

    }
}

// ====================================================================
// PROGRAMA PRINCIPAL
// ====================================================================

// Su misión es crear e inicializar los recursos de sincronización globales,
// lanzar todos los hilos y esperar a que finalicen

int main(int argc, char *argv[])
{
    register int i; // Indice para bucles
    int *id=NULL;        // Para pasar el identificador a cada hilo trabajador
    int sock_pasivo;
    struct sockaddr_in d_local;
    param_hilo_aten *q;

    procesa_argumentos(argc, argv);

    setbuf(stdout, NULL);    // quitamos el buffer de la salida estandar
    signal(SIGINT, handler); // establecemos el comportamiento ante la llegada asíncrona de la señal

    // Datos para asignar puerto al socket
    d_local.sin_family = AF_INET;
    d_local.sin_addr.s_addr = htonl(INADDR_ANY);
    d_local.sin_port = htons(puerto);

    if (es_stream) // Preparar socket TCP
    {
        // A RELLENAR
        sock_pasivo = socket(PF_INET, SOCK_STREAM, 0);
    }
    else // Preparar socket UDP
    {
        // A RELLENAR
        sock_pasivo = socket(PF_INET, SOCK_DGRAM, 0);
    }

    // Asignamos el puerto al socket
    // A RELLENAR
    bind(sock_pasivo, (struct sockaddr *)&d_local, sizeof(d_local));

    // creamos el espacio para los objetos de datos de hilo
    hilos_aten = (pthread_t *)malloc(num_hilos_aten * sizeof(pthread_t));

    // Inicializamos los mutex de exclusión a los ficheros de log
    // en que escribirán los workers
    // A RELLENAR
    for (i = 0; i < NUMFACILITIES; i++)
    {

        if (pthread_mutex_init(&(mfp[i]), NULL) == -1)
        {
            fprintf(stderr, "Error al inicializar los mutex que sincronizan el acceso a los ficheros de log que gestiona el servicio sislog.\n");
            exit(1);
        }
    }

    // Reservamos espacio para los objetos de datos de hilo de los hilos trabajadores
    hilos_work = (pthread_t *)malloc(num_hilos_work * sizeof(pthread_t));

    // Inicializamos la cola
    // A RELLENAR
    inicializar_cola(&cola_eventos,tam_cola);

    // Creamos cada uno de los hilos de atención de peticiones
    for (i = 0; i < num_hilos_aten; i++)
    {
        // A RELLENAR
        q->num_hilo = i;
        q->s = sock_pasivo;
        pthread_create(*(hilos_aten+i), NULL, (void *)AtencionPeticiones, &q);
        //hilos_aten[i]
    }
    // Y creamos cada uno de los hilos trabajadores
    for (i = 0; i < num_hilos_work; i++)
    {
        // A RELLENAR (?)
        id = malloc(sizeof(int));
        *id = i;

        // Crear el hilo pthread_create
        pthread_create(*(hilos_work+i), NULL, (void *)Worker, &id);

    }

    // Esperamos a que terminen todos los hilos
    for (i = 0; i < num_hilos_aten; i++)
    {
        pthread_join(hilos_aten[i], NULL);
    }
    for (i = 0; i < num_hilos_work; i++)
    {
        pthread_join(hilos_work[i], NULL);
    }
}
