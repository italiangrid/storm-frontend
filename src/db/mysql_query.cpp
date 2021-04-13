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

#include "mysql_query.hpp"
#include "srmlogit.h"
#include "srm_server.h"

static MYSQL_RES* _query_init(srm_dbfd *dbfd, std::string const& query)
{
    static const char * const func = "_query_init";

    srmlogit(STORM_LOG_DEBUG, func, "Executing query ``%s''\n", query.c_str());

    if (mysql_query(dbfd->mysql, query.c_str())) {
        srmlogit(STORM_LOG_ERROR, func, "mysql_query error: %s. Query: ``%s''\n",
            mysql_error(dbfd->mysql), query.c_str());
        throw storm_db::mysql_exception(dbfd->mysql);
    }

    MYSQL_RES* res = NULL;
    if (0 != mysql_field_count(dbfd->mysql)) { // The stmt has no result.
        res = mysql_store_result(dbfd->mysql);
        if (NULL == res) { // Error getting the result of the query.
            srmlogit(STORM_LOG_ERROR, func, "mysql_store_res error: %s\n",
                    mysql_error(dbfd->mysql));
            throw storm_db::mysql_exception(dbfd->mysql);
        }
    }

    return res;
}

namespace storm_db {

std::vector<std::map<std::string, std::string> > vector_exec_query(srm_dbfd *dbfd, std::string const& query)
{
    std::vector<std::map<std::string, std::string> > result;

    MYSQL_RES* const res = _query_init(dbfd, query);

    if (res) {
        MYSQL_FIELD* const fields = mysql_fetch_fields(res);
        int const num_fields = mysql_num_fields(res);
        for (MYSQL_ROW row = mysql_fetch_row(res); row != NULL; row = mysql_fetch_row(res)) {
          	std::map<std::string, std::string> x;
            for (int i = 0; i < num_fields; i++) {
                x[fields[i].name] = (NULL != row[i] ? row[i] : "");
            }
            result.push_back(x);
        }
        mysql_free_result(res);
    }
    return result;
}

/** \fn ID_exec_query
 * Exec the statement and return the last inserted ID, if any.
 *
 * @param dbfd     a pointer to a srm_dbfd struct
 * @param query    the string qery
 *
 * @return         {@c n} the value generated for an
 *                 AUTO_INCREMENT column by the previous INSERT
 *                 or UPDATE statemen. In case of a multirow insert
 *                 query, only the {@b FIRST} id will be returned.
 *
 *                 {@c 0} if the previous statement does not use
 *                 an AUTO_INCREMENT value.
 */
int ID_exec_query(srm_dbfd *dbfd, std::string const& query) {

	MYSQL_RES* p = _query_init(dbfd, query);

    mysql_free_result(p);

    return mysql_insert_id(dbfd->mysql);
}

} // namespace storm_db

