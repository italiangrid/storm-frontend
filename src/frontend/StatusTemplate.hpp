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

#ifndef STATUS_TEMPLATE_HPP
#define STATUS_TEMPLATE_HPP

#include <string>
#include "srmv2H.h"
#include "srm_server.h"
#include "FileStatusRequest.hpp"
#include "storm_mysql.h"
#include "get_socket_info.hpp"
#include <cgsi_plugin.h>

template<typename soap_request_t, typename soap_response_t>
int processRequestStatus(struct soap* soap, const char* funcName,
        storm::FileStatusRequest<soap_request_t, soap_response_t>& statusRequest, soap_response_t** resp)

{
    static const char* func = "__process_request_status<>";

    struct srm_srv_thread_info *thip = static_cast<srm_srv_thread_info *> (soap->user);

    if (thip == NULL) {
    	try
    	{
    		*resp = statusRequest.buildSpecificResponse(SRM_USCOREFATAL_USCOREINTERNAL_USCOREERROR,
    				"Cannot get DB connection from the pool");
    	} catch(std::logic_error& exc)
    	{
    		srmlogit(STORM_LOG_ERROR, funcName, "Unable to build soap response. logic_error: %s\n" , exc.what());
    		return(SOAP_FATAL_ERROR);
    	}
    	return SOAP_OK;
    }

    if (statusRequest.getRequestToken().empty()) {
    	try
    	{
    		*resp = statusRequest.buildSpecificResponse(SRM_USCOREINVALID_USCOREREQUEST, "Empty request token");
    	} catch(std::logic_error& exc)
		{
			srmlogit(STORM_LOG_ERROR, funcName, "Unable to build soap response. logic_error: %s\n" , exc.what());
			return(SOAP_FATAL_ERROR);
		}
        return SOAP_OK;
    }

    if (!(thip->db_open_done)) { // Get DB connection
        if (storm_opendb(db_srvr, db_user, db_pwd, &thip->dbfd) < 0) {
        	try
        	{
        		*resp = statusRequest.buildSpecificResponse(SRM_USCOREINTERNAL_USCOREERROR, "Cannot get a DB connection.");
        	} catch(std::logic_error& exc)
        	{
        		srmlogit(STORM_LOG_ERROR, funcName, "Unable to build soap response. logic_error: %s\n" , exc.what());
        		return(SOAP_FATAL_ERROR);
        	}
            return SOAP_OK;
        }
        thip->db_open_done = 1;
    } else { // ping connection and reconnect if needed
    	if (storm_ping_connection(&thip->dbfd.mysql) != 0) {
    		// check if reconnection succeeded
    		if (storm_ping_connection(&thip->dbfd.mysql) != 0) {
    			try
    			{
    				*resp = statusRequest.buildSpecificResponse(SRM_USCOREINTERNAL_USCOREERROR,
    						"Lost connection to the DB.");
    			} catch(std::logic_error& exc)
    			{
    				srmlogit(STORM_LOG_ERROR, funcName, "Unable to build soap response. logic_error: %s\n" , exc.what());
    				return(SOAP_FATAL_ERROR);
    			}
				return SOAP_OK;
			}
		}
    }

    try {

    	try
    	{
    		statusRequest.loadFromDB(&thip->dbfd);
    	} catch (storm::TokenNotFound& x)
    	{
    		/*srmlogit(STORM_LOG_DEBUG, func,
					"No request found for token %s . TokenNotFound: %s",
					statusRequest.getRequestToken().c_str(), x.what());*/
			try {
				*resp = statusRequest.buildSpecificResponse(
						SRM_USCOREINVALID_USCOREREQUEST,
						"No request by that token");
			} catch(std::logic_error& exc) {
				srmlogit(STORM_LOG_ERROR, funcName,
						"Unable to build soap response. logic_error: %s\n",
						exc.what());
				return (SOAP_FATAL_ERROR);
			}
    		return SOAP_OK;
    	} catch (storm_db::mysql_exception& x) {
    		srmlogit(STORM_LOG_ERROR, func,
    				"mysql exception laoding status for request token %s. Error: %s\n",
    				statusRequest.getRequestToken().c_str(), x.what());
    		try
    		{
    			*resp = statusRequest.buildSpecificResponse(SRM_USCOREFAILURE,
            			"Generic error quering the status for the request");
            } catch(std::logic_error& exc)
            {
            	srmlogit(STORM_LOG_ERROR, funcName, "Unable to build soap response. logic_error: %s\n" , exc.what());
            	return(SOAP_FATAL_ERROR);
            }
            return SOAP_OK;
        }

        if (!statusRequest.isAuthorized()) {

        	try
        	{
        		*resp = statusRequest.buildSpecificResponse(SRM_USCOREAUTHORIZATION_USCOREFAILURE,
        				"The request was made from another DN."
        				" You're not authorized to inspect it.");
        	} catch(std::logic_error& exc)
        	{
        		srmlogit(STORM_LOG_ERROR, funcName, "Unable to build soap response. logic_error: %s\n" , exc.what());
        		return(SOAP_FATAL_ERROR);
        	}
            return SOAP_OK;
        }

        try
        {
        	*resp = statusRequest.buildResponse();
        } catch(std::logic_error& exc)
        {
        	srmlogit(STORM_LOG_ERROR, funcName, "Unable to build soap response. logic_error: %s\n" , exc.what());
        	return(SOAP_FATAL_ERROR);
        }
    } catch (std::bad_alloc x) {
        srmlogit(STORM_LOG_ERROR, func, "bad_alloc exception catched: %s\n", x.what());
        return SOAP_EOM;
    }
    return SOAP_OK;
}
#endif // STATUS_TEMPLATE_HPP
