/*
 * $Id: srm_server.h.in 4801 2008-12-01 17:26:37Z alb $
 */

#ifndef _SRM_SERVER_H
#define _SRM_SERVER_H

#include <mysql.h>

            /* srm server constants */
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
};

/* srm server exit codes */
#define USERR   1   /* user error */
#define SYERR   2   /* system error */
#define CONFERR 4   /* configuration error */

#endif /* _SRM_SERVER_H */
