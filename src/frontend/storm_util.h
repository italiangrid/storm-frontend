/*
 * $Id$
 */

#ifndef _STORM_UTIL_H
#define _STORM_UTIL_H


#ifdef __cplusplus
extern "C" {
#endif

#define SRM_EP_PATH "/srm/V2/Server"   // Temporary (test Sim....)
#define SURL_MAXSIZE 256

#include <openssl/x509.h>

/* Type definitions */
typedef unsigned long long unsignedLong64;

/* Error code for the functions check_* and split_surl() */
#define SURL_VALID                   0
#define SURL_ENOMEM                  1    /* unable to allocate memory */
#define SURL_INVALID_CHAR            1<<1
#define SURL_STRING_TOO_LONG         1<<2
#define SURL_STRING_TOO_SHORT        1<<3

#define SURL_EMPTY_ENDPOINT          1<<4
#define SURL_EMPTY_HOSTNAME          1<<5
#define SURL_EMPTY_PORT              1<<6
#define SURL_EMPTY_PROTOCOL          1<<7
#define SURL_EMPTY_SFN               1<<8
#define SURL_INVALID                 1<<9    /* generic error, like a surl ending with a colon after the hostname */
#define SURL_INVALID_ENDPOINT        1<<10
#define SURL_INVALID_HOSTNAME        1<<11
#define SURL_INVALID_PORT            1<<12
#define SURL_INVALID_PROTOCOL        1<<13
#define SURL_INVALID_SFN             1<<14

#define MAX_PORT_NUMBER 1<<16

int get_supported_protocols(char ***);
int is_surl_local (char *surl);
char *pfnfromturl (char *turl);
char *sfnfromsurl (char *surl);
char* reconvertStatusCode(int  code);
//int convertStatusCode(char*  code);

/* Prototype definitions */
int convertPermission(char *mode);
int convertFileStorageType(char *fstype);
int convertFileType(char* ftype);
int convertSpaceType(char *stype);
int convertStatusCode(char*  code);

void printLine();
void printChar();
int parseSURL_getEndpoint (const char *surl, char **endpoint, char **sfn);
int parseSURL_noSFN_getEndpoint (const char *surl, char **endpoint);
int parseSURL(const char *surl);
STACK_OF(X509) *load_chain(const char *certfile);

#ifdef __cplusplus
}
#endif


#endif
