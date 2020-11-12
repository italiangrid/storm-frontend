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

#ifndef __MYSQL_QUERY_HPP
#define __MYSQL_QUERY_HPP

#include <map>
#include <string>
#include <vector>

#include <stdexcept>

#include <mysql/mysql.h>
#include "storm_functions.h"

namespace storm_db {

    class mysql_exception : public std::runtime_error {
    public:
        mysql_exception(MYSQL* mysql)
            : std::runtime_error(mysql_error(mysql)),
              _errno(mysql_errno(mysql))
        {
        }
        int get_mysql_errno() const { return _errno;}
        std::string get_mysql_error() const { return what(); }
        
    private:
        int _errno;
    };

    std::vector< std::map<std::string, std::string> > vector_exec_query(srm_dbfd* dbfd, std::string const& query);

    int ID_exec_query(srm_dbfd *dbfd, std::string const& query);
}
#endif // __MYSQL_QUERY_HPP
