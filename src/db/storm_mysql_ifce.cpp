/*
 * Copyright (C) 2004-2005 by INFN
 * All rights reserved
 */

#include <mysql/mysqld_error.h>
#include <mysql/mysql.h>
#include "storm_functions.h"
#include "srmlogit.h"
#include "srmv2H.h"
#include "storm_mysql.h"
#include <errno.h>
#include <string>
#include <map>
#include <vector>

int serrno = 0;
int seinternal = -1;

using namespace std;

map<string, vector<string> > * exec_query(struct srm_dbfd *dbfd, string query)
{
    static const char * const func = "exec_query";
    if (mysql_query(&dbfd->mysql, query.c_str())) {
        srmlogit (STORM_LOG_ERROR, func, "mysql_query error: %s. Query: ``%s''\n", mysql_error (&dbfd->mysql), query.c_str());
        throw seinternal;
    }

    MYSQL_RES *res;
    if ((res = mysql_store_result (&dbfd->mysql)) == NULL) {
        srmlogit (STORM_LOG_ERROR, func, "mysql_store_res error: %s\n", mysql_error (&dbfd->mysql));
        throw seinternal;
    }

    int num_fields = mysql_num_fields(res);
    MYSQL_FIELD *fields = mysql_fetch_fields(res);

    map<string, vector<string> > *dbmap = new map<string, vector<string> >();
    for(int i = 0; i < num_fields; i++)
        (*dbmap)[fields[i].name]=vector<string>();

    MYSQL_ROW row;
    while(NULL != (row = mysql_fetch_row(res)))
        for(int i = 0; i<num_fields; i++)
            (*dbmap)[fields[i].name].push_back((row[i] == NULL? "" : row[i]));

    mysql_free_result(res);
    return dbmap;

}

/************************
 * CONNECTION FUNCTIONS *
 ************************/
extern "C" int storm_opendb(char *db_srvr, char *db_user, char *db_pwd, struct srm_dbfd *dbfd)
{
    static const char *func = "storm_opendb";
    int ntries;

    mysql_init(&dbfd->mysql);

//#ifdef _USE_MYSQL5
//    my_bool valueTrue = 1;
//    mysql_options(&dbfd->mysql, MYSQL_OPT_RECONNECT, (void *) &valueTrue);
//#endif

    ntries = 0;
    while (1) {
        if (mysql_real_connect(&dbfd->mysql, db_srvr, db_user, db_pwd, "storm_db", 0, NULL, 0) == &dbfd->mysql) {
            srmlogit(STORM_LOG_DEBUG, func, "Connected to the DB.\n");
            return (0);
        }

        if (ntries++ >= MAXRETRY) {
        	break;
        }

        srmlogit(STORM_LOG_ERROR, func, "Failed to get a DB connection, trying again in %d seconds. Error: %s\n", RETRYI, mysql_error(&dbfd->mysql));
        sleep(RETRYI);
    }
    srmlogit (STORM_LOG_ERROR, func, "Cannot connect to the DB.\n");
    serrno = seinternal;
    return (-1);
}

extern "C" void storm_closedb(struct srm_dbfd *dbfd)
{
    mysql_close(&dbfd->mysql);
    return ;
}

extern "C" int storm_ping_connection(MYSQL *mysql)
{
	return mysql_ping(mysql);
}


/*************************
 * TRANSACTION FUNCTIONS *
 *************************/
extern "C" int storm_start_tr(int s, struct srm_dbfd *dbfd)
{
    (void) mysql_query(&dbfd->mysql, "BEGIN");
    dbfd->tr_started = 1;
    return(0);
}

extern "C" int storm_end_tr(struct srm_dbfd *dbfd)
{
    (void) mysql_query(&dbfd->mysql, "COMMIT");
    dbfd->tr_started = 0;
    return(0);
}

extern "C" void storm_abort_tr(struct srm_dbfd *dbfd)
{
    (void) mysql_query (&dbfd->mysql, "ROLLBACK");
    dbfd->tr_started = 0;
    return ;
}

extern "C" void set_savepoint(struct srm_dbfd *dbfd, const char * name)
{
    std::string query("SAVEPOINT ");
    query+=name;
    mysql_query(&dbfd->mysql, query.c_str());
    return ;
}

extern "C" void rollback_to_savepoint(struct srm_dbfd *dbfd, const char * name)
{
    std::string query("ROLLBACK TO SAVEPOINT ");
    query+=name;
    mysql_query(&dbfd->mysql, query.c_str());
    return ;
}


extern "C" int storm_exec_query(const char * const func, struct srm_dbfd *dbfd, const char *sql_stmt, MYSQL_RES **res)
{
    srmlogit(STORM_LOG_DEBUG2, func, "Executing query: ``%s''\n", sql_stmt);

    if (mysql_query(&dbfd->mysql, sql_stmt)) {
        srmlogit (STORM_LOG_ERROR, func, "mysql_query error: %s\n", mysql_error (&dbfd->mysql));
        serrno = seinternal;
        return(-1);
    }
    if ((*res = mysql_store_result (&dbfd->mysql)) == NULL) {
        srmlogit (STORM_LOG_ERROR, func, "mysql_store_res error: %s\n", mysql_error (&dbfd->mysql));
        serrno = seinternal;
        return(-1);
    }
    return(0);
}

/***************
 * FREE MEMORY *
 ***************/
/* free() a request->protocols array. This function should not be
 * called directly. You probably want call free_storm_req()
 * instead...
 * request should be not-NULL. request->protocols can be NULL.
*/
extern "C" int free_storm_req_transfer_params(struct storm_req *request)
{
    int i;
    if(NULL == request)
        return EINVAL;

    if(NULL == request->protocols )
        return 0;

    for(i=0; NULL != request->protocols[i]; i++)
        free(request->protocols[i]);

    return 0;
}

/* free() a storm_get_filereq array */
extern "C" void free_gfr_entry_array(struct storm_get_filereq **gfr_array)
{
    int i;
    if(NULL == gfr_array)
        return;

    for(i=0; NULL != gfr_array[i]; i++){
        if(NULL != gfr_array[i]->diroption)
            free(gfr_array[i]->diroption);
        free(gfr_array[i]);
    }
    free(gfr_array);
    return;
}

/* free() a storm_req struct
 * request should be not-NULL. request->protocols can be NULL.
*/
extern "C" int free_storm_req(struct storm_req *request)
{
    int ret = free_storm_req_transfer_params(request);
    if(0 != ret)
        return ret;
}

/**********************
 * DECODING FUNCTIONS *
 **********************/
static void storm_decode_cpr_entry(MYSQL_ROW row,
                       int lock,
                       storm_dbrec_addr *rec_addr,
                       struct storm_copy_filereq *cpr_entry)
{
    int i = 0;

    if (lock)
        strcpy(*rec_addr, row[i++]);
    if(row[i])
        strcpy(cpr_entry->r_token, row[i]);
    i++;

    if(row[i])
        cpr_entry->f_ordinal = atoi (row[i]);
    else
        cpr_entry->f_ordinal = -1;
    i++;

    if(row[i])
        strcpy(cpr_entry->from_surl, row[i]);
    i++;

    if(row[i])
        strcpy(cpr_entry->to_surl, row[i]);
    i++;

    if(row[i]){
        if(NULL == cpr_entry->lifetime)
            cpr_entry->lifetime = (u_signed64 *)malloc(sizeof(u_signed64));
        *(cpr_entry->lifetime) = atoi (row[i]);
    }else
        cpr_entry->lifetime = NULL;
    i++;

    if(row[i])
        cpr_entry->f_type = *row[i];
    else
        cpr_entry->f_type = 'X';
    i++;

    if(row[i])
        strcpy(cpr_entry->s_token, row[i]);
    i++;

    if(row[i])
        cpr_entry->flags = atoi (row[i]);
    else
        cpr_entry->flags = -1;
    i++;

//     if(row[i])
//         cpr_entry->actual_size = strtou64 (row[i]);
//     else
//         cpr_entry->actual_size = -1;
    i++;

    if(row[i])
        cpr_entry->status = atoi (row[i]);
    else
        cpr_entry->status = -1;
    i++;

    if(row[i])
        strcpy(cpr_entry->errstring, row[i]);
    i++;

}

static void storm_decode_gfr_entry(MYSQL_ROW row, int lock, storm_dbrec_addr *rec_addr, struct storm_get_filereq *gfr_entry)
{
    int i = 0;

    /* TODO: make this mess more like the 'lifetime' field */
    if (lock)
        strcpy(*rec_addr, row[i++]);

    if(row[i])
        strcpy(gfr_entry->r_token, row[i]);
    i++;

    if(row[i])
        gfr_entry->f_ordinal = atoi (row[i]);
    else
        gfr_entry->f_ordinal = -1;
    i++;

    if(row[i])
        strcpy(gfr_entry->from_surl, row[i]);
    i++;

    if(row[i]){
        if(NULL == gfr_entry->lifetime)
            gfr_entry->lifetime = (u_signed64 *)malloc(sizeof(u_signed64));
        *(gfr_entry->lifetime) = atoi (row[i]);
    }else
        gfr_entry->lifetime = NULL;
    i++;

    if(row[i])
        gfr_entry->f_type = *row[i];
    else
        gfr_entry->f_type = 'X';
    i++;

    if(row[i])
        strcpy(gfr_entry->s_token, row[i]);
    i++;

    if(row[i])
        gfr_entry->flags = atoi (row[i]);
    else
        gfr_entry->flags = -1;
    i++;

    if(row[i])
        strcpy(gfr_entry->server, row[i]);
    i++;

    if(row[i])
        strcpy(gfr_entry->pfn, row[i]);
    i++;

//     if(row[i])
//         gfr_entry->actual_size = strtou64 (row[i]);
//     else
//         gfr_entry->actual_size = -1;
    i++;

    if(row[i])
        gfr_entry->status = atoi (row[i]);
    else
        gfr_entry->status = -1;
    i++;

    if(row[i])
        strcpy(gfr_entry->errstring, row[i]);
    i++;

}

static void storm_decode_pfr_entry(MYSQL_ROW row,
                            int lock, storm_dbrec_addr *rec_addr,
                            struct storm_put_filereq *pfr_entry)
{
    int i = 0;

    if (lock) strcpy(*rec_addr, row[i++]);

    if (row[i]) strcpy(pfr_entry->r_token, row[i]);
    i++;

    if (row[i]) pfr_entry->f_ordinal = atoi(row[i]);
    else pfr_entry->f_ordinal = -1;
    i++;

    if(row[i]) strcpy(pfr_entry->to_surl, row[i]);
    i++;

    if(row[i]){
        if (NULL == pfr_entry->lifetime) pfr_entry->lifetime = (u_signed64 *)malloc(sizeof(u_signed64));
        *(pfr_entry->lifetime) = atoi(row[i]);
    } else pfr_entry->lifetime = NULL;
    i++;

    if (row[i]) pfr_entry->f_type = *row[i];
    else pfr_entry->f_type = 'X';
    i++;

    if(row[i]) strcpy(pfr_entry->s_token, row[i]);
    i++;

//     if (row[i]) pfr_entry->requested_size = strtou64(row[i]);
//     else pfr_entry->requested_size = -1;
    i++;

    if(row[i]) strcpy(pfr_entry->server, row[i]);
    i++;

    if(row[i]) strcpy(pfr_entry->pfn, row[i]);
    i++;

//     if(row[i]) pfr_entry->actual_size = strtou64(row[i]);
//     else pfr_entry->actual_size = -1;
    i++;

    if(row[i]) pfr_entry->status = atoi(row[i]);
    else pfr_entry->status = -1;
    i++;

    if(row[i]) strcpy(pfr_entry->errstring, row[i]);
    i++;
}

static void storm_decode_xferreq_entry(MYSQL_ROW row, int lock, storm_dbrec_addr *rec_addr, struct storm_req *storm_req)
{
    int i = 0;

    if (lock) strcpy(*rec_addr, row[i++]);

    storm_req->r_ordinal = atoi(row[i++]);
    strcpy(storm_req->r_token, row[i++]);
    storm_req->r_uid = atoi(row[i++]);
    storm_req->r_gid = atoi(row[i++]);
    strcpy(storm_req->client_dn, row[i++]);
    strcpy(storm_req->clienthost, row[i++]);
    i++; //    storm_req->r_type = *row[i++];
    strcpy(storm_req->protocol, row[i++]);
    strcpy(storm_req->u_token, row[i++]);
    storm_req->flags = atoi(row[i++]);
    storm_req->retrytime = atoi(row[i++]);
    storm_req->nbreqfiles = atoi(row[i++]);
    storm_req->ctime = atoi(row[i++]);
    storm_req->stime = atoi(row[i++]);
    storm_req->etime = atoi(row[i++]);
    storm_req->status = atoi(row[i++]);
    strcpy(storm_req->errstring, row[i]);
}


/***********************
 * INSERTING FUNCTIONS *
 ***********************/

/*********************
 * QUERING FUNCTIONS *
 *********************/

extern "C" int storm_get_cpr_by_surl(struct srm_dbfd *dbfd,
                      char *r_token,
                      char *surl,
                      struct storm_copy_filereq *cpr_entry,
                      int lock,
                      storm_dbrec_addr *rec_addr)
{
    static const char *func = "storm_get_cpr_by_surl";
    static char query[] =
        "SELECT \
         R_TOKEN, F_ORDINAL, FROM_SURL, \
         TO_SURL, LIFETIME, F_TYPE, \
         S_TOKEN, FLAGS, ACTUAL_SIZE, \
         STATUS, ERRSTRING \
        FROM storm_copy_filereq \
        WHERE r_token = '%s' AND (from_surl = '%s' OR to_surl = '%s')";
    static char query4upd[] =
        "SELECT ROWID, \
         R_TOKEN, F_ORDINAL, FROM_SURL, \
         TO_SURL, LIFETIME, F_TYPE, \
         S_TOKEN, FLAGS, ACTUAL_SIZE, \
         STATUS, ERRSTRING \
        FROM storm_copy_filereq \
        WHERE r_token = '%s' AND (from_surl = '%s' OR to_surl = '%s') \
        FOR UPDATE";
            MYSQL_RES *res;
        MYSQL_ROW row;
        char sql_stmt[2468];

    sprintf (sql_stmt, lock ? query4upd : query, r_token, surl, surl);
    if (storm_exec_query (func, dbfd, sql_stmt, &res))
        return (-1);
    if ((row = mysql_fetch_row (res)) == NULL) {
        mysql_free_result (res);
        serrno = ENOENT;
        return (-1);
    }
    storm_decode_cpr_entry (row, lock, rec_addr, cpr_entry);
    mysql_free_result (res);
    return (0);
}

extern "C" int storm_get_cpr_by_surls(struct srm_dbfd *dbfd,
                       char *r_token,
                       char *from_surl,
                       char *to_surl,
                       struct storm_copy_filereq *cpr_entry,
                       int lock,
                       storm_dbrec_addr *rec_addr)
{
    static const char *func = "storm_get_cpr_by_surls";
    static char query[] =
        "SELECT \
         R_TOKEN, F_ORDINAL, FROM_SURL, \
         TO_SURL, LIFETIME, F_TYPE, \
         S_TOKEN, FLAGS, ACTUAL_SIZE, \
         STATUS, ERRSTRING \
        FROM storm_copy_filereq \
        WHERE r_token = '%s' AND from_surl = '%s' AND to_surl = '%s'";
    static char query4upd[] =
        "SELECT ROWID, \
         R_TOKEN, F_ORDINAL, FROM_SURL, \
         TO_SURL, LIFETIME, F_TYPE, \
         S_TOKEN, FLAGS, ACTUAL_SIZE, \
         STATUS, ERRSTRING \
        FROM storm_copy_filereq \
        WHERE r_token = '%s' AND from_surl = '%s' AND to_surl = '%s' \
        FOR UPDATE";
            MYSQL_RES *res;
        MYSQL_ROW row;
        char sql_stmt[2469];

    sprintf (sql_stmt, lock ? query4upd : query, r_token, from_surl, to_surl);
    if (storm_exec_query (func, dbfd, sql_stmt, &res))
        return (-1);
    if ((row = mysql_fetch_row (res)) == NULL) {
        mysql_free_result (res);
        serrno = ENOENT;
        return (-1);
    }
    storm_decode_cpr_entry (row, lock, rec_addr, cpr_entry);
    mysql_free_result (res);
    return (0);
}


extern "C" int storm_get_gfr_by_surl(struct srm_dbfd *dbfd,
                          char *r_token,
                          char *from_surl,
                          struct storm_get_filereq *gfr_entry,
                          int lock,
                          storm_dbrec_addr *rec_addr)
{
    static const char *func="storm_get_gfr_by_surl";
    static char query[] =
        "SELECT \
         R_TOKEN, F_ORDINAL, FROM_SURL, \
         LIFETIME, F_TYPE, S_TOKEN, \
         FLAGS, SERVER, PFN, \
         ACTUAL_SIZE, STATUS, ERRSTRING \
        FROM storm_get_filereq \
        WHERE r_token = '%s' AND from_surl = '%s'";
    static char query4upd[] =
        "SELECT ROWID, \
         R_TOKEN, F_ORDINAL, FROM_SURL, \
         LIFETIME, F_TYPE, S_TOKEN, \
         FLAGS, SERVER, PFN, \
         ACTUAL_SIZE, STATUS, ERRSTRING \
        FROM storm_get_filereq \
        WHERE r_token = '%s' AND from_surl = '%s' \
        FOR UPDATE";
    MYSQL_RES *res;
    MYSQL_ROW row;
    char sql_stmt[1344];
    int ret;

    ret = snprintf (sql_stmt, 1344, lock ? query4upd : query, r_token, from_surl);
    if(ret >= 1344 || ret < 0){
        serrno = ENOMEM;
        return -1;
    }
    if (storm_exec_query (func, dbfd, sql_stmt, &res))
        return (-1);
    if ((row = mysql_fetch_row (res)) == NULL) {
        mysql_free_result (res);
        serrno = ENOENT;
        return (-1);
    }
    storm_decode_gfr_entry (row, lock, rec_addr, gfr_entry);
    mysql_free_result (res);
    return (0);
}


extern "C" int storm_get_pending_req_by_token(struct srm_dbfd *dbfd, char *r_token, struct storm_req *storm_req,
                                   int lock, storm_dbrec_addr *rec_addr)
{
    static const char *func = "storm_get_pending_req_by_token";
    static const char *query =
                        "SELECT R_ORDINAL, R_TOKEN, R_UID, R_GID, CLIENT_DN, CLIENTHOST, R_TYPE, "
                        "PROTOCOL, U_TOKEN, FLAGS, RETRYTIME, NBREQFILES, CTIME, STIME, ETIME, STATUS, ERRSTRING "
                        "FROM storm_pending_req "
                        "WHERE r_token = '%s'";
    static const char *query4upd =
                        "SELECT ROWID, R_ORDINAL, R_TOKEN, R_UID, R_GID, CLIENT_DN, CLIENTHOST, R_TYPE, PROTOCOL, "
                        "U_TOKEN, FLAGS, RETRYTIME, NBREQFILES, CTIME, STIME, ETIME, STATUS, ERRSTRING "
                        "FROM storm_pending_req "
                        "WHERE r_token = '%s' "
                        "FOR UPDATE";
    char sql_stmt[1024];
    MYSQL_RES *res;
    MYSQL_ROW row;

    sprintf(sql_stmt, lock ? query4upd : query, r_token);

    if (storm_exec_query(func, dbfd, sql_stmt, &res)) return(-1);

    if ((row = mysql_fetch_row(res)) == NULL) {
        mysql_free_result(res);
        return(-ENOENT);
    }
    storm_decode_xferreq_entry(row, lock, rec_addr, storm_req);
    mysql_free_result(res);
    return(0);
}


extern "C" int storm_get_pfr_by_surl(struct srm_dbfd *dbfd,
                          char *r_token,
                          char *to_surl,
                          struct storm_put_filereq *pfr_entry,
                          int lock,
                          storm_dbrec_addr *rec_addr)
{
    static const char *func = "storm_get_pfr_by_surl";
    static char query[] =
                    "SELECT R_TOKEN, F_ORDINAL, TO_SURL, LIFETIME, F_TYPE, S_TOKEN, REQUESTED_SIZE, "
                    "SERVER, PFN, ACTUAL_SIZE, STATUS, ERRSTRING "
                    "FROM storm_put_filereq "
                    "WHERE r_token = '%s' AND to_surl = '%s'";
    static char query4upd[] =
                    "SELECT ROWID, R_TOKEN, F_ORDINAL, TO_SURL, LIFETIME, F_TYPE, S_TOKEN, "
                    "REQUESTED_SIZE, SERVER, PFN, ACTUAL_SIZE, STATUS, ERRSTRING "
                    "FROM storm_put_filereq "
                    "WHERE r_token = '%s' AND to_surl = '%s' "
                    "FOR UPDATE";
    char sql_stmt[1349];
    MYSQL_RES *res;
    MYSQL_ROW row;

    sprintf(sql_stmt, lock ? query4upd : query, r_token, to_surl);

    if (storm_exec_query(func, dbfd, sql_stmt, &res)) return(-1);

    if ((row = mysql_fetch_row(res)) == NULL) {
        mysql_free_result (res);
        serrno = ENOENT;
        return(-1);
    }

    storm_decode_pfr_entry(row, lock, rec_addr, pfr_entry);
    mysql_free_result (res);
    return(0);
}

extern "C" int storm_get_req_by_token(struct srm_dbfd *dbfd, char *r_token, struct storm_req *storm_req,
                           int lock, storm_dbrec_addr *rec_addr)
{
    static const char *func = "storm_get_req_by_token";
    static const char *query = "SELECT R_ORDINAL, R_TOKEN, R_UID, R_GID, CLIENT_DN, CLIENTHOST, R_TYPE, "
                               "PROTOCOL, U_TOKEN, FLAGS, RETRYTIME, NBREQFILES, CTIME, STIME, ETIME, "
                               "STATUS, ERRSTRING "
                               "FROM storm_req "
                               "WHERE r_token = '%s'";
    static const char *query4upd = "SELECT ROWID, R_ORDINAL, R_TOKEN, R_UID, R_GID, CLIENT_DN, CLIENTHOST, "
                                   "R_TYPE, PROTOCOL, U_TOKEN, FLAGS, RETRYTIME, NBREQFILES, "
                                   "CTIME, STIME, ETIME, STATUS, ERRSTRING "
                                   "FROM storm_req "
                                   "WHERE r_token = '%s' "
                                   "FOR UPDATE";
    char sql_stmt[1024];
    MYSQL_RES *res;
    MYSQL_ROW row;

    snprintf(sql_stmt,1024, lock ? query4upd : query, r_token);

    if (storm_exec_query(func, dbfd, sql_stmt, &res)) return(-1);

    if ((row = mysql_fetch_row(res)) == NULL) {
        mysql_free_result(res);
        return(-ENOENT);
    }
    storm_decode_xferreq_entry(row, lock, rec_addr, storm_req);
    mysql_free_result(res);
    return(0);
}


extern "C" int storm_insert_cpr_entry(struct srm_dbfd *dbfd,
                       struct storm_copy_filereq *cpr_entry)
{
    static const char *func ="storm_insert_cpr_entry";
    char filesize_str[21];
    static char insert_stmt_lifetime[] =
        "INSERT INTO storm_copy_filereq \
        (R_TOKEN, F_ORDINAL, FROM_SURL, \
         TO_SURL, LIFETIME, F_TYPE, \
         S_TOKEN, FLAGS, ACTUAL_SIZE, \
         STATUS, ERRSTRING) \
        VALUES \
        ('%s', %d, '%s', '%s', %lld, '%c', '%s', %d, '%s', %d, '%s')";
    static char insert_stmt_no_lifetime[] =
        "INSERT INTO storm_copy_filereq \
        (R_TOKEN, F_ORDINAL, FROM_SURL, \
         TO_SURL, F_TYPE, \
         S_TOKEN, FLAGS, ACTUAL_SIZE, \
         STATUS, ERRSTRING) \
        VALUES \
        ('%s', %d, '%s', '%s', '%c', '%s', %d, '%s', %d, '%s')";
    char sql_stmt[2800];
    int ret;

    if(NULL == cpr_entry->lifetime){
        ret = snprintf (sql_stmt, 2800, insert_stmt_no_lifetime,
                 cpr_entry->r_token, cpr_entry->f_ordinal, cpr_entry->from_surl,
                 cpr_entry->to_surl, cpr_entry->f_type,
                 cpr_entry->s_token, cpr_entry->flags,
                 u64tostr (cpr_entry->actual_size, filesize_str, -1),
                 cpr_entry->status, cpr_entry->errstring);
        if(ret >= 2800 || ret < 0){
            serrno = ENOMEM;
            return -1;
        }
    }else{
        ret = snprintf (sql_stmt, 2800, insert_stmt_lifetime,
                 cpr_entry->r_token, cpr_entry->f_ordinal, cpr_entry->from_surl,
                 cpr_entry->to_surl,(long long)*(cpr_entry->lifetime), cpr_entry->f_type,
                 cpr_entry->s_token, cpr_entry->flags,
                 u64tostr (cpr_entry->actual_size, filesize_str, -1),
                 cpr_entry->status, cpr_entry->errstring);
        if(ret >= 2800 || ret < 0){
            serrno = ENOMEM;
            return -1;
        }
    }

    srmlogit(STORM_LOG_DEBUG2,func,"Executing query: ``%s''\n",sql_stmt);
    if (mysql_query (&dbfd->mysql, sql_stmt)) {
        if (mysql_errno (&dbfd->mysql) == ER_DUP_ENTRY)
            serrno = EEXIST;
        else {
            srmlogit (STORM_LOG_ERROR, func, "INSERT error: %s\n",
                mysql_error (&dbfd->mysql));
            serrno = seinternal;
        }
        return (-1);
    }
    return (0);
}

/*
   This function take a gfr_entry struct pointer and the ID of the
   request and insert a get entry into the DB.

   return code:
    0 => success
   -1 => error inserting in DirOption
   -2 => error inserting in request_Get
   -2 => error inserting in status_Get

   in case of error, serrno can be:
   ENOMEM     => error allocating memory (probably in snprintf)
*/
extern "C" int storm_insert_gfr_entry(struct srm_dbfd *dbfd,
                           struct storm_get_filereq *gfr_entry,
                           storm_id_t request_id)
{
    static const char *func = "storm_insert_gfr_entry";
    static const char *diroption_stmt =
        "INSERT INTO request_DirOption "
        "(isSourceADirectory, allLevelRecursive, numOfLevels) values "
        "(%s, %s, %s)";
    static const char *get_stmt =
        "INSERT INTO request_Get "
        "(request_DirOptionID, request_queueID, sourceSURL) values "
        "(%s, %lu, '%s')";
    static const char *getstatus_stmt =
        "INSERT INTO status_Get"
        "(statusCode,  request_GetID) values"
        "(%d,  %lu)";
    char filesize_str[21];
    char sql_stmt[2880];
    int ret;

    char srcdir_str[4];
    char recursive_str[4];
    char numlevel_str[sizeof(int)*2+3];
    char diroptionID_str[sizeof(int)*2+3];
    storm_id_t diroptionID;
    storm_id_t getID;

    if(NULL == gfr_entry){
        serrno = EINVAL;
        return -1;
    }

    /* Insert into request_DirOption table, if needed */
    if(NULL != gfr_entry->diroption){
        struct storm_diroption *d = gfr_entry->diroption;
        ret = snprintf(
            sql_stmt, 2880, diroption_stmt,
            itoa(srcdir_str, d->srcdir),
            d->recursive <0 ? "NULL" : itoa(recursive_str, d->recursive),
            d->numlevel <0 ? "NULL" : itoa(numlevel_str, d->numlevel));
        if ((ret >= 2880) || (ret < 0)) {
            serrno = ENOMEM;
            return(-1);
        }
        if(mysql_query(&dbfd->mysql, sql_stmt)){
            srmlogit(STORM_LOG_ERROR, func, "Error in query = '%s', error: '%s'",sql_stmt, mysql_error(&dbfd->mysql));
            serrno = seinternal;
            return -1;
        }
        diroptionID = (storm_id_t) mysql_insert_id(&dbfd->mysql);
    }else{
        diroptionID = -1;
        strcpy(diroptionID_str,"NULL");
    }

    /* Insert into request_Get table */
    ret = snprintf(
        sql_stmt, 2880, get_stmt,
        diroptionID == -1? "NULL" : itoa(diroptionID_str, diroptionID),
        (unsigned long)request_id,
        gfr_entry->surl);
    if ((ret >= 2880) || (ret < 0)) {
        serrno = ENOMEM;
        return(-2);
    }
    if(mysql_query(&dbfd->mysql, sql_stmt)){
        srmlogit(STORM_LOG_ERROR, func, "Error in query = '%s', error: '%s'",sql_stmt, mysql_error(&dbfd->mysql));
        serrno = seinternal;
        return -2;
    }
    getID = mysql_insert_id(&dbfd->mysql);

    /* Insert into status_Get */
    ret = snprintf(
        sql_stmt, 2880, getstatus_stmt,
        SRM_USCOREREQUEST_USCOREQUEUED,
        (unsigned long)getID);
    if ((ret >= 2880) || (ret < 0)) {
        serrno = ENOMEM;
        return(-3);
    }
    if(mysql_query(&dbfd->mysql, sql_stmt)){
        srmlogit(STORM_LOG_ERROR, func, "Error in query = '%s', error: '%s'",sql_stmt, mysql_error(&dbfd->mysql));
        serrno = seinternal;
        return -3;
    }

    return (0);
}

extern "C" int storm_insert_gfr_entry_array(struct srm_dbfd *dbfd,
                                 struct storm_get_filereq **gfr_array,
                                 storm_id_t request_id)
{
    int i;
    if(NULL == dbfd || NULL == gfr_array)
        return EINVAL;
    for( i = 0; NULL != gfr_array[i]; i++){
        int ret = storm_insert_gfr_entry(dbfd, gfr_array[i], request_id);
        if(0 != ret)
            return ret;
    }
    return 0;
}

/* This function uses the MYSQL_RES res and fill the array pointed by
   gfr_array_ptr.  The function allocate memory. You should free() the
   array calling free_gfr_entry_array().

   return:

   EINVAL  invalid argument (gfr_array_ptr == NULL or res == NULL)
   ENOMEM
   ERAMGE the number of requested file is greater of the number of
          SURL found in the DB for the request.

   Note: this function does not call mysql_free_result().
 */
static int _fill_getreq_array_from_dbres(MYSQL_RES *res,
                                         struct storm_get_filereq ***gfr_array_ptr,
                                         struct storm_req *req)
{
/* Cfr storm_get_request_status.
 *   The query used should be:
 *
 * SELECT r.ID, r.r_token, r.status, r.errstring, r.nbreqfiles,
 * g.sourceSURL, s.statusCode, s.explanation, s.transferURL FROM
 * request_queue r JOIN (request_Get g, status_Get s) ON
 * (r.ID=g.request_queueID AND g.ID=s.request_GetID) WHERE r.r_token='%s'
 * AND r.config_RequestTypeID='%s';
 *
 * The field are:
 *
 *   row[0] = r.ID
 *   row[1] = r.r_token
 *   row[2] = r.status
 *   row[3] = r.errstring
 *   row[4] = r.nbreqfiles
 *   row[5] = g.sourceSURL
 *   row[6] = s.statusCode
 *   row[7] = s.explanation
 *   row[8] = s.transferURL
*/
    static const char * const func="_fill_getreq_array_from_dbres";
    MYSQL_ROW row;
    struct storm_get_filereq **gfr_array;
    int size, nr_request;
    int ret;
    int i;
    /* Check... */
    if(NULL == res || NULL == gfr_array_ptr || NULL == req)
        return EINVAL;
    if(9 != (ret = mysql_num_fields(res))){ /* this should never happen... */
        srmlogit(STORM_LOG_ERROR, func, "Oops! Fatal DB error: I'm expecting ONLY 9 field, but got %d. Exiting.\n",ret);
        return EINVAL;
    }

    /* Get the number of surl */
    size = mysql_num_rows(res);

    /* empty the status of the request */
    req->nr_completed=0;
    req->nr_waiting=0;
    req->nr_failed=0;

    /* allocate memory for the (NULL-terminated) array */
    *gfr_array_ptr = (struct storm_get_filereq **)calloc(size+1 , sizeof(struct storm_get_filereq *));
    gfr_array = *gfr_array_ptr;
    if(NULL == gfr_array)
        return ENOMEM;

    for (i = 0; NULL != (row = mysql_fetch_row(res)); i++){
        if(0 == i){ /* Check if the nbreqfiles field is correct */
            if(atoi(row[4]) > size){
                free_gfr_entry_array(gfr_array);
                return ERANGE;
            }
        }
        gfr_array[i] = (struct storm_get_filereq *)calloc(1,sizeof(struct storm_get_filereq));
        if(NULL == gfr_array[i]){
            free_gfr_entry_array(gfr_array);
            return ENOMEM;
        }
        /* Fill some field... */
        /* surl */
        strncpy(gfr_array[i]->surl, row[5], ST_MAXSFNLEN+1);
        /* status */
        gfr_array[i]->status = atoi(row[6]);
        switch(gfr_array[i]->status){
        case SRM_USCOREREQUEST_USCOREQUEUED:
        case SRM_USCOREREQUEST_USCOREINPROGRESS:
/*      case SRM_SPACE_AVAILABLE: /\* Only for PUT request *\/ */
            req->nr_waiting++;
            break;
        case SRM_USCOREDONE:
        case SRM_USCORESUCCESS:
            req->nr_completed++;
            break;
        default:
            req->nr_failed++;
            break;
        }
        if(NULL != row[7])
            strncpy(gfr_array[i]->errstring, row[7], 256);
        if(NULL != row[8])
            strncpy(gfr_array[i]->turl, row[8], ST_MAXSFNLEN+1);
    }
    /* Global status of the request */

    if(req->nr_waiting > 0){ /* Some request not yet completed */
        if(req->status != SRM_USCOREREQUEST_USCOREINPROGRESS
           && req->status != SRM_USCOREREQUEST_USCOREQUEUED) /* should never happen */
        req->status = SRM_USCOREREQUEST_USCOREQUEUED;
    }else{ /* all request completed */
        if(req->nr_completed > 0){ /* some request successful */
            if(req->nr_failed > 0) /* some request failed */
                req->status = SRM_USCOREPARTIAL_USCORESUCCESS;
            else /* *ALL* request successful */
                req->status = SRM_USCORESUCCESS;
        }else{ /* no request successful */
            req->status = SRM_USCOREFAILURE;
        }
    }

    return 0;
}
/* request status */
/*
 * request->r_type MUST be filled. It will be used to distinguish
 *                            between Get request and Put request.
 * request->r_token MUST be filled.
 *
 * request_array must a *** pointer to a struct storm_get_filereq,
 * storm_put_filereq or storm_copy_filereq. The array will be
 * allocated dinamically. Please, call free_gfr_entry_array() to free
 * memory
 *
 * Return code:
 *     0       success
 *     EINVAL  request is NULL, request->r_type unknown
 *     ENOMEM  unable to allocate memory
 *     ENOENT  there is no request with the given request token
 *     serrno  the value of the global var serrno as returned by storm_exec_query()
 */
extern "C" int storm_get_gfr_status(struct srm_dbfd *dbfd,
                         struct storm_req *request,
                         struct storm_get_filereq ***filerequest_array)
{
    static const char * const func="storm_get_request_status";
    static const char * const select_queue_stmt =
        "SELECT r.ID, r.r_token, r.status, r.errstring, r.nbreqfiles"
        ", g.sourceSURL, s.statusCode, s.explanation, s.transferURL"
        "FROM request_queue r JOIN (request_Get g, status_Get s) ON"
        "(r.ID=g.request_queueID AND g.ID=s.request_GetID)"
        " WHERE r.r_token='%s' AND r.config_RequestTypeID='%s'";
    static const char * const select_process_stmt =
        "SELECT r.ID, r.r_token, r.status, r.errstring, r.nbreqfiles"
        ", g.sourceSURL, s.statusCode, s.explanation, s.transferURL"
        "FROM request_process r JOIN (request_Get g, status_Get s) ON"
        "(r.ID=g.request_queueID AND g.ID=s.request_GetID)"
        " WHERE r.r_token='%s' AND r.config_RequestTypeID='%s'";
    char sql_stmt[1880];
    int sql_size = 1880;
    int ret;
    int num_rows;
    MYSQL_RES *res;
    MYSQL_ROW row;

    if(NULL == filerequest_array
       || NULL == request
       || NULL == dbfd)
        return EINVAL;

    /* Check if the request is in progress */
    ret = snprintf(sql_stmt, sql_size, select_process_stmt, request->r_token, request->r_type);
    if ((ret >= sql_size) || (ret < 0))
        return ENOMEM;

    if (0 != storm_exec_query(func, dbfd, sql_stmt, &res))
        return serrno;


    /* If not in progress, check if the request is queued */
    if(0 == (num_rows = mysql_num_rows(res))){
        ret = snprintf(sql_stmt, sql_size, select_queue_stmt, request->r_token, request->r_type);
        if ((ret >= sql_size) || (ret < 0)){
            mysql_free_result(res);
            return ENOMEM;
        }
        if (0 != storm_exec_query(func, dbfd, sql_stmt, &res)){
            mysql_free_result(res);
            return serrno;
        }
        /* The r_token does not match any request, nor in the process
         * table, nor in the queue table. */
        if(0 == (num_rows = mysql_num_rows(res))){
            mysql_free_result(res);
            return ENOENT;
        }
        request->status = SRM_USCOREREQUEST_USCOREQUEUED;
    }else
        request->status = SRM_USCOREREQUEST_USCOREINPROGRESS;

    ret = _fill_getreq_array_from_dbres(res,filerequest_array, request);

    mysql_free_result(res);

    return ret;
}

/*
  Return:
  int > 0        the ID of the request
  -1             generic inserting error
  -ENOPROTOOPT   protocols not supported
*/
extern "C" storm_id_t storm_insert_pending_entry(struct srm_dbfd *dbfd,
                                      struct storm_req *storm_req)
{
    static const char *func = "storm_insert_pending_entry";
    static const char *insert_stmt =
        "INSERT INTO request_queue "
        "(client_dn, u_token, config_FilestorageTypeID, "
        "r_token, config_RequestTypeID, config_OverwriteID, "
        "pinLifetime, nbreqfiles, timeStamp, status) VALUES "
        "('%s', %s, %s, "
        "'%s', '%s', %s, "
        "%s, %d, current_timestamp(), %d)";
    char sql_stmt[2888];
    int sql_size=2888;
    int ret;

    char retrytime_str[sizeof(storm_req->retrytime)*2+3];
    char pinlifetime_str[sizeof(storm_req->lifetime)*2+3];
    char ftype_str[4];
    char overwrite_str[4];

    static const char *protocol_stmt =
        "INSERT INTO request_TransferProtocols "
        "(request_queueID, config_protocolsID) values "
        "(%lu, '%s')";

    ret = snprintf(
        sql_stmt, sql_size, insert_stmt,
        storm_req->client_dn,
        storm_req->u_token[0] == '\0'? "NULL" : storm_req->u_token,
        storm_req->f_type == DB_FILE_TYPE_UNKNOWN? "NULL" : ctoa(ftype_str, storm_req->f_type),
        storm_req->r_token,
        storm_req->r_type,
        storm_req->overwrite == DB_OVERWRITE_UNKNOWN ? "NULL": ctoa(overwrite_str, storm_req->overwrite),
        storm_req->pinlifetime < 0? "NULL" : itoa(pinlifetime_str, storm_req->pinlifetime),
        storm_req->nbreqfiles,
        storm_req->status);

    if ((ret >= sql_size) || (ret < 0)){
        serrno = ENOMEM;
        return(-1);
    }

    srmlogit(STORM_LOG_DEBUG2, func, "Executing query: ``%s''\n", sql_stmt);

    if (mysql_query(&dbfd->mysql, sql_stmt)) {
        if (mysql_errno(&dbfd->mysql) == ER_DUP_ENTRY) serrno = EEXIST;
        else {
            srmlogit (STORM_LOG_ERROR, func, "INSERT error: %s\n", mysql_error (&dbfd->mysql));
            serrno = seinternal;
        }
        return(-1);
    }
    /* Fill into the storm_req struct the request ID retourned by MySQL */
    storm_req->request_id = (storm_id_t)mysql_insert_id(&dbfd->mysql);

    /* Devo inserire i protocolli */
    if(NULL != storm_req->protocols && NULL != storm_req->protocols[0]){
        int i;
        int inserted=0;
        int no_proto=0;
        for(i = 0; NULL != storm_req->protocols[i]; i++){
            snprintf(sql_stmt, sql_size, protocol_stmt, (unsigned long)storm_req->request_id, storm_req->protocols[i]);
            if(mysql_query(&dbfd->mysql, sql_stmt)){
                if(ER_NO_REFERENCED_ROW == mysql_errno(&dbfd->mysql))
                    no_proto++; /* Protocol not supported */
                srmlogit(STORM_LOG_ERROR, func, "Error inserting protocol %s. Continuing. query = '%s', error: '%s'\n",storm_req->protocols[i], sql_stmt, mysql_error(&dbfd->mysql));
                serrno = seinternal;
            }else
               inserted++;
        }
        if(0 == inserted){ /* None of specified protocols is supported.*/
            srmlogit(STORM_LOG_ERROR, func, "None of specified protocol is supported\n");
            serrno = seinternal;
            if(no_proto == i)
                return -ENOPROTOOPT;
            else
                return -1;
        }
    }

    /* Insert into request_ClientNetworks NOT IMPLEMENTED (but very
     * like the above code) */

    /* Insert into request_ExtraInfo NOT IMPLEMENTED */

    /* Insert into request_RetentionPolicyInfo NOT IMPLEMENTED */

    return(storm_req->request_id);
}

extern "C" int storm_insert_pfr_entry(struct srm_dbfd *dbfd, struct storm_put_filereq *pfr_entry)
{
    static const char *func = "storm_insert_pfr_entry";
    static const char *insert_stmt =
                    "INSERT INTO storm_put_filereq \
                    (R_TOKEN, F_ORDINAL, TO_SURL, \
                    LIFETIME, F_TYPE, S_TOKEN, \
                    REQUESTED_SIZE, SERVER, PFN, \
                    ACTUAL_SIZE, STATUS, ERRSTRING) \
                    VALUES \
                    ('%s', %d, '%s', %lld, '%c', '%s', %s, '%s', '%s', %s, %d, '%s')";
    static const char *insert_stmt_no_lifetime =
                    "INSERT INTO storm_put_filereq \
                    (R_TOKEN, F_ORDINAL, TO_SURL, \
                    F_TYPE, S_TOKEN, \
                    REQUESTED_SIZE, SERVER, PFN, \
                    ACTUAL_SIZE, STATUS, ERRSTRING) \
                    VALUES \
                    ('%s', %d, '%s', '%c', '%s', %s, '%s', '%s', %s, %d, '%s')";
    char reqsize_str[21];
    char filesize_str[21];
    char sql_stmt[2880];
    int ret;

    if(NULL == pfr_entry){
        serrno=ENOMEM;
        return -1;
    }

    if(NULL != pfr_entry->lifetime){
        ret = snprintf (sql_stmt, 2880, insert_stmt,
                 pfr_entry->r_token, pfr_entry->f_ordinal, pfr_entry->to_surl,
                 (long long)*(pfr_entry->lifetime), pfr_entry->f_type, pfr_entry->s_token,
                 u64tostr (pfr_entry->requested_size, reqsize_str, -1),
                 pfr_entry->server, pfr_entry->pfn,
                 u64tostr (pfr_entry->actual_size, filesize_str, -1), pfr_entry->status,
                 pfr_entry->errstring);
        if(ret >= 2880 || ret < 0){
            serrno = ENOMEM;
            return -1;
        }

    }else{
        ret = snprintf (sql_stmt, 2880, insert_stmt_no_lifetime,
                 pfr_entry->r_token, pfr_entry->f_ordinal, pfr_entry->to_surl,
                 pfr_entry->f_type, pfr_entry->s_token,
                 u64tostr (pfr_entry->requested_size, reqsize_str, -1),
                 pfr_entry->server, pfr_entry->pfn,
                 u64tostr (pfr_entry->actual_size, filesize_str, -1), pfr_entry->status,
                 pfr_entry->errstring);
        if(ret >= 2880 || ret < 0){
            serrno = ENOMEM;
            return -1;
        }
    }
    srmlogit(STORM_LOG_DEBUG2,func,"Executing query: ``%s''\n",sql_stmt);
    if (mysql_query (&dbfd->mysql, sql_stmt)) {
        if (mysql_errno (&dbfd->mysql) == ER_DUP_ENTRY)
            serrno = EEXIST;
        else {
            srmlogit (STORM_LOG_ERROR, func, "INSERT error: %s\n",
                mysql_error (&dbfd->mysql));
            serrno = seinternal;
        }
        return (-1);
    }
    return (0);
}

extern "C" int storm_insert_xferreq_entry(struct srm_dbfd *dbfd, struct storm_req *storm_req)
{
    static const char *func = "storm_insert_xferreq_entry";
    static char insert_stmt[] =
                "INSERT INTO storm_req (R_ORDINAL, R_TOKEN, R_UID, R_GID, CLIENT_DN, CLIENTHOST, "
                "R_TYPE, PROTOCOL, U_TOKEN, FLAGS, RETRYTIME, NBREQFILES, CTIME, STIME, ETIME, "
                "STATUS, ERRSTRING) "
                "VALUES (%d, '%s', %d, %d, '%s', '%s', '%c', '%s', '%s', %d, %d, %d, %d, %d, %d, %d, '%s')";
    char sql_stmt[1224];
    int ret;

    ret = snprintf (sql_stmt, 1224, insert_stmt,
        storm_req->r_ordinal, storm_req->r_token, storm_req->r_uid,
        storm_req->r_gid, storm_req->client_dn, storm_req->clienthost,
        storm_req->r_type, storm_req->protocol, storm_req->u_token,
        storm_req->flags, storm_req->retrytime, storm_req->nbreqfiles,
        storm_req->ctime, storm_req->stime, storm_req->etime,
        storm_req->status, storm_req->errstring);
    if(ret >= 1224 || ret < 0){
        serrno = ENOMEM;
        return -1;
    }

    srmlogit(STORM_LOG_DEBUG2,func,"Executing query: ``%s''\n",sql_stmt);
    if (mysql_query (&dbfd->mysql, sql_stmt)) {
        if (mysql_errno (&dbfd->mysql) == ER_DUP_ENTRY)
            serrno = EEXIST;
        else {
            srmlogit (STORM_LOG_ERROR, func, "INSERT error: %s\n",
                mysql_error (&dbfd->mysql));
            serrno = seinternal;
        }
        return (-1);
    }
    return (0);
}

extern "C" int storm_list_cpr_entry(struct srm_dbfd *dbfd,
                     int bol,
                     char *r_token,
                     struct storm_copy_filereq *cpr_entry,
                     int lock,
                     storm_dbrec_addr *rec_addr,
                     int endlist,
                     DBLISTPTR *dblistptr)
{
    static const char *func = "storm_list_cpr_entry";
    static char query[] =
                    "SELECT R_TOKEN, F_ORDINAL, FROM_SURL, TO_SURL, LIFETIME, F_TYPE, "
                    "S_TOKEN, FLAGS, ACTUAL_SIZE, STATUS, ERRSTRING "
                    "FROM storm_copy_filereq "
                    "WHERE r_token = '%s' "
                    "ORDER BY f_ordinal";
    static char query4upd[] =
                    "SELECT ROWID, R_TOKEN, F_ORDINAL, FROM_SURL, TO_SURL, LIFETIME, F_TYPE, "
                    "S_TOKEN, FLAGS, ACTUAL_SIZE, STATUS, ERRSTRING "
                    "FROM storm_copy_filereq "
                    "WHERE r_token = '%s' "
                    "ORDER BY f_ordinal "
                    "FOR UPDATE";
    MYSQL_ROW row;
    char sql_stmt[1024];
    int ret;

    if (endlist) {
        if (*dblistptr)
            mysql_free_result (*dblistptr);
        return (1);
    }
    if (bol) {
        ret = snprintf(sql_stmt, 1024, lock ? query4upd : query, r_token);
        if (ret >= 1024 || ret < 0) {
            serrno = ENOMEM;
            return(-1);
        }
        if (storm_exec_query(func, dbfd, sql_stmt, dblistptr)) return(-1);
    }

    if ((row = mysql_fetch_row (*dblistptr)) == NULL) return (1);
    storm_decode_cpr_entry (row, lock, rec_addr, cpr_entry);

    return (0);
}

extern "C" int storm_list_gfr_entry(struct srm_dbfd *dbfd,
                         int bol,
                         char *r_token,
                         struct storm_get_filereq *gfr_entry,
                         int lock,
                         storm_dbrec_addr *rec_addr,
                         int endlist,
                         DBLISTPTR *dblistptr)
{
    static const char *func = "storm_list_gfr_entry";
    static char query[] =
                    "SELECT R_TOKEN, F_ORDINAL, FROM_SURL, LIFETIME, F_TYPE, S_TOKEN, "
                    "FLAGS, SERVER, PFN, ACTUAL_SIZE, STATUS, ERRSTRING "
                    "FROM storm_get_filereq "
                    "WHERE r_token = '%s' "
                    "ORDER BY f_ordinal";
    static char query4upd[] =
                    "SELECT ROWID, R_TOKEN, F_ORDINAL, FROM_SURL, LIFETIME, F_TYPE, S_TOKEN, "
                    "FLAGS, SERVER, PFN, ACTUAL_SIZE, STATUS, ERRSTRING "
                    "FROM storm_get_filereq "
                    "WHERE r_token = '%s' "
                    "ORDER BY f_ordinal "
                    "FOR UPDATE";
    MYSQL_ROW row;
    char sql_stmt[1024];
    int ret;

    if (endlist) {
        if (*dblistptr) mysql_free_result(*dblistptr);
        return(1);
    }

    if (bol) {
        ret = snprintf(sql_stmt, 1024, lock ? query4upd : query, r_token);
        if((ret>=1024) || (ret<0)) {
            serrno = ENOMEM;
            return(-1);
        }

        if (storm_exec_query(func, dbfd, sql_stmt, dblistptr)) return(-1);
    }

    if ((row = mysql_fetch_row (*dblistptr)) == NULL) return(1);
    storm_decode_gfr_entry(row, lock, rec_addr, gfr_entry);

    return (0);
}

extern "C" int storm_list_pending_req(struct srm_dbfd *dbfd,
                       int bol,
                       struct storm_req *storm_req,
                       int lock,
                       storm_dbrec_addr *rec_addr,
                       int endlist,
                       DBLISTPTR *dblistptr)
{
    static const char *func = "storm_list_pending_req";
    static char query[] =
        "SELECT \
         R_ORDINAL, R_TOKEN, R_UID, \
         R_GID, CLIENT_DN, CLIENTHOST, \
         R_TYPE, PROTOCOL, U_TOKEN, \
         FLAGS, RETRYTIME, NBREQFILES, \
         CTIME, STIME, ETIME, \
         STATUS, ERRSTRING \
        FROM storm_pending_req";
    static char query4upd[] =
        "SELECT ROWID, \
         R_ORDINAL, R_TOKEN, R_UID, \
         R_GID, CLIENT_DN, CLIENTHOST, \
         R_TYPE, PROTOCOL, U_TOKEN, \
         FLAGS, RETRYTIME, NBREQFILES, \
         CTIME, STIME, ETIME, \
         STATUS, ERRSTRING \
        FROM storm_pending_req \
        FOR UPDATE";
    MYSQL_ROW row;

    if (endlist) {
        if (*dblistptr)
            mysql_free_result (*dblistptr);
        return (1);
    }
    if (bol) {
        if (storm_exec_query (func, dbfd, lock ? query4upd : query, dblistptr))
            return (-1);
    }
    if ((row = mysql_fetch_row (*dblistptr)) == NULL)
        return (1);
    storm_decode_xferreq_entry (row, lock, rec_addr, storm_req);
    return (0);
}

extern "C" int storm_list_pfr_entry(struct srm_dbfd *dbfd, int bol, char *r_token, struct storm_put_filereq *pfr_entry,
                     int lock, storm_dbrec_addr *rec_addr, int endlist, DBLISTPTR *dblistptr)
{
    static const char *func = "storm_list_pfr_entry";
    static const char *query =
                            "SELECT R_TOKEN, F_ORDINAL, TO_SURL, LIFETIME, F_TYPE, S_TOKEN, REQUESTED_SIZE, "
                            "SERVER, PFN, ACTUAL_SIZE, STATUS, ERRSTRING "
                            "FROM storm_put_filereq "
                            "WHERE r_token = '%s' "
                            "ORDER BY f_ordinal";
    static const char *query4upd =
                            "SELECT ROWID, R_TOKEN, F_ORDINAL, TO_SURL, LIFETIME, F_TYPE, S_TOKEN, "
                            "REQUESTED_SIZE, SERVER, PFN, ACTUAL_SIZE, STATUS, ERRSTRING "
                            "FROM storm_put_filereq "
                            "WHERE r_token = '%s' "
                            "ORDER BY f_ordinal "
                            "FOR UPDATE";
    char sql_stmt[1024];
    MYSQL_ROW row;

    if (endlist) {
        if (*dblistptr) mysql_free_result(*dblistptr);
        return(1);
    }

    if (bol) {
        sprintf(sql_stmt, lock ? query4upd : query, r_token);
        if (storm_exec_query(func, dbfd, sql_stmt, dblistptr)) return(-1);
    }

    if ((row = mysql_fetch_row(*dblistptr)) == NULL) return(1);

    storm_decode_pfr_entry(row, lock, rec_addr, pfr_entry);

    return(0);
}

/* protocol must be an array of string. nbprot is the lenght of the
   array. protlen is the lenght of each string in the array.  If
   called with protocol == NULL or nbprot = 0, then only the number of
   supported protocols are returned.  If protocol != NULL then the
   array will be filled with the supported protocols.

   In case of error a negative integer is returned.
   If nbrot < number of supported protocols, then -(number of supported protocols + 10) are returned.
*/
extern "C" int storm_list_protocol(struct srm_dbfd *dbfd,
                        char **protocol,
                        int nbprot,
                        int protlen,
                        storm_dbrec_addr *rec_addr)
{
    static const char *func = "storm_list_protocol";
    static const char *query = "SELECT id FROM config_Protocols";
    int i, nb_sup_proto;
    DBLISTPTR dblistptr;
    MYSQL_ROW row;

    if (storm_exec_query(func, dbfd, query, &dblistptr)) return(-1);
    nb_sup_proto = 0;
    nb_sup_proto = mysql_num_rows(dblistptr);

    if ((0 == nbprot) || (NULL == protocol)) {
        mysql_free_result(dblistptr);
        return(nb_sup_proto);
    }
    i = 0;
    while ((row=mysql_fetch_row(dblistptr)) != NULL) {
        if(i >= nbprot){
            mysql_free_result(dblistptr);
            return(-(nb_sup_proto+10));
        }
        strncpy(protocol[i], row[0], protlen-1);
        protocol[i][protlen-1] = '\0';
        i++;
    }

    mysql_free_result(dblistptr);
    return(i);
}

