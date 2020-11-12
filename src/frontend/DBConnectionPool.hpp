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

#ifndef DBCONNECTIONPOOL_HPP
#define DBCONNECTIONPOOL_HPP

#include "srm_server.h"
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>

class DBConnectionPool {
public:
    DBConnectionPool(int pool_size);
    ~DBConnectionPool();
    srm_srv_thread_info* getConnection();

private:
    typedef std::map<boost::thread::id, srm_srv_thread_info> Contexts;
    
    Contexts m_thread_contexts;
    srm_srv_thread_info** mysql_connection_pool;
    boost::thread::id* id_map;
    int _pool_size;
    int _curr_size;
    boost::mutex mtx;
};

#endif /* DBCONNECTIONPOOL_HPP_ */
