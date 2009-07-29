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
#include <vector>
#include <map>
#include <iostream>
#include <sstream>

#include "bol.hpp"
#include "mysql_query.hpp"
#include "storm_mysql.h"

using namespace storm;
void bol::load(struct ns1__srmBringOnlineRequest *req) {
    if (NULL == req) {
        throw invalid_request("Request is NULL");
    }
    if (NULL == req->arrayOfFileRequests) {
        throw invalid_request("SURLs array is NULL");
    }
    if (0 == req->arrayOfFileRequests->__sizerequestArray) {
        throw invalid_request("SURLs array is empty");
    }

    // Initial values
    _retrytime = -1; // -1 = not specified
    _pinLifetime = -1;
    _remainingTime = -1;
    _lifetime = -1;
    _deferredStartTime = 0;
    _f_type = DB_FILE_TYPE_UNKNOWN;

    // Request type
    _r_type = DB_BOL_REQUEST;

    // Status
    status( SRM_USCOREREQUEST_USCOREQUEUED);

    // Client DN
    if (getClientDN().size() == 0 && NULL != req->authorizationID) {
        setClientDN(req->authorizationID);
    }

    // Request Token is not being set now

    // Fill surl
    for (int i = 0; i < req->arrayOfFileRequests->__sizerequestArray; ++i) {

        ns1__TGetFileRequest* reqSURL = req->arrayOfFileRequests->requestArray[i];
        _surls.push_back(surl_t(reqSURL->sourceSURL, reqSURL->dirOption));

    }
    _n_files = _surls.size();

    // User Token
    if (NULL != req->userRequestDescription && u_token().size() == 0) {
        u_token(req->userRequestDescription);
    }

    // Storage System Info

    // File Storage Type
    if (NULL != req->desiredFileStorageType) {
        switch (*req->desiredFileStorageType) {
        case VOLATILE:
            _f_type = DB_FILE_TYPE_VOLATILE;
            break;
        case DURABLE:
            _f_type = DB_FILE_TYPE_DURABLE;
            break;
        case PERMANENT:
            _f_type = DB_FILE_TYPE_PERMANENT;
            break;
        default:
            throw std::string("Invalid desiredFileStorageType");
        }
    }

    // Total Request Time

    // deferredStartTime
    if (req->deferredStartTime != NULL) {
        _deferredStartTime = *(req->deferredStartTime);
    }

    // Pin Lifetime
    if (NULL != req->desiredPinLifeTime) {
        _pinLifetime = *req->desiredPinLifeTime;
    }

    // Space Token
    if (NULL != req->targetSpaceToken) {
        _s_token = req->targetSpaceToken;
    }

    // Retention policy

    // Transfer Parameters
    if (NULL != req->transferParameters) {
        // Access Pattern
        // Connection Type
        // Client Networks
        // Transfer Protocols
        if (NULL != req->transferParameters->arrayOfTransferProtocols) {
            for (int i = 0; i < req->transferParameters->arrayOfTransferProtocols->__sizestringArray; ++i) {
                _protocols.push_back(req->transferParameters->arrayOfTransferProtocols->stringArray[i]);
            }
        }
    }
}

ns1__srmBringOnlineResponse* bol::response() {

    // soap struct status
    if (NULL == _response) {
        _response = storm::soap_calloc<ns1__srmBringOnlineResponse>(_soap);
    }

    if (NULL == _response->returnStatus) {
        _response->returnStatus = storm::soap_calloc<ns1__TReturnStatus>(_soap);
    }

    _response->returnStatus->statusCode = status();
    if (NULL == _response->returnStatus->explanation) {
        _response->returnStatus->explanation = soap_strdup(_soap, _explanation.c_str());
    } else {
        // how to free() memory allocated with soap_strdup???
        snprintf(_response->returnStatus->explanation, strlen(_response->returnStatus->explanation),
                _explanation.c_str());
    }

    // Fill per-surl info.
    try {
        if (NULL == _response->arrayOfFileStatuses)
            _response->arrayOfFileStatuses = storm::soap_calloc<ns1__ArrayOfTBringOnlineRequestFileStatus>(
                    _soap);

        if (NULL == _response->arrayOfFileStatuses->statusArray)
            _response->arrayOfFileStatuses->statusArray = storm::soap_calloc<
                    ns1__TBringOnlineRequestFileStatus>(_soap, _surls.size());
        _response->arrayOfFileStatuses->__sizestatusArray = _surls.size();

        int n = 0;
        for (std::vector<bol::surl_t>::const_iterator i = _surls.begin(); i != _surls.end(); ++i, ++n) {
            if (NULL == _response->arrayOfFileStatuses->statusArray[n])
                _response->arrayOfFileStatuses->statusArray[n] = storm::soap_calloc<
                        ns1__TBringOnlineRequestFileStatus>(_soap);
            if (NULL == _response->arrayOfFileStatuses->statusArray[n]->status)
                _response->arrayOfFileStatuses->statusArray[n]->status = storm::soap_calloc<
                        ns1__TReturnStatus>(_soap);
            _response->arrayOfFileStatuses->statusArray[n]->sourceSURL
                    = soap_strdup(_soap, i->source.c_str());
            _response->arrayOfFileStatuses->statusArray[n]->status->statusCode = i->status;
            _response->arrayOfFileStatuses->statusArray[n]->status->explanation = soap_strdup(_soap,
                    i->explanation.c_str());
        }
    } catch (std::invalid_argument x) {
        // continuing???
    }

    // Fill request token
    if (NULL == _response->requestToken) {
        if (_r_token.size() > 0)
            _response->requestToken = soap_strdup(_soap, r_token().c_str());
    } else
        snprintf(_response->requestToken, strlen(_response->requestToken), r_token().c_str());
    return _response;
}

void bol::insert(struct srm_dbfd *db) {
    _db = db;
    std::string nullcomma("NULL, ");
    std::ostringstream query_s;
    std::string q("INSERT INTO request_queue ("
        "  config_FileStorageTypeID"
        ", config_RequestTypeID"
        ", client_dn"
        ", pinLifetime"
        ", fileLifetime"
        ", r_token"
        ", s_token"
        ", status"
        ", nbreqfiles"
        ", numOfCompleted"
        ", numOfWaiting"
        ", numOfFailed"
        ", proxy"
        ", timeStamp) values (");
    query_s << q;
    if (_f_type == DB_FILE_TYPE_UNKNOWN)
        query_s << nullcomma;
    else
        query_s << "'" << _f_type << "', ";

    query_s << "'" << _r_type << "', ";
    query_s << "'" << getClientDN() << "', ";

    if (_pinLifetime == -1)
        query_s << nullcomma;
    else
        query_s << _pinLifetime << ", ";

    if (_lifetime == -1)
        query_s << nullcomma;
    else
        query_s << _lifetime << ", ";

    if (r_token().size() == 0)
        throw std::string("Request token empty");
    else {
        query_s << "'" << r_token() << "', ";
    }

    if (s_token().size() == 0)
        query_s << "NULL, ";
    else
        query_s << "'" << s_token() << "', ";

    query_s << status() << ", " << _n_files;
    query_s << ", 0, " << _n_files << ",0 , ";

    // Temporary hack: using the proxy column to store FQANs
    sql_string fqansOneString = _credentials.getFQANsOneString();
    if (fqansOneString.empty()) {
        query_s << "NULL, ";
    } else {
        query_s << "'" << fqansOneString << "', ";
    }

    query_s << "current_timestamp() )";

    storm_start_tr(0, _db);
    // Insert into request_queue
    int request_id;
    try {
        request_id = storm_db::ID_exec_query(_db, query_s.str());
    } catch (int e) {
        _response_errno = e;
        _response_error = "DB error inserting into request_queue a BOL request";
        _response_error += ". Errno: ";
        _response_error += e;
        storm_abort_tr( _db);
        throw _response_error;
    }
    // Insert into request_Bol using the requestID
    for (std::vector<bol::surl_t>::const_iterator i = _surls.begin(); i != _surls.end(); ++i) {
        // DirOption
        int diroption_id;
        if (i->has_diroption) {
            std::ostringstream query_d;
            query_d
                    << "INSERT INTO request_DirOption (isSourceADirectory, allLevelRecursive, numOfLevels) values (";
            if (false == i->isdirectory)
                query_d << "0, ";
            else
                query_d << "1, ";

            if (i->allrecursive == -1)
                query_d << "NULL, ";
            else
                query_d << "1, ";

            if (i->n_levels != -1)
                query_d << i->n_levels << ")";
            else
                query_d << "NULL )";
            set_savepoint(_db, "BOLFILE");
            try {
                diroption_id = storm_db::ID_exec_query(_db, query_d.str());
            } catch (int e) {

                _response_errno = 0;
                _response_error = "DB error inserting DirOption for surl " + i->source;
                _response_error += "Error = ";
                _response_error += e;
                rollback_to_savepoint(_db, "BOLFILE");
                continue;
            }
        }

        std::ostringstream query_s;
        query_s << "INSERT INTO request_BoL (sourceSURL, request_queueID, request_DirOptionID) VALUES ";
        query_s << "('" << i->source << "', ";
        query_s << request_id << ", ";
        if (i->has_diroption)
            query_s << diroption_id << ")";
        else
            query_s << "NULL )";
        int bol_id;
        set_savepoint(_db, "BOLFILE");
        try {
            bol_id = storm_db::ID_exec_query(_db, query_s.str());
        } catch (int e) {
            // Qua impostiamo l'errore per il surl e lo dobbiamo dedurre dal tipo di errore di mysql.
            // Poi dobbiamo impostare la stringa di errore.
            // Infine aggiorniamo _n_failed.
            // Dobbiamo fare un continue, sempre che funzioni, e continuare con le altre surl.

            _response_errno = e;
            _response_error = "DB error inserting surl " + i->source;
            _response_error += " into request_BoL. Errno: " + e;
            rollback_to_savepoint(_db, "BOLFILE");
            ++_n_failed;
            continue;
        }

        // Insert into status_BoL using the request_GetID
        std::ostringstream query2_s;
        query2_s << "INSERT INTO status_BoL (request_BoLID, statusCode) values (";
        query2_s << bol_id << ", " << SRM_USCOREREQUEST_USCOREQUEUED << ")";
        try {
            storm_db::ID_exec_query(_db, query2_s.str());
        } catch (int e) {
            _response_errno = e;
            _response_error = "DB error inserting into status_BoL";
            _response_error += ". Errno: " + e;
            rollback_to_savepoint(_db, "BOLFILE");
            ++_n_failed;
            continue;
        }
    }
    // Check the nr of successfully inserted qery.

    // Check the number of correctly inserted protocols.

    storm_end_tr( _db);
    // insert into retention policy, clientNetworks, extrainfo, VOMS
    // attribute using the requestID

}
