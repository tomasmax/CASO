// TcpListener.h
// author: dipina@eside.deusto.es
#ifndef __TCPLISTENER_H
#define __TCPLISTENER_H

#define BUFFER_SIZE 1024

extern "C" {
	#include <sys/socket.h>
	#include <sys/types.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <netdb.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include </usr/include/signal.h>
	#include <errno.h>
	#include <string.h>
	#include <strings.h>
	#include <unistd.h>
}

#include <iostream>
#include <string>

using namespace std;

namespace PracticaCaso
{
	class TcpClient {
		private:
			int client_socket;
			string ipAddress;
			int port;

		public:
			TcpClient() {}
			~TcpClient(){}
			TcpClient(int c, string i, int p): client_socket(c), ipAddress(i), port(p) {}
			void connect(string ipAddress, int port);
			string receive();
			long receive(char* &dataReceived);
			void send(string msg);
			void send(const char* buffer, long bytes2Send);
			void close();
			friend ostream & operator << (ostream &os, TcpClient &t);
	};

	class TcpListener {
		protected:
			int server_socket;
			int port;
		public:
			TcpListener(int p);
			~TcpListener();
			void start();
			void stop();
			TcpClient *acceptTcpClient();
			friend ostream & operator << (ostream &os, TcpListener &t);
	};
};
#endif
