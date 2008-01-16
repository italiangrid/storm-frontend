/*
 * $Id$
 */

#ifndef _STORM_FUNCTIONS_H
#define _STORM_FUNCTIONS_H
#if defined(_WIN32)
#include <sys/utime.h>
#else
#include <utime.h>
#endif
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

typedef char storm_dbrec_addr[21];
typedef MYSQL_RES *DBLISTPTR;
typedef time_t storm_time_t;
typedef long long storm_size_t;
typedef long storm_id_t;

struct storm_req {
    char            client_dn[256]; 
    char            u_token[256]; /* User token */
    char            f_type; /* DB_FILE_TYPE_* */
    storm_time_t    retrytime; /* lifetime == -1 => not specified */
    storm_time_t    pinlifetime; /* lifetime == -1 => not specified */
    char            s_token[ST_MAXDPMTOKENLEN+1];
    char            access_pattern; /* DB_ACCESS_PATTERN_* */
    char            conn_type; /* DB_CONN_TYPE_* */
    int             status; /* SRM STATUS of the request */
    char            errstring[256]; /* Error string, if any, read from the DB */
    char            r_token[ST_MAXDPMTOKENLEN+1]; /* Request token */
    int             remaining_totaltime;
    char            r_type[4]; /* DB_*_REQUEST */
    char            overwrite; /* DB_OVERWRITE_* */
    storm_time_t    lifetime; /* lifetime == -1 => not specified */
    unsigned long   nbreqfiles; /* Number of SURL in the request */
    int             nr_completed; /* Number of SURL successfully processed */
    int             nr_waiting;   /* Number of SURL not yet processed */
    int             nr_failed;    /* Number of SURL failed to process */
    char            **protocols; /* NULL-terminated array of protorols
                                  If protocols == NULL, then we
                                  suppose the list of protocols is
                                  empty.*/  
    char            **client_networks; /* Like protocols... */
    storm_id_t      request_id; /* Request ID. Used as glue between
                                 * all the tables related to the
                                 * request... (io sono la gomma, tu la colla :-))*/
    /* obsoleti - serve solo per compilare con le vecchie funzioni */
        int             r_ordinal;
        uid_t           r_uid;
        gid_t           r_gid;
        char            clienthost[ST_MAXHOSTNAMELEN+1];
        char            protocol[ST_MAXPROTOLISTLEN+1];
        int             flags;
        time_t          ctime;          /* submit time */
        time_t          stime;          /* start time */
        time_t          etime;          /* end time */
};

struct storm_diroption {
    int srcdir; /* Must be specified. */
    int recursive; /* -1 => not specified */
    int numlevel; /* -1 => not specified */
};

struct storm_get_filereq {
    char            surl[ST_MAXSFNLEN+1];
    char            turl[ST_MAXSFNLEN+1];
    int             status; /* SRM STATUS of the SURL */
    char            errstring[256]; /* Error string, if any, read from the DB */
    storm_size_t    size; /* Actual size */
    storm_time_t    wait_time; /* Extimated Waiting Time */
    storm_time_t    pin_time;  /* Remaining Pinning Time, aka the TURL
                                * remaining lifetime */
    struct storm_diroption *diroption; /* Can be NULL */
    /* obsoleti - serve solo per compilare con le vecchie funzioni */
    char            r_token[ST_MAXDPMTOKENLEN+1];
    int             f_ordinal;
    char            from_surl[ST_MAXSFNLEN+1];
        u_signed64*     lifetime;
        char            f_type;
        char            s_token[ST_MAXDPMTOKENLEN+1];
        int             flags;
        char            server[ST_MAXHOSTNAMELEN+1];
        char            pfn[ST_MAXSFNLEN+1];
        u_signed64      actual_size;
};

struct storm_put_filereq {
        char            r_token[ST_MAXDPMTOKENLEN+1];
        int             f_ordinal;
        char            to_surl[ST_MAXSFNLEN+1];
        u_signed64*     lifetime;
        char            f_type;
        char            s_token[ST_MAXDPMTOKENLEN+1];
        u_signed64      requested_size;
        char            server[ST_MAXHOSTNAMELEN+1];
        char            pfn[ST_MAXSFNLEN+1];
        u_signed64      actual_size;
        int             status;
        char            errstring[256];
};

struct storm_copy_filereq {
        char            r_token[ST_MAXDPMTOKENLEN+1];
        int             f_ordinal;
        char            from_surl[ST_MAXSFNLEN+1];
        char            to_surl[ST_MAXSFNLEN+1];
        u_signed64*     lifetime;
        char            f_type;
        char            s_token[ST_MAXDPMTOKENLEN+1];
        int             flags;
        u_signed64      actual_size;
        int             status;
        char            errstring[256];
};

struct storm_tokeninfo {
        char            r_token[ST_MAXDPMTOKENLEN+1];
        time_t          c_time;
};


/* File status operations */

struct storm_filestatus {
        char            *surl;
        int             status;
        char            *errstring;
};


struct storm_filestatg {
    u_signed64  fileid;
    char        guid[ST_MAXGUIDLEN+1];
    mode_t      filemode;
    int     nlink;      /* number of files in a directory */
    uid_t       uid;
    gid_t       gid;
    u_signed64  filesize;
    time_t      atime;      /* last access to file */
    time_t      mtime;      /* last file modification */
    time_t      ctime;      /* last metadata modification */
    short       fileclass;  /* 1 --> experiment, 2 --> user */
    char        status;     /* ' ' --> online, 'm' --> migrated */
    char        csumtype[3];
    char        csumvalue[33];
};

/* Permissions */

struct storm_acl {
        unsigned char   a_type;
        int             a_id;
        unsigned char   a_perm;
};


            /* function prototypes */
/* File & Dir operations */
EXTERN_C int DLL_DECL storm_getacl _PROTO((const char *, int, struct storm_acl *));
EXTERN_C int DLL_DECL storm_mkdir _PROTO((const char *, mode_t));
EXTERN_C int DLL_DECL storm_rmdir _PROTO((const char *));
EXTERN_C int DLL_DECL storm_rm _PROTO((int, char **, int *, struct storm_filestatus **));
EXTERN_C int DLL_DECL storm_statg _PROTO((const char *, const char *, struct storm_filestatg *));

/* Request operations */
EXTERN_C int DLL_DECL storm_getreqid _PROTO((const char *, int *, struct storm_tokeninfo **));
EXTERN_C int DLL_DECL storm_putdone _PROTO((char *, int, char **, int *, struct storm_filestatus **));
EXTERN_C int DLL_DECL storm_abortreq _PROTO((char *));
EXTERN_C int DLL_DECL storm_extendfilelife _PROTO((char *, char *, time_t, time_t *));


/* Database operations */
EXTERN_C int storm_opendb _PROTO((char *, char *, char *, struct srm_dbfd *));
EXTERN_C int storm_start_tr _PROTO((int, struct srm_dbfd *));
EXTERN_C int storm_unique_id _PROTO((struct srm_dbfd *, u_signed64 *));
EXTERN_C int storm_insert_gfr_entry _PROTO((struct srm_dbfd *, struct storm_get_filereq *, storm_id_t));
EXTERN_C storm_id_t storm_insert_pending_entry _PROTO((struct srm_dbfd *, struct storm_req *));
EXTERN_C int storm_end_tr _PROTO((struct srm_dbfd *));
EXTERN_C void storm_abort_tr _PROTO((struct srm_dbfd *));
EXTERN_C int storm_insert_xferreq_entry _PROTO((struct srm_dbfd *, struct storm_req *));
EXTERN_C int storm_insert_pfr_entry _PROTO((struct srm_dbfd *, struct storm_put_filereq *));
EXTERN_C int storm_insert_xferreq_entry _PROTO((struct srm_dbfd *, struct storm_req *));
EXTERN_C int storm_insert_cpr_entry _PROTO((struct srm_dbfd *, struct storm_copy_filereq *));
EXTERN_C int storm_get_req_by_token _PROTO((struct srm_dbfd *, char *, struct storm_req *, int, storm_dbrec_addr *));
EXTERN_C int storm_get_pending_req_by_token _PROTO((struct srm_dbfd *, char *, struct storm_req *, int, storm_dbrec_addr *));
EXTERN_C int storm_list_gfr_entry _PROTO((struct srm_dbfd *, int, char *, struct storm_get_filereq *, int, storm_dbrec_addr *, int, DBLISTPTR *));
EXTERN_C int storm_get_gfr_by_surl _PROTO((struct srm_dbfd *, char *, char *, struct storm_get_filereq *, int, storm_dbrec_addr *));
EXTERN_C int storm_list_pfr_entry _PROTO((struct srm_dbfd *, int, char *, struct storm_put_filereq *, int, storm_dbrec_addr *, int, DBLISTPTR *));
EXTERN_C int storm_get_pfr_by_surl _PROTO((struct srm_dbfd *, char *, char *, struct storm_put_filereq *, int, storm_dbrec_addr *));
EXTERN_C int storm_list_cpr_entry _PROTO((struct srm_dbfd *, int, char *, struct storm_copy_filereq *, int, storm_dbrec_addr *, int, DBLISTPTR *));
EXTERN_C int storm_list_protocol _PROTO((struct srm_dbfd *dbfd, char **protocol, int nbprot, int protlen, storm_dbrec_addr *rec_addr));
EXTERN_C int storm_get_cpr_by_surl _PROTO((struct srm_dbfd *, char *, char *, struct storm_copy_filereq *, int, storm_dbrec_addr *));
EXTERN_C int storm_relonefile _PROTO((struct srm_srv_thread_info *, char *, char, char *, int *));
EXTERN_C int storm_getonereqsummary _PROTO((struct srm_srv_thread_info *, char *,char *, int *, int *, int *));
EXTERN_C void set_savepoint _PROTO((struct srm_dbfd *, const char *));
EXTERN_C void rollback_to_savepoint _PROTO((struct srm_dbfd *, const char *));
EXTERN_C int _srmv2_init_contex(struct soap *soap, struct conn_extra_info *info, const char *func);
#endif

