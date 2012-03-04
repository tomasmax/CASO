// mDNSTypes.h
// author: Dr. Pablo García Bringas <pgb@eside.deusto.es>

#ifndef __MDNSTYPES_H
#define __MDNSTYPES_H

#define MDNS_PORT 12346
#define MDNS_GROUP "225.0.0.38"
#define MDNS_TIMEOUT 3.0
#define MDNS_RESPONSE "MDNS_RESPONSE"
#define MDNS_REQUEST "MDNS_REQUEST"
#define MDNS_TIMEOUT_ERROR "MDNS_TIMEOUT_ERROR"
#define MDNS_COMMAND_LENGTH 16

#include "TcpListener.h"
#include <vector>

using namespace std;

//  maybe: typedef int mDNSNodeId;
//  maybe: typedef string mDNSException;

namespace PracticaCaso
{
	struct mDNSEvent {
		string cmd;
		// Security application-level identifier: string id;
		string payload;
	};
};

#endif
