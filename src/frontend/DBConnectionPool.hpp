/*
 * DBConnectionPool.hpp
 *
 *  Created on: Dec 11, 2008
 *      Author: alb
 */

#ifndef DBCONNECTIONPOOL_HPP_
#define DBCONNECTIONPOOL_HPP_

#include "srm_server.h"
#include <pthread.h>
#include <boost/thread.hpp>

class DBConnectionPool {
public:
    DBConnectionPool(int pool_size);
    srm_srv_thread_info* getConnection(boost::thread::id thread_id);

private:
    struct srm_srv_thread_info** mysql_connection_pool;
    int _pool_size;
    static pthread_mutex_t log_mutex;


};

#endif /* DBCONNECTIONPOOL_HPP_ */
