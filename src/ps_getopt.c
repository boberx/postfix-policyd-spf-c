#include "ps_getopt.h"

globalArgs ga = { 0, 0, 0 };

void GetOpt ( int argc, char** argv )
{
	static const char *optString = "hvtw:d:";

	int opt;

	opt = getopt ( argc, argv, optString );

	while	( opt != -1 )
	{
		switch ( opt )
		{
			case 'h':
				printf ( "Usage:\n-h print help\n-v print version info\n-t run in test mode\n"
							"-w whitelist\n-d debug level\n	0 - info\n	1 - debug\n	2 - verbose debug\n\n" );
				exit ( 1 );
				break;
			case 'v':
				printf ( "Version: 1.28\n" );
				exit ( 1 );
				break;
			case 'w':
					ga.m_white = optarg;
				break;
			case 'd':
					switch ( optarg[0] )
					{
						case '2':
							ga.m_debug = 2;
							break;
						case '1':
							ga.m_debug = 1;
							break;
						case '0':
						default:
							ga.m_debug = 0;
							break;
					}
				break;
			case 't':
					ga.m_test = 1;
				break;
			case '?':
				exit ( 0 );
				break;
		}

		opt = getopt ( argc, argv, optString );
	}
}
