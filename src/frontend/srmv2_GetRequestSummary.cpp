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
#include <sstream>
#include "srmv2H.h"
#include "srmlogit.h"
#include "soap_util.hpp"
#include "mysql_query.hpp"
#include "storm_mysql.h"

#include "Authorization.hpp"
#include "MonitoringHelper.hpp"
#include "boost/date_time/posix_time/posix_time.hpp"
#include "Credentials.hpp"
#include "get_socket_info.hpp"
#include "token_validator.hpp"
#include "storm_exception.hpp"
#include "xmlrpc_client.hpp"

#include <cgsi_plugin.h>

using namespace std;

// Conversion from string to numeric value
template <typename type_t>
bool string2num(type_t& x, const string& s)
{
  istringstream ss(s);
  return (ss >> x).fail();
}

// Validates 'req->arrayOfRequestTokens'
bool validate_array_of_req_tokens(const char* func, struct ns1__ArrayOfString* arrayOfRequestTokens)
{

    if (arrayOfRequestTokens == NULL) {
         srmlogit(STORM_LOG_ERROR, func, "arrayOfRequestTokens=NULL\n");
        return false;
    }
    if (arrayOfRequestTokens->__sizestringArray == 0) {
        srmlogit(STORM_LOG_ERROR, func, "arrayOfRequestTokens (size)=0\n");
        return false;
    }
    if (arrayOfRequestTokens->stringArray == NULL) {
        srmlogit(STORM_LOG_ERROR, func, "arrayOfRequestTokens->stringArray=NULL (and size != 0)\n");
        return false;
    }
    return true;
}

// Converts the request type found in the DB in ns1__TRequestType
ns1__TRequestType getRequestType(std::string& r_type)
{
    if (r_type.compare("PTG") == 0)
        return ns1__TRequestType(0);
    if (r_type.compare("PTP") == 0)
        return ns1__TRequestType(1);
    if (r_type.compare("COP") == 0)
        return ns1__TRequestType(2);
    if (r_type.compare("BOL") == 0)
        return ns1__TRequestType(3);

    srmlogit(STORM_LOG_ERROR, "GetRequestSummary", "BUG: Failed conversion of request type!!!\n");
    return ns1__TRequestType(PREPARE_USCORETO_USCOREPUT);
}

extern "C" int ns1__srmGetRequestSummary(struct soap *soap,
                                         struct ns1__srmGetRequestSummaryRequest *req,
                                         struct ns1__srmGetRequestSummaryResponse_ *rep)
{
    static const char *func = "GetRequestSummary";
    storm::Credentials credentials(soap);
    srmLogRequest("Get request summary",get_ip(soap).c_str(),credentials.getDN().c_str());
    boost::posix_time::ptime start_time = boost::posix_time::microsec_clock::local_time();
    struct ns1__srmGetRequestSummaryResponse* repp;
    struct srm_srv_thread_info *thip = static_cast<srm_srv_thread_info *>(soap->user);
    bool requestSuccess, requestFailure;
    int i;

    try {
        /************************ Allocate response structure *******************************/
        repp = storm::soap_calloc<struct ns1__srmGetRequestSummaryResponse>(soap);
        repp->returnStatus = storm::soap_calloc<struct ns1__TReturnStatus>(soap);
        // Assign the repp response structure to the output parameter rep.
        rep->srmGetRequestSummaryResponse = repp;

        if (credentials.getDN().empty()) {
            srmlogit(STORM_LOG_ERROR, func, "Client DN not found!\n");
            repp->returnStatus->statusCode = SRM_USCOREAUTHENTICATION_USCOREFAILURE;
            repp->returnStatus->explanation = const_cast<char*>("Unable to retrieve client DN");
            storm::MonitoringHelper::registerOperationError(start_time, storm::SRM_GET_REQUEST_SUMMARY_MONITOR_NAME);
            return SOAP_OK;
        }
        srmlogit(STORM_LOG_INFO, func, "UserDN=%s\n", credentials.getDN().c_str());

        if(storm::authz::is_blacklisted(soap))
		{
			srmlogit(STORM_LOG_INFO, func, "The user is blacklisted\n");
			repp->returnStatus->statusCode = SRM_USCOREAUTHORIZATION_USCOREFAILURE;
			repp->returnStatus->explanation = const_cast<char*>("User not authorized");
			storm::MonitoringHelper::registerOperationFailure(start_time, storm::SRM_GET_REQUEST_SUMMARY_MONITOR_NAME);
			return SOAP_OK;
		}
		else
		{
			srmlogit(STORM_LOG_DEBUG, func, "The user is not blacklisted\n");
		}
        // Check for a valid input
        if (!validate_array_of_req_tokens(func, req->arrayOfRequestTokens)) {
            srmlogit(STORM_LOG_ERROR, func, "Invalid 'arrayOfRequestTokens'\n");
            repp->returnStatus->statusCode = SRM_USCOREINVALID_USCOREREQUEST;
            repp->returnStatus->explanation = const_cast<char*>("invalid array of request tokens");
            storm::MonitoringHelper::registerOperationFailure(start_time, storm::SRM_GET_REQUEST_SUMMARY_MONITOR_NAME);
            return SOAP_OK;
        }
        int numOfRequestTokens = req->arrayOfRequestTokens->__sizestringArray;

        for (i=0; i < numOfRequestTokens; i++){


        	if (req->arrayOfRequestTokens->stringArray[i] == NULL){
        		srmlogit(STORM_LOG_DEBUG, func, "Received token[%d]=NULL\n", i);
        	}else{
        		srmlogit(STORM_LOG_DEBUG, func, "Received token[%d]=%s\n", i, req->arrayOfRequestTokens->stringArray[i]);
        		std::string token(req->arrayOfRequestTokens->stringArray[i]);
        		if (!storm::token::valid(token)){
        			repp->returnStatus->statusCode = SRM_USCOREINVALID_USCOREREQUEST;
        			repp->returnStatus->explanation = const_cast<char*>("invalid token");
        			storm::MonitoringHelper::registerOperationFailure(start_time, storm::SRM_GET_REQUEST_SUMMARY_MONITOR_NAME);
        			return SOAP_OK;
        		}
        	}
        }

        // Connect to the DB, if needed.
        if (!(thip->db_open_done)) {
            if (storm_opendb(db_srvr, db_user, db_pwd, &thip->dbfd) < 0) {
                srmlogit(STORM_LOG_ERROR, func, "DB open error!\n");
                repp->returnStatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
                repp->returnStatus->explanation = const_cast<char*>("DB open error");
                storm::MonitoringHelper::registerOperationError(start_time, storm::SRM_GET_REQUEST_SUMMARY_MONITOR_NAME);
                return SOAP_OK;
            }
            thip->db_open_done = 1;
        }

        // Create the DB query
        std::string query_sql = std::string("SELECT ID, r_token, config_RequestTypeID, status, errstring, nbreqfiles "
                                            "FROM request_queue "
                                            "WHERE client_dn='" + credentials.getDN() + "' AND (");
        for (i=0; i<numOfRequestTokens; i++) {
            if (req->arrayOfRequestTokens->stringArray[i] == NULL)
                continue;
            std::string token = string(req->arrayOfRequestTokens->stringArray[i]);
            if (token.empty())
                continue;
            query_sql += "r_token='";
            query_sql += token;
            query_sql += '\'';
            query_sql += " OR ";
        }
        query_sql.erase(query_sql.length()-4, 4);  // remove the last " OR "
        query_sql += ')';
        // Execute the DB query
        storm_start_tr(0, &thip->dbfd);
        vector< map<string, string> > results;
        storm_db::vector_exec_query(&thip->dbfd, query_sql, results);

        int resultsSize = results.size();
        srmlogit(STORM_LOG_INFO, func, "Results (size): %d\n", resultsSize);

        if (resultsSize < 1) {
            srmlogit(STORM_LOG_INFO, func, "Return status: SRM_FAILURE\n");
            repp->returnStatus->statusCode = SRM_USCOREFAILURE;
            repp->returnStatus->explanation = const_cast<char*>("None of the requested tokens were found");
            storm::MonitoringHelper::registerOperationFailure(start_time, storm::SRM_GET_REQUEST_SUMMARY_MONITOR_NAME);
            return SOAP_OK;
        }

        // Allocate 'arrayOfRequestSummaries' in the response structure
        repp->arrayOfRequestSummaries = storm::soap_calloc<struct ns1__ArrayOfTRequestSummary>(soap);
        if(numOfRequestTokens > 0)
        {
			repp->arrayOfRequestSummaries->__sizesummaryArray = numOfRequestTokens;
			repp->arrayOfRequestSummaries->summaryArray = storm::soap_calloc<struct ns1__TRequestSummary>(soap, numOfRequestTokens);
			// For each requested token, if it was found in the DB then the retrieved sumary is returned
			// (by querying the table corresponding to the request to calculate some values to be returned),
			// otherwise a SRM_INVALID_REQUEST with "Invalid request token" explanation are returned.
			requestSuccess = true;
			requestFailure = true;
			int i;
			for (i=0; i<numOfRequestTokens; i++) {
				if (req->arrayOfRequestTokens->stringArray[i] == NULL) {
					(repp->arrayOfRequestSummaries->__sizesummaryArray)--;
					continue;
				}
				std::string reqToken = req->arrayOfRequestTokens->stringArray[i];
				repp->arrayOfRequestSummaries->summaryArray[i] = storm::soap_calloc<struct ns1__TRequestSummary>(soap);
				struct ns1__TRequestSummary* reqSummary = repp->arrayOfRequestSummaries->summaryArray[i];
				reqSummary->status = storm::soap_calloc<struct ns1__TReturnStatus>(soap);
				// Check if the requested token was found in the DB
				bool found = false;
				int j;
				for (j=0; j<resultsSize; j++) {
					if (reqToken.compare(results[j]["r_token"]) == 0) {
						found = true;
						break;
					}
				}
				reqSummary->requestToken = soap_strdup(soap, reqToken.c_str());
				if (found) {
					requestFailure = false;

					// statusCode
					int intVal;
					string2num(intVal, results[j]["status"]);
					enum ns1__TStatusCode r_status = ns1__TStatusCode(intVal);
					reqSummary->status->statusCode = r_status;

					// explanation
					reqSummary->status->explanation = soap_strdup(soap, results[j]["errstring"].c_str());

					// totalNumFilesInRequest
					int r_nbreqfiles;
					if (!string2num(r_nbreqfiles, results[j]["nbreqfiles"])) {
						reqSummary->totalNumFilesInRequest = storm::soap_calloc<int>(soap);
						*(reqSummary->totalNumFilesInRequest) = r_nbreqfiles;
					}

					// requestType
					reqSummary->requestType = storm::soap_calloc<enum ns1__TRequestType>(soap);
					enum ns1__TRequestType r_type = getRequestType(results[j]["config_RequestTypeID"]);
					*(reqSummary->requestType) = r_type;

					// Create the SQL query to retrieve the status of the request
					switch (r_type) {
						case PREPARE_USCORETO_USCOREGET:
							query_sql = "SELECT statusCode FROM status_Get s JOIN request_Get c ON "
										"(c.request_queueID='";
							query_sql += results[j]["ID"];
							query_sql += "' AND s.request_GetID=c.ID)";
							break;
						case PREPARE_USCORETO_USCOREPUT:
							query_sql = "SELECT statusCode FROM status_Put s JOIN request_Put c ON "
										"(c.request_queueID='";
							query_sql += results[j]["ID"];
							query_sql += "' AND s.request_PutID=c.ID)";
							break;
						case COPY:
							query_sql = "SELECT statusCode FROM status_Copy s JOIN request_Copy c ON "
										"(c.request_queueID='";
							query_sql += results[j]["ID"];
							query_sql += "' AND s.request_CopyID=c.ID)";
							break;
						case BRING_USCOREONLINE:
							query_sql = "SELECT statusCode FROM status_BoL s JOIN request_BoL c ON "
										"(c.request_queueID='";
							query_sql += results[j]["ID"];
							query_sql += "' AND s.request_BoLID=c.ID)";
							break;
						default:
							query_sql = string();
							break;
					}
					if (!query_sql.empty()) {
						// Calculate the values: numOfCompletedFiles, numOfWaitingFiles, numOfFailedFiles
						vector< map<string, string> > results_status;
						storm_db::vector_exec_query(&thip->dbfd, query_sql, results_status);

						if (results_status.size() != static_cast<size_t>(r_nbreqfiles)) {
							reqSummary->status->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
							reqSummary->status->explanation = const_cast<char*>("BUG! In DB wrong nbreqfiles");
						} else {
							int numOfCompleted = 0;
							int numOfWaiting = 0;
							int numOfFailed = 0;
							int k;
							for (k=0; k<r_nbreqfiles; k++) {
								string2num(intVal, results_status[k]["statusCode"]);
								enum ns1__TStatusCode r_status = ns1__TStatusCode(intVal);
								switch (r_status) {
									case SRM_USCORESUCCESS:
									case SRM_USCORERELEASED:
									case SRM_USCOREFILE_USCOREPINNED:
									case SRM_USCOREFILE_USCOREIN_USCORECACHE:
									case SRM_USCORESPACE_USCOREAVAILABLE:
									case SRM_USCOREDONE:
										numOfCompleted++;
										break;
									case SRM_USCOREREQUEST_USCOREQUEUED:
									case SRM_USCOREREQUEST_USCOREINPROGRESS:
										numOfWaiting++;
										break;
									default:
										numOfFailed++;
										break;
								}
							}
							// numOfCompletedFiles
							reqSummary->numOfCompletedFiles = storm::soap_calloc<int>(soap);
							*(reqSummary->numOfCompletedFiles) = numOfCompleted;
							// numOfWaitingFiles
							reqSummary->numOfWaitingFiles = storm::soap_calloc<int>(soap);
							*(reqSummary->numOfWaitingFiles) = numOfWaiting;
							// numOfFailedFiles
							reqSummary->numOfFailedFiles = storm::soap_calloc<int>(soap);
							*(reqSummary->numOfFailedFiles) = numOfFailed;
						}
					}
				} else {
					requestSuccess = false;
					reqSummary->status->statusCode = SRM_USCOREINVALID_USCOREREQUEST;
					reqSummary->status->explanation = const_cast<char*>("Invalid request token");
				}
			}
		}
    }
	catch (soap_bad_alloc) {
		if ((thip->dbfd).tr_started == 1)
			storm_end_tr(&thip->dbfd);
		srmlogit(STORM_LOG_ERROR, func, "Memory allocation error (response structure)!\n");
		storm::MonitoringHelper::registerOperationError(start_time, storm::SRM_GET_REQUEST_SUMMARY_MONITOR_NAME);
		return SOAP_EOM;
	}
	catch (std::invalid_argument) {
		if ((thip->dbfd).tr_started == 1)
			storm_end_tr(&thip->dbfd);
		srmlogit(STORM_LOG_ERROR, func, "soap pointer is NULL!\n");
		storm::MonitoringHelper::registerOperationError(start_time, storm::SRM_GET_REQUEST_SUMMARY_MONITOR_NAME);
		return SOAP_NULL;
	}

    storm_end_tr(&thip->dbfd);
    if (requestSuccess) {
        srmlogit(STORM_LOG_INFO, func, "Return status: SRM_SUCCESS\n");
        repp->returnStatus->statusCode = SRM_USCORESUCCESS;
        repp->returnStatus->explanation = const_cast<char*>("All tokens retrieved");
    } else if (requestFailure) {
        // This means that the first DB query found some of the user requested tokens in the DB but
        // (in the following for loop) none of the user requested tokens matched any of the tokens
        // retrieved in the DB. This cannot happen, unless there is a bug.
        srmlogit(STORM_LOG_INFO, func, "Return status: SRM_INTERNAL_ERROR (that's a BUG)\n");
        repp->returnStatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
        repp->returnStatus->explanation = const_cast<char*>("Failure due to a bug");
    } else {
        srmlogit(STORM_LOG_INFO, func, "Return status: SRM_PARTIAL_SUCCESS\n");
        repp->returnStatus->statusCode = SRM_USCOREPARTIAL_USCORESUCCESS;
        repp->returnStatus->explanation = const_cast<char*>("Some request summaries are failed");
    }
    storm::MonitoringHelper::registerOperation(start_time, storm::SRM_GET_REQUEST_SUMMARY_MONITOR_NAME, SRM_USCORESUCCESS);
    return(SOAP_OK);
}
