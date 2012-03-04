# Makefile
# author: pgarcia@eside.deusto.es
CC=g++
LDFLAGS=-L../util -lm -lpthread -lesidesocket -lesidethread -lesidesqlite
INCLUDE=-I../util

main: mNameServer 

mNameServer: mNameServer.o
	$(CC) mNameServer.o $(LDFLAGS) -o mNameServer

mNameServer.o: mNameServer.cc mNameServer.h
	$(CC) $(INCLUDE) -c mNameServer.cc

clean:
	rm *.o
	rm mNameServer
	rm *.exe
