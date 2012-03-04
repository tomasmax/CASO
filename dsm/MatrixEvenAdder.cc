// MatrixEvenAdder.cc
// author: dipina@eside.deusto.es

#include "TcpListener.h"
#include "Dsm.h"

#define BUFFER_SIZE 1024

void usage() {
	cout << "Usage: MatrixEvenAdder <dsm-server-port>" << endl;
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

	int a[100];
	// Initialize an array with 100 integers and publish it in DSM under name "Array100Nums"
	try {
		driver->dsm_malloc("Array100Nums", sizeof(a));
		for (int i=0; i<100; i++) 
			a[i] = i;
		try {
			driver->dsm_put("Array100Nums", (void *)a, sizeof(a)); 
		} catch (DsmException dsme) {
			cerr << "ERROR: dsm_put(\"Array100Nums\", a, " << sizeof(a) << ")): " << dsme << endl;
			driver->dsm_free("Array100Nums");
			exit(1);
		}
	} catch (DsmException dsme) {
		// There may be several processes doing a dsm_malloc, only the first one will succeed 
		cerr << "ERROR in dsm_malloc(\"Array100Nums\", sizeof(" << sizeof(a) << ")): " << dsme << endl;
		exit(1);
	}
	

	// Calculate the sum of the even number indexed cells
	int sumEvenNums = 0;
	for (int i=0; i<100; i+=2) {
		sumEvenNums += a[i];
	}	
	cout << "\tSum of even nums: " << sumEvenNums << endl;
	
	// Wait till the sum of the odd number indexed cells has been published
	bool sumOddNumsGet = false;
	while (!sumOddNumsGet) {
		try {
			data = driver->dsm_get("SumOddNums");
			sumOddNumsGet = true;
		} catch (DsmException dsme) {
			cerr << "ERROR: dsm_get(\"SumOddNums\") - Waiting for other process to initialise it: " << dsme << endl;
			driver->dsm_wait("SumOddNums");
		}
	} 
	
	// Calculate the total sum and publish the result for 1 sec
	int sumOddNums = *((int *)data.addr);
	int totalSum = (sumOddNums+sumEvenNums);
	cout << "***TOTAL SUM: " << totalSum << "***" << endl;
	
	try {
		driver->dsm_malloc("TotalSum", sizeof(totalSum));
		try {
			driver->dsm_put("TotalSum", (void *)&totalSum, sizeof(totalSum));
			driver->dsm_free("Array100Nums");
		} catch (...) {
			cerr << "ERROR: dsm_put(\"TotalSum\", (void *)&totalSum, sizeof(totalSum))" << endl;
			driver->dsm_free("Array100Nums");
			exit(1);
		}
	} catch (DsmException dsme) {
		cerr << "ERROR in dsm_malloc(\"TotalSum\", sizeof(" << sizeof(totalSum) << ")): " << dsme << endl;
		driver->dsm_free("Array100Nums");
		exit(1);
	}
	
	cout << "Sleeping for a second before finishing ..." << endl;
	sleep(1);
	driver->dsm_free("TotalSum");
	delete driver;
}

