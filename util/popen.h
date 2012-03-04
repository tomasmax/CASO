// popen.h
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
typedef struct pinfo {
	FILE *file;
	pid_t pid;
	struct pinfo *next;
} pinfo;

static pinfo *plist = NULL;

FILE* popen(const char *command, const char *mode);
int pclose(FILE *file);
