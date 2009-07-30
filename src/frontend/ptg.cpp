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

#include "ptg.hpp"
#include "mysql_query.hpp"
#include "storm_mysql.h"

using namespace storm;

void ptg::load(ns1__srmPrepareToGetRequest *req)
{
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
    _overwrite = DB_OVERWRITE_UNKNOWN;
    _f_type = DB_FILE_TYPE_UNKNOWN;

    // Request type
    _r_type = DB_GET_REQUEST;
    
    // Request status
    status(SRM_USCOREREQUEST_USCOREQUEUED);

    // Client DN
    if (getClientDN().size() == 0 && NULL != req->authorizationID) {
        setClientDN(req->authorizationID);
    }
    
    // Request Token is not being set now

    // Fill surl
    for (int i=0; i<req->arrayOfFileRequests->__sizerequestArray; ++i) {
        ns1__TGetFileRequest* reqSURL = req->arrayOfFileRequests->requestArray[i];
        _surls.push_back(surl_t(reqSURL->sourceSURL, reqSURL->dirOption));
    }
    
    _n_files = _surls.size();
    
    // User description
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
            for (int i=0; i<req->transferParameters->arrayOfTransferProtocols->__sizestringArray; ++i) {
                _protocols.push_back(req->transferParameters->arrayOfTransferProtocols->stringArray[i]);
            }
        }
    }
}


struct ns1__srmPrepareToGetResponse* ptg::response() {
    // soap struct status
    if (NULL == _response) {
        _response = storm::soap_calloc<struct ns1__srmPrepareToGetResponse>(_soap);
    }

    if (NULL == _response->returnStatus) {
        _response->returnStatus = storm::soap_calloc<ns1__TReturnStatus>(_soap);
    }

    _response->returnStatus->statusCode = status();
    if ( NULL == _response->returnStatus->explanation ) {
        _response->returnStatus->explanation = soap_strdup(_soap, _explanation.c_str());
    } else { // how to free() memory allocated with soap_strdup???
        snprintf(_response->returnStatus->explanation,
                 strlen(_response->returnStatus->explanation),
                 _explanation.c_str());
    }

    // Fill arrayOfFileStatuses
    try {
        if (NULL == _response->arrayOfFileStatuses) {
            _response->arrayOfFileStatuses = storm::soap_calloc<ns1__ArrayOfTGetRequestFileStatus>(_soap);
        }

        _response->arrayOfFileStatuses->statusArray = storm::soap_calloc<ns1__TGetRequestFileStatus>(_soap, _surls.size());
        _response->arrayOfFileStatuses->__sizestatusArray = _surls.size();

        int n = 0;
        for (std::vector<ptg::surl_t>::const_iterator i = _surls.begin();
                i != _surls.end();
                ++i, ++n)
        {
            if (NULL == _response->arrayOfFileStatuses->statusArray[n]) {
                _response->arrayOfFileStatuses->statusArray[n] =
                    storm::soap_calloc<ns1__TGetRequestFileStatus>(_soap);
            }
            ns1__TGetRequestFileStatus* fileStatus = _response->arrayOfFileStatuses->statusArray[n];
            
            fileStatus->sourceSURL = soap_strdup(_soap, i->sourceSURL.c_str());

            if (NULL == fileStatus->status) {
                fileStatus->status = storm::soap_calloc<ns1__TReturnStatus>(_soap);
            }
            fileStatus->status->statusCode = i->status;
            fileStatus->status->explanation = soap_strdup(_soap, i->explanation.c_str());
        }
    } catch (std::invalid_argument x) {
        // continuing???
    }
    
    // Fill request token
    if (NULL == _response->requestToken) {
        if (_r_token.size() > 0) {
            _response->requestToken = soap_strdup(_soap, r_token().c_str());
        }
    } else {
        snprintf(_response->requestToken, strlen(_response->requestToken), r_token().c_str());
    }
    
    return _response;
}

void ptg::insert(struct srm_dbfd *db) {
    _db = db;
    std::string nullcomma("NULL, ");
    std::ostringstream query_s;
    std::string q("INSERT INTO request_queue ("
                  "  config_FileStorageTypeID"
                  ", config_RequestTypeID"
                  ", client_dn"
                  ", u_token"
                  ", pinLifetime"
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
    
    if (_f_type == DB_FILE_TYPE_UNKNOWN) {
        query_s << nullcomma;
    } else {
        query_s << "'" << _f_type << "', ";
    }

    query_s << "'"<< _r_type << "', ";
    query_s << "'"<< getClientDN() << "', ";

    if (u_token().empty()) {
        query_s << nullcomma;
    } else {
        query_s << "'" << u_token() << "', ";
    }

    if (_pinLifetime == -1) {
        query_s << nullcomma;
    } else {
        query_s << _pinLifetime << ", ";
    }

    if (r_token().size() == 0) {
        throw std::string("Request token empty");
    } else {
        query_s << "'" << r_token() <<"', ";
    }
    
    if (s_token().size() == 0) {
        query_s << "NULL, ";
    } else {
        query_s << "'"<< s_token() <<"', ";
    }

    query_s << status() << ", "<< _n_files;
    query_s << ", 0, "<< _n_files << ",0 , ";
    
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
    } catch (storm_db::mysql_exception e) {
        storm_abort_tr(_db);
        throw e.what();
    }
    
    
    // Insert into request_Get using the requestID
    for (std::vector<ptg::surl_t>::const_iterator i = _surls.begin();
            i != _surls.end();
            ++i)
    {
        int dirOptionId = -1;
        
        set_savepoint(_db, "GETFILE");
        if (i->has_diroption) { // Insert into request_DirOption
            query_s.str("");
            query_s << "INSERT INTO request_DirOption (isSourceADirectory, allLevelRecursive, numOfLevels) VALUES (";

            query_s << sql_format(i->isdirectory);

            if (i->allLevelRecursive == -1) {
                query_s << nullcomma;
            } else {
                query_s << i->allLevelRecursive << ", ";
            }
            if (i->n_levels == -1) {
                query_s << "NULL)";
            } else {
                query_s << i->n_levels << ")";
            }

            try {
                dirOptionId = storm_db::ID_exec_query(_db, query_s.str());
            } catch (storm_db::mysql_exception e) {
                srmlogit(STORM_LOG_ERROR, "ptg::insert()",
                         "Error %s inserting surl %s into request_Get. Continuing\n",
                         e.what(), i->sourceSURL.c_str());
                rollback_to_savepoint(_db, "GETFILE");
                ++_n_failed;
                continue;
            }
        }
        query_s.str("");
        query_s << "INSERT INTO request_Get (sourceSURL, request_DirOptionID, request_queueID) VALUES ('";
        query_s << i->sourceSURL << "', ";
        if (dirOptionId == -1) {
            query_s << nullcomma;
        } else {
            query_s << dirOptionId << ", ";
        }
        query_s << request_id << ")";
        int getId;
        try {
            getId = storm_db::ID_exec_query(_db, query_s.str());
        } catch (storm_db::mysql_exception e) {
            // Qua impostiamo l'errore per il surl e lo dobbiamo dedurre dal tipo di errore di mysql.
            // Poi dobbiamo impostare la stringa di errore.
            // Infine aggiorniamo _n_failed.
            // Dobbiamo fare un continue, sempre che funzioni, e continuare con le altre surl.

            srmlogit(STORM_LOG_ERROR, "ptg::insert()",
                     "Error %s inserting surl %s into request_Get. Continuing\n",
                     e.what(), i->sourceSURL.c_str());
            rollback_to_savepoint(_db, "GETFILE");
            ++_n_failed;
            continue;
        }

        // Insert into status_Get using the getId
        query_s.str("");
        query_s << "INSERT INTO status_Get (request_GetID, statusCode) values ('";
        query_s << getId << "', " << SRM_USCOREREQUEST_USCOREQUEUED <<")";
        try {
            storm_db::ID_exec_query(_db, query_s.str());
        } catch (storm_db::mysql_exception e) {
            srmlogit(STORM_LOG_ERROR, "ptg::insert()",
                     "Error %s inserting surl %s into status_Get. Continuing\n",
                     e.what(), i->sourceSURL.c_str());
            rollback_to_savepoint(_db, "GETFILE");
            ++_n_failed;
            continue;
        }
    }
    // Check the nr of successfully inserted qery.

    // Insert into request_TransferProtocols using the request_ID
    for (std::vector<sql_string>::const_iterator i = _protocols.begin();
            i != _protocols.end();
            ++i)
    { // separati insert, nel caso che uno solo fallisca.
        query_s.str("");
        query_s << "INSERT into request_TransferProtocols (request_queueID, config_protocolsID) values (";
        query_s << request_id << ", '" << *i << "')";
        try {
            storm_db::ID_exec_query(_db, query_s.str());
        } catch (storm_db::mysql_exception e) {
            srmlogit(STORM_LOG_ERROR, "ptg::insert()",
                     "Error %s inserting transfer protocol %s into DB. Continuing\n",
                     e.what(), i->c_str());
            continue;
        }
    }

    // Check the number of correctly inserted protocols.

    storm_end_tr(_db);
    // insert into retention policy, clientNetworks, extrainfo, VOMS
    // attribute using the requestID
}
