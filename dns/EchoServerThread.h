// EchoServerThread.h
// author: dipina@eside.deusto.es
#ifndef __ECHOSERVERTHREAD_H
#define __ECHOSERVERTHREAD_H

#include "Thread.h"
#include "TcpListener.h"

namespace PracticaCaso
{
	class EchoServerThread: public Thread {
		private:
			TcpClient* client;
			void run();
		public:
			EchoServerThread(TcpClient* c): client(c) {}
			~EchoServerThread();
	};
};
#endif
