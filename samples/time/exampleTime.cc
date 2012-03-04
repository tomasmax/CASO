// Compile: g++ exampleTime.cc -o exampleTime
// Run: ./exampleTime
extern "C" {
	#include <sys/time.h>
}

#include <iostream>

using namespace std;

int main() {
	// How to get sytem time
	struct timeval first, second, lapsed;
	struct timezone tzp;
	gettimeofday (&first, &tzp);

	/* lapsed time */
	gettimeofday (&second, &tzp);
	if (first.tv_usec > second.tv_usec) {
		second.tv_usec += 1000000;
        second.tv_sec--;
	}
	lapsed.tv_usec = second.tv_usec - first.tv_usec;
	lapsed.tv_sec = second.tv_sec - first.tv_sec;
	
	cout << "lapsed.tv_usec: " << lapsed.tv_usec << endl;
	cout << "lapsed.tv_sec: " << lapsed.tv_sec << endl;
	
	// How to set sytem time
	settimeofday(&first, &tzp);
	
	// How to print out the current time in console
	char buffer[100];
	struct timeval tv;
	time_t curtime;

	gettimeofday(&tv, NULL); 
	curtime=tv.tv_sec;
    strftime(buffer,100,"%d-%m-%Y, %H:%M:%S",localtime(&curtime));
	cout << "new system time set " << buffer << endl;
}
