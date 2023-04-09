/*
Cliente de RPC que simula las operaciones de varios clientes del servidor de log
*/
#include <rpc/rpc.h>
#include <sys/types.h>
#include <sys/times.h>
#include <unistd.h>
#include <pthread.h>
#include "util.h"
#include "sislog.h"

#define TAMLINEA 1024
#define TAMMSG 1024

// Variables globales

// IP del proceso sislog
char *ip_sislog = NULL;

// numero de clientes
int num_clientes;

// Mutex para controlar el archivo
pthread_mutex_t mutex;

// tipo de datos que recibiran los hilos cliente
struct datos_hilo
{
    FILE *fp;
    int id_cliente;
};

typedef struct datos_hilo datos_hilo;

void *Cliente(datos_hilo *p)
{
    // Implementación del hilo que genera los eventos que se envían al
    // servidor via RPC
    CLIENT *cl = NULL;
    FILE *fp = NULL;
    int id_cliente;
    char buffer[TAMLINEA]; // Buffer de lectura de lineas del fichero de eventos
    char msg[TAMLINEA * 2];

    Resultado *res = NULL;
    char *s = NULL;
    char *token = NULL;
    char *loc = NULL;

    /* Para saber si se está en el final de fichero de eventos.txt

    */
    char *ptr = NULL;
    int i, j;

    eventsislog evt;
    // evt.msg = malloc(sizeof(texto));
    evt.msg = (char *)malloc(TAMMSG * sizeof(char));

    id_cliente = p->id_cliente; // Capturar el id del cliente en una variable local
    fp = p->fp;
    free(p); // Ya no necesitamos el parámetro recibido, lo liberamos

    if (fp == NULL)
    {
        fprintf(stderr, "Error: could not open event file.\n");
        exit(1);
    }

    // creamos un hadler de conexión con el servidor RPC
    // y comprobamos que se ha creado correctamente
    cl = clnt_create(ip_sislog, SISLOG, PRIMERA, "tcp");
    if (cl == NULL)
    {
        clnt_pcreateerror(ip_sislog);
        exit(1);
    }

    // Bucle de lectura de eventos
    do
    {

        // leemos mediante exclusión la siguiente línea del fichero cuyo *FILE
        // recibimos en uno de los campos de de la estructura datos_hilo
        // RELLENA ESTE HUECO
        pthread_mutex_init(&mutex, NULL);
        pthread_mutex_lock(&mutex);
        s = fgets(buffer, TAMLINEA, fp);
        pthread_mutex_unlock(&mutex);

        // si la cadena leida del fichero no es nula, tokenizamos de manera
        // reentrante la línea para extraer sus tokens e ir componiendo
        // el mensaje de la invocación remota al sislog
        if (s != NULL)
        {
            // Tokenizar cadena y rellenado de la estructura de datos
            // RELLENA ESTE HUECO

            // p = (datos_hilo *)malloc(sizeof(datos_hilo));
            // p->facilidad = malloc(sizeof(dato_cola));

            token = strtok_r(buffer, ":", &loc);

            evt.facilidad = atoi(token); // Se debe convertir a entero  (token-48?)

            token = strtok_r(NULL, ":", &loc);

            evt.nivel = atoi(token); // Se debe convertir a entero

            token = strtok_r(NULL, ":", &loc);

            strcpy(evt.msg, token);

            // Comprobar si se ha copiado bien
            if (strcmp(evt.msg, token) != 0)
            {
                fprintf(stderr, "Error: en strcmp\n");
            }

            /*
                [eventos.txt]
                0:0:Cero (\n)
                1:1:Uno (\n)
                2:2:Dos (\n)
                3:3:Tres (\n)
                4:4:Cuatro (\n)
                5:5:Cinco (\n)
                6:6:Seis (\n)
                7:7:Siete (\n)
                8:7:Ocho (\n)
                9:7:Nueve (EOF)
                [/eventos.txt]

                Si no se trata correctamente el último facXX.dat de eventos.txt, las lineas se van a guardar consecutivamente sin saltos
                de linea, es por ello que se arregla en el posterior código.

                Buscamos el salto de línea dentro de la linea recogida del fichero eventos.txt, como es una sucesión de líneas, todas
                excepto la última tienen salto de línea por lo que ptr será != null en todas las ocasiones excepto para la última linea.



            */

            // ¿Contiene msg '\n'?
            ptr = strchr(evt.msg, '\n');

            // Si no lo contiene, es el último fichero
            if (ptr == NULL)
            {

                char *new_str = "\n";

                // Buscamos el final de msg
                while (evt.msg[i] != '\0')
                {
                    i++;
                }

                // Copiamos el \n al final de msg
                j = 0;
                while (new_str[j] != '\0')
                {
                    evt.msg[i] = new_str[j];
                    i++;
                    j++;
                }

                evt.msg[i] = '\0';
            }

            // Mensaje de depuración
            sprintf(msg, "Cliente %d envia evento. Facilidad: %d, Nivel: %d, Texto: %s\n", id_cliente, evt.facilidad, evt.nivel, evt.msg);
            log_debug(msg);

            // Enviar evento por RPC e imprimir el velor retornado
            // y liberar seguidamente las estructuras de datos utilizadas
            // RELLENA ESTE HUECO
            res = registrar_evento_1(&evt, cl);

            if(res->caso == 0)
                sprintf(msg, "Registrar evento devuelve: %d\n", res->Resultado_u.valor);
            else
                sprintf(msg, "Registrar evento devuelve: %s\n", res->Resultado_u.msg);

            log_debug(msg);
        }

    } while (s);

    clnt_destroy(cl);
    free(evt.msg);
    return NULL;
}

int main(int argc, char *argv[])
{
    register int i; // Indice para bucles
    pthread_t *th = NULL;
    datos_hilo *q = NULL;
    FILE *fp = NULL;
    char msg[TAMLINEA * 2];

    if (argc != 4)
    {
        fprintf(stderr, "Forma de uso: %s <numero_clientes> <ip_serv_sislog> <fich_eventos>\n", argv[0]);
        exit(1);
    }
    if (atoi((char *)argv[1]) <= 0)
    {
        fprintf(stderr, "El parametro  <numero_clientes> debe ser un entero positivo\n");
        exit(3);
    }
    ip_sislog = strdup(argv[2]);
    if (!valida_ip(argv[2]))
    {
        fprintf(stderr, "La IP introducida no es valida\n");
        exit(4);
    }
    num_clientes = atoi(argv[1]);

    // Reservamos memoria para los objetos de datos de hilo
    th = (pthread_t *)malloc(sizeof(pthread_t) * num_clientes);
    if (th == NULL)
    {

        sprintf(msg, "Error: no hay memoria suficiente para los objetos de datos de hilo\n");
        log_debug(msg);
        exit(5);
    }

    if ((fp = fopen(argv[3], "r")) == NULL)
    {
        perror("Error al abrir el fichero de eventos");
        exit(6);
    }

    // Creación de un hilo para cada cliente. Estos sí reciben como parámetro
    // un puntero a entero que será su id_cliente. Se crea dinámicamente uno
    // para cada hilo y se le asigna el contador del bucle
    for (i = 0; i < num_clientes; i++)
    {
        // RELLENA ESTE HUECO

        q = malloc(sizeof(datos_hilo));
        q->fp = fp;
        q->id_cliente = i;

        pthread_create(&th[i], NULL, (void *)Cliente, q);
    }

    // main espera a que terminen todos los buques
    for (i = 0; i < num_clientes; i++)
    {
        pthread_join(th[i], NULL);
    }
    free(th);
}
