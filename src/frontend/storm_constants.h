/*
 * $Id$
 */

#ifndef _STORM_CONSTANTS_H
#define _STORM_CONSTANTS_H
#include "storm_limits.h"

/* SOAP constants */
#define SOAP_RECV_TIMEOUT   300
#define SOAP_SEND_TIMEOUT    50
#define SOAP_MAX_KEEPALIVE  100
#define SOAP_ACCEPT_TIMEOUT  60

            /* Disk pool manager constants */

#define DEFAULT_PUT_RETENP  86400
#define DEFAULT_SPACE_TYPE  'V' /* cfr SRM v.2.1, the default file
                                 * type must be 'Volatile' */ 
#define DEFAULT_FSS_POLICY  "maxfreespace"
#define DEFAULT_GC_POLICY   "lru"
#define DEFAULT_RS_POLICY   "fifo"

            /* Filesystem statuses */

#define FS_DISABLED 1
#define FS_RDONLY   2

            /* Long options for Disk pool manager utilities */

#define OPT_DEF_FSZ 0
#define OPT_FS      1
#define OPT_FS_SERVER   2
#define OPT_GC_POLICY   3
#define OPT_POOL_GID    4
#define OPT_POOL_GROUP  5
#define OPT_POOL_NAME   6
#define OPT_FSS_POLICY  7
#define OPT_RS_POLICY   8
#define OPT_S_TYPE  9
#define OPT_STATUS  10
#define OPT_DEF_PTIME   11
#define OPT_PUT_RETENP  12
#define OPT_GC_START    13
#define OPT_GC_STOP 14

            /* Disk pool manager utilities exit codes */

#define USERR   1   /* user error */
#define SYERR   2   /* system error */
#define CONFERR 4   /* configuration error */
#endif
