#include "ps_main.h"

int main ( int argc, char* argv[] )
{
	const char* const module = "main";

	GetOpt ( argc, argv );

	SPF_client_request_t* req = NULL;
	SPF_server_t* spf_server = NULL;
	SPF_request_t* spf_request = NULL;
	SPF_response_t* spf_response = NULL;
	SPF_errcode_t err;

	int request_limit = 0;
	int res = 0;

	const char *partial_result;
	char *result = NULL;
	int result_len = 0;

	openlog ( progname, LOG_PID | LOG_CONS | LOG_NDELAY | LOG_NOWAIT, LOG_MAIL );

	SPF_error_handler = SPF_error_syslog;
	SPF_warning_handler = SPF_warning_syslog;
	SPF_info_handler = SPF_info_syslog;
	SPF_debug_handler = SPF_debug_syslog;

	if ( ga.m_debug > 1 )
		spf_server = SPF_server_new ( SPF_DNS_CACHE, 1 );
	else
		spf_server = SPF_server_new ( SPF_DNS_CACHE, 0 );

	err = SPF_server_set_explanation ( spf_server, DEFAULT_EXPLANATION, &spf_response );

	if ( err )
	{
		ResponseLogErrors ( "Error setting default explanation",
			spf_response,
			err );

		res = 255;
	}

	ResponseFree ( &spf_response );

	if ( ga.m_white != 0 )
		ReadWhiteFromFile ( ga.m_white );

	request_limit = 0;

	while ( request_limit < REQUEST_LIMIT )
	{
		request_limit ++;

		if ( request_limit == 0 )
			free ( req );

		req = (SPF_client_request_t *) malloc ( sizeof ( SPF_client_request_t) );
		memset ( req, 0, sizeof ( SPF_client_request_t) );

		if ( ReadRequest ( req ) )
		{
			syslog ( LOG_WARNING, "%s: io closed while reading, exiting", module );
			res = 0;
			break;
		}

		if ( ga.m_debug != 0 ) syslog ( LOG_DEBUG, "%s: reincarnation %d", module, request_limit );

		FREE_REQUEST ( spf_request );

		ResponseFree ( &spf_response );

		spf_request = SPF_request_new ( spf_server );

		if ( SPF_request_set_ipv4_str ( spf_request, req->ip ) && SPF_request_set_ipv6_str ( spf_request, req->ip ) )
		{
			syslog ( LOG_WARNING, "%s: Invalid IP address", module );

			res = 255;

			if ( ga.m_test == 0 )
				PostfixAccessReject ( req );
			else
				PostfixAccessOk ( "test mode", req );

			continue;
		}

		if ( strchr ( req->sender, '@' ) > 0 )
		{
			if ( SPF_request_set_env_from ( spf_request, req->sender ) )
			{
				syslog ( LOG_WARNING, "Invalid envelope from address" );

				res = 255;

				if ( ga.m_test == 0 )
					PostfixAccessReject ( req );
				else
					PostfixAccessOk ( "TM: Reject", req );

				continue;
			}
		}
		else
		{
			res = 255;

			if ( ga.m_test == 0 )
				PostfixAccessDunno ( "no valid email address found", req );
			else
				PostfixAccessOk ( "TM: Dunno: no valid email address found", req );

			continue;
		}

		if ( ThisIsIpAddressInWhitelist ( req->ip ) )
		{
			res = 255;
			PostfixAccessOk ( "whitelisted", req );
			continue;
		}

		err = SPF_request_query_mailfrom ( spf_request, &spf_response );

		if ( ga.m_debug != 0 ) ResponseLog ( "Main query", spf_response );

		if ( err )
		{
			if ( ga.m_debug != 0 ) ResponseLogErrors ( "Failed to query MAIL-FROM", spf_response, err );

			res = 255;

			if ( ga.m_test == 0 )
				PostfixAccessDunno ( "no SPF record found", req );
			else
				PostfixAccessOk ( "TM: Dunno: no SPF record found", req );

			continue;
		}

		if ( result != NULL )
			result[0] = '\0';

		APPEND_RESULT ( SPF_response_result ( spf_response ) );

		ResponsePrint ( spf_response, req );

		res = SPF_response_result ( spf_response );

		fflush ( stdout );
	}

	FREE ( result, free );
	ResponseFree ( &spf_response );
	FREE_REQUEST ( spf_request );
	FREE ( spf_server, SPF_server_free );

	syslog ( LOG_INFO, "%s: Terminating with result %d, Reincarnation: %d", module, res, request_limit );

	return res;
}
