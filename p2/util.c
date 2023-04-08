#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include "util.h"

#include <arpa/inet.h>

// Función de utilidad que determina si los caracteres de una cadena
// son todos numericos
int valida_numero(char *str)
{
  // A RELLENAR

  // Recorre cada carácter de la cadena
  for (int i = 0; str[i] != '\0'; i++)
  {
    // Si el carácter no es un dígito numérico, devuelve 0
    if (!isdigit(str[i]))
    {
      return 0;
    }
  }
  // Si todos los caracteres son dígitos numéricos, devuelve 1
  return 1;
}

// Función de utilidad que valida si una cadena de caracteres representa
// una IPv4 valida
int valida_ip(char *ip)
{
  // Comprueba si la cadena contiene una ip válida
  // RELLENA ESTE HUECO
  struct in_addr addr;

  // Convertir la dirección IP en formato de cadena a una estructura de in_addr
  if (inet_pton(AF_INET, ip, &addr) <= 0)
  {
    return 0;
  }

  return 1;
}

// Función de utilidad, para generar los tiempos usados por los buques en las operaciones portuarias
// Devuelve un número aleatorio comprendido entre min y max
double randRange(double min, double max)
{
  return min + (rand() / (double)RAND_MAX * (max - min + 1));
}

// Función de utilidad para depuración. Emite por pantalla el mensaje
// que se le pasa como parámetro, pero pone delante del mensaje un
// timestamp, para poder ordenar la salida por si saliera desordenada
//
// Ejemplo de uso:
//
//  log_debug("Menesaje a imprimir")
//
// Más ejemplos en el programa principal.
void log_debug(char *msg)
{
  struct timespec t;
  clock_gettime(_POSIX_MONOTONIC_CLOCK, &t);
  printf("[%ld.%09ld] %s", t.tv_sec, t.tv_nsec, msg);
}

// Función de utilidad para volcar el contenido de un array de contadores
void mostrar_recuento_eventos(int nfils, int ncols, char **filas, char **columnas, int **valores)
{
  register int i, j;
  printf("*****************  RECUENTO EVENTOS  *******************\n");

  printf("\t");
  for (i = 0; i < ncols; i++)
    printf("%s\t", columnas[i]);
  printf("\n");
  for (i = 0; i < nfils; i++)
  {
    printf("%s\t", filas[i]);
    for (j = 0; j < ncols; j++)
      printf("%d\t", valores[i][j]);
    printf("\n");
  }
}
