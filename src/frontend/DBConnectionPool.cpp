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

#include "DBConnectionPool.hpp"
#include "storm_mysql.hpp"
#include "srmlogit.h"
#include <mysql/mysql.h>

DBConnectionPool::DBConnectionPool(int pool_size)
{
}

DBConnectionPool::~DBConnectionPool()
{
    for (Contexts::iterator it = m_thread_contexts.begin(), e = m_thread_contexts.end(); it != e; ++it) {
        // hope it should not be done in the thread that did the mysql_init
        mysql_close(it->second.dbfd.mysql);
    }
}

srm_srv_thread_info*
DBConnectionPool::getConnection()
{
    boost::lock_guard<boost::mutex> l(mtx);

    boost::thread::id const tid = boost::this_thread::get_id();

    Contexts::iterator it = m_thread_contexts.lower_bound(tid);
    if (it->first == tid) {
        return &it->second;
    } else {
        // can release the lock here while connecting to the db
        // the insertion doesn't need to provide a hint since other threads
        //   will have worked on the map in the meantime
        // of course the insertion needs to be done under the lock
        srm_srv_thread_info context{};
        context.is_used = true;
        context.db_open_done = true;
        context.request_id = NULL;
        context.dbfd.tr_started = 0;
        context.dbfd.mysql = mysql_init(NULL);
        storm_opendb(db_srvr, db_user, db_pwd, &context.dbfd);
        Contexts::iterator new_it = m_thread_contexts.insert(it, std::make_pair(tid, context));
        return &new_it->second;
    }
}
