// Thread.h
// author: dipina@eside.deusto.es
#ifndef __THREAD_H
#define __THREAD_H

extern "C" {
	#include <pthread.h>	
	#include <errno.h>
}
#include <iostream>
#include <cstdlib>

namespace PracticaCaso
{
	void *thread_listener(void *arg);
	class Thread {
	private:
			pthread_t thread_id;
		public:
			virtual ~Thread() {}
			virtual void run() = 0;
			void start();
			void stop();
	};
};
#endif
