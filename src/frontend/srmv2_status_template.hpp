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
#include "storm_mysql.h"
#include <cgsi_plugin.h>

using namespace std;

template<typename soap_response_t>
int __process_request_status(struct soap * soap, const char * const r_token, const char* funcName,
        storm::file_status<soap_response_t> &status, soap_response_t **resp)

{
    static const char* func = "__process_request_status<>";
    struct srm_srv_thread_info *thip = static_cast<srm_srv_thread_info *> (soap->user);
    string clientDN = status.getClientDN();

    srmlogit(STORM_LOG_INFO, func, "%s request from: %s\n", funcName, clientDN.c_str());
    srmlogit(STORM_LOG_INFO, func, "Client IP=%d.%d.%d.%d\n", (soap->ip >> 24) & 0xFF,
             (soap->ip >> 16) & 0xFF, (soap->ip >> 8) & 0xFF, (soap->ip) & 0xFF);

    if (NULL == r_token) {
        *resp = status.error_response(SRM_USCOREINVALID_USCOREREQUEST, "Empty request token");
        return SOAP_OK;
    }

    string requestToken(r_token);

    // load data from DB
    if (!(thip->db_open_done)) {
        if (storm_opendb(db_srvr, db_user, db_pwd, &thip->dbfd) < 0) {
            *resp = status.error_response(SRM_USCOREINTERNAL_USCOREERROR, "DB open error");
            return SOAP_OK;
        }
        thip->db_open_done = 1;
    }

    // Try for possible soap_bad_alloc
    try {

        try {
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
