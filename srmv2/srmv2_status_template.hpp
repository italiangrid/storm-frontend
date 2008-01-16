#ifndef SRMV2_STATUS_TEMPLATE_HPP
#define SRMV2_STATUS_TEMPLATE_HPP
// Copyright (C) 2006 by Antonio Messina <antonio.messina@ictp.it> for the ICTP project EGRID.
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
#include <string>
#include "srmv2H.h"
#include "srm_server.h"
#include "file_status.hpp"
#include <cgsi_plugin.h>

using namespace std;


extern char db_user[33];
extern char db_srvr[33];
extern char db_pwd[33];


template<typename soap_response_t>
int __process_request_status (struct soap * soap,
                              const char * const r_token,
                              storm::file_status<soap_response_t> &status,
                              soap_response_t **resp)

{
    struct srm_srv_thread_info *thip = static_cast<srm_srv_thread_info *>(soap->user);
    char clientdn[256];
    if(NULL == r_token){
        *resp = status.error_response(SRM_USCOREINVALID_USCOREREQUEST,
                                      "Empty request token");
        return SOAP_OK;
    }
    string requestToken(r_token);

#if defined(GSI_PLUGINS)
    get_client_dn(soap, clientdn, sizeof(clientdn));
#endif

    // load data from DB
    if (!(thip->db_open_done)) {
        if (storm_opendb(db_srvr, db_user, db_pwd, &thip->dbfd) < 0) {
            *resp = status.error_response(SRM_USCOREINTERNAL_USCOREERROR,
                                 "DB open error");
            return SOAP_OK;
        }
        thip->db_open_done = 1;
    }
    

// Try for possible soap_bad_alloc
    try{

        try {
            status.load(&thip->dbfd, requestToken);
        }catch (token_not_found x) {            
            srmlogit(STORM_LOG_DEBUG, "__process_request_status<>", 
                     "No request by token %s", requestToken.c_str());
            *resp = status.error_response(SRM_USCOREINVALID_USCOREREQUEST, 
                                              "No request by that token");
            return SOAP_OK;
        }catch(storm_db::mysql_exception x){
            srmlogit(STORM_LOG_ERROR, "__process_request_status<>", 
                     "mysql exception laoding status for request token %s. Error: %s\n",
                     requestToken.c_str(), x.what());
            *resp = status.error_response(SRM_USCOREFAILURE,
                                          "Generic error quering the status for the request");
            return SOAP_OK;
        }

        // is client authorized?
        // i.e., the same DN must have issued the Copy request and this Status request
        if (! status.is_authorized(clientdn)){
            
            *resp = status.error_response(SRM_USCOREAUTHORIZATION_USCOREFAILURE, 
                                          "The request was made from another DN."
                                          " You're not authorized to inspect it.");
            return SOAP_OK;
        }

        // OK, fill in SOAP output structure
        *resp = status.response();
    
    }catch(bad_alloc x){
        srmlogit(STORM_LOG_ERROR,"__process_request_status<>()","bad_alloc exception catched: %s\n",x.what());
        return SOAP_EOM;
    }
    return SOAP_OK;
}
#endif
