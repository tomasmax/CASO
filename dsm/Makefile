# Makefile
# author: dipina@eside.deusto.es
CC=g++
LDFLAGS=-L../util -lm -lpthread -lesidesocket -lesidethread
INCLUDE=-I../util

main: libesidedsm.so DsmServer DsmTest MatrixEvenAdder MatrixOddAdder 

libesidedsm.so: Dsm.o
	$(CC) -shared $(LDFLAGS) Dsm.o -o libesidedsm.so
	cp libesidedsm.so libesidedsm.dll
	cp libesidedsm.* ../util/
		
Dsm.o: Dsm.cc Dsm.h DsmTypes.h
	$(CC) $(INCLUDE) -c Dsm.cc
	
DsmServer: DsmServer.o
	$(CC)  DsmServer.o $(LDFLAGS) -o DsmServer

DsmTest: DsmTest.o
	$(CC) DsmTest.o $(LDFLAGS) -L. -lesidedsm -o DsmTest
	
MatrixEvenAdder: MatrixEvenAdder.o
	$(CC) MatrixEvenAdder.o $(LDFLAGS) -L. -lesidedsm -o MatrixEvenAdder
	
MatrixOddAdder: MatrixOddAdder.o
	$(CC) MatrixOddAdder.o $(LDFLAGS) -L. -lesidedsm -o MatrixOddAdder

DsmServer.o: DsmServer.cc DsmServer.h DsmTypes.h
	$(CC) $(INCLUDE) -c DsmServer.cc

DsmTest.o: DsmTest.cc
	$(CC) $(INCLUDE) -c DsmTest.cc

MatrixEvenAdder.o: MatrixEvenAdder.cc
	$(CC) $(INCLUDE) -c MatrixEvenAdder.cc
	
MatrixOddAdder.o: MatrixOddAdder.cc
	$(CC) $(INCLUDE) -c MatrixOddAdder.cc

clean:
	rm *.o
	rm *.so
	rm *.dll
	rm DsmServer DsmTest MatrixEvenAdder MatrixOddAdder 
