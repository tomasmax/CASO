// DsmServer.h
// author: dipina@eside.deusto.es
#ifndef __DSM_H
#define __DSM_H

#include <map>
#include <sstream>
#include <vector>


extern "C" {
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <time.h>
	#include <string.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <unistd.h>
}

#include "TcpListener.h"
#include "Thread.h"
#include "DsmTypes.h"

using namespace std;

namespace PracticaCaso
{
	class DsmServer: public TcpListener {
		private:
			int lastNid;
			int nidCounter;
			int nodeCounter;
			map<DsmNodeId, DsmNodeMetadata> dsmNodeMap;
			map<string, DsmBlock> blockMetadataMap; 
			// TODO: declare variable of type pthread_rwlock_t 

			void dsm_notify(string message);
		public:
			DsmServer(int port);
			~DsmServer();
			
			DsmNodeId dsm_init(TcpClient *);
			void dsm_exit(DsmNodeId);
			
			void * dsm_malloc(DsmNodeId nid, string blockId, int size);
			bool dsm_put(DsmNodeId nid, string blockId, void * content, int size);
			
			void dsm_notify_put(DsmNodeId nid, string blockId);
			void dsm_notify_free(DsmNodeId nid, string blockId);

			DsmBlock dsm_get(DsmNodeId nid, string blockId); 
			bool dsm_free(DsmNodeId nid, string blockId);

			friend ostream & operator << (ostream &os, DsmServer &n);

	};


	class DsmServerThread: public Thread {
		private:
			TcpClient* dsmClient;
			DsmServer& dsmServer;
			void run();
		public:
			DsmServerThread(TcpClient* c, DsmServer& dms): dsmClient(c), dsmServer(dms) {}
			~DsmServerThread();
	};

	class DsmNotifierThread: public Thread {
		private:
			// Multicast notification file descriptor and address
			int fd;
			struct sockaddr_in addr;

			string message;
			void run();
		public:
			DsmNotifierThread(string msg);
			~DsmNotifierThread();
	};
};
#endif
