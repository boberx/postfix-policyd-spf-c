#include "ps_white_read.h"

void ReadWhiteFromFile ( const char* filename )
{
	const char* const module = "read_whitelist";

	char dbfilename[PATH_MAX];

	if ( ga.m_white == 0 )
		return;

	strcpy ( dbfilename, ga.m_white );
	strcat ( dbfilename, ".db" );

	struct stat fs;

	if ( stat ( filename, &fs ) != -1 )
	{
		struct stat ds;

		int r = stat ( dbfilename, &ds );

			if ( ds.st_mtime != fs.st_mtime || r == -1 )
				if ( ! WriteWhiteToFile ( filename, dbfilename ) )
					return;
	}
	else
		if ( ga.m_debug != 0 ) syslog ( LOG_INFO, "%s: file not found: \"%s\"", module, filename );

	FILE* dbfile;

	dbfile = fopen ( dbfilename, "rb" );

	if ( dbfile != 0 )
	{
		unsigned int id = 0;
		fread ( &id, sizeof ( id), 1, dbfile );

		if ( id != PS_WL_ID )
			syslog ( LOG_ERR, "%s: this is no whitelist file: \"%s\"", module, dbfilename );
		else
		{
			uint32_t net;
			uint32_t msk;
			__uint128_t net6;
			__uint128_t msk6;

			while ( ! feof ( dbfile ) )
			{
				if ( fread ( &id, sizeof ( id), 1, dbfile ) == 1 )
				{
					if ( ferror ( dbfile ) )
						break;

					switch ( id ) {
						case PS_WL_N4:
							fread ( &net, sizeof ( net), 1, dbfile );
							break;
						case PS_WL_M4:
							fread ( &msk, sizeof ( msk), 1, dbfile );
							AddWhite4 ( net, msk );
							net = 0;
							msk = 0;
							break;
						case PS_WL_N6:
							fread ( &net6, sizeof ( net6), 1, dbfile );
							break;
						case PS_WL_M6:
							fread ( &msk6, sizeof ( msk6), 1, dbfile );
							AddWhite6 ( net6, msk6 );
							net6 = 0;
							msk6 = 0;
							break;
						default:
							break;
					}
				}
			}
		}

		fclose ( dbfile );
	}
	else
		if ( ga.m_debug != 0 ) syslog ( LOG_INFO, "%s: file not found: \"%s\"", module, dbfilename );
}
