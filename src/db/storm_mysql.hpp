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

#ifndef STORM_MYSQL_HPP
#define STORM_MYSQL_HPP

#include <mysql/mysql.h>
#include <string>

class srm_dbfd;

int storm_opendb(std::string const& server, std::string const& user, std::string const& pw, srm_dbfd *);
void storm_closedb(srm_dbfd *dbfd);
int storm_list_protocol(srm_dbfd*, char **protocol, int nbprot, int protlen);
int storm_ping_connection(MYSQL *mysql);
int storm_start_tr(int, srm_dbfd *);
int storm_end_tr(srm_dbfd *);
void storm_abort_tr(srm_dbfd *);
void set_savepoint(srm_dbfd *, const char *);
void rollback_to_savepoint(srm_dbfd *, const char *);

#endif
