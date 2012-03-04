// Dsm.cc
// author: dipina@eside.deusto.es
#include "Dsm.h"
#define BUFFER_SIZE 1024

namespace PracticaCaso {
	DsmObserver::DsmObserver(DsmDriver *c): client(c), keepRunning(true) {
		struct ip_mreq mreq;
		
		u_int yes=1;           
		/* create what looks like an ordinary UDP socket */
		if ((fd=socket(AF_INET,SOCK_DGRAM,0)) < 0) {
			cerr << "socket" << endl;
			exit(1);
		}

		/* allow multiple sockets to use the same PORT number */
		if (setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(yes)) < 0) {
			cerr << "Reusing ADDR failed" << endl;
			exit(1);
		}


		/* set up destination address */
		memset(&addr,0,sizeof(addr));
		addr.sin_family=AF_INET;
		addr.sin_addr.s_addr=htonl(INADDR_ANY); /* N.B.: differs from sender */
		addr.sin_port=htons(DSM_PORT);

		/* bind to receive address */
		if (bind(fd,(struct sockaddr *) &addr,sizeof(addr)) < 0) {
			cerr << "bind" << endl;
			exit(1);
		}

		/* use setsockopt() to request that the kernel join a multicast group */
		mreq.imr_multiaddr.s_addr=inet_addr(DSM_GROUP);
		mreq.imr_interface.s_addr=htonl(INADDR_ANY);
		if (setsockopt(fd,IPPROTO_IP,IP_ADD_MEMBERSHIP,&mreq,sizeof(mreq)) < 0) {
			cerr << "setsockopt" << endl;
			exit(1);
		}
	}

	void DsmObserver::run() {
		int nbytes;
		socklen_t addrlen;
		char msgbuf[MSGBUFSIZE];
		memset(msgbuf,0,sizeof(msgbuf));
		/* now just enter a read-print loop */
		while (this->keepRunning) {
			addrlen=sizeof(addr);
			if ((nbytes=recvfrom(fd,msgbuf,MSGBUFSIZE,0,
					   (struct sockaddr *) &addr,&addrlen)) < 0) {
			   cerr << "recvfrom" << endl;
			   exit(1);
			}

			istringstream ins;
			ins.str(msgbuf);
			string command, blockId;
			int nid;
			ins >> command >> nid >> blockId;
			this->client->dsm_notify(command, blockId);
			memset(msgbuf,0,sizeof(msgbuf));
		}
	}

	void DsmObserver::stop() {
		this->keepRunning = false;
	}
	
	
	// MODIFICACIÓN PRÁCTICA DSM: descripción en 3.3.5 (punto 2): 
	// Nueva signature constructor: DsmDriver(string ipAddressNameServer, int portNameServer, string dmsServerName2Lookup); 
	DsmDriver::DsmDriver(string DSMServerIPaddress, int DSMServerPort) {
		// Lookup pop.deusto.es in NameServer
		this->observer = new DsmObserver(this);
		this->observer->start();

		this->connect(DSMServerIPaddress, DSMServerPort);
		this->send("dsm_init");
		this->nid = atoi((this->receive()).c_str());

		// Incluir el lookup en el servidor de nombres para encontrar dirección IP y puerto de dmsServerName2Lookup 
	}


	DsmDriver::~DsmDriver() {
		ostringstream outs;  // Declare an output string stream.
		outs << "dsm_exit " << this->nid;
		this->send(outs.str());
		string exitOK = this->receive();
		this->observer->stop();
		this->close();
	}

	DsmNodeId DsmDriver::get_nid() {
		return this->nid;
	}

	void DsmDriver::dsm_malloc(string blockId, int size) throw (DsmException) {
		ostringstream outs;  // Declare an output string stream.
		outs << "dsm_malloc " << this->nid << " " << blockId << " " << size;
		this->send(outs.str());
		string response = this->receive();
		if (response.find("ERROR") == 0) {
			throw DsmException(response);
		} else {
			istringstream ins;
			ins.str(response);
			unsigned long addr;
			ins >> addr;
		}
	}

	void DsmDriver::dsm_put(string blockId, void * content, int size) throw (DsmException) {
		ostringstream outs;  // Declare an output string stream.
		outs << "dsm_put " << this->nid << " " << blockId << " " << size << " ";
		for (int i=0; i<size; i++) {
			outs << ((char *)content)[i];
		}
		this->send(outs.str());
		string response = this->receive();
		if (response.find("ERROR") == 0) {
			throw DsmException(response);
		} 
	}

	DsmData DsmDriver::dsm_get(string blockId) throw (DsmException) {
		ostringstream outs;  // Declare an output string stream.
		outs << "dsm_get " << this->nid << " " << blockId;
		this->send(outs.str());
				
		char * content = NULL;
		long bytesRead = this->receive(content);
		DsmData data;
		data.size = bytesRead;
		data.addr = malloc(bytesRead);
		memcpy(data.addr, content, bytesRead);
		delete [] content;
		if (data.size >= 5) {
			char msg[6];
			memcpy(msg, data.addr, 5);
			msg[5] = '\0';
			if (strcmp(msg, "ERROR") == 0) {
				string response((char*)data.addr);
				delete [] (char*)data.addr;
				throw DsmException(response);
			}
		}
		return data;
	}


	void DsmDriver::dsm_free(string blockId) throw (DsmException) {
		ostringstream outs;  // Declare an output string stream.
		outs << "dsm_free " << this->nid << " " << blockId;
		this->send(outs.str());
		string response = this->receive();
		if (response.find("ERROR") == 0) {
			throw DsmException(string(response));
		}
	}


	void DsmDriver::dsm_notify(string cmd, string blockId) {
		// MODIFICACIÓN PRÁCTICA DSM: seguir indicaciones de 3.3.5 (punto 3)
		cout << "***NOTIFICATION: " << cmd << " " << blockId << endl;
		if (cmd == "dsm_put") {
			// Add the new DsmEvent received
			DsmEvent dsmEvent;
			dsmEvent.cmd = cmd;
			dsmEvent.blockId = blockId;
			this->putEvents.push_back(dsmEvent);
		} else if (cmd == "dsm_free") {
			for (vector<DsmEvent>::iterator it = this->putEvents.begin(); it!=this->putEvents.end(); ++it) {
				if ((it->cmd == "dsm_put") && (it->blockId == blockId)) {
					// We should wipe out all the dsm_puts received for a block (problem when removing break)
					this->putEvents.erase(it);
					// TODO: not remove the break
					break;
				}
			}
		}
	}

	void DsmDriver::dsm_wait(string blockId) {
		bool blockPutEventReceived = false;
		while (!blockPutEventReceived) {
			for (vector<DsmEvent>::iterator it = this->putEvents.begin(); it!=this->putEvents.end(); ++it) {
				if ((it->cmd == "dsm_put") && (it->blockId == blockId)) {
					blockPutEventReceived = true;
					this->putEvents.erase(it);
					break;
				}
			}
			if (!blockPutEventReceived) {
				// TODO: use binary semaphore initialized to 0 for conditional synchronisation
				// MODIFICACIÓN PRÁCTICA DSM: Seguir instrucciones de modificación 3.3.5.3
				sleep(1);
			}
		}
	}

	
	ostream & operator << (ostream &os, DsmDriver &n) {
		os << "nid: " << n.nid << endl;
		return os;
	}
}

