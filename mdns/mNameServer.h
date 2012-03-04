// mNameServer.h
// author: Dr. Pablo García Bringas <pgb@eside.deusto.es>

#ifndef __MNAMESERVER_H
#define __MNAMESERVER_H

#include <fstream>
#include <iostream>
#include <map>
#include <sstream>

#include "TcpListener.h"
#include "Thread.h"
#include "SQLiteMap.h"
#include "mDNSTypes.h"

using namespace std;

namespace PracticaCaso
{
	class mNameServer;
	class mDNSObserver: public Thread {
		private:
			int fd4Receiving;
			bool keepRunning;
			struct sockaddr_in addr;
			void run();
			mNameServer* client;

		public:
			mDNSObserver(mNameServer* client);
			~mDNSObserver();
			void stop();
	};

	class mDNSQueryWrapper {
		private:
			int fd4Sending;
			struct sockaddr_in addr;
			mNameServer* client;

		public:
			mDNSQueryWrapper(mNameServer* client);
			~mDNSQueryWrapper();
			void send (string str);
	};

	class mNameServer: public TcpListener {
		private:
			string domain, pendingQuery, pendingQueryCode, solvedQuery;
			int randSeed;
			SQLiteMap * sqliteMap;
			map<string, string> dns2IpPortMap;
			bool leerCache, satisfiedQuery;
			void loadMappings(string mappinsFileName);
			string delegateExternalDnsServer(string serverDetails, string dnsName);
			string delegatemDNSCommunity(string dnsName);
			mDNSObserver* observer;
			mDNSQueryWrapper* queryWrapper;
 
		public:
			mNameServer(int p, string m, bool leerCache);
			mNameServer(const mNameServer&);
			mNameServer & operator = (const mNameServer &);
			~mNameServer();
			string mNameServer::translate(string dnsEntry);
			void mNameServer::mdns_management(string cmd, string payload, string code); 
			void mNameServer::mdns_manage_response(string cmd, string payload, string code); 
			void mNameServer::mdns_manage_request(string cmd, string payload, string code); 
			friend ostream & operator << (ostream &os, mNameServer &n);
	};

	class mNameServerThread: public Thread {
		private:
			TcpClient* dnsClient;
			mNameServer& dnsServer;
			void run();
		public:
			mNameServerThread(TcpClient* c, mNameServer& s): dnsClient(c), dnsServer(s) {}
			~mNameServerThread();
	};
};
#endif
