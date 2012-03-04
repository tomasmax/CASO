// EchoTcpListener.cc
// author: dipina@eside.deusto.es

#include "TcpListener.h"
#include "EchoServerThread.h"

extern "C" {
	#include </usr/include/signal.h>
}

// global variable
PracticaCaso::TcpListener * listener_pointer;

// function called when CTRL-C is pressed
void ctrl_c(int)
{
    cout << "CTRL-C was pressed..." << endl;
	listener_pointer->stop();
}

/*
void *thread_listener(void *arg) {
	PracticaCaso::TcpClient *c = (PracticaCaso::TcpClient *)arg;
	string msg = c->receive();
	cout << "Message received: " << msg << endl;
	c->send(msg);
	cout << "Message envidado: " << msg << endl;
	c->close();
}*/



int main() {
	signal(SIGINT,ctrl_c);
	PracticaCaso::TcpListener listener(4321);
	listener_pointer = &listener;
	cout << "TcpListener created: " << listener << endl;
	listener.start();

	while (true) {
		PracticaCaso::TcpClient* client = listener.acceptTcpClient();
		
		//string msg = client->receive();
		//client->send(msg);
		//client->close();
		
		// MODIFICATION 2.3.5
		PracticaCaso::EchoServerThread* t = new PracticaCaso::EchoServerThread(client);
		t->start();
		
		/*
		pthread_t thread_id;
		int status_listener = pthread_create(&thread_id, NULL, thread_listener, client);
		// check if the thread wass well created
		if(status_listener==0) {
			cout << "Thread was correctly initialized" << endl;
		} else {
			cout << "[aborted]\n" << "[error - initialising thread]" << endl;
			exit(1);
		}*/
	}
	cout << "Finishing server ..." << endl;
	listener.stop();
}
