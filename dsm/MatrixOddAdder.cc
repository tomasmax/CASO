// MatrixOddAdder.cc
// author: dipina@eside.deusto.es

#include "TcpListener.h"
#include "Dsm.h"

#define BUFFER_SIZE 1024

void usage() {
	cout << "Usage: MatrixOddAdder <dsm-server-port>" << endl;
	exit(1);
}


int main(int argc, char** argv) {

	if (argc != 2) {
		usage();
	}

	// Hacer lookup dsm.deusto.es 
	PracticaCaso::DsmDriver * driver = new PracticaCaso::DsmDriver("127.0.0.1", atoi(argv[1]));
	PracticaCaso::DsmData data;

	cout << "Adding array of 100 nums from 0 to 99: " << driver->get_nid() << endl;
	// Wait till Array100Nums has been initialised with data in DSM, i.e. data has been associated to it
	bool array100NumsGet = false;
	while (!array100NumsGet) {
		try {
			data = driver->dsm_get("Array100Nums");
			array100NumsGet = true;
		} catch (DsmException dsme) {
			cerr << "ERROR in dsm_get(\"Array100Nums\") - waiting for other process to initialize it: " << dsme << endl;
			driver->dsm_wait("Array100Nums");
		}
	}
	
	// Add the odd number indexed elements
	int *b = (int *)data.addr;
	int sumOddNums = 0;
	for (int i=1; i<100; i+=2) {
		sumOddNums += b[i];
	}
	cout << "\tSum of odd nums: " << sumOddNums << endl;
	
	// Publish the sum of the odd number indexed elements
	try {
		driver->dsm_malloc("SumOddNums", sizeof(sumOddNums));
		try {
			driver->dsm_put("SumOddNums", (void *)&sumOddNums, sizeof(sumOddNums));
		} catch (...) {
			cerr << "ERROR: dsm_put(\"SumOddNums\", (void *)&sumOddNums, sizeof(sumOddNums))" << endl;
			exit(1);
		}
	} catch (DsmException dsme) {
		cerr << "ERROR in dsm_malloc(\"SumOddNums\", sizeof(" << sizeof(sumOddNums) << ")): " << dsme << endl;
		exit(1);
	}

	// Wait till TotalSum has been published, print it out and clean up 
	bool totalSumGet = false;
	while (!totalSumGet) {
		try {
			data = driver->dsm_get("TotalSum");
			totalSumGet = true;
		} catch (DsmException dsme) {
			cerr << "ERROR in dsm_get(\"TotalSum\") - waiting for other process to initialize it: " << dsme << endl;
			driver->dsm_wait("TotalSum");
		}
	}
	
	cout << "***TOTAL SUM: " << *((int *)data.addr) << "***" << endl;
	
	driver->dsm_free("SumOddNums");
	cout << "Sleeping for a second before finishing ..." << endl;
	sleep(1);
	delete driver;
}

