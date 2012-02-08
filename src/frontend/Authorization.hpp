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

#ifndef AUTHORIZATION_HPP_
#define AUTHORIZATION_HPP_

// C includes
#include <argus/pep.h>

// STL includes
#include <string>
#include <iostream>
#include <sstream>

#include "Credentials.hpp"
#include "AuthorizationException.hpp"
#include "FrontendConfiguration.hpp"
#include "srmlogit.h"

namespace storm {
    
class Authorization {
public:
	Authorization(Credentials *cred) throw (storm::AuthorizationException) {
		char* funcName = "Authorization";
		credentials = cred;

		blacklistRequested = FrontendConfiguration::getInstance()->getUserCheckBlacklist();
		if(blacklistRequested)
		{
			// dump library version
			srmlogit(STORM_LOG_DEBUG, funcName, "Using %s\n", pep_version());
			// create the PEP client handle
			pep = pep_initialize();
			if (pep == NULL) {
				srmlogit(STORM_LOG_ERROR, funcName, "Failed to create PEP client\n");
				std::string  errMessage("Failed to create PEP client\n");
				storm::AuthorizationException exc(errMessage);
				throw exc;
			}

			// debugging options
			pep_setoption(pep, PEP_OPTION_LOG_STDERR, stderr);
			pep_setoption(pep, PEP_OPTION_LOG_LEVEL, PEP_LOGLEVEL_INFO);

			std::string pep_url = "https://";
			std::string argusPepHostname = FrontendConfiguration::getInstance()->getArgusPepHostname();
			if(argusPepHostname.empty())
			{
				srmlogit(STORM_LOG_ERROR, funcName, "No Hostname for Argus PEP server available in configuration! Unable to build PEP client\n");
				std::string  errMessage("No Hostname for Argus PEP server available in configuration\n");
				storm::AuthorizationException exc(errMessage);
				throw exc;
			}
			std::string argusPepAuthzPort = FrontendConfiguration::getInstance()->getArgusPepAuthzPort();
			if(argusPepAuthzPort.empty())
			{
				srmlogit(STORM_LOG_ERROR, funcName, "No Port for Argus PEP service available in configuration! Unable to build PEP client\n");
				std::string  errMessage("No Port for Argus PEP service available in configuration\n");
				storm::AuthorizationException exc(errMessage);
				throw exc;
			}
			pep_url += argusPepHostname + ":" + argusPepAuthzPort + "/authz";
			// configure PEP client: PEP Server endpoint url
			pep_error_t pep_rc = pep_setoption(pep, PEP_OPTION_ENDPOINT_URL, pep_url.c_str());
			if (pep_rc != PEP_OK) {
				srmlogit(STORM_LOG_ERROR, funcName, "Failed to set PEP endpoint: %s: %s\n", pep_url.c_str(), pep_strerror(pep_rc));
				std::string  errMessage("Failed to set PEP endpoint\n");
				storm::AuthorizationException exc(errMessage);
				throw exc;
			}
			// configure PEP client: private key and certificate required to access the PEP Server
			// endpoint (HTTPS with client authentication)
			std::string hostKey = FrontendConfiguration::getInstance()->getHostKeyFile();
			pep_rc = pep_setoption(pep, PEP_OPTION_ENDPOINT_CLIENT_KEY, hostKey.c_str());
			if (pep_rc != PEP_OK) {
				srmlogit(STORM_LOG_ERROR, funcName, "Failed to set client key: %s: %s\n", hostKey.c_str(), pep_strerror(pep_rc));
				std::string  errMessage("Failed to set client key\n");
				storm::AuthorizationException exc(errMessage);
				throw exc;
			}
			std::string hostCert = FrontendConfiguration::getInstance()->getHostCertFile();
			pep_rc = pep_setoption(pep, PEP_OPTION_ENDPOINT_CLIENT_CERT, hostCert.c_str());
			if (pep_rc != PEP_OK) {
				srmlogit(STORM_LOG_ERROR, funcName, "Failed to set client cert: %s: %s\n", hostCert.c_str(), pep_strerror(pep_rc));
				std::string  errMessage("Failed to set client cert\n");
				storm::AuthorizationException exc(errMessage);
				throw exc;
			}
			std::string caCertFolder = FrontendConfiguration::getInstance()->getCaCertificatesFolder();
			// server certificate CA path for validation
			pep_rc = pep_setoption(pep, PEP_OPTION_ENDPOINT_SERVER_CAPATH, caCertFolder.c_str());
			if (pep_rc != PEP_OK) {
				srmlogit(STORM_LOG_ERROR, funcName, "Failed to set server CA path: %s: %s\n", caCertFolder.c_str(), pep_strerror(pep_rc));
				std::string  errMessage("Failed to set server CA path\n");
				storm::AuthorizationException exc(errMessage);
				throw exc;
			}
		}
	};

	~Authorization()
	{
		pep_destroy(pep);
	}

	static bool checkBlacklist(struct soap *soap)
	 {
		bool response = false;
		if(FrontendConfiguration::getInstance()->getUserCheckBlacklist())
		{
			storm::Credentials cred(soap);
			storm::Authorization auth((storm::Credentials*)&cred);
			response = auth.isBlacklisted();
		}
		return response;
	 }

    bool isAuthorized(std::string resource, std::string action) throw (storm::AuthorizationException) ;
    bool isBlacklisted() throw (storm::AuthorizationException) ;

private:
    Credentials * credentials;
    PEP * pep;
    bool blacklistRequested;
    int create_xacml_request(xacml_request_t ** request,const char * subjectid, const char * resourceid, const char * actionid);
    xacml_decision_t process_xacml_response(const xacml_response_t * response) throw (storm::AuthorizationException);
    char * fulfillon_tostring(xacml_fulfillon_t fulfillon) ;
    const char * decision_tostring(xacml_decision_t decision) ;
    void printXamlObligation(xacml_obligation_t * obligation);
    void printXamlResult(xacml_result_t * result);
    static const std::string DEFAULT_RESOURCE;
    static const std::string DEFAULT_ACTION;
};

}

#endif /*AUTHORIZATION_HPP_*/
