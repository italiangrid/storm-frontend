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

#ifndef STORM_UTIL_HPP
#define STORM_UTIL_HPP

#include <vector>
#include <string>

/* Type definitions */
typedef time_t storm_time_t;
typedef long long storm_size_t;

/* Prototype definitions */
std::vector<std::string> get_supported_protocols(std::string const& server, std::string const& user, std::string const& pw);
char const* reconvertStatusCode(int  code);
int convertStatusCode(char const*  code);

#endif
