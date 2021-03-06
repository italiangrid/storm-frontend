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

#ifndef SRMV2_FILEREQUEST_TEMPLATE_HPP
#define SRMV2_FILEREQUEST_TEMPLATE_HPP

#include <uuid/uuid.h>

#include "srmlogit.h"
#include "srm_server.h"

#include <exception>
#include <stdexcept>
#include "file_request.hpp"
#include "Credentials.hpp"

#include "mysql_query.hpp"
#include "storm_mysql.hpp"
#include "sql_string.hpp"

#include "get_socket_info.hpp"

#include <vector>
#include <sstream>
#include "storm_util.hpp"
#include "ProtocolChecker.hpp"
#include "storm_exception.hpp"

using namespace std;

template<typename soap_in_t, typename soap_out_t>
int __process_file_request(struct soap *soap, storm::file_request<soap_in_t, soap_out_t>& request,
        const char* funcName, soap_in_t * /* req */, soap_out_t **resp)
{
    static const char* func = "__process_file_request<>";

    struct srm_srv_thread_info* thip = static_cast<srm_srv_thread_info *> (soap->user);

    if (thip == NULL) {
    	try
		{
			*resp = request.buildSpecificResponse(SRM_USCOREFATAL_USCOREINTERNAL_USCOREERROR,
					"Cannot get DB connect from the pool");
		} catch(storm::storm_error& exc)
		{
			srmlogit(STORM_LOG_ERROR, funcName, "Unable to build soap response. InvalidResponse: %s\n" , exc.what());
			return(SOAP_FATAL_ERROR);
		}
        return SOAP_OK;
    }

    // Generate the request token (unique identifier)
    uuid_t uuid;
	const int ST_MAXDPMTOKENLEN = 36;
    char r_token[ST_MAXDPMTOKENLEN + 1];
    uuid_generate(uuid);
    uuid_unparse(uuid, r_token);

    // Set the Request Token into the request
    request.setRequestToken(r_token);

    srmlogit(STORM_LOG_DEBUG, func, "Request token: %s\n", request.getRequestToken().c_str());

    // ping connection (which would reconnect if needed)
	if (storm_ping_connection(thip->dbfd.mysql) != 0) {
		try {
			*resp = request.buildSpecificResponse(SRM_USCOREINTERNAL_USCOREERROR,
					"Lost connection to the DB.");
		} catch(storm::storm_error& exc) {
			srmlogit(STORM_LOG_ERROR, funcName, "Unable to build soap response. InvalidResponse: %s\n" , exc.what());
			return(SOAP_FATAL_ERROR);
		}
		return SOAP_OK;
	}

    try {

        if(request.supportsProtocolSpecification())
        {
			srmlogit(STORM_LOG_DEBUG, func, "Checking provided protocols\n");
			std::vector<sql_string> protocols;
			try
			{
				protocols = request.getRequestedProtocols();
			} catch (std::logic_error &e)
			{
				//not thrown since request.supportsProtocolSpecification() is true
				srmlogit(STORM_LOG_ERROR, funcName, "Unexpected exception in request.getRequestedProtocols() call. logic_error: %s\n" , e.what());
				return(SOAP_FATAL_ERROR);
			}
			if(ProtocolChecker::getInstance()->checkProtocols(protocols) != 0)
			{
				srmlogit(STORM_LOG_INFO, func, "Protocol check failed, received some unsupported protocols\n");
				std::vector<sql_string> newProtocolVector = ProtocolChecker::getInstance()->removeUnsupportedProtocols(request.getRequestedProtocols());
				int size = protocols.size();
				int newSize = newProtocolVector.size();
				int removedCount = size - newSize;

				srmlogit(STORM_LOG_INFO, func, "Received - %d - protocols, %d are supported, %d are not supported\n" , size, newSize, removedCount);
				if(newProtocolVector.empty())
				{
					srmlogit(STORM_LOG_INFO, func, "None of the requested protocols is supported. Request failed\n");
					request.setGenericFailureSurls();
					try {
						*resp = request.buildSpecificResponse(SRM_USCORENOT_USCORESUPPORTED,
												"No supported protocols provided.");
					} catch(storm::storm_error &exc) {
						srmlogit(STORM_LOG_ERROR, funcName, "Unable to build soap response. InvalidResponse: %s\n" , exc.what());
						return(SOAP_FATAL_ERROR);
					}
					return SOAP_OK;
				}
				else
				{
					srmlogit(STORM_LOG_INFO, func, "Some of the provided protocols are supported, proceeding\n");
					request.setProtocolVector(newProtocolVector);
				}
			}
			srmlogit(STORM_LOG_DEBUG, func, "Valid protocols available\n");
        }
        try {

            request.insertIntoDB(&thip->dbfd);

        } catch (storm_db::mysql_exception& e) {
        	srmlogit(STORM_LOG_ERROR, func, "mysql exception inserting data for request token %s. Error: %s\n"
        			, r_token, e.what());
        	request.invalidateRequestToken(); // Do not supply a request token in case of error.
        	try {
        		*resp = request.buildSpecificResponse(SRM_USCOREINTERNAL_USCOREERROR, "Database error. "
        				"Might be caused by a high load of the database server... try again later.");
        	} catch(storm::storm_error& exc) {
        		srmlogit(STORM_LOG_ERROR, funcName, "Unable to build soap response. InvalidResponse: %s\n" , exc.what());
        		return(SOAP_FATAL_ERROR);
        	}
        	return SOAP_OK;
        } catch (std::logic_error& x) {
        	srmlogit(STORM_LOG_ERROR, func,
        			"logic_error exception for request token %s. Error: %s\n", r_token, x.what());
        	request.invalidateRequestToken();
        	try {
        		*resp = request.buildSpecificResponse(SRM_USCORENOT_USCORESUPPORTED, x.what());
        	} catch(storm::storm_error& exc) {
        		srmlogit(STORM_LOG_ERROR, funcName, "Unable to build soap response. InvalidResponse: %s\n" , exc.what());
        		return(SOAP_FATAL_ERROR);
        	}
        	return SOAP_OK;
        }

        // Fill SOAP output structure.
        try {
        	*resp = request.buildResponse();
        } catch(storm::storm_error &exc) {
			srmlogit(STORM_LOG_ERROR, funcName, "Unable to build soap response. InvalidResponse: %s\n" , exc.what());
			return(SOAP_FATAL_ERROR);
		}
    } catch (bad_alloc& x) {
        srmlogit(STORM_LOG_ERROR, func, "bad_alloc exception.  token: %s, error: %s\n", r_token,
                x.what());
        return SOAP_EOM;
    }
    return SOAP_OK;
}

#endif
