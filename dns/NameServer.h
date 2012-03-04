// NameServer.h
// author: dipina@eside.deusto.es
#ifndef __NAMESERVER_H
#define __NAMESERVER_H

#include <fstream>
#include <iostream>
#include <map>
#include <sstream>

#include "TcpListener.h"
#include "Thread.h"
#include "SQLiteMap.h"

using namespace std;

namespace PracticaCaso
{
	class NameServer: public TcpListener {
		private:
			string domain;
			SQLiteMap * sqliteMap;
			map<string, string> dns2IpPortMap;
			bool leerCache;

			void loadMappings(string mappinsFileName);
			string delegateExternalDnsServer(string serverDetails, string dnsName);
		public:
			NameServer(int p, string m, bool leerCache);
			NameServer(const NameServer&);
			NameServer & operator = (const NameServer &);
			~NameServer();
			string translate(string dnsEntry);
			friend ostream & operator << (ostream &os, NameServer &n);

	};


	class NameServerThread: public Thread {
		private:
			TcpClient* dnsClient;
			NameServer& dnsServer;
			void run();
		public:
			NameServerThread(TcpClient* c, NameServer& s): dnsClient(c), dnsServer(s) {}
			~NameServerThread();
	};
};
#endif
