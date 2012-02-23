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


#include <stdexcept>
#include <map>
#include <vector>
#include <string>
#include "srmv2H.h"
#include "srmlogit.h"
#include "soap_util.hpp"
#include "mysql_query.hpp"
#include "storm_mysql.h"

#include "Authorization.hpp"
#include "Monitoring.hpp"
#include "boost/date_time/posix_time/posix_time.hpp"

#include "Credentials.hpp"
#include "get_socket_info.hpp"

#include <cgsi_plugin.h>

using namespace std;

extern "C" int ns1__srmGetRequestTokens(struct soap *soap,
                                        struct ns1__srmGetRequestTokensRequest *req,
                                        struct ns1__srmGetRequestTokensResponse_ *rep)
{
    static const char *func = "GetRequestTokens";
    storm::Credentials credentials(soap);
    srmLogRequest("Get request tokens",get_ip(soap).c_str(),credentials.getDN().c_str());
    boost::posix_time::ptime start_time = boost::posix_time::microsec_clock::local_time();
    struct ns1__srmGetRequestTokensResponse* repp;
    struct srm_srv_thread_info *thip = static_cast<srm_srv_thread_info *>(soap->user);
    
    try {
        /************************ Allocate response structure *******************************/
        repp = storm::soap_calloc<struct ns1__srmGetRequestTokensResponse>(soap);
        repp->returnStatus = storm::soap_calloc<struct ns1__TReturnStatus>(soap);
        // Assign the repp response structure to the output parameter rep.
        rep->srmGetRequestTokensResponse = repp;
        
        if (credentials.getDN().empty()) {
            srmlogit(STORM_LOG_ERROR, func, "Client DN not found!\n");
            repp->returnStatus->statusCode = SRM_USCOREAUTHENTICATION_USCOREFAILURE;
            repp->returnStatus->explanation = "Unable to retrieve client DN";
        	boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
        	boost::posix_time::time_duration et = (end_time - start_time);
			try
			{
				storm::Monitoring::getInstance()->getMonitor(
						storm::SRM_GET_REQUEST_TOKENS_MONITOR_NAME)->registerError(et.total_milliseconds());
			}catch(storm::MonitorNotEnabledException *exc)
			{
				srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
			}
            return SOAP_OK;
        }
        
        if(storm::Authorization::checkBlacklist(soap))
		{
			srmlogit(STORM_LOG_INFO, func, "The user is blacklisted\n");
			repp->returnStatus->statusCode = SRM_USCOREAUTHORIZATION_USCOREFAILURE;
			repp->returnStatus->explanation = "User not authorized";
			boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
			boost::posix_time::time_duration et = (end_time - start_time);
			try
			{
				storm::Monitoring::getInstance()->getMonitor(
						storm::SRM_GET_REQUEST_TOKENS_MONITOR_NAME)->registerFailure(et.total_milliseconds());
			}catch(storm::MonitorNotEnabledException *exc)
			{
				srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
			}
			return SOAP_OK;
		}
		else
		{
			srmlogit(STORM_LOG_DEBUG, func, "The user is not blacklisted\n");
		}

        // Connect to the DB, if needed.
        if (!(thip->db_open_done)) {
            if (storm_opendb(db_srvr, db_user, db_pwd, &thip->dbfd) < 0) {
                srmlogit(STORM_LOG_ERROR, func, "DB open error!\n");
                repp->returnStatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
                repp->returnStatus->explanation = "DB open error";
            	boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
            	boost::posix_time::time_duration et = (end_time - start_time);
    			try
    			{
    				storm::Monitoring::getInstance()->getMonitor(
    						storm::SRM_GET_REQUEST_TOKENS_MONITOR_NAME)->registerError(et.total_milliseconds());
    			}catch(storm::MonitorNotEnabledException *exc)
    			{
    				srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
    			}
                return SOAP_OK;
            }
            thip->db_open_done = 1;
        }
        
        // Get userRequestDescription
        std::string u_token;
        if (req->userRequestDescription != NULL)
            u_token = std::string(req->userRequestDescription);
            
        // Create the DB query
        std::string query_sql("SELECT r_token, DATE_FORMAT(timeStamp, '%Y-%m-%dT%H:%i:%S') FROM request_queue WHERE "
                      "client_dn='" + credentials.getDN() + "'");
        if (!u_token.empty())
            query_sql += " AND u_token='" + u_token + "'";
        srmlogit(STORM_LOG_DEBUG, func, "Query: %s\n", query_sql.c_str());
        
        storm_start_tr(0, &thip->dbfd);
        vector< map<string, string> > results;
        storm_db::vector_exec_query(&thip->dbfd, query_sql, results);
        storm_end_tr(&thip->dbfd);
        
        int resultsSize = results.size();
        srmlogit(STORM_LOG_DEBUG, func, "Results (size): %d\n", resultsSize);
        
        if (resultsSize < 1) {
            if (u_token.empty()) {
                srmlogit(STORM_LOG_ERROR, func, "Return status: SRM_FAILURE\n");
                repp->returnStatus->statusCode = SRM_USCOREFAILURE;
                repp->returnStatus->explanation = "No request tokens owned by this user";
            }
            else {
                srmlogit(STORM_LOG_ERROR, func, "Return status: SRM_INVALID_REQUEST\n");
                repp->returnStatus->statusCode = SRM_USCOREINVALID_USCOREREQUEST;
                repp->returnStatus->explanation = "'userRequestDescription' does not refer to any existing known requests";
            }
        	boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
        	boost::posix_time::time_duration et = (end_time - start_time);
			try
			{
				storm::Monitoring::getInstance()->getMonitor(
						storm::SRM_GET_REQUEST_TOKENS_MONITOR_NAME)->registerFailure(et.total_milliseconds());
			}catch(storm::MonitorNotEnabledException *exc)
			{
				srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
			}
            return SOAP_OK;
        } 
        
        // Allocate 'arrayOfRequestTokens' in the response structure
        repp->arrayOfRequestTokens = storm::soap_calloc<struct ns1__ArrayOfTRequestTokenReturn>(soap);
        repp->arrayOfRequestTokens->__sizetokenArray = resultsSize;
        repp->arrayOfRequestTokens->tokenArray = storm::soap_calloc<struct ns1__TRequestTokenReturn>(soap, resultsSize);
        
        int i;
        for (i=0; i<results.size(); i++) {
            repp->arrayOfRequestTokens->tokenArray[i] = storm::soap_calloc<struct ns1__TRequestTokenReturn>(soap);
            repp->arrayOfRequestTokens->tokenArray[i]->requestToken = soap_strdup(soap, results[i]["r_token"].c_str());
            repp->arrayOfRequestTokens->tokenArray[i]->createdAtTime =
                                        soap_strdup(soap, results[i]["DATE_FORMAT(timeStamp, '%Y-%m-%dT%H:%i:%S')"].c_str());
        }
    }
    catch (soap_bad_alloc) {
        srmlogit(STORM_LOG_ERROR, func, "Memory allocation error (response structure)!\n");
    	boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
    	boost::posix_time::time_duration et = (end_time - start_time);
		try
		{
			storm::Monitoring::getInstance()->getMonitor(
					storm::SRM_GET_REQUEST_TOKENS_MONITOR_NAME)->registerError(et.total_milliseconds());
		}catch(storm::MonitorNotEnabledException *exc)
		{
			srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
		}
        return SOAP_EOM;
    }
    catch (std::invalid_argument) {
        srmlogit(STORM_LOG_ERROR, func, "soap pointer is NULL!\n");
    	boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
    	boost::posix_time::time_duration et = (end_time - start_time);
		try
		{
			storm::Monitoring::getInstance()->getMonitor(
					storm::SRM_GET_REQUEST_TOKENS_MONITOR_NAME)->registerError(et.total_milliseconds());
		}catch(storm::MonitorNotEnabledException *exc)
		{
			srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
		}
        return SOAP_NULL;
    }
    
    srmlogit(STORM_LOG_ERROR, func, "Return status: SRM_SUCCESS\n");
    repp->returnStatus->statusCode = SRM_USCORESUCCESS;
    repp->returnStatus->explanation = "Request successfully completed";
	boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
	boost::posix_time::time_duration et = (end_time - start_time);
	try
	{
		storm::Monitoring::getInstance()->getMonitor(
				storm::SRM_GET_REQUEST_TOKENS_MONITOR_NAME)->registerSuccess(et.total_milliseconds());
	}catch(storm::MonitorNotEnabledException *exc)
	{
		srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
	}
    return(SOAP_OK);
}
