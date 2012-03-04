// TcpListener.cc
// author: dipina@eside.deusto.es
#include "TcpListener.h"

namespace PracticaCaso
{

	void TcpClient::connect(string ipAddress, int port) {
		//create the descriptor to client socket
		//create the structure to store the address of client and server
		struct sockaddr_in client_addr, server_addr;
		//create the socket and return the descriptor suitable to this socket
		this->client_socket = socket(AF_INET, SOCK_STREAM,IPPROTO_TCP);	
		// AF_INET is defined in /usr/include/bits/socket.h
		// SOCK_STREAM is defined in /usr/include/bits/socket.h
		// IPPROTO_TCP is defined in /usr/include/netinet/in.h
		//check if the socket was created
		if (this->client_socket < 0 ) {
			cerr << "[aborted] [error - failure in the creation of socket]" << endl;
            		exit(1);
		}

		// initialize the client address and port to bind with this socket
		bzero( (char *) &client_addr, sizeof(client_addr));
		client_addr.sin_family = AF_INET;
		client_addr.sin_addr.s_addr = htonl(INADDR_ANY);
		this->ipAddress = (string)inet_ntoa(client_addr.sin_addr);
		this->port = client_addr.sin_port = htons(0);
		//make the bind
		if( bind(this->client_socket, (struct sockaddr *) &client_addr, sizeof(client_addr) ) < 0 )
		{
			cerr << "[aborted] [error - failure in the socket binding]" << endl;
			exit(1);
		}
		cout << "Client using port " << ntohs(client_addr.sin_port) << " connecting to " << ipAddress << ":" << port << endl;
		//initialize the server address and server port to connect
		bzero( ( char *) &server_addr, sizeof(server_addr));
		server_addr.sin_family = AF_INET;
		server_addr.sin_addr.s_addr = inet_addr (ipAddress.c_str());
		server_addr.sin_port = htons(port);
		
		if(::connect(this->client_socket,(struct sockaddr *)&server_addr,sizeof(server_addr) ) < 0 ) {
			cerr << "[aborted] [error - failure in the connection]" << endl;
			exit(1);
		}
	}

	string TcpClient::receive() {
		char * responseBuffer;
		long bytesRead = this->receive(responseBuffer);
		char *temp = new char[bytesRead+1];
		memcpy(temp, responseBuffer, bytesRead);
		temp[bytesRead] = '\0';
		string response = temp;
		delete []responseBuffer;
		delete []temp;
		return response;
	}


	long TcpClient::receive(char* &responseBuffer) {
		int n = 0;
		char * responseReceived = NULL;
		char buffer[BUFFER_SIZE]; // the buffer to read from a socket
		long bytesReceived = 0;
		
		// recover the size of the buffer to read
		bool dollarFound = false;
		int sizeDigitsReceived = 0;
		do {
			n = read(this->client_socket, buffer+sizeDigitsReceived, 1);
			if (buffer[sizeDigitsReceived] == '$') {
				dollarFound = true;
			} else if (dollarFound) {
				sizeDigitsReceived++;
			}
		} while (!dollarFound || (dollarFound && buffer[sizeDigitsReceived-1] != '*'));
		buffer[sizeDigitsReceived-1] = '\0';
		int bytes2Read = atoi(buffer);
		do {
			if ((n=read(this->client_socket, buffer, BUFFER_SIZE)) == -1) {
				if (errno == EINTR) {
					cerr << "RECV: Interruption detected ..." << endl;
					n=BUFFER_SIZE;
					continue;
				} else {
					cerr << "[error - failure in the receive]" << endl;
					exit(1);
				}
			}
			if (responseReceived == NULL) {
				responseReceived = new char[n];
				memcpy(responseReceived+bytesReceived, buffer, n);
			} else {
				char * temp = new char[bytesReceived+n];
				memcpy(temp, responseReceived, bytesReceived);
				memcpy(temp+bytesReceived, buffer, n);
				delete []responseReceived;
				responseReceived = temp;
			}
			
			bytesReceived += n;
		} while (bytesReceived < bytes2Read);
		responseBuffer = responseReceived;
		return bytesReceived;
	}

	void TcpClient::send(string msg) {
		this->send(msg.c_str(), msg.length());
	}
	

	void TcpClient::send(const char* buffer, long bytes2Send) {
		long bytesSent = 0;
		
		// Before writting the content, write the size of the buffer sent
		char bytes2SendBuf[50];
		int n=sprintf (bytes2SendBuf, "$%ld*", bytes2Send);
		write(this->client_socket, string(bytes2SendBuf).c_str(), n);
		do {
			int bytesSentThisTime = write(this->client_socket, buffer+bytesSent, bytes2Send-bytesSent);
			if (bytesSentThisTime < 0) {
				if (errno == EINTR) {
					cerr << "SEND: Interruption detected ..." << endl;
					continue;
				} else {
					cerr << "[aborted] Client>error in send..." << endl;
					exit(1);
				}
			} else {
				bytesSent += bytesSentThisTime;
			}
		} while (bytesSent < bytes2Send);
		//write(this->client_socket, "\0", 1);
	}


	void TcpClient::close() {
		if( shutdown(this->client_socket, SHUT_RDWR) == 0)
		{
			//cout << "Client>Close: (1) shutdown was executed " << this->client_socket << endl;
		}else
		{
			cerr << "Error in shutdown " << this->client_socket << endl;
		}
		::close(this->client_socket); // close the socket
		//cout << "Client>Close: (2) close was executed " << this->client_socket << endl;
	}

	TcpListener::TcpListener(int p): server_socket(-1), port(p) {
		int addr_len=0;
		this->server_socket = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);	// AF_INET is defined in /usr/include/bits/socket.h
																	// SOCK_STREAM is defined in /usr/include/bits/socket.h
																	// IPPROTO_TCP is defined in /usr/include/netinet/in.h
		//check if the socket was created
		if (this->server_socket < 0) {
			cerr << "[aborted] [error - failure in socket creation]" << endl;
			exit(1);
		}

		//create the structure to store the server address
		struct sockaddr_in server_addr; //struct sockaddr_in is defined at usr/include/netinet/in.h
		// initialize the local address and port to bind with this socket
		bzero( (char *) &server_addr, sizeof(server_addr));
		server_addr.sin_family = AF_INET; // AF_INET is defined at usr/include/bits/socket.h
		server_addr.sin_addr.s_addr = htonl(INADDR_ANY); //INADDR_ANY is defined at /usr/include/netinet/in.h
		server_addr.sin_port = htons((u_short) this->port);
		//
		//make the binding
		cout << "Server binding the socket ..." << endl;
		if ( bind(this->server_socket, (struct sockaddr *) &server_addr, sizeof(server_addr) ) < 0) {
			cerr << "[aborted] [error - failure in the binding]" << endl;
			exit(1);
		}
	}
	
	TcpListener::~TcpListener() {
		this->stop();
	}
				
	

	void TcpListener::start() {
		// make the listening
		cout << "Server executing the listen..." << endl;
		if (listen(this->server_socket, SOMAXCONN) < 0) { //SOMAXCONN is defined in /usr/include/bits/socket.h 
			cerr << "[aborted] [error - failure in the listening!]" << endl;
			exit(1);
		}
		cout << "[OK] number of queued connections=" << SOMAXCONN << endl;
	}


	void TcpListener::stop() {
		//shutdown the connection
		if( shutdown(this->server_socket, SHUT_RDWR) == 0)
		{
			cout << "Shutdown was executed " << this->server_socket << endl;
		}
		else
		{
			cerr << "Error in shutdown " << this->server_socket << endl;
		}
		::close(this->server_socket); //close the socket
	}
	
	TcpClient* TcpListener::acceptTcpClient() {
		//prepare to accept connections
		//create a socket descriptor to store the descriptor of a new socket returned of te method 'accept'
		int client_socket=0;	// stores the descriptor of the socket connection returned by
							//"int accept (int __fd, __SOCKADDR_ARG __addr, socklen_t *__restrict __addr_len)"
		//create a client address to store the address of the client
		struct sockaddr_in addr_client;
		//initialize the client address
		socklen_t addr_client_len=sizeof(addr_client);
		bzero( (char *) &addr_client, sizeof(addr_client) );
		cout << "Server waiting for connection in the socket " << this->server_socket << endl;
		client_socket = accept(this->server_socket,(struct sockaddr *) &addr_client,&addr_client_len);
		// check if this new socket (client_socket) has a good descriptor
		if (client_socket < 0)
		{
			cerr << "[aborted]" << endl << "[error - failure in the accept]" << endl;
			this->stop();
			return 0;
		}
		cout << "Server>socket " << client_socket << " was linked with the new connection stabilished with " << (string)inet_ntoa(addr_client.sin_addr) << " " << ntohs(addr_client.sin_port) << endl;
		return new TcpClient(client_socket, (string)inet_ntoa(addr_client.sin_addr), ntohs(addr_client.sin_port));
	}

	ostream & operator << (ostream & os, TcpListener &t) {
		os << "server_socket: " << t.server_socket << " - port: " << t.port << endl;
		return os;
	}

	ostream & operator << (ostream & os, TcpClient &t) {
		os << "client_socket: " << t.client_socket << " - ipAddress: " << t.ipAddress << " - port: " << t.port << endl;
		return os;
	}
}
