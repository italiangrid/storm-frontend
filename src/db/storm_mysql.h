/*
 * storm_mysql.h
 *
 *  Created on: Dec 5, 2008
 *      Author: alb
 */

#ifndef STORM_MYSQL_H_
#define STORM_MYSQL_H_

typedef MYSQL_RES *DBLISTPTR;
typedef time_t storm_time_t;
typedef long long storm_size_t;
typedef char storm_dbrec_addr[21];
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

#endif /* STORM_MYSQL_H_ */
