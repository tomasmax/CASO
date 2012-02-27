// NameServer.cc
// author: dipina@eside.deusto.es

#include "NameServer.h"

extern "C" {
	#include </usr/include/signal.h>
}

namespace PracticaCaso {
	NameServer::NameServer(int p, string m, bool leerCache): TcpListener(p) {
		//cout << "Creating SQLiteMap " << endl;
		// Process the contents of the mapping file
		//this->sqliteMap = new SQLiteMap(m+"_cache.db");
		//cout << "Creating SQLiteMap!!!" << endl;
		this->leerCache = leerCache;
		cout << "Calling to loadMappings" << endl;
		this->loadMappings(m);
	}

	NameServer::NameServer(const NameServer& ns): TcpListener(ns.port) {
		domain = ns.domain;
		dns2IpPortMap = ns.dns2IpPortMap;
		server_socket = ns.server_socket;
		port = ns.port;
		//sqliteMap = ns.sqliteMap;
		leerCache = ns.leerCache;
	}

	NameServer::~NameServer() {
		//delete this->sqliteMap;
		cout << "NameServer destructor called" << endl;
	}


	NameServer& NameServer::operator = (const NameServer& rhs) {
		// check for identity
		if (this == &rhs) return *this;
		domain = rhs.domain;
		dns2IpPortMap = rhs.dns2IpPortMap;
		server_socket = rhs.server_socket;
		port = rhs.port;
		//sqliteMap = rhs.sqliteMap;
		leerCache = rhs.leerCache;
		return *this;
	}

	void NameServer::loadMappings(string mappingsFileName) {
		char line[1024];
		ifstream in(mappingsFileName.c_str());
		if(!in) {
			cout << "Cannot open file: " << mappingsFileName << endl;
			exit(1);
		}

		while (!in.eof()) {
			in.getline(line, 1024, '\n');
			// Process the line
			if (((string)line).find("#") == 0) {
				continue;
			}
			istringstream ist(line);
			string word[2];
			ist >> word[0];
			if (!(ist >> word[1])) {
				if (this->domain.length() == 0) {
					this->domain = word[0];
				}
			} else {
				this->dns2IpPortMap[word[0]] = word[1];
			}
		}
		in.close();

		// TODO: If there is a .DB file with previously learned mappings load them into dns2IpPortMap
	}

	string NameServer::delegateExternalDnsServer(string serverDetails, string dnsName) {
		string ipAddressServer;
		int portServer;
		if (serverDetails.find(":") >=0) {
			ipAddressServer = serverDetails.substr(0, serverDetails.find(":"));
			portServer = atoi((serverDetails.substr(serverDetails.find(":")+1)).c_str());
		}
		PracticaCaso::TcpClient clientDns;
		clientDns.connect(ipAddressServer, portServer);
		// Lookup in a new NameServer
		clientDns.send(dnsName);
		string ipAddressAndPort = clientDns.receive();
		if (ipAddressAndPort.find("ERROR") == 0) {
			cout << "The DNS name " << dnsName << " could not be resolved." << endl;
			ipAddressAndPort = "ERROR: The DNS name " + dnsName + " could not be resolved.";
		} else {
			cout << "The DNS name: " << dnsName << " corresponds to: " << ipAddressAndPort << endl;
		}
		clientDns.close();
		return ipAddressAndPort;
	}

	string NameServer::translate(string dnsName) {
		map<string, string>::iterator p;
		string dnsValue;

		p = (this->dns2IpPortMap).find(dnsName);
		if (p != this->dns2IpPortMap.end()) {
			dnsValue = p->second;
		} else {
			int npos = dnsName.rfind(this->domain);
			// If our domain is part of the dnsName passed
			if (npos>=0 && npos < dnsName.length()) {
				// redirect to one of the dns name server children
				typedef map<string, string>::const_iterator CI;
				for (CI p = (this->dns2IpPortMap).begin(); p != (this->dns2IpPortMap).end(); ++p) {
					if (p->first.length() > this->domain.length()) {
						npos =  (dnsName).rfind(p->first);
						if (npos>0 && (npos<dnsName.length())) {
							cout << "Child Name server to process request: " << p->first << endl;
							string ipPortTemp = delegateExternalDnsServer(p->second, dnsName);
							// TODO: cache the already resolved names in other DNS servers both in memory and sqlite3
							return ipPortTemp;
						}
					}
				}
				dnsValue = "ERROR: domain cannot be resolved in NS " + this->domain + ": " + dnsName;
			} else {
				string segment(dnsName);
				npos = segment.find(".");
				while (npos > 0) {
					segment = segment.substr(npos+1);
					if (this->dns2IpPortMap.find(segment) != this->dns2IpPortMap.end()) {
						cout << "Parent Name server to process request: " << segment << ": " << this->dns2IpPortMap[segment] << endl;
						string ipPortTemp = delegateExternalDnsServer(this->dns2IpPortMap[segment], dnsName);
						// TODO: cache the already resolved names in other DNS servers both in memory and sqlite3
						return ipPortTemp;
					} else {
						npos = segment.find(".");
					}
				}
				dnsValue = "ERROR: domain cannot be resolved in NS " + this->domain + ": " + dnsName;
			}
		}
		return dnsValue;
	}


	NameServerThread::~NameServerThread() {
		delete this->dnsClient;
	}

// MODIFICATION 2.3.6
	void NameServerThread::run() {
		// make the type casting and recuperate the parameters using "arg"
		string dnsEntry = (this->dnsClient)->receive();
		string ipAddressAndPort = (this->dnsServer).translate(dnsEntry);
		(this->dnsClient)->send(ipAddressAndPort);
		cout << "DNS resolution for: " << dnsEntry << " is: " << ipAddressAndPort << endl;
		(this->dnsClient)->close();
	}


	ostream & operator << (ostream & os, NameServer &t) {
		os << "server_socket: " << t.server_socket << " - port: " << t.port << " - domain: " << t.domain << endl;
		os << "DNS mappings:" << endl;
		typedef map<string, string>::const_iterator CI;
		for (CI p = t.dns2IpPortMap.begin(); p != t.dns2IpPortMap.end(); ++p) {
			os <<  p->first << " : " << p->second << endl;
		}
		return os;
	}
}


// global variable
PracticaCaso::NameServer * nameServer_pointer;

// function called when CTRL-C is pressed
void ctrl_c(int)
{
    printf("\nCTRL-C was pressed...\n");
	nameServer_pointer->stop();

}


void usage() {
	cout << "Usage: NameServer <port> <name-mappings-file>" << endl;
	exit(1);
}

/*
void processClientRequest(PracticaCaso::TcpClient *dnsClient, PracticaCaso::NameServer* dnsServer) {
	string dnsEntry = dnsClient->receive();
	string ipAddressAndPort = dnsServer->translate(dnsEntry);
	dnsClient->send(ipAddressAndPort);
	cout << "DNS resolution for: " << dnsEntry << " is: " << ipAddressAndPort << endl;
	dnsClient->close();
}
//processClientRequest(client, &nameServer);
*/

int main(int argc, char** argv) {
	signal(SIGINT,ctrl_c);

	if (argc != 3) {
		usage();
	}

	PracticaCaso::NameServer nameServer(atoi(argv[1]), (string)argv[2], false);
	cout << "NameServer instance: " << endl << nameServer << endl;
	// MODIFICATION 2.3.6
	nameServer_pointer = &nameServer;
	nameServer.start();
	while (true) {
		PracticaCaso::TcpClient *client = nameServer.acceptTcpClient();
		if (client) {
			//string dnsEntry = client->receive();
			//string ipAddressAndPort = nameServer.translate(dnsEntry);
			//client->send(ipAddressAndPort);
			//cout << "DNS resolution for: " << dnsEntry << " is: " << ipAddressAndPort << endl;
			//client->close();
			// MODIFICATION 2.3.6
			PracticaCaso::NameServerThread* t = new PracticaCaso::NameServerThread(client, nameServer);
			t->start();
		} else {
			break;
		}
	}
	// MODIFICATION 2.3.6
	nameServer.stop();
}
