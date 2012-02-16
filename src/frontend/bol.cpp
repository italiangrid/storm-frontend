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

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <sstream>

#include "bol.hpp"
#include "mysql_query.hpp"
#include "storm_mysql.h"

using namespace storm;

bool bol::supportsProtocolSpecification()
{
	return true;
}

std::vector<sql_string>* bol::getRequestedProtocols()
{
	return &(this->_protocols);
}

void bol::setProtocolVector(std::vector<sql_string>* protocolVector)
{
	this->_protocols = *protocolVector;
}

void bol::setGenericFailureSurls()
{
	srmlogit(STORM_LOG_DEBUG, "bol::setGenericFailureSurls()", "Setting the status of all requested SURLs to SRM_FAILURE\n");
    for (int i = 0; i < _surls.size(); i++) {
        _surls.at(i).status = SRM_USCOREFAILURE;
    }
}

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
    if (NULL != req->desiredLifeTime) {
        _pinLifetime = *req->desiredLifeTime;
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
    _response = storm::soap_calloc<ns1__srmBringOnlineResponse>(_soap);
    _response->returnStatus = storm::soap_calloc<ns1__TReturnStatus>(_soap);

    _response->returnStatus->statusCode = status();

    if (_explanation.c_str() != NULL) {
        _response->returnStatus->explanation = soap_strdup(_soap, _explanation.c_str());
    }

    // Fill per-surl info.
    try {
        _response->arrayOfFileStatuses = storm::soap_calloc<ns1__ArrayOfTBringOnlineRequestFileStatus>(_soap);
        _response->arrayOfFileStatuses->statusArray = storm::soap_calloc<ns1__TBringOnlineRequestFileStatus>(
                _soap, _surls.size());

        _response->arrayOfFileStatuses->__sizestatusArray = _surls.size();

        int n = 0;
        for (std::vector<bol::surl_t>::const_iterator i = _surls.begin(); i != _surls.end(); ++i, ++n) {

            struct ns1__TBringOnlineRequestFileStatus *statusArray = storm::soap_calloc<
                    ns1__TBringOnlineRequestFileStatus>(_soap);
            _response->arrayOfFileStatuses->statusArray[n] = statusArray;

            statusArray->sourceSURL = soap_strdup(_soap, i->sourceSURL.c_str());
            statusArray->fileSize = NULL;
            statusArray->estimatedWaitTime = NULL;
            statusArray->remainingPinTime = NULL;

            statusArray->status = storm::soap_calloc<ns1__TReturnStatus>(_soap);
            statusArray->status->statusCode = i->status;
            statusArray->status->explanation = soap_strdup(_soap, i->explanation.c_str());
        }
    } catch (std::invalid_argument x) {
        // continuing???
    }

    // Fill request token
    if (_r_token.size() > 0) {
        _response->requestToken = soap_strdup(_soap, r_token().c_str());
    }

    _response->remainingTotalRequestTime = NULL;
    _response->remainingDeferredStartTime = NULL;

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
        ", deferredStartTime"
        ", timeStamp) values (");
    query_s << q;

    if (_f_type == DB_FILE_TYPE_UNKNOWN) {
        query_s << nullcomma;
    } else {
        query_s << sql_format(_f_type) << ", ";
    }

    query_s << sql_format(_r_type) << ", ";
    query_s << sql_format(getClientDN()) << ", ";

    if (_pinLifetime == -1) {
        query_s << nullcomma;
    } else {
        query_s << _pinLifetime << ", ";
    }
    if (_lifetime == -1) {
        query_s << nullcomma;
    } else {
        query_s << _lifetime << ", ";
    }

    if (r_token().size() == 0) {
        throw std::string("Request token empty");
    } else {
        query_s << sql_format(r_token()) << ", ";
    }

    if (s_token().size() == 0) {
        query_s << nullcomma;
    } else {
        query_s << sql_format(s_token()) << ", ";
    }

    query_s << status() << ", " << _n_files << ", 0, " << _n_files << ",0 , ";

    // Temporary hack: using the proxy column to store FQANs
    sql_string fqansOneString = _credentials.getFQANsOneString();
    if (fqansOneString.empty()) {
        query_s << nullcomma;
    } else {
        query_s << "'" << fqansOneString << "', ";
    }

    // deferredStartTime
    query_s << _deferredStartTime << ", ";

    query_s << "current_timestamp() )";

    storm_start_tr(0, _db);

    // Insert into request_queue
    int request_id;
    try {
        request_id = storm_db::ID_exec_query(_db, query_s.str());
    } catch (storm_db::mysql_exception e) {
        storm_abort_tr( _db);
        throw e;
    }

    // Insert into request_Bol using the requestID
    for (std::vector<bol::surl_t>::const_iterator i = _surls.begin(); i != _surls.end(); ++i) {
        // DirOption
        int diroption_id;
        if (i->has_diroption) {
            std::ostringstream query_d;
            query_d
                    << "INSERT INTO request_DirOption (isSourceADirectory, allLevelRecursive, numOfLevels) values (";

            query_d << sql_format(i->isdirectory) << ", ";

            if (i->allLevelRecursive == -1) {
                query_d << "NULL, ";
            } else {
                query_d << "1, ";
            }

            if (i->n_levels != -1) {
                query_d << i->n_levels << ")";
            } else {
                query_d << "NULL )";
            }

            set_savepoint(_db, "BOLFILE");

            try {
                diroption_id = storm_db::ID_exec_query(_db, query_d.str());
            } catch (storm_db::mysql_exception e) {

                srmlogit(STORM_LOG_ERROR, "bol::insert()",
                        "Error %s inserting surl %s into request_DirOption. Continuing\n", e.what(),
                        i->sourceSURL.c_str());
                rollback_to_savepoint(_db, "BOLFILE");
                ++_n_failed;
                continue;
            }
        }

        std::ostringstream query1_s;

        query1_s << "INSERT INTO request_BoL (sourceSURL, request_queueID, request_DirOptionID) VALUES (";
        query1_s << sql_format(i->sourceSURL) << ", ";
        query1_s << request_id << ", ";

        if (i->has_diroption) {
            query1_s << diroption_id << ")";
        } else {
            query1_s << "NULL )";
        }

        int bol_id;
        set_savepoint(_db, "BOLFILE");
        try {
            bol_id = storm_db::ID_exec_query(_db, query1_s.str());
        } catch (storm_db::mysql_exception e) {
            // Qua impostiamo l'errore per il surl e lo dobbiamo dedurre dal tipo di errore di mysql.
            // Poi dobbiamo impostare la stringa di errore.
            // Infine aggiorniamo _n_failed.
            // Dobbiamo fare un continue, sempre che funzioni, e continuare con le altre surl.
            srmlogit(STORM_LOG_ERROR, "bol::insert()",
                    "Error %s inserting surl %s into request_BoL. Continuing\n", e.what(),
                    i->sourceSURL.c_str());
            rollback_to_savepoint(_db, "BOLFILE");
            ++_n_failed;
            continue;
        }

        // Insert into status_BoL using the request_BoLID
        std::ostringstream query2_s;
        query2_s << "INSERT INTO status_BoL (request_BoLID, statusCode) values (";
        query2_s << bol_id << ", " << SRM_USCOREREQUEST_USCOREQUEUED << ")";

        try {
            storm_db::ID_exec_query(_db, query2_s.str());
        } catch (storm_db::mysql_exception e) {
            srmlogit(STORM_LOG_ERROR, "bol::insert()",
                    "Error %s inserting surl %s into status_BoL. Continuing\n", e.what(),
                    i->sourceSURL.c_str());
            rollback_to_savepoint(_db, "BOLFILE");
            ++_n_failed;
            continue;
        }
    }

    // Insert into request_TransferProtocols using the request_ID
    for (std::vector<sql_string>::const_iterator i = _protocols.begin(); i != _protocols.end(); ++i) { // separati insert, nel caso che uno solo fallisca.
        query_s.str("");
        query_s << "INSERT INTO request_TransferProtocols (request_queueID, config_protocolsID) VALUES (";
        query_s << request_id << ", '" << *i << "')";

        try {
            storm_db::ID_exec_query(_db, query_s.str());
        } catch (storm_db::mysql_exception e) {
            srmlogit(STORM_LOG_ERROR, "bol::insert()",
                    "Error %s inserting transfer protocol %s into DB. Continuing\n", e.what(), i->c_str());
            continue;
        }
    }

    // Check the number of correctly inserted protocols.

    storm_end_tr( _db);
    // insert into retention policy, clientNetworks, extrainfo, VOMS
    // attribute using the requestID

}
