CC=gcc
CFLAGS=-I. -pthread

all: sislog cliente

sislog: sislog.c 
	$(CC) sislog.c util.c cola.c -o sislog $(CFLAGS)

cliente: cliente.c
	$(CC) cliente.c util.c -o cliente $(CFLAGS)


limpia:
	-rm *.o

cleanall: limpia
	-rm sislog cliente *.dat
