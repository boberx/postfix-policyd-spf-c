#include "ps_white_write.h"

const char* const module = "write_whitelist";

char CreateWhiteFile ( const char* dbfilename )
{
	FILE* dbfile;

	dbfile = fopen ( dbfilename, "wb" );

	if ( dbfile != 0 )
	{
		unsigned int id = PS_WL_ID;

		fwrite ( &id, 4, 1, dbfile );

		fclose ( dbfile );

		return 1;
	}

	return 0;
}

char AddWhite4ToFile ( const char* dbfilename, uint32_t net, uint32_t msk )
{
	FILE* dbfile;

	dbfile = fopen ( dbfilename, "ab" );

	if ( dbfile != 0 )
	{
		unsigned int id = PS_WL_N4;

		fwrite ( &id, 4, 1, dbfile );
		fwrite ( &net, sizeof ( net), 1, dbfile );

		id = PS_WL_M4;

		fwrite ( &id, 4, 1, dbfile );
		fwrite ( &msk, sizeof ( msk), 1, dbfile );

		fclose ( dbfile );

		return 1;
	}
	else
		syslog ( LOG_WARNING, "%s: error opened file to write: \"%s\"", module, dbfilename );

	return 0;
}

char AddWhite6ToFile ( const char* dbfilename, __uint128_t net, __uint128_t msk )
{
	FILE* dbfile;

	dbfile = fopen ( dbfilename, "ab" );

	if ( dbfile != 0 )
	{
		unsigned int id = PS_WL_N6;

		fwrite ( &id, 4, 1, dbfile );
		fwrite ( &net, sizeof ( net), 1, dbfile );

		id = PS_WL_M6;

		fwrite ( &id, 4, 1, dbfile );
		fwrite ( &msk, sizeof ( msk), 1, dbfile );

		fclose ( dbfile );

		return 1;
	}
	else
		syslog ( LOG_WARNING, "%s: error opened file to write: \"%s\"", module, dbfilename );

	return 0;
}

char WriteWhiteToFile ( const char* filename, const char* dbfilename )
{
	FILE* wlfile;

	wlfile = fopen ( filename, "rb" );

	if ( wlfile != 0 )
	{
		if ( CreateWhiteFile ( dbfilename ) )
		{
			char ipstr[40];
			unsigned char ipstrn = 0;

			for (;; )
			{
				unsigned char line[256];

				fread ( line, sizeof ( line), 1, wlfile );

				if ( ! ferror ( wlfile ) )
				{
					unsigned char i;
					for ( i = 0; i < sizeof ( line) / sizeof ( *line); i ++ )
					{
						if ( line[i] != 0 )
						{
							if ( ipstrn < 40 )
							{
								if ( line[i] != '\n' )
									ipstr[ipstrn ++] = line[i];
								else
								{
									ipstr[ipstrn ++] = 0;
									if ( ga.m_debug != 0 ) syslog  ( LOG_INFO, "%s: sring: \"%s\"", module, ipstr );

									uint32_t ip;
									uint32_t mask;

									if ( StrNet4ToB ( ipstr, &ip, &mask ) )
									{
										uint32_t net = ip & mask;
										AddWhite4ToFile ( dbfilename, net, mask );
										AddWhite4 ( net, mask );
									}
									else
									{
										__uint128_t ip6;
										__uint128_t mask6;

										if ( StrNet6ToB ( ipstr, &ip6, &mask6 ) )
										{
											__uint128_t net6 = ip6 & mask6;
											AddWhite6ToFile ( dbfilename, net6, mask6 );
											AddWhite6 ( net6, mask6 );
										}
										else
											if ( ga.m_debug != 0 ) syslog  (
											LOG_INFO,
											"%s: sring: \"%s\" isn't ipv6 or ipv4 addres",
											module,
											ipstr );
									}

									ipstrn = 0;
								}
							}
							else
							{
								// broken file
								break;
							}
						}
						else
							break;
					}

					if ( feof ( wlfile ) )
					{
						return 1;
					}
				}
				else
				{
					break;
				}
			}
		}
		else
			syslog ( LOG_ERR, "%s: CreateWhiteFile false", module );

		fclose ( wlfile );
	}
	else
		syslog ( LOG_ERR, "%s: file open error: \"%s\"", module, filename );

	return 0;
}
