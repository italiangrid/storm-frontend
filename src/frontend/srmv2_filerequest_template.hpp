#ifndef SRMV2_FILEREQUEST_TEMPLATE_HPP
#define SRMV2_FILEREQUEST_TEMPLATE_HPP
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

#include <uuid/uuid.h>
//@todo
extern "C" {
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <voms_apic.h>
#include "storm_util.h"
}


#include "srmlogit.h"
#include "storm_functions.h"
#include "srm_server.h"

#include <exception>
#include <stdexcept>
#include "file_request.hpp"
#include "Credentials.hpp"

#include "mysql_query.hpp"
#include "sql_string.hpp"

using namespace std;

template<typename soap_in_t, typename soap_out_t>
int __process_file_request(struct soap *soap,
                           storm::file_request<soap_in_t, soap_out_t> &request,
                           soap_in_t *req,
                           soap_out_t **resp)
{
    static const char * const func = "__process_file_request<>"; 
    struct srm_srv_thread_info *thip = static_cast<srm_srv_thread_info *>(soap->user);
    
	//Generete Unique request identifier as REQUEST TOKEN
    uuid_t uuid;
    char r_token[ST_MAXDPMTOKENLEN+1];
    uuid_generate (uuid);
    uuid_unparse (uuid, r_token);
    
    // Set the Request Token into the request
    request.r_token(r_token);
    
    srmlogit(STORM_LOG_DEBUG, func, "Request token: %s\n", request.r_token().c_str());

    // Connect to the DB, if needed.
    if (!(thip->db_open_done)) {
        if (storm_opendb(db_srvr, db_user, db_pwd, &thip->dbfd) < 0) {
            *resp = request.error_response(SRM_USCOREINTERNAL_USCOREERROR,
                                           "DB open error");
            return SOAP_OK;
        }
        thip->db_open_done = 1;
    }

    // Load data from the soap request struct.
    try {
        try {
            request.load(req);
        } catch (storm::invalid_request x) {
            srmlogit(STORM_LOG_DEBUG, func,
                     "Error loading data for request token %s: %s\n",
                     r_token, x.what());
            request.r_token(""); // We do not want to send the Request token, in case of error.
            *resp = request.error_response(SRM_USCOREINVALID_USCOREREQUEST,
                                           x.what());
            return SOAP_OK;
        }
        
        // Temporary hack: proxy saved as a file. TODO: insert the proxy into DB.
        request.saveProxy();
		
		try {
            request.insert(&thip->dbfd);
        } catch (storm_db::mysql_exception x) {
            srmlogit(STORM_LOG_ERROR, func,
                     "mysql exception inserting data for request token %s. Error: %s\n",
                     r_token, x.what());
            request.r_token(""); // Do not supply a request token in case of error.
            *resp = request.error_response(SRM_USCOREFAILURE,
                                           "Generic DB error");
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
        srmlogit(STORM_LOG_ERROR, func,
                 "bad_allc exception.  token: %s, error: %s\n",
                 r_token, x.what());
        srmlogit(STORM_LOG_ERROR,func,"bad_alloc exception catched: %s\n",x.what());
        return SOAP_EOM;
    }
    return SOAP_OK;
}

#endif
