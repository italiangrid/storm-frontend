/*
 * DBConnectionPool.hpp
 *
 *  Created on: Dec 11, 2008
 *      Author: alb
 */

#ifndef DBCONNECTIONPOOL_HPP_
#define DBCONNECTIONPOOL_HPP_

#include "srm_server.h"
#include <boost/thread.hpp>

class DBConnectionPool {
public:
    DBConnectionPool(int pool_size);
    ~DBConnectionPool();
    srm_srv_thread_info* getConnection(boost::thread::id thread_id);

private:
    struct srm_srv_thread_info** mysql_connection_pool;
    boost::thread::id* id_map;
    int _pool_size;
    int _curr_size;
    pthread_mutex_t mtx;
};

#endif /* DBCONNECTIONPOOL_HPP_ */
