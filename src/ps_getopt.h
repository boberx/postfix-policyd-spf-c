#ifndef PS_GETOPT_H
#define	PS_GETOPT_H

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

typedef struct globalArgs_t
{
	char* m_white;
	char m_debug;
	char m_test;
} globalArgs;

extern globalArgs ga;

void GetOpt ( int argc, char** argv );

#endif
