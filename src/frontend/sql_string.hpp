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

#ifndef SQL_STRING_HPP
#define SQL_STRING_HPP

#include <string>

class sql_string : public std::string {
public:
    sql_string() : std::string() {};
    
    sql_string(const std::string &s) : std::string(s) {
        _escape_sql(*this);
    }
    
    sql_string(const char *s) : std::string(s) {
        _escape_sql(*this);
    }
private:
    void _escape_sql(std::string &s) {
        static const char * to_escape="\\'%\"";
        std::string::size_type i;
        for (i = s.find_first_of(to_escape,0);
             i != npos;
             i = s.find_first_of(to_escape, i+2))
        {
            s.insert(i, "\\");
        }
        assign(s);
    }
};

#endif
