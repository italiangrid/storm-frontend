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

/*
 * Copyright (C) 2004-2005 by INFN
 * All rights reserved
 */

#include "storm_mysql.hpp"
#include "srm_server.h"
#include "srmlogit.h"
#include "srmv2H.h"
#include <mysql/mysqld_error.h>
#include <mysql/mysql.h>
#include <string>

using namespace std;

/************************
 * CONNECTION FUNCTIONS *
 ************************/
int storm_opendb(std::string const& server, std::string const& user, std::string const& pw, srm_dbfd *dbfd)
{
    static const char *func = "storm_opendb";

    if (dbfd->mysql == NULL) {
        dbfd->mysql = mysql_init(NULL);
    }

    my_bool const reconnect = 1;
    mysql_options(dbfd->mysql, MYSQL_OPT_RECONNECT, &reconnect);
    unsigned int const connect_timeout = 10; // seconds
    mysql_options(dbfd->mysql, MYSQL_OPT_CONNECT_TIMEOUT, &connect_timeout);

    if (mysql_real_connect(dbfd->mysql, server.c_str(), user.c_str(), pw.c_str(), "storm_db", 0, NULL, 0) == dbfd->mysql) {
        srmlogit(STORM_LOG_DEBUG, func, "Connected to the DB.\n");
        return 0;
    } else {
        srmlogit (STORM_LOG_ERROR, func, "Cannot connect to the DB.\n");
        return -1;
    }
}

void storm_closedb(srm_dbfd *dbfd)
{
    mysql_close(dbfd->mysql);
    return ;
}

int storm_ping_connection(MYSQL *mysql)
{
	return mysql_ping(mysql);
}


/*************************
 * TRANSACTION FUNCTIONS *
 *************************/
int storm_start_tr(int, srm_dbfd *dbfd)
{
    (void) mysql_query(dbfd->mysql, "BEGIN");
    dbfd->tr_started = 1;
    return(0);
}

int storm_end_tr(srm_dbfd *dbfd)
{
    (void) mysql_query(dbfd->mysql, "COMMIT");
    dbfd->tr_started = 0;
    return(0);
}

void storm_abort_tr(srm_dbfd *dbfd)
{
    (void) mysql_query (dbfd->mysql, "ROLLBACK");
    dbfd->tr_started = 0;
    return ;
}

void set_savepoint(srm_dbfd *dbfd, const char * name)
{
    std::string query("SAVEPOINT ");
    query+=name;
    mysql_query(dbfd->mysql, query.c_str());
    return ;
}

void rollback_to_savepoint(srm_dbfd *dbfd, const char * name)
{
    std::string query("ROLLBACK TO SAVEPOINT ");
    query+=name;
    mysql_query(dbfd->mysql, query.c_str());
    return ;
}


int storm_exec_query(const char * const func, srm_dbfd *dbfd, const char *sql_stmt, MYSQL_RES **res)
{
    srmlogit(STORM_LOG_DEBUG2, func, "Executing query: ``%s''\n", sql_stmt);

    if (mysql_query(dbfd->mysql, sql_stmt)) {
        srmlogit (STORM_LOG_ERROR, func, "mysql_query error: %s\n", mysql_error (dbfd->mysql));
        return(-1);
    }
    if ((*res = mysql_store_result (dbfd->mysql)) == NULL) {
        srmlogit (STORM_LOG_ERROR, func, "mysql_store_res error: %s\n", mysql_error (dbfd->mysql));
        return(-1);
    }
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
int storm_list_protocol(srm_dbfd *dbfd,
                        char **protocol,
                        int nbprot,
                        int protlen)
{
    static const char *func = "storm_list_protocol";
    static const char *query = "SELECT id FROM config_Protocols";
    int i, nb_sup_proto;
    MYSQL_RES* dblistptr;
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

