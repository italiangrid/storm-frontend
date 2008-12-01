#ifndef SQL_STRING_HPP
#define SQL_STRING_HPP
// Copyright (C) 2006 by Antonio Messina <antonio.messina@ictp.it> for the ICTP project EGRID.
/*
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by the
 *  Free Software Foundation; either version 2 of the License, or (at your
 *  option) any later version.
 *
 *  This program is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include <string>

class sql_string : public std::string {
public:
//     sql_string(std::string *s) : std::string() {
//         _escape_sql(*s);
//     }
//     sql_string(std::string s) : std::string() {
//         _escape_sql(s);
//     }
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
