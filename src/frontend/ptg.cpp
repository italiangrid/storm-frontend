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

#include <sstream>

#include "srmlogit.h"
#include "soap_util.hpp"
#include "mysql_query.hpp"
#include "storm_mysql.h"

#include "ptg.hpp"

bool storm::ptg::supportsProtocolSpecification()
{
	return true;
}

std::vector<sql_string>* storm::ptg::getRequestedProtocols()
{
	return &(this->protocols);
}

void storm::ptg::setProtocolVector(std::vector<sql_string>* protocolVector)
{
	this->protocols = *protocolVector;
}

void storm::ptg::setGenericFailureSurls()
{
	srmlogit(STORM_LOG_DEBUG, "ptg::setGenericFailureSurls()", "Setting the status of all requested SURLs to SRM_FAILURE\n");
    for (int i = 0; i < surls.size(); i++) {
        surls.at(i).setStatus(SRM_USCOREFAILURE);
    }
}

void storm::ptg::load(ns1__srmPrepareToGetRequest *req)
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
        surls.push_back(storm::PtgSurl(reqSURL->sourceSURL, reqSURL->dirOption));
    }
    
    _n_files = surls.size();
    
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
                protocols.push_back(req->transferParameters->arrayOfTransferProtocols->stringArray[i]);
            }
        }
    }
}


struct ns1__srmPrepareToGetResponse* storm::ptg::response() {
    // soap struct status
    if (NULL == builtResponse) {
    	builtResponse = storm::soap_calloc<struct ns1__srmPrepareToGetResponse>(_soap);
    }

    if (NULL == builtResponse->returnStatus) {
    	builtResponse->returnStatus = storm::soap_calloc<ns1__TReturnStatus>(_soap);
    }

    builtResponse->returnStatus->statusCode = status();
    if ( NULL == builtResponse->returnStatus->explanation ) {
    	builtResponse->returnStatus->explanation = soap_strdup(_soap, _explanation.c_str());
    } else { // how to free() memory allocated with soap_strdup???
        snprintf(builtResponse->returnStatus->explanation,
                 strlen(builtResponse->returnStatus->explanation),
                 _explanation.c_str());
    }

    // Fill arrayOfFileStatuses
    try {
        if (NULL == builtResponse->arrayOfFileStatuses) {
        	builtResponse->arrayOfFileStatuses = storm::soap_calloc<ns1__ArrayOfTGetRequestFileStatus>(_soap);
        }

        builtResponse->arrayOfFileStatuses->statusArray = storm::soap_calloc<ns1__TGetRequestFileStatus>(_soap, surls.size());
        builtResponse->arrayOfFileStatuses->__sizestatusArray = surls.size();

        int n = 0;
        for (std::vector<storm::PtgSurl>::const_iterator i = surls.begin();
                i != surls.end();
                ++i, ++n)
        {
            if (NULL == builtResponse->arrayOfFileStatuses->statusArray[n]) {
            	builtResponse->arrayOfFileStatuses->statusArray[n] =
                    storm::soap_calloc<ns1__TGetRequestFileStatus>(_soap);
            }
            ns1__TGetRequestFileStatus* fileStatus = builtResponse->arrayOfFileStatuses->statusArray[n];
            
            fileStatus->sourceSURL = soap_strdup(_soap, ((storm::PtgSurl)*i).getSurl().c_str());

            if (NULL == fileStatus->status) {
                fileStatus->status = storm::soap_calloc<ns1__TReturnStatus>(_soap);
            }
            fileStatus->status->statusCode = ((storm::PtgSurl)*i).getStatus();
            fileStatus->status->explanation = soap_strdup(_soap, ((storm::PtgSurl)*i).getExplanation().c_str());
        }
    } catch (std::invalid_argument x) {
        // continuing???
    }
    
    // Fill request token
    if (NULL == builtResponse->requestToken) {
        if (_r_token.size() > 0) {
        	builtResponse->requestToken = soap_strdup(_soap, r_token().c_str());
        }
    } else {
        snprintf(builtResponse->requestToken, strlen(builtResponse->requestToken), r_token().c_str());
    }
    
    return builtResponse;
}

void storm::ptg::insert(struct srm_dbfd *db) {
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
        throw e;
    }
    
    
    // Insert into request_Get using the requestID
    for (std::vector<storm::PtgSurl>::const_iterator i = surls.begin();
            i != surls.end();
            ++i)
    {
        int dirOptionId = -1;
        
        set_savepoint(_db, "GETFILE");
        if (((storm::PtgSurl)*i).hasDirOption()) { // Insert into request_DirOption
            query_s.str("");
            query_s << "INSERT INTO request_DirOption (isSourceADirectory, allLevelRecursive, numOfLevels) VALUES (";

            query_s << sql_format(((storm::PtgSurl)*i).isDirectory()) << ", ";

            if (((storm::PtgSurl)*i).isAllLevelRecursive()) {
            	query_s << "1, ";
            } else {
            	query_s << "0, ";
			}
            /*if (((storm::PtgSurl)*i).allLevelRecursive == -1) {
                query_s << nullcomma;
            } else {
                query_s << ((storm::PtgSurl)*i).allLevelRecursive << ", ";
            }*/
            if (((storm::PtgSurl)*i).hasNumLevels()) {
            	query_s << ((storm::PtgSurl)*i).getNumLevels() << ")";
			} else {
				query_s << "NULL)";
			}
            /*if (((storm::PtgSurl)*i).n_levels == -1) {
                query_s << "NULL)";
            } else {
                query_s << ((storm::PtgSurl)*i).n_levels << ")";
            }*/

            try {
                dirOptionId = storm_db::ID_exec_query(_db, query_s.str());
            } catch (storm_db::mysql_exception e) {
                srmlogit(STORM_LOG_ERROR, "ptg::insert()",
                         "Error %s inserting surl %s into request_Get. Continuing\n",
                         e.what(), ((storm::PtgSurl)*i).getSurl().c_str());
                rollback_to_savepoint(_db, "GETFILE");
                ++_n_failed;
                continue;
            }
        }
        query_s.str("");
        query_s << "INSERT INTO request_Get (sourceSURL, request_DirOptionID, request_queueID) VALUES ('";
        query_s << ((storm::PtgSurl)*i).getSurl() << "', ";
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
                     e.what(), ((storm::PtgSurl)*i).getSurl().c_str());
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
                     e.what(), ((storm::PtgSurl)*i).getSurl().c_str());
            rollback_to_savepoint(_db, "GETFILE");
            ++_n_failed;
            continue;
        }
    }
    // Check the nr of successfully inserted qery.

    // Insert into request_TransferProtocols using the request_ID
    for (std::vector<sql_string>::const_iterator i = protocols.begin();
            i != protocols.end();
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
