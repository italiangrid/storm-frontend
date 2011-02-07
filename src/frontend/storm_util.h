/* Copyright (c) Istituto Nazionale di Fisica Nucleare (INFN). 2006-2010.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/

#ifndef _STORM_UTIL_H
#define _STORM_UTIL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <openssl/x509.h>

/* Type definitions */
typedef unsigned long long unsignedLong64;
typedef time_t storm_time_t;
typedef long long storm_size_t;
typedef long storm_id_t;

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

/* Prototype definitions */
int get_supported_protocols(char ***);
char* reconvertStatusCode(int  code);
int convertStatusCode(char*  code);
int convertPermission(char *mode);
int convertFileStorageType(char *fstype);
int convertFileType(char* ftype);
int convertSpaceType(char *stype);
int convertStatusCode(char*  code);
STACK_OF(X509) *load_chain(const char *certfile);

#ifdef __cplusplus
}
#endif


#endif
