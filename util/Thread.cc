// Thread.cc
// author: dipina@eside.deusto.es
#include "Thread.h"

using namespace std;

namespace PracticaCaso
{
	void *thread_listener(void *arg) {
		Thread *t = (Thread *)arg;
		t->run();
		t->stop();
		delete t;
	}


	void Thread::stop(){
		pthread_join(thread_id,NULL);
	}


	void Thread::start() {
		int status_listener = pthread_create(&thread_id, NULL, thread_listener, this);
		// It allows an application to inform the threads implementation of its desired concurrency level,
		pthread_setconcurrency(1000);
		// check if the thread wass well created
		if(status_listener!=0) {
			cerr << "[aborted]\n" << "[error - initialising thread] "<< endl;
			switch(status_listener){
				case EAGAIN: cerr << "EAGAIN error" << endl;
					     break;
				case EPERM: cerr << "EPERM error" << endl;
					     break;
				case EINVAL: cerr << "EINVAL error" << endl;
					     break;
				case ENOMEM: cerr << "ENOMEM error" << endl;
					     break;
			}
			exit(1);
		} else {
			// It is used to indicate to the implementation  that storage for the thread thread can be reclaimed when the thread terminates
			pthread_detach (thread_id);
		}
	}
};

