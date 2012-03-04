// Thread.h
// author: dipina@eside.deusto.es
#ifndef __MYSH_H
#define __MYSH_H

#include <iostream>
#include <string>
#include <cstdlib>

extern "C" {
	#include <errno.h>
	#include <stdio.h>
}

using namespace std;

namespace PracticaCaso {
	class MyShell {
		public:
			static string execute(string cmd);
	};
}

#endif
