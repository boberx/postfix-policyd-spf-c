#ifndef PS_OTHER_H
#define PS_OTHER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>
#include <netdb.h>
#include <spf2/spf.h>
#include "ps_getopt.h"

extern const char* const POSTFIX_DUNNO;
extern const char* const POSTFIX_REJECT;
extern const char* const POSTFIX_OK;

typedef struct SPF_client_request_struct
{
	char* ip;
	char* sender;
	char* helo;
	char* rcpt_to;
} SPF_client_request_t;

void ResponseLogErrors (const char* context,
						SPF_response_t* spf_response,
						SPF_errcode_t err );

void ResponseLog ( const char* context, SPF_response_t* spf_response );

void ResponsePrint ( SPF_response_t* spf_response, SPF_client_request_t* req );

void ResponseFree ( SPF_response_t** spf_response );

inline void PostfixAccessReject ( const SPF_client_request_t* const req );

inline void PostfixAccessDunno ( const char* const s, const SPF_client_request_t* const req );

inline void PostfixAccessOk ( const char* const s, const SPF_client_request_t* const req );

#define REQUEST_LIMIT	100
#define RESULTSIZE		1024

#define DEFAULT_EXPLANATION "Please see http://www.openspf.org/Why?id=%{S}&ip=%{C}&receiver=%{R}"

#define FREE(x, f) do { if ((x)) (f)((x)); (x) = NULL; } while(0)
#define FREE_REQUEST(x) FREE((x), SPF_request_free)

#define RESIZE_RESULT(n) do { \
	if (result == NULL) { \
	result_len = 256 + n; \
	result = malloc(result_len); \
	result[0] = '\0'; \
	} \
	else if (strlen(result) + n >= result_len) { \
	result_len = result_len + (result_len >> 1) + 8 + n; \
	result = realloc(result, result_len); \
} \
} while(0)
#define APPEND_RESULT(n) do { \
	partial_result = SPF_strresult(n); \
	RESIZE_RESULT(strlen(partial_result)); \
	strcat(result, partial_result); \
} while(0)

#endif
