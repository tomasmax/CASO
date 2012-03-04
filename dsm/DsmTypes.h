// DsmTypes.h
// author: dipina@eside.deusto.es
#ifndef __DSMTYPES_H
#define __DSMTYPES_H

#define DSM_PORT 12345
#define DSM_GROUP "225.0.0.37"

#include "TcpListener.h"
#include <vector>

using namespace std;

typedef int DsmNodeId;
typedef string DsmException;

namespace PracticaCaso
{
	struct DsmEvent {
		string cmd;
		string blockId;
	};

	struct DsmData {
		void *addr;
		int size;
	};

	struct DsmBlock {
		string blockId;
		DsmNodeId creatorNode;
		DsmNodeId lastAccessNode;
		int blockSize;
		void *addr;
		int size;
	};

	struct DsmNodeMetadata {
		DsmNodeId nid;
		TcpClient* client;
		vector <DsmBlock> dsmBlocksRequested;
	};
};

#endif
