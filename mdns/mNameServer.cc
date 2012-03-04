// mNameServer.cc
// author: Dr. Pablo García Bringas <pgb@eside.deusto.es>
// Modifications by dipina@eside.deusto.es: 


#include "mNameServer.h"

#define MSGBUFSIZE 1024

extern "C" {
	#include </usr/include/signal.h>
}

namespace PracticaCaso {
	mNameServer::mNameServer(int p, string m, bool leerCache): TcpListener(p) {
		// Process the contents of the mapping file
		this->sqliteMap = new SQLiteMap(m+"_cache.db");
		this->leerCache = leerCache;
		this->loadMappings(m);
		// For global event observation, you must activate the mDNSObserver thread here.
		// But... for local event observation, you ought to use one different observer for each mDNS request.
		this->observer = new mDNSObserver (this);
		this->observer->start();
		// Create the mDNSQueryWrapper.
		this->queryWrapper = new mDNSQueryWrapper (this);
	}

	mNameServer::mNameServer(const mNameServer& ns): TcpListener(ns.port) {
		domain = ns.domain;
		dns2IpPortMap = ns.dns2IpPortMap;
		server_socket = ns.server_socket;
		port = ns.port;
		sqliteMap = ns.sqliteMap;
		leerCache = ns.leerCache;
		// Feed the random number generator with some real random data: system time, por instance.
		randSeed = (int)time(0);
	}

	mNameServer::~mNameServer() {
		delete this->sqliteMap;
		delete this->observer;
	}

	mNameServer& mNameServer::operator = (const mNameServer& rhs) {
		// Check for identity.
		if (this == &rhs) return *this;
		domain = rhs.domain;
		dns2IpPortMap = rhs.dns2IpPortMap;
		server_socket = rhs.server_socket;
		port = rhs.port;
		sqliteMap = rhs.sqliteMap;
		leerCache = rhs.leerCache;
		return *this;
	}

	void mNameServer::loadMappings(string mappingsFileName) {
		char line[1024];
		ifstream in(mappingsFileName.c_str());
		if(!in) {
			cout << "Cannot open file: " << mappingsFileName << endl;
			exit(1);
		}

		while (!in.eof()) {
			in.getline(line, 1024, '\n');
			// Process the line.
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

		if (leerCache == true) {
			// Retrieve all the mapping previously saved.
			typedef map<string, string>::const_iterator CI;
			map<string, string> cachedSqliteMap = this->sqliteMap->getMap();
			for (CI p = cachedSqliteMap.begin(); p != cachedSqliteMap.end(); ++p) {
				this->dns2IpPortMap[p->first] = p->second;
				cout << "CACHED dns2IpPortMap[" << p->first << "] = " << p->second << endl;
			}
		} 
	}
	
	mDNSQueryWrapper::mDNSQueryWrapper(mNameServer* c): client(c) {
		// Setup the mDNSClient multicast socket. Something similar to an UDP standard socket.
		// Once configured, you only need to call queryWrapper->send(string str) method.
		if ((fd4Sending=socket(AF_INET,SOCK_DGRAM,0)) < 0) {
			cerr << "Problems creating sending multicast socket" << endl;
			exit(1);
		}
		// Set up multicast group destination address for sending. (It's the same than UDP datagram sending, with an special destination address.)
		memset(&addr,0,sizeof(addr));
		addr.sin_family=AF_INET;
		addr.sin_addr.s_addr=inet_addr(MDNS_GROUP);
		addr.sin_port=htons(MDNS_PORT);
	}

	void mDNSQueryWrapper::send(string str) {
		int result=0;
		// Send the requests through the multicast UDP socket.
		result=sendto(fd4Sending, str.c_str(),str.size(),0,(struct sockaddr *) &addr, sizeof(addr));
		if (result < 0) {
			cerr << "ERROR: sendto in delegatemDNSCommunity()" << endl;
			perror("hola");
			exit(1);
		}
	}

	mDNSObserver::mDNSObserver(mNameServer* c): client(c), keepRunning(true) {
		struct ip_mreq mreq;
		u_int yes=1;
		// Set up multicast group destination address for receiving.
		if ((fd4Receiving=socket(AF_INET,SOCK_DGRAM,0)) < 0) {
			cerr << "Problems creating receiving multicast socket" << endl;
			exit(1);
		}
		// Trick just to allow multiple sockets to use the same PORT number: Only to try several processes in the same machine.
		if (setsockopt(fd4Receiving,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(yes)) < 0) {
			cerr << "Problems: Reusing ADDR failed" << endl;
			exit(1);
		}
		// Set up destination address.
		memset(&addr,0,sizeof(addr));
		addr.sin_family=AF_INET;
		addr.sin_addr.s_addr=htonl(INADDR_ANY); /* N.B.: differs from sender */
		addr.sin_port=htons(MDNS_PORT);
		// Bind to receive address.
		if (bind(fd4Receiving,(struct sockaddr *) &addr,sizeof(addr)) < 0) {
			cerr << "Problems with bind for multicast UDP socket" << endl;
			exit(1);
		}
		// Use setsockopt() to request that the kernel join a multicast group.
		mreq.imr_multiaddr.s_addr=inet_addr(MDNS_GROUP);
		mreq.imr_interface.s_addr=htonl(INADDR_ANY);
		if (setsockopt(fd4Receiving,IPPROTO_IP,IP_ADD_MEMBERSHIP,&mreq,sizeof(mreq)) < 0) {
			cerr << "Problems with setsockopt setup" << endl;
			exit(1);
		}
	}

	void mDNSObserver::run() {
		int nbytes,addrlen;
		char msgbuf[MSGBUFSIZE];
		cout << "mDNSObserver: running" << endl;
		memset(msgbuf,0,sizeof(msgbuf));
		// Now just enter a read-print loop.
		while (this->keepRunning) {
			addrlen=sizeof(addr);
			// Read one multicast UDP packet.
			if ((nbytes=recvfrom(fd4Receiving,msgbuf,MSGBUFSIZE,0, (struct sockaddr *)&addr,
																						 (socklen_t*)&addrlen)) < 0) {
			   cerr << "recvfrom" << endl;
			   exit(1);
			}
			cout << "mDNSObserver: event received: " << msgbuf << " " << endl;
			// Process the multicast UDP messages with the utility mdns_management function.
			istringstream ins;
			ins.str(msgbuf);
			string command, payload, code; // Payload can be one dnsName string or one serverDetails string.
			// Parse a little bit event parameters.
			// Three parameters: Command, Payload [dnsName or IpPort string], and random verification-code.
			ins >> command >> payload >> code;
			
		}	
	}

	void mDNSObserver::stop() {
		this->keepRunning = false;
	}

	mDNSObserver::~mDNSObserver() {
		this->stop();
	}

	void mNameServer::mdns_management(string cmd, string payload, string code) {
		map<string, string>::iterator p;
		string dnsValue;
		// Begin management utility function.

	}

	void mNameServer::mdns_manage_response(string cmd, string payload, string code) {
	cout << "mdns_management: MDNS_RESPONSE received" << endl;

	// Check if there is any pending query.
	// The arrived MDNS_RESPONSE may be addressed to me.
	// And then check if the MDNS_RESPONSE corresponds to pendingQuery. Use random code.
	// satisfiedQuery establishes a default FIRST-FIT criterion. Other methods are welcome. */
	// Yes, there was a MDNS_RESPONSE, but not for me. This MDNS_RESPONSE can flow, or it  can crash... be the mdns_response, my friend. 
	// It they don't come to me, snoopy cache can be implemented for efficiency. 
	// Warning! Man-in-the-middle poisoning attacks enabling.
	// Query cache
	// More MDNS_RESPONSES can come to me, or not to me. First approach, ignore them.
	// If they come to me, combination methods can be accomplished for completion.
	// It they don't come to me, snoopy cache can be implemented for efficiency. 
	// Warning! Man-in-the-middle poisoning attacks enabling.
	// Query cache

	}
			
	void mNameServer::mdns_manage_request(string cmd, string payload, string code) {
	map<string, string>::iterator p;
	string dnsValue;

	// One MDNS_REQUEST received: you must lookup your table and answer or not.
	// Lookup the local table. RFC doesn't recommend recursive looking up.
	// If the requested dnsName is in the local table, response. If don't, not to.
	// Send the good MDNS_RESPONSE.
	// If the requested dnsName is not in the local table, don't do anything.
	// It can be interesting to use a MDNS_ERROR RESPONSE, but with some overhead.	

	}

	string mNameServer::delegatemDNSCommunity(string dnsName) {
		// Several temporization parameters.
		clock_t timeStamp1, timeStamp2;
		bool tooMuchTime=false;
		double elapsedTime=0.0;
		// Initialize query parameters.
		satisfiedQuery=false;
		solvedQuery="";
		pendingQuery=dnsName;
		ostringstream temp;
		// Prepare the random number generator initial seed. Por example, system time.
		temp << (int)rand()%1024; // Certain control, numbers between 0 and 1023. This can be changed.
		pendingQueryCode=temp.str();
		cout << "delegatemDNSCommunity: generating random code...ok: code=" << pendingQueryCode << " " << endl;
		// Send the request using the mDNSQueryWrapper.
		ostringstream query;
		// Build the MDNS_REQUEST. Three parameters: Command, dnsName, and random verification code.
		query << MDNS_REQUEST << " " << dnsName << " " << pendingQueryCode;
		cout << "delegatemDNSCommunity: entering queryWrapper->send(query.str()): " << query.str() << " " << endl;
		queryWrapper->send(query.str());
		// Wait for one or many MDNS_RESPONSES.
		// Active waiting is only the first approach. Control the elapsed time with two timestamps.
		timeStamp1=time(0);
		cout << "delegatemDNSCommunity: entering MDNS_RESPONSES waiting loop. TimeStamp1: " << timeStamp1 << " " << endl;
		while (!satisfiedQuery && !tooMuchTime) {
			timeStamp2=time(0);
			cout << "delegatemDNSCommunity: Waiting MDSN_RESPONSES. TimeStamp2: " << timeStamp2 << ", tooMuchTime: " << tooMuchTime << " " << endl;			
			elapsedTime=difftime(timeStamp2, timeStamp1);
			if (elapsedTime>=MDNS_TIMEOUT) {
				tooMuchTime=true;
				if (!satisfiedQuery) solvedQuery="MDNS_TIMEOUT ERROR";
			}
			sleep(1);
		}
		// And return the resulting ipAddressAndPort string.
		return(solvedQuery);
	}

	string mNameServer::delegateExternalDnsServer(string serverDetails, string dnsName) {
		string ipAddressServer;
		int portServer;
		if (serverDetails.find(":") >=0) {
			ipAddressServer = serverDetails.substr(0, serverDetails.find(":"));
			portServer = atoi((serverDetails.substr(serverDetails.find(":")+1)).c_str());
		}
		PracticaCaso::TcpClient clientDns;
		clientDns.connect(ipAddressServer, portServer);
		// Lookup in mNameServer
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

	string mNameServer::translate(string dnsName) {
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
							// cache the already resolved names in other DNS servers
							string ipPortTemp = delegateExternalDnsServer(p->second, dnsName);
							// Caching
							this->dns2IpPortMap[dnsName] = ipPortTemp;
							// Checkpointing
							this->sqliteMap->set(dnsName, ipPortTemp);
							return this->dns2IpPortMap[dnsName];
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
						// cache the already resolved names in other DNS servers
						string ipPortTemp = delegateExternalDnsServer(this->dns2IpPortMap[segment], dnsName);
						// Caching
						this->dns2IpPortMap[dnsName] = ipPortTemp;
						// Checkpointing
						this->sqliteMap->set(dnsName, ipPortTemp);
						return this->dns2IpPortMap[dnsName];
					} else {
						npos = segment.find(".");
					}
				}
			}
			// If it's not possible to solve dnsName in the normal way, then let's multicast.
			cout << "translate: entering delegatemDNSCommunity" << endl;
			// Query delegation. 
			dnsValue = delegatemDNSCommunity(dnsName);
			// Check eventual errors.
			if (dnsValue.find("ERROR")==0) {
				dnsValue = "ERROR: domain cannot be resolved in NS " + this->domain + ": " + dnsName;
			} else {
				// If everything is ok, query caching and checkpointing.
				this->dns2IpPortMap[dnsName] = dnsValue;
				this->sqliteMap->set(dnsName, dnsValue);
			}
		}
		return dnsValue;
	}

	mNameServerThread::~mNameServerThread() {
		delete this->dnsClient;
	}

	void mNameServerThread::run() {
		// make the type casting and recuperate the parameters using "arg"
		string dnsEntry = (this->dnsClient)->receive();
		string ipAddressAndPort = (this->dnsServer).translate(dnsEntry);
		(this->dnsClient)->send(ipAddressAndPort);
		cout << "DNS resolution for: " << dnsEntry << " is: " << ipAddressAndPort << endl;
		(this->dnsClient)->close();
	}

	ostream & operator << (ostream & os, mNameServer &t) {
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
PracticaCaso::mNameServer * nameServer_pointer;

// function called when CTRL-C is pressed
void ctrl_c(int)
{
    printf("\nCTRL-C was pressed...\n");
	nameServer_pointer->stop();

}

void usage() {
	cout << "Usage: mNameServer <port> <name-mappings-file> [leer-fichero-cache --> true | false]" << endl;
	exit(1);
}

/*
void processClientRequest(PracticaCaso::TcpClient *dnsClient, PracticaCaso::mNameServer* dnsServer) {
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

	if (argc != 3 && argc != 4) {
		usage();
	}

	 
	bool leerCache = true;
	if ((argc == 4) && (string(argv[3]) == "false")) {
		leerCache = false;
	} 

	PracticaCaso::mNameServer nameServer(atoi(argv[1]), (string)argv[2], leerCache);
	cout << "mNameServer instance: " << endl << nameServer << endl;
	nameServer_pointer = &nameServer;
	nameServer.start();
	while (true) {
		PracticaCaso::TcpClient *client = nameServer.acceptTcpClient();
		PracticaCaso::mNameServerThread* t = new PracticaCaso::mNameServerThread(client, nameServer);
		t->start();
	}
	nameServer.stop();
}
