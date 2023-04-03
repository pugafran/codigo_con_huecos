#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>

int check_ip(const char* ip_address) {
    struct in_addr addr;
    
    // Convertir la dirección IP en formato de cadena a una estructura de in_addr
    if (inet_pton(AF_INET, ip_address, &addr) <= 0) {
        printf("%s no es una dirección IP válida.\n", ip_address);
        return 0;
    }
    
    // Verificar si la dirección es una dirección de broadcast, multicast o de red
    if (addr.s_addr == INADDR_BROADCAST ||
        (ntohl(addr.s_addr) & 0xff000000) == 0x7f000000 ||
        (ntohl(addr.s_addr) & 0xf0000000) == 0xe0000000 ||
        (ntohl(addr.s_addr) & 0xf0000000) == 0xf0000000) {
        printf("%s es una dirección de broadcast, multicast o de red.\n", ip_address);
        return 0;
    }
    
    printf("%s es una dirección IP válida.\n", ip_address);
    return 1;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Uso: %s [direccion_ip]\n", argv[0]);
        return 1;
    }
    
    check_ip(argv[1]);
    return 0;
}
