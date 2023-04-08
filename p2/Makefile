#****************************************************
# Variables
#****************************************************


CC=gcc
RPCGEN=rpcgen
CFLAGS= -I. -Wall -ggdb
LDFLAGS= -pthread
CLIENTE = cliente
SERVIDOR= servidor
INICIALIZADOR= inicializador_sislog

todo : $(CLIENTE) $(SERVIDOR) $(INICIALIZADOR)

#**************************************************
# Generacion del cliente y del servidor
#**************************************************

$(CLIENTE) : cliente.c
	$(CC) cliente.c util.c sislog_clnt.c sislog_xdr.c -o cliente $(LDFLAGS)

$(INICIALIZADOR) : inicializador_sislog.c
	$(CC) inicializador_sislog.c sislog_xdr.c sislog_clnt.c util.c -o inicializador_sislog 

$(SERVIDOR) : servicios.c
	$(CC) -o servidor servicios.c sislog_svc.c util.c sislog_xdr.c

sislog_svc.o : sislog.h sislog_svc.c 
	$(CC) $(CFLAGS) -c sislog_svc.c 

servicios.o: servicios.c sislog.h util.h
	$(CC) $(CFLAGS) -c servicios.c

sislog_clnt.o : sislog.h sislog_clnt.c 
	$(CC) $(CFLAGS) -c sislog_clnt.c

sislog_xdr.o : sislog.h sislog_xdr.c 
	$(CC) $(CFLAGS) -c sislog_xdr.c

cliente.o : cliente.c sislog.h util.h 
	$(CC) $(CFLAGS) -c cliente.c

inicializador_sislog.o : inicializador_sislog.c util.c util.h sislog.h
	$(CC) $(CFLAGS) -c inicializador_sislog.c 

util.o : util.c util.h
	$(CC) $(CFLAGS) -c util.c

sislog.h: sislog.x
	$(RPCGEN) sislog.x

#**************************************************
# Utilidades
#**************************************************

limpia : 
	rm -f *.o *.dat	
	

cleanall: limpia
	rm -f sislog_*
	rm -f sislog.h
	rm -f cliente servidor inicializador_sislog
