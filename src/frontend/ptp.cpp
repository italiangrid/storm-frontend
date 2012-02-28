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
#include <sstream>

#include "ptp.hpp"
#include "mysql_query.hpp"
#include "storm_mysql.h"
#include "srmlogit.h"

bool storm::ptp::supportsProtocolSpecification()
{
	return true;
}

std::vector<sql_string>* storm::ptp::getRequestedProtocols()
{
	return &(this->protocols);
}

void storm::ptp::setProtocolVector(std::vector<sql_string>* protocolVector)
{
	this->protocols = *protocolVector;
}

void storm::ptp::setGenericFailureSurls()
{
    srmlogit(STORM_LOG_DEBUG, "ptp::setGenericFailureSurls()", "Setting the status of all requested SURLs to SRM_FAILURE\n");
    for (int i = 0; i < surls.size(); i++) {
        surls.at(i).setStatus(SRM_USCOREFAILURE);
    }
}

void storm::ptp::load(ns1__srmPrepareToPutRequest *req) {
    if (NULL == req)
        throw invalid_request("Request is NULL");
    if (NULL == req->arrayOfFileRequests)
        throw invalid_request("SURLs array is NULL");
    if (0 == req->arrayOfFileRequests->__sizerequestArray)
        throw invalid_request("SURLs array is empty");

    // Initial values
    _retrytime = -1; // -1 = not specified
    _pinLifetime = -1;
    _remainingTime = -1;
    _lifetime = -1;
    _overwrite = DB_OVERWRITE_UNKNOWN;
    _f_type = DB_FILE_TYPE_UNKNOWN;

    // Request type
    _r_type = DB_PUT_REQUEST;

    // Fill surl
    for (int i = 0; i < req->arrayOfFileRequests->__sizerequestArray; ++i) {
        if (NULL != req->arrayOfFileRequests->requestArray[i]->expectedFileSize) {
            surls.push_back(storm::PtpSurl(req->arrayOfFileRequests->requestArray[i]->targetSURL,
                    *req->arrayOfFileRequests->requestArray[i]->expectedFileSize));
            srmlogit(STORM_LOG_DEBUG, "ptp::load()",
                                "Debug: expectedFileSize!=NULL, value= %s\n",
                                req->arrayOfFileRequests->requestArray[i]->expectedFileSize);
        }
        else
        {
            surls.push_back(storm::PtpSurl(req->arrayOfFileRequests->requestArray[i]->targetSURL));
        }
    }
    _n_files = surls.size();
    // Status
    status(SRM_USCOREREQUEST_USCOREQUEUED);

    // Client DN
    if (getClientDN().size() == 0 && NULL != req->authorizationID) {
        setClientDN(req->authorizationID);
    }

    // User Token
    if (NULL != req->userRequestDescription && u_token().size() == 0)
        u_token(req->userRequestDescription);

    // Request Token will be not written

    // File Storage Type
    if (NULL != req->desiredFileStorageType)
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

    // Overwrite Option
    if (NULL != req->overwriteOption) {
        switch (*req->overwriteOption) {
        case NEVER:
            _overwrite = DB_OVERWRITE_NEVER;
            //overwrite(DB_OVERWRITE_NEVER);
            break;
        case ALWAYS:
            _overwrite = DB_OVERWRITE_ALWAYS;
            //            overwrite(DB_OVERWRITE_ALWAYS);
            break;
        case WHEN_USCOREFILES_USCOREARE_USCOREDIFFERENT:
            _overwrite = DB_OVERWRITE_IF_DIFFERENT;
            //            overwrite(DB_OVERWRITE_IF_DIFFERENT);
            break;
        default:
            throw std::string("Invalid overwriteOption");
        }
    }
    // Total Request Time

    // Pin Lifetime
    if (NULL != req->desiredPinLifeTime)
        _pinLifetime = *req->desiredPinLifeTime;

    // File Lifetime
    if (NULL != req->desiredFileLifeTime)
        _lifetime = *req->desiredFileLifeTime;

    // Transfer Parameters
    if (NULL != req->transferParameters) {
        // Access Pattern

        // Connection Type

        // Client Networks

        // Transfer Protocols
        if (NULL != req->transferParameters->arrayOfTransferProtocols) {
            for (int i = 0; i
                    < req->transferParameters->arrayOfTransferProtocols->__sizestringArray; ++i)
                protocols.push_back(
                        req->transferParameters->arrayOfTransferProtocols->stringArray[i]);
        }

    }
    // Space Token
    if (NULL != req->targetSpaceToken)
        _s_token = req->targetSpaceToken;

    // retention policy

    // Storage System Info
}

struct ns1__srmPrepareToPutResponse *
storm::ptp::response() {
    // soap struct status
    if (NULL == this->builtResponse)
    	this->builtResponse = storm::soap_calloc<struct ns1__srmPrepareToPutResponse>(_soap);

    if (NULL == this->builtResponse->returnStatus)
    	this->builtResponse->returnStatus = storm::soap_calloc<ns1__TReturnStatus>(_soap);

    this->builtResponse->returnStatus->statusCode = status();
    if (NULL == this->builtResponse->returnStatus->explanation)
    	this->builtResponse->returnStatus->explanation = soap_strdup(_soap, _explanation.c_str());
    else
        // how to free() memory allocated with soap_strdup???
        snprintf(this->builtResponse->returnStatus->explanation,
                strlen(this->builtResponse->returnStatus->explanation), _explanation.c_str());

    // Fill per-surl info.
    try {
        if (NULL == this->builtResponse->arrayOfFileStatuses)
        	this->builtResponse->arrayOfFileStatuses = storm::soap_calloc<ns1__ArrayOfTPutRequestFileStatus>(
                    _soap);

        this->builtResponse->arrayOfFileStatuses->statusArray
                = storm::soap_calloc<ns1__TPutRequestFileStatus>(_soap, surls.size());
        this->builtResponse->arrayOfFileStatuses->__sizestatusArray = surls.size();

        int n = 0;
        for (std::vector<storm::PtpSurl>::const_iterator i = surls.begin(); i != surls.end(); ++i, ++n) {

            if (NULL == this->builtResponse->arrayOfFileStatuses->statusArray[n])
            	this->builtResponse->arrayOfFileStatuses->statusArray[n] = storm::soap_calloc<
                        ns1__TPutRequestFileStatus>(_soap);
            this->builtResponse->arrayOfFileStatuses->statusArray[n]->SURL = soap_strdup(_soap,
            		((storm::PtpSurl)*i).getSurl().c_str());

            if (NULL == this->builtResponse->arrayOfFileStatuses->statusArray[n]->status)
            	this->builtResponse->arrayOfFileStatuses->statusArray[n]->status = storm::soap_calloc<
                        ns1__TReturnStatus>(_soap);
            this->builtResponse->arrayOfFileStatuses->statusArray[n]->status->statusCode = ((storm::PtpSurl)*i).getStatus();
            this->builtResponse->arrayOfFileStatuses->statusArray[n]->status->explanation = soap_strdup(
                    _soap, ((storm::PtpSurl)*i).getExplanation().c_str());
        }
    } catch (std::invalid_argument x) {
        // continuing???
    }
    // Fill request token
    if (NULL == this->builtResponse->requestToken) {
        if (_r_token.size() > 0)
        	this->builtResponse->requestToken = soap_strdup(_soap, r_token().c_str());
    } else
        snprintf(this->builtResponse->requestToken, strlen(this->builtResponse->requestToken), r_token().c_str());
    return this->builtResponse;
}

void storm::ptp::insert(struct srm_dbfd *db) {
    _db = db;
    std::string nullcomma("NULL, ");
    std::ostringstream query_s;
    std::string q("INSERT INTO request_queue ("
        "  config_FileStorageTypeID"
        ", config_OverwriteID"
        ", config_RequestTypeID"
        ", client_dn"
        ", u_token"
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

    if (_f_type == DB_FILE_TYPE_UNKNOWN) {
        query_s << nullcomma;
    } else {
        query_s << "'" << _f_type << "', ";
    }

    if (overwrite() == DB_OVERWRITE_UNKNOWN) {
        query_s << nullcomma;
    } else {
        query_s << "'" << overwrite() << "', ";
    }

    query_s << "'" << _r_type << "', ";
    query_s << "'" << getClientDN() << "', ";

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

    if (_lifetime == -1) {
        query_s << nullcomma;
    } else {
        query_s << _lifetime << ", ";
    }

    if (r_token().size() == 0) {
        throw std::string("Request token empty");
    } else {
        query_s << "'" << r_token() << "', ";
    }

    if (s_token().size() == 0) {
        query_s << "NULL, ";
    } else {
        query_s << "'" << s_token() << "', ";
    }

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

    // Start DB transaction
    storm_start_tr(0, _db);

    // Insert into request_queue
    int request_id;
    try {
        request_id = storm_db::ID_exec_query(_db, query_s.str());
    } catch (storm_db::mysql_exception& e) {
        storm_abort_tr(_db);
        set_surl_status(SRM_USCOREFAILURE);
        throw e;
    }

    // Insert into request_TransferProtocols using the request_ID
    int inserted = 0;
    for (std::vector<sql_string>::const_iterator i = protocols.begin(); i != protocols.end(); ++i) {
        std::ostringstream query_s;
        query_s << "INSERT into request_TransferProtocols "
            "(request_queueID, config_protocolsID) values (";
        query_s << request_id << ", '" << *i << "')";

        try {
            storm_db::ID_exec_query(_db, query_s.str());
        } catch (storm_db::mysql_exception e) {
            srmlogit(STORM_LOG_ERROR, "ptp::insert()",
                    "Error: requested protocol \"%s\" is not supported (mysql error: %s)\n",
                    i->c_str(), e.what());
            continue;
        }
        inserted++;
    }

    if (0 == inserted && protocols.size() != 0) { // All requested protocols are not supported
        storm_abort_tr(_db);
        // Request status
        status(SRM_USCORENOT_USCORESUPPORTED);
        set_surl_status(SRM_USCOREFAILURE);

        throw storm::not_supported("None of the requested transfer protocols is supported");
    }


    // Insert into request_Put using the requestID
    for (std::vector<storm::PtpSurl>::const_iterator i = surls.begin(); i != surls.end(); ++i) {
        std::ostringstream query_s;
        query_s << "INSERT INTO request_Put (targetSURL, expectedFileSize, request_queueID) VALUES ";
        query_s << "('" << ((storm::PtpSurl)*i).getSurl() << "', ";
        if (((storm::PtpSurl)*i).hasExpected_size())
            query_s << ((storm::PtpSurl)*i).getExpected_size();
        else
            query_s << "NULL";
        query_s << ", " << request_id << ")";
        int put_id;
        set_savepoint(_db, "PUTFILE");
        try {
            put_id = storm_db::ID_exec_query(_db, query_s.str());
        } catch (storm_db::mysql_exception e) {
            // Qua impostiamo l'errore per il surl e lo dobbiamo dedurre dal tipo di errore di mysql.
            // Poi dobbiamo impostare la stringa di errore.
            // Infine aggiorniamo _n_failed.
            // Dobbiamo fare un continue, sempre che funzioni, e continuare con le altre surl.

            srmlogit(STORM_LOG_ERROR, "ptp::insert()",
                    "Error %s inserting surl %s into request_Put. Continuing\n", e.what(),
                    ((storm::PtpSurl)*i).getSurl().c_str());
            rollback_to_savepoint(_db, "PUTFILE");
            ++_n_failed;
            continue;
        }

        // Insert into status_Put using the request_PutID
        std::ostringstream query2_s;
        query2_s << "INSERT INTO status_Put (request_PutID, statusCode) values (";
        query2_s << put_id << ", " << SRM_USCOREREQUEST_USCOREQUEUED << ")";
        try {
            storm_db::ID_exec_query(_db, query2_s.str());
        } catch (storm_db::mysql_exception e) {
            srmlogit(STORM_LOG_ERROR, "ptp::insert()",
                    "Error %s inserting surl %s into status_Put. Continuing\n", e.what(),
                    ((storm::PtpSurl)*i).getSurl().c_str());
            rollback_to_savepoint(_db, "PUTFILE");
            ++_n_failed;
            continue;
        }
    }

    storm_end_tr(_db);
    // insert into retention policy, clientNetworks, extrainfo, VOMS
    // attribute using the requestID

}

void storm::ptp::set_surl_status(ns1__TStatusCode status) {
    for (int i = 0; i < surls.size(); i++) {
        surls.at(i).setStatus(SRM_USCOREFAILURE);
    }
}
