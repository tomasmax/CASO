// NameClient.cc
// author: dipina@eside.deusto.es

#include "NameServer.h"

void usage() {
	cout << "Usage: NameClient <server-port> <domain-name-to-resolve>" << endl;
	exit(1);
}

int main(int argc, char** argv) {

	if (argc != 3) {
		usage();
	}

	PracticaCaso::TcpClient client;
	client.connect("127.0.0.1", atoi(argv[1]));
	// Lookup in NameServer
	string dnsName = argv[2];
	client.send(dnsName);
	string ipAddressAndPort = client.receive();
	if (ipAddressAndPort.find("ERROR") == 0) {
		cout << "The DNS name " << dnsName << " could not be resolved." << endl;
	} else {
		cout << "The DNS name: " << dnsName << " corresponds to: " << ipAddressAndPort << endl;
	}
	client.close();
}

