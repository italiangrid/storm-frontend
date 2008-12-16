/*
 * DBConnectionPool.cpp
 *
 *  Created on: Dec 11, 2008
 *      Author: alb
 */

#include "DBConnectionPool.hpp"
#include "srmlogit.h"
#include <mysql/mysql.h>

DBConnectionPool::DBConnectionPool(int pool_size) {

    pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

    mysql_connection_pool = new (struct srm_srv_thread_info(*[pool_size]));

    id_map = new (boost::thread::id[pool_size]);

    _curr_size = 0;
    _pool_size = pool_size;

    for (int i = 0; i < _pool_size; i++) {
        mysql_connection_pool[i] = new struct srm_srv_thread_info();
        mysql_connection_pool[i]->is_used = 0;          // i.e. = false
        mysql_connection_pool[i]->db_open_done = 0;     // i.e. = false
    }


}

DBConnectionPool::~DBConnectionPool() {

    for (int i=0; i<_pool_size; i++) {

        if ((mysql_connection_pool[i]->is_used == 1) && (mysql_connection_pool[i]->db_open_done == 1)) {
            mysql_close(&((mysql_connection_pool[i]->dbfd).mysql));
        }

        delete mysql_connection_pool[i];
    }

    delete [] mysql_connection_pool;

    delete [] id_map;

}

srm_srv_thread_info*
DBConnectionPool::getConnection(boost::thread::id tid) {

    srm_srv_thread_info* free_connection = NULL;


    bool found = false;
    int i;

    for (i=0; i<_curr_size; i++) {
        if (id_map[i] == tid) {
            found = true;
            break;
        }
    }

    if (found) {
        free_connection = mysql_connection_pool[i];
    } else {
        srmlogit(STORM_LOG_DEBUG, "DBConnectionPool", "Creating new connection (number %d) to the DB\n", _curr_size);

        if (_curr_size == _pool_size)
            srmlogit(STORM_LOG_ERROR, "DBConnectionPool", "BUG: connection pool overflow!\n");

        pthread_mutex_lock(&mtx);
        i = _curr_size;
        id_map[i] = tid;
        _curr_size++;
        pthread_mutex_unlock(&mtx);

        free_connection = mysql_connection_pool[i];
        free_connection->is_used = true;
    }

    srmlogit(STORM_LOG_DEBUG, "DBConnectionPool", "Assigned connection: %d\n", i);


    return free_connection;

}
