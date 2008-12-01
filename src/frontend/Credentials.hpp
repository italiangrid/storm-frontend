#ifndef CREDENTIALS_HPP_
#define CREDENTIALS_HPP_

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

// C includes
#include "storm_functions.h"
#include "serrno.h"
#include "srm_server.h"
#include "srmv2H.h"
#include "storm_util.h"
#include "srmlogit.h"

// STL includes
#include <string>
#include <vector>
#include <map>

// storm_db include
#include "mysql_query.hpp"

// parent
#include "sql_string.hpp"

using namespace std;

namespace storm {
    
class Credentials {
public:
    Credentials(struct soap *soap);
    void setDN(string dn) { _clientDN = dn; };
    string getDN() { return _clientDN; }
    std::vector<sql_string> getFQANsVector() { return _fqans_vector; }
    sql_string getFQANsOneString();
    bool Credentials::saveProxy(string requestToken);
    //void getVOMS();
    
    //void insert(struct srm_dbfd *dbfd);

private:
    struct soap *_soap;
    string _clientDN;
    std::vector<sql_string> _fqans_vector;
    
};

}

#endif /*CREDENTIALS_HPP_*/
