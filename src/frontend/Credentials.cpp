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
#include <cgsi_plugin.h>
#include "FrontendConfiguration.hpp"
#include "string.h"
#include "stdlib.h"

using namespace storm;

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

    get_client_dn(_soap, clientdn, sizeof(clientdn));
    _clientDN = std::string(clientdn);

    // retrieving FQANs
    _fqans_vector = std::vector<sql_string>();
    // fqans will point to a memory area in the soap structure: it must not be freed
    fqans = get_client_roles(_soap, &nbfqans);

    if (fqans == NULL) {
        return;
    }

    /* Paranoic error check for the result returned by the get_client_roles() function */
    if ((nbfqans > 0) && (fqans == NULL)) {
        srmlogit(STORM_LOG_ERROR, funcName, "ERROR: FQAN not found (but they should exist): fqans=NULL\n");
        return;
    }
    for (int i=0; i<nbfqans; i++) {
        if (fqans[i] == NULL) {
            srmlogit(STORM_LOG_ERROR, funcName, "Strange error: NULL FQAN\n");
        } else {
        	srmlogit(STORM_LOG_DEBUG, funcName, "Adding FQAN %s\n", fqans[i]);
            _fqans_vector.push_back(fqans[i]);
        }
    }
}

sql_string Credentials::getFQANsOneString()
{
    static const char* const funcName = "Credentials::getFQANsOneString()";
    std::string returnString;

    for (int i=0; i<_fqans_vector.size(); i++) {
        returnString += _fqans_vector[i];
        returnString += "#";
    }

    // remove the last '#'
    if (returnString.length() > 0) {
        returnString.erase(returnString.length() - 1);
    }

    return sql_string(returnString);
};
