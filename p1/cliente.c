// Archivos de cabecera para manipulación de sockets
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <ctype.h>
#include <pthread.h>
#include <errno.h>
#include "util.h"

#define TAMLINEA  1024
#define FALSO   0
#define CIERTO  1
#define SINASIGNAR -1

// tipo de datos que recibiran los hilos lectores
struct datos_hilo{
  FILE *fp;
  struct sockaddr *dserv;
};

typedef struct datos_hilo datos_hilo;


//
// VARIABLES GLOBALES
//

//IP del proceso syslog
char *ip_syslog;

//Puerto en el que espera el proceso syslog los
int puerto_syslog;

//Numero de hilos lectores
int nhilos;

//Es o no orientado a conexion
unsigned char es_stream=CIERTO;

// nombre del fichero fuente de eventos
char *fich_eventos;

// handler de archivo
FILE * fp;

//Mutex para controlar el archivo
pthread_mutex_t mutex;

struct in_addr addr;

void procesa_argumentos(int argc,char *argv[])
{
    // A RELLENAR
      if (argc != 6) {
        fprintf(stderr, "El número de argumentos es incorrecto.\n");
        fprintf(stderr, "Uso: %s <ip_sislog> <puerto_sislog> <t|u> <nhilos> <fich_eventos>\n", argv[0]);
        exit(1);
    }

    ip_syslog = argv[1];
    if (inet_pton(AF_INET, ip_syslog, &addr) <= 0) {
        fprintf(stderr, "%s no es una dirección IP válida.\n", ip_syslog);
        return 0;
    }

    puerto_syslog = atoi(argv[2]);
    if (puerto_syslog < 1024 || puerto_syslog > 65535) {
        fprintf(stderr, "El número de puerto debe estar entre 1024 y 65535.\n");
        exit(1);
    }

    char tipo_socket = * argv[3];
    if (tipo_socket != 't' && tipo_socket != 'u') {
        fprintf(stderr, "El tipo de socket debe ser 't' o 'u'.\n");
        exit(1);
    }

    else if(tipo_socket == 't')
        es_stream = CIERTO;
    
    else if (tipo_socket == 'u')
        es_stream = FALSO;  

    nhilos = atoi(argv[4]);
    if (nhilos <= 0) {
        fprintf(stderr, "El número de hilos debe ser un número positivo.\n");
        exit(1);
    }
    
    // verificar si el archivo existe
    fich_eventos = argv[5];
    if (access(fich_eventos, F_OK) != -1) {
        // abrir el archivo en modo de lectura
        FILE* fp = fopen(fich_eventos, "r");
        
        // verificar si el archivo se abrió correctamente
        if (fp == NULL) {
            fprintf(stderr, "Error al abrir el archivo %s\n", fich_eventos);
            exit(1);
        }
        
        
        
        // cerrar el archivo
        fclose(fp);
    } else {
        fprintf(stderr, "El archivo %s no existe.\n", fich_eventos);
        exit(EXIT_FAILURE);
    }


}

void salir_bien(int s)
{
  pthread_mutex_destroy(&mutex);
  fclose(fp);
  

  exit(0);
}

void *hilo_lector(datos_hilo *p)
{
  int  enviados;
  char buffer[TAMLINEA];
  char *s;
  int sock_dat;
  printf("\nSector hilo 1\n");
 
  do
  {
      printf("\nSector hilo 1.5\n");
      bzero(buffer,TAMLINEA);
      printf("\nSector hilo 2\n");
      // Leer la siguiente linea del fichero con fgets
      // (haciendo exclusión mutua con otros hilos)
      // El fichero (ya abierto por main) se recibe en uno de los parámetros
      // A RELLENAR -----------------
      
      pthread_mutex_init(&mutex, NULL);
      printf("\nSector hilo 3\n");
      pthread_mutex_lock(&mutex);
      s = fgets(buffer, TAMLINEA, p->fp);
      pthread_mutex_unlock(&mutex);
        printf("\nSector hilo 4\n");
      if (s!=NULL)
      {
          printf("\nSector hilo 4.5\n");
          printf("%s",s);
          // La IP y puerto del servidor están en una estructura sockaddr_in
          // que se recibe en uno de los parámetros
          if (es_stream)  // Enviar la línea por un socket TCP
          {
            // A RELLENAR
            
            printf("\nSector hilo 4.7\n");
            sock_dat = socket(PF_INET, SOCK_STREAM, 0);
            printf("\nSector hilo 4.8\n");
            ((struct sockaddr_in*) p->dserv)->sin_family = AF_INET;
            printf("\nSector hilo 4.9\n");
            ((struct sockaddr_in*) p->dserv)->sin_addr.s_addr = inet_addr(ip_syslog);
            ((struct sockaddr_in*) p->dserv)->sin_port = htons(puerto_syslog);
            printf("\nSector hilo 5\n");
            int conexion = connect(sock_dat, p->dserv, sizeof(*(p->dserv)));

            if (conexion == -1){
                fprintf(stderr, "Error al conectar con el servidor\n");
                exit(1);
            }

            printf("\nSector hilo 6\n");
            int bytes_enviados = send(sock_dat, s, TAMLINEA, 0); 
            
            if (bytes_enviados == -1){
                fprintf(stderr, "Error al enviar los datos\n");
                exit(1);
            }



          }
          else // Enviar la línea por un socket UDP
          {
            printf("\nSector UDP WTF\n");
            // A RELLENAR
            sock_dat = socket(PF_INET, SOCK_DGRAM, 0);
            ((struct sockaddr_in*) &p->dserv)->sin_family = AF_INET; 
            ((struct sockaddr_in*) &p->dserv)->sin_addr.s_addr = inet_addr(ip_syslog);
            ((struct sockaddr_in*) &p->dserv)->sin_port = htons(puerto_syslog);

            int bytes_enviados = sendto(sock_dat, buffer, TAMLINEA, 0, &p->dserv, sizeof(p->dserv));

            if (bytes_enviados == -1){
                fprintf(stderr, "Error al enviar los datos\n");
                exit(1);
            }

          }
          close(sock_dat);
          printf("%s",s); // Para depuración, imprimios la línea que hemos enviado
      }
      
  }
  while(s); // Mientras no se llegue al final del fichero
}


void main(int argc, char * argv[])
{
    // La función main crea los hilos lector, pasándoles los parámetros necesarios,
    // y espera a que terminen

    register int i;

    pthread_t *th;
    datos_hilo *q = NULL;

    int sock_dat, enviados;
    
    struct sockaddr_in d_serv;

    socklen_t ldir;
    char buffer[50];

    // Instalar la rutina de tratamiento de la señal SIGINT
    // A RELLENAR
    signal(SIGINT, salir_bien);

    // Procesar los argumentos de la línea de comandos
    procesa_argumentos(argc,argv);

    printf("IP servidor=%s, es_stream=%d\n",ip_syslog,es_stream);
    if ((fp=fopen(fich_eventos,"r"))==NULL)
    {
      perror("Error al abrir el fichero de eventos");
      exit(6);
    }

    // creamos espacio para los objetos de datos de hilo
    // A RELLENAR
    printf("\nSector 1\n");
    q = malloc(sizeof(datos_hilo));
    q->dserv = malloc(sizeof(struct sockaddr));
    th = (pthread_t *)malloc(nhilos * sizeof(pthread_t));
    printf("\nSector 2\n");

    // incicializamos los datos que le vamos a pasar como parámetro a los hilo_lector
    // (se pasa a todos el mismo parámetro)
    // A RELLENAR
    
    q->fp = fp;
    printf("\nSector 3\n");

    for (i=0;i<nhilos;i++)
    {
      // lanzamos el hilo lector
      // A RELLENAR
     
      printf("\nSector 4\n");
      pthread_create(&th[i], NULL, (void *)hilo_lector, q);

    }
    printf("\nSector 5\n");
    // Una vez lanzados todos, hacemos un join sobre cada uno de ellos
    for (i=0;i<nhilos;i++)
    {
      pthread_join(th[i],NULL);
    }
    // Al llegar aquí, todos los hilos han terminado
    fclose(fp);
}
