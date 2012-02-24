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
#include "storm_mysql.h"
#include "sql_string.hpp"

#include "get_socket_info.hpp"

#include <vector>
#include <sstream>
#include "storm_util.h"
#include "ProtocolChecker.hpp"

using namespace std;

template<typename soap_in_t, typename soap_out_t>
int __process_file_request(struct soap *soap, storm::file_request<soap_in_t, soap_out_t>& request,
        const char* funcName, soap_in_t *req, soap_out_t **resp)
{
    static const char* func = "__process_file_request<>";

    struct srm_srv_thread_info* thip = static_cast<srm_srv_thread_info *> (soap->user);

    if (thip == NULL) {
        *resp = request.error_response(SRM_USCOREFATAL_USCOREINTERNAL_USCOREERROR,
                "Cannot get DB connect from the pool");
        return SOAP_OK;
    }

    srmlogit(STORM_LOG_DEBUG, func, "%s request from: %s\n", funcName, request.getClientDN().c_str());
    srmlogit(STORM_LOG_DEBUG, func, "Client IP=%s\n", get_ip(soap).c_str());

    // Generete the request token (unique identifier)
    uuid_t uuid;
    char r_token[ST_MAXDPMTOKENLEN + 1];
    uuid_generate(uuid);
    uuid_unparse(uuid, r_token);

    // Set the Request Token into the request
    request.r_token(r_token);

    srmlogit(STORM_LOG_DEBUG, func, "Request token: %s\n", request.r_token().c_str());

    // Connect to the DB, if needed.
    if (!(thip->db_open_done)) { // Get DB connection
        if (storm_opendb(db_srvr, db_user, db_pwd, &thip->dbfd) < 0) {
            *resp = request.error_response(SRM_USCOREINTERNAL_USCOREERROR, "Cannot get a DB connection.");
            return SOAP_OK;
        }
        thip->db_open_done = 1;
    } else { // ping connection and reconnect if needed
		if (storm_ping_connection(&thip->dbfd.mysql) != 0) {
			// check if reconnection succeeded
			if (storm_ping_connection(&thip->dbfd.mysql) != 0) {
				*resp = request.error_response(SRM_USCOREINTERNAL_USCOREERROR,
						"Lost connection to the DB.");
				return SOAP_OK;
			}
		}
	}

    // Load data from the soap request struct.
    try {
        try {

            request.load(req);

        } catch (storm::invalid_request x) {
            srmlogit(STORM_LOG_DEBUG, func, "Error loading data for request token %s: %s\n",
                    r_token, x.what());
            request.r_token(""); // We do not want to send the Request token, in case of error.
            *resp = request.error_response(SRM_USCOREINVALID_USCOREREQUEST, x.what());

            return SOAP_OK;
        }
        ProtocolChecker::getInstance()->ProtocolChecker::printProtocols();
        if(request.supportsProtocolSpecification())
        {
			srmlogit(STORM_LOG_DEBUG, func, "Checking provided protocols\n");
			if(ProtocolChecker::getInstance()->ProtocolChecker::checkProtocols(request.getRequestedProtocols()) != 0)
			{
				srmlogit(STORM_LOG_INFO, func, "Protocol check failed, received some unsupported protocols\n");
				std::vector<sql_string> newProtocolVector = ProtocolChecker::getInstance()->ProtocolChecker::removeUnsupportedProtocols(request.getRequestedProtocols());
				int size = request.getRequestedProtocols()->size();
				int newSize = newProtocolVector.size();
				int removedCount = size - newSize;

				srmlogit(STORM_LOG_INFO, func, "Received - %d - protocols, %d are supported, %d are not supported\n" , size, newSize, removedCount);
				if(newProtocolVector.empty())
				{
					srmlogit(STORM_LOG_INFO, func, "None of the requested protocols is supported. Request failed\n");
					request.setGenericFailureSurls();
					*resp = request.error_response(SRM_USCORENOT_USCORESUPPORTED,
											"No supported protocols provided.");
					return SOAP_OK;
				}
				else
				{
					srmlogit(STORM_LOG_INFO, func, "Some of the provided protocols are supported, proceeding\n");
					request.setProtocolVector(&newProtocolVector);
				}
			}
			srmlogit(STORM_LOG_DEBUG, func, "Valid protocols available\n");
        }
        try {

            request.insert(&thip->dbfd);

        } catch (storm_db::mysql_exception& e) {
            srmlogit(STORM_LOG_ERROR, func,
                    "mysql exception inserting data for request token %s. Error: %s\n", r_token,
                    e.what());
            request.r_token(""); // Do not supply a request token in case of error.
            *resp = request.error_response(SRM_USCOREINTERNAL_USCOREERROR, "Database error. "
                    "Might be caused by a high load of the database server... try again later.");

            return SOAP_OK;

        } catch (storm::not_supported x) {
            srmlogit(STORM_LOG_ERROR, func,
                    "not_supported exception for request token %s. Error: %s\n", r_token, x.what());
            request.r_token("");
            *resp = request.error_response(SRM_USCORENOT_USCORESUPPORTED, x.what());

            return SOAP_OK;

        }

        // Fill SOAP output structure.
        *resp = request.response();

    } catch (bad_alloc x) {
        srmlogit(STORM_LOG_ERROR, func, "bad_allc exception.  token: %s, error: %s\n", r_token,
                x.what());
        srmlogit(STORM_LOG_ERROR,func, "bad_alloc exception catched: %s\n", x.what());
        return SOAP_EOM;
    }

    return SOAP_OK;
}

#endif
