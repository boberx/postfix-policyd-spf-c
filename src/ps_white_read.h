#ifndef PS_WHITE_READ_H
#define PS_WHITE_READ_H

#include <string.h>
#include <stdio.h>
#include <linux/limits.h>
#include <sys/stat.h>

#include "ps_white.h"
#include "ps_white_write.h"
#include "ps_white_dbfile.h"
#include "ps_getopt.h"

void ReadWhiteFromFile ( const char* filename );

#endif
