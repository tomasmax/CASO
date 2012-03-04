// DsmTest.cc
// author: dipina@eside.deusto.es

#include "TcpListener.h"
#include "Dsm.h"

#define BUFFER_SIZE 1024

void usage() {
	cout << "Usage: DsmTest <dsm-server-port>" << endl;
	exit(1);
}


int main(int argc, char** argv) {

	if (argc != 2) {
		usage();
	}

	// Hacer lookup dsm.deusto.es 

	PracticaCaso::DsmDriver * driver = new PracticaCaso::DsmDriver("127.0.0.1", atoi(argv[1]));
	for (int i=0; i<10;i++) {
		string blockId = "BlockA";

		try {
			driver->dsm_malloc(blockId, 1024);
			cout << endl << endl << "****** " << i << ": " << blockId << " created" << endl;
			driver->dsm_put(blockId, (void *)"aupa estudiantes caso", strlen("aupa estudiantes caso"));
			cout << "data put: " << blockId << " aupa estudiantes caso " << strlen("aupa estudiantes caso") << endl;
			PracticaCaso::DsmData data = driver->dsm_get(blockId);
			cout << "data read: " << data.size << ": " << string((char *)data.addr, 0, data.size) << endl;

			driver->dsm_put(blockId, (void *)"this assignment really rocks, it's wonderful", strlen("this assignment really rocks, it's wonderful"));
			cout << "data put: " << blockId << " this assignment really rocks, it's wonderful " << strlen("this assignment really rocks, it's wonderful") << endl;
			data = driver->dsm_get(blockId);
			cout << "data read: " << data.size << ": " << string((char*)data.addr, 0, data.size) << endl;

			
			int dato = 289;
			driver->dsm_put(blockId, (void *)&dato, sizeof(dato));
			cout << "data put: " << blockId << " " << dato << " " << sizeof(dato) << endl;	
			data = driver->dsm_get(blockId);
			cout << "data read: " << data.size << ": " << *((int *)data.addr) << endl;


			int a[100];
			for (int k=0; k<100; k++) a[k] = k;
			driver->dsm_put(blockId, (void *)a, sizeof(a));
			cout << "data put: " << blockId << " " << a << " " << sizeof(a) << endl;	
			data = driver->dsm_get(blockId);
			cout << "data read: " << data.size << endl;
			int *b = (int *)data.addr;
			for (int j=0; j<100; j++) {
				cout << b[j] << " ";
			}
				
			driver->dsm_free(blockId);
			cout << i << ": releasing block: " << blockId << endl;
		} catch (DsmException dsme) {
			cerr << dsme << endl;
		}
	}
	sleep(1);
	cout << "Closing driver" << endl;
	delete driver;
}

