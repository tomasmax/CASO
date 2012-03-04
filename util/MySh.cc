// MySh.cc
// author: dipina@eside.deusto.es
#include "MySh.h"
//#include "popen.h"

namespace PracticaCaso {
	string MyShell::execute(string cmd) {
		FILE * pCmd = popen(cmd.c_str(), "r");
		/* read the file character by character */
		string resultCmd = "";
		char ch;
		while( (ch = fgetc(pCmd)) != EOF) {
			resultCmd += ch;
		}
		pclose(pCmd);
		return resultCmd;
	}
}

static void Usage() {
	cout << "Usage: mysh <command-to-execute>\nIf the commans requires arguments, specify the whole command under double quotes, e.g.:" << endl << "mysh \"ls -la\"" << endl;
	exit(1);
}


int main(int argc, char** argv) {
	if (argc != 2) {
		Usage();
	}
	cout << "Result of executing command: " << argv[1] << " is:" << endl << PracticaCaso::MyShell::execute(argv[1]) << endl;
}

