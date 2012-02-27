// EchoTcpClient.cc
// author: dipina@eside.deusto.es

#include "TcpListener.h"

int main(int argc, char *argv[]) {
/*
	// MODIFICATION 2.3.3
		PracticaCaso::TcpClient * client = new PracticaCaso::TcpClient();
		client->connect("127.0.0.1", 4321);
		string msg = "¡Hello CASO students!";
		client->send(msg);
		cout << "Message sent: " << msg << endl;
		msg = client->receive();
		cout << "Message received: " << msg << endl;
		client->close();	
		delete client;
*/
	// MODIFICATION 2.3.5
//	for (int i=0; i<100; i++) {
		PracticaCaso::TcpClient * client = new PracticaCaso::TcpClient();
		cout << "Petición: " << i << endl;
		
		// REMINDER GNU/LINUX: export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/itzi/src/util
		// REMINDER Windows-Cygwin: export PATH=$PATH:/home/itzi/src/util
		
		// MODIFICACIÓN 2.3.7.
		// Conexión al NameServer "echo.deusto.es"
		// send(), recibimos: miramos respuesta(comprobar si es diferente de error), troceamos y pasamos en el connect() ip y puerto
		
		// Connect to NameServer(CONNECT)
		// Look for "echo.deusto.es" (SEND)
		// (RECEIVE)
		// if (ERROR) -> Error Message
		// else -> turn "IP:PORT" string into IP and PORT variables 
		
		
		
		//CORRECCIONES
		//MEJOR HACER 2 CLIENTES LISTENERS
		//MEJOR PASAR IP Y PORT POR PARAMETROS.
		
		
		client->connect("127.0.0.1", 1234);
		string dnsName = argv[1];
		client->send(dnsName);
		cout << "Message sent: " << dnsName << endl;
		string ipAddressAndPort = client->receive();
		cout << "Message received: " << ipAddressAndPort << endl;
		
		string ipAddress;
		int port;
		if (ipAddressAndPort.find("ERROR") == 0 {
			cout << "ERROR" << endl;
		}
		else 
		{
			ipAddress = ipAddressAndPort.substr(0, ipAddressAndPort.find(":"));
			port = atoi((ipAddressAndPort.substr(ipAddressAndPort.find(":")+1)).c_str());
		}
		client->close();
		
		
		client->connect(ipAddress, port);
		string msg = "¡Hello CASO students!";
		client->send(msg);
		cout << "Message sent: " << msg << endl;
		msg = client->receive();
		cout << "Message received: " << msg << endl;
		client->close();	
		delete client;
	}
}

