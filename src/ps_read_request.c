#include "ps_read_request.h"

char ReadRequest ( SPF_client_request_t* const req )
{
	const char* const module = "readrequest";

	const char req_client_address[] = "client_address=";
	const char req_sender[]         = "sender=";
	const char req_helo_name[]      = "helo_name=";
	const char req_recipient[]      = "recipient=";

	char line[BUFSIZ];
	char args = 0;

	while ( fgets ( line, BUFSIZ, stdin ) != NULL )
	{
		line[strcspn ( line, "\n" )] = '\0';

		if ( ga.m_debug == 2 ) syslog ( LOG_DEBUG, "%s: line: %s", module, line );

		switch ( line[0] )
		{
		case '\0':
			if ( args > 0 ) return 0;
			break;
		case 'c':
			if ( strncasecmp (	line,
				req_client_address,
				sizeof ( req_client_address ) / sizeof ( *req_client_address ) - 1 ) == 0 )
			{
				req->ip = strdup ( &line[sizeof ( req_client_address ) / sizeof ( *req_client_address ) - 1] );
				if ( ga.m_debug != 0 ) syslog ( LOG_DEBUG, "%s: %s%s", module, req_client_address, req->ip );
				args ++;
				continue;
			}
			break;
		case 's':
			if ( strncasecmp (	line,
				req_sender,
				sizeof ( req_sender ) / sizeof ( *req_sender ) - 1 ) == 0 )
			{
				req->sender = strdup ( &line[sizeof ( req_sender ) / sizeof ( *req_sender ) - 1] );
				if ( ga.m_debug != 0 ) syslog ( LOG_DEBUG, "%s: %s%s", module, req_sender, req->sender );
				args ++;
				continue;
			}
			break;
		case 'h':
			if ( strncasecmp (	line,
				req_helo_name,
				sizeof ( req_helo_name ) / sizeof ( *req_helo_name ) - 1 ) == 0 )
			{
				req->helo = strdup ( &line[sizeof ( req_helo_name ) / sizeof ( *req_helo_name ) - 1] );
				if ( ga.m_debug != 0 ) syslog ( LOG_DEBUG, "%s: %s%s", module, req_helo_name, req->helo );
				args ++;
				continue;
			}
			break;
		case 'r':
			if ( strncasecmp (	line,
				req_recipient,
				sizeof ( req_recipient ) / sizeof ( *req_recipient ) - 1 ) == 0 )
			{
				req->rcpt_to = strdup ( &line[sizeof ( req_recipient ) / sizeof ( *req_recipient ) - 1] );
				if ( ga.m_debug != 0 ) syslog ( LOG_DEBUG, "%s: %s%s", module, req_recipient, req->rcpt_to );
				args ++;
				continue;
			}
			break;
		}
	}

	if ( feof ( stdin ) )
		return 1;
	else
		return 0;
}
