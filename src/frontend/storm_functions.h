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

#ifndef _STORM_FUNCTIONS_H
#define _STORM_FUNCTIONS_H

#include <utime.h>
#include <mysql/mysql.h>
#include "srm_server.h"
#include "storm_constants.h"

                        /* ACL types */

#define STORM_ACL_USER_OBJ        1
#define STORM_ACL_USER            2
#define STORM_ACL_GROUP_OBJ       3
#define STORM_ACL_GROUP           4
#define STORM_ACL_MASK            5
#define STORM_ACL_OTHER           6
#define STORM_ACL_DEFAULT         0x20

/*
 * 1 means that call does not fail in case of not well-formed authorizationId
 * 0 means that call does fail in case of not well-formed authorizationId
 *
 */
#define DONT_FAIL_FOR_AUTHORIZATION_ID 1


/* storm_req File Type */
#define DB_FILE_TYPE_VOLATILE  'V'
#define DB_FILE_TYPE_DURABLE   'D'
#define DB_FILE_TYPE_PERMANENT 'P'
#define DB_FILE_TYPE_UNKNOWN   '\0'

/* storm_req Access Pattern */
#define DB_ACCESS_PATTERN_TRANSFER 'T' /* TRANSFER_MODE */
#define DB_ACCESS_PATTERN_PROCESS 'P' /* PROCESSING_MODE */
#define DB_ACCESS_PATTERN_UNKNOWN '\0' /* PROCESSING_MODE */

/* storm_req Connection Type */
#define DB_CONN_TYPE_LAN 'L' /* LAN */
#define DB_CONN_TYPE_WAN 'W' /* WAN */
#define DB_CONN_TYPE_UNKNOWN '\0'

/* storm_req Request Type */
#define DB_BOL_REQUEST  "BOL"
#define DB_COPY_REQUEST "COP"
#define DB_GET_REQUEST  "PTG"
#define DB_PUT_REQUEST  "PTP"

/* storm_req Overwrite Mode */
#define DB_OVERWRITE_NEVER         'N'
#define DB_OVERWRITE_ALWAYS        'A'
#define DB_OVERWRITE_IF_DIFFERENT  'D'
#define DB_OVERWRITE_UNKNOWN       '\0'

/* storm_req Access Latency */
#define DB_ACCESS_LATENCY_NEARLINE 'N'
#define DB_ACCESS_LATENCY_ONLINE   'O'
#define DB_ACCESS_LATENCY_UNKNOWN  '\0'

/* Conversion function */
#define itoa(buf, var)  (snprintf(buf, sizeof(var)*2+2, "0x%x", var),buf)
#define ctoa(buf, var)  (snprintf(buf, 4, "'%c'", var), buf)
#define itoa(buf, var)  (snprintf(buf, sizeof(var)*2+2, "0x%x", var),buf)
#define u64tostr(var, buf, i)  (snprintf(buf, sizeof(var)*2+2, "0x%llx", var),buf)

#endif

