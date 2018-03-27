#include "ps_other.h"

const char* const POSTFIX_DUNNO = "DUNNO";
const char* const POSTFIX_REJECT = "REJECT";
const char* const POSTFIX_OK = "OK";

void ResponseFree ( SPF_response_t** spf_response )
{
	if ( *spf_response )
		SPF_response_free ( *spf_response );

	*spf_response = 0;
}

void ResponsePrint ( SPF_response_t* spf_response, SPF_client_request_t* req )
{
	const char* const module = "responseprint";

	char result[RESULTSIZE];
	char spf_comment[RESULTSIZE];

	switch ( spf_response->result ) {
		case SPF_RESULT_PASS:
			strcpy ( result, POSTFIX_OK );
			printf ( "action=OK X-%s\n", SPF_response_get_received_spf ( spf_response ) );
			snprintf ( spf_comment, RESULTSIZE, SPF_response_get_received_spf ( spf_response ) );
			break;
		case SPF_RESULT_FAIL:
			strcpy ( result, POSTFIX_REJECT );
			snprintf ( spf_comment,
				RESULTSIZE,
				"SPF Reject: %s",
				(spf_response->smtp_comment ? \
				spf_response->smtp_comment : (spf_response->header_comment ? \
				spf_response->header_comment : "")) );
			break;
		case SPF_RESULT_TEMPERROR:
		case SPF_RESULT_PERMERROR:
		case SPF_RESULT_INVALID:
			snprintf ( result,
				RESULTSIZE,
				"450 temporary failure: %s",
				(spf_response->smtp_comment ? spf_response->smtp_comment : "") );
			spf_comment[0] = '\0';
			break;
		case SPF_RESULT_SOFTFAIL:
		case SPF_RESULT_NEUTRAL:
		case SPF_RESULT_NONE:
		default:
			strcpy ( result, POSTFIX_DUNNO );
			printf ( "action=PREPEND X-%s\n", SPF_response_get_received_spf ( spf_response ) );
			snprintf ( spf_comment, RESULTSIZE, SPF_response_get_received_spf ( spf_response ) );
			break;
	}

	result[RESULTSIZE - 1] = '\0';
	if ( ga.m_debug == 2 ) syslog ( LOG_DEBUG, "%s: %s", module, result );

	if ( strcmp ( result, POSTFIX_REJECT ) == 0 )
		printf ( "action=%s %s\n\n", result, spf_comment );
	else
		printf ( "action=%s\n\n", result );

	fflush ( stdout );

	if ( ga.m_debug != 0 ) syslog ( LOG_INFO,
		"%s: action=%s %s (ip=%s from=%s helo=%s to=%s)",
		module,
		result,
		spf_comment,
		req->ip,
		req->sender,
		req->helo,
		req->rcpt_to );
}

void ResponseLogErrors ( const char* context,
						SPF_response_t* spf_response,
						SPF_errcode_t err )
{
	const char* const module = "responselogerrors";

	SPF_error_t* spf_error;

	if ( context != NULL )
		syslog ( LOG_CRIT, "%s: Context: %s", module, context );

	if ( err != SPF_E_SUCCESS )
		syslog ( LOG_CRIT, "%s: ErrorCode: (%d) %s", module, err, SPF_strerror ( err ) );

	if ( spf_response != NULL )
	{
		int i = 0;

		for ( i = 0; i < SPF_response_messages ( spf_response ); i++ )
		{
			spf_error = SPF_response_message ( spf_response, i );

			syslog ( LOG_CRIT,
				"%s: %s: %s%s",
				module,
				SPF_error_errorp ( spf_error ) ? "Error" : "Warning",
				((SPF_error_errorp ( spf_error ) && (!err)) ? "[UNRETURNED] " : ""),
				SPF_error_message ( spf_error ) );
		}
	}
	else
		syslog ( LOG_CRIT, "%s: libspf2 gave a NULL spf_response", module );
}

void ResponseLog ( const char* context, SPF_response_t* spf_response )
{
	const char* const module = "responselog";

	syslog ( LOG_DEBUG, "%s: Context: %s", module, context );

	if ( spf_response == NULL )
		syslog ( LOG_DEBUG, "%s: NULL RESPONSE!", module );
	else
	{
		syslog ( LOG_DEBUG,
			"%s: Response result: %s",
			module,
			SPF_strresult ( SPF_response_result ( spf_response ) ) );

		syslog ( LOG_DEBUG,
			"%s: Response reason: %s",
			module,
			SPF_strreason ( SPF_response_reason ( spf_response ) ) );

		syslog ( LOG_DEBUG,
			"%s: Response err: %s",
			module,
			SPF_strerror ( SPF_response_errcode ( spf_response ) ) );

		ResponseLogErrors ( NULL, spf_response, SPF_response_errcode ( spf_response ) );
	}
}

/*inline void ContinueError ( char* pf_result, int* res, const SPF_client_request_t * const req )
{
	*res = 255;
	sprintf ( pf_result,
		"450 temporary failure: please contact postmaster if the error remains" );

	printf ( "action=%s\n\n", pf_result );

	fflush ( stdout );

	syslog ( LOG_INFO,
		"action=%s (ip=%s from=%s helo=%s to=%s)",
		pf_result,
		req->ip,
		req->sender,
		req->helo,
		req->rcpt_to );
}*/

inline void PostfixAccessReject ( const SPF_client_request_t* const req )
{
	const char* reject = "450 temporary failure: please contact postmaster if the error remains";

	printf ( "action=%s\n\n", reject );

	fflush ( stdout );

	syslog ( LOG_INFO,
		"action=%s (ip=%s from=%s helo=%s to=%s)",
		reject,
		req->ip,
		req->sender,
		req->helo,
		req->rcpt_to );
}

inline void PostfixAccessDunno ( const char* const s, const SPF_client_request_t* const req )
{
	printf ( "action=PREPEND X-Received-SPF: %s\n", s );
	printf ( "action=%s\n\n", POSTFIX_DUNNO );

	fflush ( stdout );

	syslog ( LOG_INFO,
		"action=%s %s (ip=%s from=%s helo=%s to=%s)\n",
		POSTFIX_DUNNO,
		s,
		req->ip,
		req->sender,
		req->helo,
		req->rcpt_to );
}

inline void PostfixAccessOk ( const char* const s, const SPF_client_request_t* const req )
{
	printf ( "action=PREPEND X-Received-SPF: %s\n", s );
	printf ( "action=%s\n\n", POSTFIX_OK );

	fflush ( stdout );

	syslog ( LOG_INFO,
		"action=%s %s (ip=%s from=%s helo=%s to=%s)\n",
		POSTFIX_OK,
		s,
		req->ip,
		req->sender,
		req->helo,
		req->rcpt_to );
}

/*inline void ContinueDunno ( const char* const s, int* res, const SPF_client_request_t * const req )
{

}*/

/*inline void ContinueOK ( const char* const s, int* res, const SPF_client_request_t * const req )
{
	printf ( "action=PREPEND X-Received-SPF: %s\n", s );
	printf ( "action=%s\n\n", POSTFIX_OK );

	fflush ( stdout );

	*res = 255;

	syslog ( LOG_INFO,
		"action=%s %s (ip=%s from=%s helo=%s to=%s)\n",
		POSTFIX_OK,
		s,
		req->ip,
		req->sender,
		req->helo,
		req->rcpt_to );
}*/
