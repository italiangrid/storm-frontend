/*
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by the
 *  Free Software Foundation; either version 2 of the License, or (at your
 *  option) any later version.
 *
 *  This program is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
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

#include <cgsi_plugin.h>

using namespace std;

extern "C" int ns1__srmGetRequestTokens(struct soap *soap,
                                        struct ns1__srmGetRequestTokensRequest *req,
                                        struct ns1__srmGetRequestTokensResponse_ *rep)
{
    static const char *func = "GetRequestTokens";
    struct ns1__srmGetRequestTokensResponse* repp;
    struct srm_srv_thread_info *thip = static_cast<srm_srv_thread_info *>(soap->user);
    
    try {
        /************************ Allocate response structure *******************************/
        repp = storm::soap_calloc<struct ns1__srmGetRequestTokensResponse>(soap);
        repp->returnStatus = storm::soap_calloc<struct ns1__TReturnStatus>(soap);
        // Assign the repp response structure to the output parameter rep.
        rep->srmGetRequestTokensResponse = repp;
        
        // Get client DN
        std::string client_dn;
    #if defined(GSI_PLUGINS)
        char clientdn[256];
        get_client_dn(soap, clientdn, sizeof(clientdn));
        client_dn = std::string(clientdn);
     #endif
        if (client_dn.empty()) {
            srmlogit(STORM_LOG_ERROR, func, "Client DN not found!\n");
            repp->returnStatus->statusCode = SRM_USCOREAUTHENTICATION_USCOREFAILURE;
            repp->returnStatus->explanation = "Unable to retrieve client DN";
            return SOAP_OK;
        }
        
        // Connect to the DB, if needed.
        if (!(thip->db_open_done)) {
            if (storm_opendb(db_srvr, db_user, db_pwd, &thip->dbfd) < 0) {
                srmlogit(STORM_LOG_ERROR, func, "DB open error!\n");
                repp->returnStatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
                repp->returnStatus->explanation = "DB open error";
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
                      "client_dn='" + client_dn + "'");
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
        return SOAP_EOM;
    }
    catch (std::invalid_argument) {
        srmlogit(STORM_LOG_ERROR, func, "soap pointer is NULL!\n");
        return SOAP_NULL;
    }
    
    srmlogit(STORM_LOG_ERROR, func, "Return status: SRM_SUCCESS\n");
    repp->returnStatus->statusCode = SRM_USCORESUCCESS;
    repp->returnStatus->explanation = "Request successfully completed";
    return(SOAP_OK);
}
