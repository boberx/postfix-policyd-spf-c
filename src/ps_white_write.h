#ifndef PS_WHITE_WRITE_H
#define PS_WHITE_WRITE_H

#include <string.h>
#include <stdio.h>
#include <linux/limits.h>

#include "ps_getopt.h"
#include "ps_white.h"
#include "ps_white_dbfile.h"

char WriteWhiteToFile ( const char* filename, const char* dbfilename );

#endif
