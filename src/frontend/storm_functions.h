/*
 * $Id$
 */

#ifndef _STORM_FUNCTIONS_H
#define _STORM_FUNCTIONS_H
//#if defined(_WIN32)
//#include <sys/utime.h>
//#else
#include <utime.h>
//#endif
#include <mysql.h>
#include "srm_server.h"
#include "storm_constants.h"
#include "osdep.h"

int *C__storm_errno();
#define storm_errno (*C__storm_errno())

#define RETURN(x) \
        { \
        if ( x != 0) { srmlogit (STORM_LOG_ERROR, func, "returns %d\n", (x)); }\
        else {srmlogit (STORM_LOG_INFO, func, "returns %d\n", (x)); }\
        if (thip->dbfd.tr_started) \
                if (x) \
                        (void) storm_abort_tr (&thip->dbfd); \
                else \
                        (void) storm_end_tr (&thip->dbfd); \
        return ((x)); \
        }

#define END_TRANS_AND_RETURN(x) RETURN(x)

                        /* ACL types */

#define STORM_ACL_USER_OBJ        1
#define STORM_ACL_USER            2
#define STORM_ACL_GROUP_OBJ       3
#define STORM_ACL_GROUP           4
#define STORM_ACL_MASK            5
#define STORM_ACL_OTHER           6
#define STORM_ACL_DEFAULT         0x20


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
#define u64tostr(var, buf, i)  (snprintf(buf, sizeof(var)*2+2, "0x%x", var),buf)


/* DB operations */



/* Database operations */
EXTERN_C int storm_opendb _PROTO((char *, char *, char *, struct srm_dbfd *));
EXTERN_C int storm_start_tr _PROTO((int, struct srm_dbfd *));
EXTERN_C int storm_end_tr _PROTO((struct srm_dbfd *));
EXTERN_C void storm_abort_tr _PROTO((struct srm_dbfd *));
EXTERN_C void set_savepoint _PROTO((struct srm_dbfd *, const char *));
EXTERN_C void rollback_to_savepoint _PROTO((struct srm_dbfd *, const char *));
#endif

