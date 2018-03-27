/*!
*  \brief     postfix-policyd-spf-c
*  \details   SPF Policy Deamon for Postfix.
*  \details   This program is based on Matthias Cramer's policyd-spf-fs.
*  \author    Unknown
*  \version   1.28
*  \date      09.11.2015
*  \copyright GNU Public License.
*/

#ifndef PS_MAIN_H
#define PS_MAIN_H

#include "ps_white_read.h"
#include "ps_white.h"
#include "ps_read_request.h"
#include "ps_other.h"
#include "ps_white_test.h"
#include "ps_getopt.h"

/*!
*  \brief название демона в логах syslog
*/
const char* const progname = "postfix_spfpd";

#endif
