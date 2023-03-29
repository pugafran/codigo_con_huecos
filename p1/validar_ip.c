#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>

int main(int argc, char *argv[]){
    struct sockaddr_in sa;
    char *ip = argv[1];
    int result = inet_pton(AF_INET, ip, &(sa.sin_addr));
    if (result != 1) {
        // No es una dirección IP válida
        printf("\nResult = %d",result);
    }

    printf("\nResult = %d",result);
    return 0;
}
