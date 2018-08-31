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


/**
 * Saves the proxy to the default directory only if the user has delegated
 * credentials. Returns "true" if the proxy is successfully saved or "false"
 * otherwise.
 **/
bool Credentials::saveProxy(std::string requestToken)
{
    static const char* const funcName = "Credentials::saveProxy()";
    bool result = false;


    FrontendConfiguration* configuration = FrontendConfiguration::getInstance();

    /* Check for delegated credentials */
    if (has_delegated_credentials(_soap)) {
        /* Export client credential to proxy_filename */
        std::string proxy_filename = configuration->getProxyDir();

        proxy_filename += "/" + requestToken;
        char *tmp = strdup(proxy_filename.c_str()); // why export_delegated_cretentials() needs char* instead of const char *????
        int stat = export_delegated_credentials(_soap, tmp);
        free(tmp);
        /* check export status */
        if (stat == 0) {
            srmlogit(STORM_LOG_DEBUG, funcName, "Proxy successfully written: %s\n", _clientDN.c_str());
            result = true;
        } else {
            srmlogit(STORM_LOG_ERROR, funcName, "Unable to write the proxy. Token: %s , proxy_filename %s , return code %d\n" , requestToken.c_str(), proxy_filename.c_str(), stat);
        }
        if (getuid() != proxy_uid || getgid() != proxy_gid) {
		   chown(proxy_filename.c_str(), proxy_uid, proxy_gid);
	   }
    } else { /* No delegation. */
        srmlogit(STORM_LOG_DEBUG, funcName, "%s: has NOT delegated credentials to server\n", _clientDN.c_str());
    }
    return result;
}
