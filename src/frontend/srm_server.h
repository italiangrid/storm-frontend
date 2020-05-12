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


#ifndef _SRM_SERVER_H
#define _SRM_SERVER_H

#include <mysql/mysql.h>
#include <argus/pep.h>

#define MAXRETRY 2
#define RETRYI  5
#define BACKLOG (100) // Max. request backlog
#define LOGBUFSZ 1024
#define PRTBUFSZ 180
#define RPC_ASYNCH_TIMEOUT 5

extern uid_t proxy_uid;
extern gid_t proxy_gid;
extern char *db_pwd;
extern char *db_srvr;
extern char *db_user;
extern char *xmlrpc_endpoint;

/* srm server structures */
struct srm_dbfd {
    MYSQL   mysql;
    int     tr_started;
};

struct srm_srv_thread_info {
    int              is_used;
    int              db_open_done;
    struct srm_dbfd  dbfd;
    char             errbuf[PRTBUFSZ];
    const char*      request_id;
    PEP*             pep_handle;
};

/* srm server exit codes */
#define USERR   1   /* user error */
#define SYERR   2   /* system error */
#define CONFERR 4   /* configuration error */

#endif /* _SRM_SERVER_H */
