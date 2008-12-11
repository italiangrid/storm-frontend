/*
 * DBConnectionPool.cpp
 *
 *  Created on: Dec 11, 2008
 *      Author: alb
 */

#include "DBConnectionPool.hpp"
#include "srmlogit.h"

DBConnectionPool::pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;

DBConnectionPool::DBConnectionPool(int pool_size) {
    mysql_connection_pool = new (struct srm_srv_thread_info(*[nThreads]));

    for (int i = 0; i < nThreads; i++) {
        mysql_connection_pool[i] = new struct srm_srv_thread_info();
        mysql_connection_pool[i]->is_used = false;
        mysql_connection_pool[i]->db_open_done = 0;
    }

    _pool_size = pool_size;

}

srm_srv_thread_info*
DBConnectionPool::getConnection(boost::thread::id tid) {

    srm_srv_thread_info* free_connection = NULL;

    pthread_mutex_lock(&log_mutex);

    bool found = false;
    for (int i=0; i<_pool_size; i++) {
        if (mysql_connection_pool[i]->thread_owner_id == tid) {
            found = true;
            free_connection = mysql_connection_pool[i];
            break;
        }
    }
    if (!found) {
        for (int i=0; i<_pool_size; i++) {
            if (! mysql_connection_pool[i]->is_used) {
                free_connection = mysql_connection_pool[i];
                free_connection->thread_owner_id = tid;
                free_connection->is_used = true;
            }
        }
    }
    pthread_mutex_unlock(&log_mutex);

    return free_connection;

}
