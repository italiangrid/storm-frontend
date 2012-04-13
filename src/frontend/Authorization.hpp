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

#include "FullCredentials.hpp"
#include "AuthorizationException.hpp"
#include "ArgusException.hpp"
#include "FrontendConfiguration.hpp"
#include "srmlogit.h"

namespace storm {
const std::string DEFAULT_AUTHORIZATION_RESOURCE("StoRM");
const std::string DEFAULT_AUTHORIZATION_ACTION("access");
const std::string DEFAULT_AUTHORIZATION_PROFILE("http://glite.org/xacml/profile/grid-wn/1.0");
class Authorization {
public:
	Authorization(FullCredentials *cred) throw (AuthorizationException) {
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

			std::string argusPepdEndpoint = FrontendConfiguration::getInstance()->getArgusPepdEndpoint();
			if(argusPepdEndpoint.empty())
			{
				srmlogit(STORM_LOG_ERROR, funcName, "No endpoint for Argus PEPD server available in configuration! Unable to build PEP client\n");
				std::string  errMessage("No endpoint for Argus PEP server available in configuration\n");
				storm::AuthorizationException exc(errMessage);
				throw exc;
			}

			// configure PEP client: PEP Server endpoint url
			pep_error_t pep_rc = pep_setoption(pep, PEP_OPTION_ENDPOINT_URL, argusPepdEndpoint.c_str());
			if (pep_rc != PEP_OK) {
				srmlogit(STORM_LOG_ERROR, funcName, "Failed to set PEP endpoint: %s: %s\n", argusPepdEndpoint.c_str(), pep_strerror(pep_rc));
				std::string  errMessage("Failed to set PEP endpoint\n");
				storm::AuthorizationException exc(errMessage);
				throw exc;
			}
			// configure PEP client: private key and certificate required to access the PEP Server
			// endpoint (HTTPS with client authentication)
			std::string hostKey = FrontendConfiguration::getInstance()->getHostKeyFile();
			if(hostKey.empty())
			{
				srmlogit(STORM_LOG_ERROR, funcName, "No hostkey file path available in configuration! Unable to build PEP client\n");
				std::string  errMessage("No hostkey file path available in configuration\n");
				storm::AuthorizationException exc(errMessage);
				throw exc;
			}
			pep_rc = pep_setoption(pep, PEP_OPTION_ENDPOINT_CLIENT_KEY, hostKey.c_str());
			if (pep_rc != PEP_OK) {
				srmlogit(STORM_LOG_ERROR, funcName, "Failed to set client key: %s: %s\n", hostKey.c_str(), pep_strerror(pep_rc));
				std::string  errMessage("Failed to set client key\n");
				storm::AuthorizationException exc(errMessage);
				throw exc;
			}
			std::string hostCert = FrontendConfiguration::getInstance()->getHostCertFile();
			if(hostCert.empty())
			{
				srmlogit(STORM_LOG_ERROR, funcName, "No hostcert file path available in configuration! Unable to build PEP client\n");
				std::string  errMessage("No hostcert file path available in configuration\n");
				storm::AuthorizationException exc(errMessage);
				throw exc;
			}
			pep_rc = pep_setoption(pep, PEP_OPTION_ENDPOINT_CLIENT_CERT, hostCert.c_str());
			if (pep_rc != PEP_OK) {
				srmlogit(STORM_LOG_ERROR, funcName, "Failed to set client cert: %s: %s\n", hostCert.c_str(), pep_strerror(pep_rc));
				std::string  errMessage("Failed to set client cert\n");
				storm::AuthorizationException exc(errMessage);
				throw exc;
			}
			std::string caCertFolder = FrontendConfiguration::getInstance()->getCaCertificatesFolder();
			if(caCertFolder.empty())
			{
				srmlogit(STORM_LOG_ERROR, funcName, "No CA folder path available in configuration! Unable to build PEP client\n");
				std::string  errMessage("No CA folder path available in configuration\n");
				storm::AuthorizationException exc(errMessage);
				throw exc;
			}
			// server certificate CA path for validation
			pep_rc = pep_setoption(pep, PEP_OPTION_ENDPOINT_SERVER_CAPATH, caCertFolder.c_str());
			if (pep_rc != PEP_OK) {
				srmlogit(STORM_LOG_ERROR, funcName, "Failed to set server CA path: %s: %s\n", caCertFolder.c_str(), pep_strerror(pep_rc));
				std::string  errMessage("Failed to set server CA path\n");
				storm::AuthorizationException exc(errMessage);
				throw exc;
			}
			// debugging options
			pep_rc = pep_setoption(pep, PEP_OPTION_LOG_STDERR, stderr);
			if (pep_rc != PEP_OK) {
				srmlogit(STORM_LOG_ERROR, funcName,
						"Failed to set log file: %s\n", pep_strerror(pep_rc));
				std::string errMessage("Failed to set log file\n");
				storm::AuthorizationException exc(errMessage);
				throw exc;
			}
			pep_rc = pep_setoption(pep, PEP_OPTION_LOG_LEVEL, PEP_LOGLEVEL_WARN);
			if (pep_rc != PEP_OK) {
				srmlogit(STORM_LOG_ERROR, funcName,
						"Failed to set log level: %s\n", pep_strerror(pep_rc));
				std::string errMessage("Failed to set log level\n");
				storm::AuthorizationException exc(errMessage);
				throw exc;
			}
		}
	};

	~Authorization()
	{
		pep_destroy(pep);
	}

	static bool checkBlacklist(struct soap *soap) throw (AuthorizationException, ArgusException)
	{
		bool response = false;
		if (FrontendConfiguration::getInstance()->getUserCheckBlacklist())
		{
			FullCredentials cred(soap);
			Authorization auth((storm::FullCredentials*) &cred);
			response = auth.isBlacklisted();
		}
		return response;
	}

    bool isAuthorized(std::string resource, std::string action) throw (storm::AuthorizationException) ;
    bool isBlacklisted() throw (storm::AuthorizationException, storm::ArgusException) ;

private:
    FullCredentials * credentials;
    PEP * pep;
    bool blacklistRequested;

    xacml_request_t* create_xacml_request(const char * subjectid, const char * resourceid, const char * actionid) throw (storm::AuthorizationException);
    xacml_subject_t * create_xacml_subject(const char * subject_id) throw (storm::AuthorizationException);
    xacml_resource_t* create_xacml_resource(const char * resourceid) throw (storm::AuthorizationException);
    xacml_action_t * create_xacml_action(const char * actionid) throw (storm::AuthorizationException);
    xacml_environment_t* create_xacml_environment_profile(const char * profileid) throw (storm::AuthorizationException);
    xacml_request_t * assemble_xacml_request(xacml_subject_t * subject, xacml_resource_t * resource, xacml_action_t * action, xacml_environment_t * environment) throw (storm::AuthorizationException);
    xacml_decision_t process_xacml_response(const xacml_response_t * response) throw (storm::AuthorizationException);
    bool evaluateResponse(xacml_decision_t* decision);
    char * fulfillon_tostring(xacml_fulfillon_t fulfillon) ;
    const char * decision_tostring(xacml_decision_t decision) ;
    void printXamlObligation(xacml_obligation_t * obligation);
    void printXamlResult(xacml_result_t * result);
};

}

#endif /*AUTHORIZATION_HPP_*/
