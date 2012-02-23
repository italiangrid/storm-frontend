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

#ifndef SRMV2_STATUS_TEMPLATE_HPP
#define SRMV2_STATUS_TEMPLATE_HPP

#include <string>
#include "srmv2H.h"
#include "srm_server.h"
#include "file_status.hpp"
#include "storm_mysql.h"
#include "get_socket_info.hpp"
#include <cgsi_plugin.h>

using namespace std;

template<typename soap_response_t>
int __process_request_status(struct soap * soap, const char * const r_token, const char* funcName,
        storm::file_status<soap_response_t> &status, soap_response_t **resp)

{
    static const char* func = "__process_request_status<>";

    struct srm_srv_thread_info *thip = static_cast<srm_srv_thread_info *> (soap->user);

    if (thip == NULL) {
        *resp = status.error_response(SRM_USCOREFATAL_USCOREINTERNAL_USCOREERROR,
                "Cannot get DB connection from the pool");
        return SOAP_OK;
    }

    string clientDN = status.getClientDN();

    srmlogit(STORM_LOG_DEBUG, func, "%s request from: %s\n", funcName, clientDN.c_str());
    srmlogit(STORM_LOG_DEBUG, func, "Client IP=%s\n", get_ip(soap).c_str());

    if (NULL == r_token) {
        *resp = status.error_response(SRM_USCOREINVALID_USCOREREQUEST, "Empty request token");
        return SOAP_OK;
    }

    string requestToken(r_token);

    if (!(thip->db_open_done)) { // Get DB connection
        if (storm_opendb(db_srvr, db_user, db_pwd, &thip->dbfd) < 0) {
            *resp = status.error_response(SRM_USCOREINTERNAL_USCOREERROR, "Cannot get a DB connection.");
            return SOAP_OK;
        }
        thip->db_open_done = 1;
    } else { // ping connection and reconnect if needed
    	if (storm_ping_connection(&thip->dbfd.mysql) != 0) {
    		// check if reconnection succeeded
    		if (storm_ping_connection(&thip->dbfd.mysql) != 0) {
				*resp = status.error_response(SRM_USCOREINTERNAL_USCOREERROR,
						"Lost connection to the DB.");
				return SOAP_OK;
			}
		}
    }

    // Try for possible soap_bad_alloc
    try {

        try {
        	// load data from DB
            status.load(&thip->dbfd, requestToken);
        } catch (token_not_found x) {
            srmlogit(STORM_LOG_DEBUG, func, "No request by token %s", requestToken.c_str());
            *resp = status.error_response(SRM_USCOREINVALID_USCOREREQUEST, "No request by that token");
            return SOAP_OK;
        } catch (storm_db::mysql_exception x) {
            srmlogit(STORM_LOG_ERROR, func,
                    "mysql exception laoding status for request token %s. Error: %s\n",
                    requestToken.c_str(), x.what());
            *resp = status.error_response(SRM_USCOREFAILURE,
                    "Generic error quering the status for the request");
            return SOAP_OK;
        }

        // is client authorized?
        // i.e., the same DN must have issued the Copy request and this Status request
        if (!status.is_authorized(clientDN)) {

            *resp = status.error_response(SRM_USCOREAUTHORIZATION_USCOREFAILURE,
                    "The request was made from another DN."
                        " You're not authorized to inspect it.");
            return SOAP_OK;
        }

        // OK, fill in SOAP output structure
        *resp = status.response();

    } catch (bad_alloc x) {
        srmlogit(STORM_LOG_ERROR, func, "bad_alloc exception catched: %s\n", x.what());
        return SOAP_EOM;
    }
    return SOAP_OK;
}
#endif
