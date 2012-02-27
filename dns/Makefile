# Makefile
# author: dipina@eside.deusto.es
CC=g++
LDFLAGS=-L../util -lm -lpthread -lesidesocket -lesidethread 
INCLUDE=-I../util

main: EchoTcpListener EchoTcpClient NameServer NameClient

EchoTcpListener: EchoServerThread.o EchoTcpListener.o
	$(CC) EchoServerThread.o EchoTcpListener.o $(LDFLAGS) -o EchoTcpListener

EchoTcpClient: EchoTcpClient.o
	$(CC) EchoTcpClient.o $(LDFLAGS) -o EchoTcpClient

NameServer: NameServer.o
	$(CC) NameServer.o $(LDFLAGS) -o NameServer

NameClient: NameClient.o
	$(CC) NameClient.o $(LDFLAGS) -o NameClient

EchoServerThread.o: EchoServerThread.cc EchoServerThread.h
	$(CC) $(INCLUDE) -c EchoServerThread.cc

EchoTcpListener.o: EchoTcpListener.cc
	$(CC) $(INCLUDE) -c EchoTcpListener.cc

EchoTcpClient.o: EchoTcpClient.cc
	$(CC) $(INCLUDE) -c EchoTcpClient.cc

NameServer.o: NameServer.cc NameServer.h
	$(CC) $(INCLUDE) -c NameServer.cc

NameClient.o: NameClient.cc 
	$(CC) $(INCLUDE) -c NameClient.cc

SQLiteMap.o: SQLiteMap.cc SQLiteMap.h
	$(CC) -c SQLiteMap.cc
	
clean:
	rm *.o
	rm EchoTcpListener EchoTcpClient NameServer NameClient
	rm *.exe
