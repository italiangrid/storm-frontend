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

#include "Credentials.hpp"
#include "srmlogit.h"
//#include <cgsi_plugin.h>
#include "FrontendConfiguration.hpp"
#include "string.h"
#include "stdlib.h"
#include <string>
#include <vector>

using namespace storm;

std::string get_client_dn(soap *soap)
{
    return std::string();
}

std::vector<sql_string> get_client_roles(soap* soap)
{
    return std::vector<sql_string>();
}

Credentials::Credentials(struct soap *soap)
{
    static const char* const funcName = "Credentials()";
    char clientdn[256];
    char **fqans = NULL;
    int nbfqans = 0;

    if(soap == NULL)
    {
    	srmlogit(STORM_LOG_ERROR, funcName, "Received null soap pointer\n");
    	return;
    }
    _soap = soap;

    _clientDN = get_client_dn(_soap);

    // retrieving FQANs
    _fqans_vector = get_client_roles(_soap);

    for (int i=0; i<_fqans_vector.size(); i++) {
        if (_fqans_vector[i].empty()) {
            srmlogit(STORM_LOG_ERROR, funcName, "Strange error: NULL FQAN\n");
        } else {
        	srmlogit(STORM_LOG_DEBUG, funcName, "Adding FQAN %s\n", _fqans_vector[i]);
        }
    }
}

sql_string Credentials::getFQANsOneString()
{
    std::string returnString;

    for (int i = 0, size = _fqans_vector.size(); i < size; i++) {
        returnString += _fqans_vector[i];
        returnString += "#";
    }

    // remove the last '#'
    if (returnString.length() > 0) {
        returnString.erase(returnString.length() - 1);
    }

    return sql_string(returnString);
};
