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

#include "copy.hpp"
#include "mysql_query.hpp"
#include "storm_mysql.h"

#include "srmlogit.h"

bool storm::copy::supportsProtocolSpecification()
{
	return false;
}

std::vector<sql_string>* storm::copy::getRequestedProtocols()
{
	return NULL;
}

void storm::copy::setProtocolVector(std::vector<sql_string>* protocolVector)
{
	//nothing to do form Copy, no protocol list in the request
}

void storm::copy::setGenericFailureSurls()
{
	srmlogit(STORM_LOG_DEBUG, "copy::setGenericFailureSurls()", "Setting the status of all requested SURLs to SRM_FAILURE\n");
    for (int i = 0; i < surls.size(); i++) {
        surls.at(i).setStatus(SRM_USCOREFAILURE);
    }
}

void storm::copy::load(struct ns1__srmCopyRequest *req)
{
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
    _r_type = DB_COPY_REQUEST;

    // Fill surl
    for (int i=0; i<req->arrayOfFileRequests->__sizerequestArray; ++i)
    {
		surls.push_back(
			storm::CopySurl(req->arrayOfFileRequests->requestArray[i]->sourceSURL,
				   req->arrayOfFileRequests->requestArray[i]->targetSURL,
				   req->arrayOfFileRequests->requestArray[i]->dirOption));
    }
    _n_files = surls.size();
    // Status
    status(SRM_USCOREREQUEST_USCOREQUEUED);

    // Client DN
    if (getClientDN().size() == 0 && NULL != req->authorizationID) {
        setClientDN(req->authorizationID);
    }

    // User Token
    if (NULL != req->userRequestDescription
        && u_token().size() == 0)
    {
        u_token(req->userRequestDescription);
    }
    // Request Token will be not written

    // File Storage Type
    if (NULL != req->targetFileStorageType)
        switch(*req->targetFileStorageType) {
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
        switch(*req->overwriteOption) {
        case NEVER:
            _overwrite = DB_OVERWRITE_NEVER;
            break;
        case ALWAYS:
            _overwrite = DB_OVERWRITE_ALWAYS;
            break;
        case WHEN_USCOREFILES_USCOREARE_USCOREDIFFERENT:
            _overwrite = DB_OVERWRITE_IF_DIFFERENT;
            break;
        default:
            throw std::string("Invalid overwriteOption");
        }
    }

    if (req->desiredTargetSURLLifeTime != NULL)
    	_lifetime = *req->desiredTargetSURLLifeTime;

    if (NULL != req->targetSpaceToken)
        _s_token = req->targetSpaceToken;

    // Temporary hack: proxy saved as a file. TODO: insert the proxy into the DB.
    saveProxy();
}


ns1__srmCopyResponse* storm::copy::response()
{
    // soap struct status
    if (NULL == this->builtResponse)
    	this->builtResponse = storm::soap_calloc<ns1__srmCopyResponse>(_soap);

    if (NULL == this->builtResponse->returnStatus)
    	this->builtResponse->returnStatus = storm::soap_calloc<ns1__TReturnStatus>(_soap);

    this->builtResponse->returnStatus->statusCode = status();
    if ( NULL == this->builtResponse->returnStatus->explanation )
    	this->builtResponse->returnStatus->explanation = soap_strdup(_soap, _explanation.c_str());
    else // how to free() memory allocated with soap_strdup???
        snprintf(this->builtResponse->returnStatus->explanation,
                 strlen(this->builtResponse->returnStatus->explanation),
                 _explanation.c_str());

    // Fill per-surl info.
    try{
        if (NULL == this->builtResponse->arrayOfFileStatuses)
        {
        	this->builtResponse->arrayOfFileStatuses = storm::soap_calloc<ns1__ArrayOfTCopyRequestFileStatus>(_soap);
        }

        if (NULL == this->builtResponse->arrayOfFileStatuses->statusArray)
        {
        	this->builtResponse->arrayOfFileStatuses->statusArray = storm::soap_calloc<ns1__TCopyRequestFileStatus>(_soap, surls.size());
        }

        this->builtResponse->arrayOfFileStatuses->__sizestatusArray = surls.size();

        int n=0;
        for (std::vector<storm::CopySurl>::const_iterator i = surls.begin();
            i != surls.end(); ++i, ++n) {
            if (NULL == this->builtResponse->arrayOfFileStatuses->statusArray[n])
            {
            	this->builtResponse->arrayOfFileStatuses->statusArray[n]
                    = storm::soap_calloc<ns1__TCopyRequestFileStatus>(_soap);
            }
            if (NULL == this->builtResponse->arrayOfFileStatuses->statusArray[n]->status )
            {
            	this->builtResponse->arrayOfFileStatuses->statusArray[n]->status
                    = storm::soap_calloc<ns1__TReturnStatus>(_soap);
            }
            this->builtResponse->arrayOfFileStatuses->statusArray[n]->sourceSURL = soap_strdup(_soap, ((storm::CopySurl)*i).getSourceSurl().c_str());
            this->builtResponse->arrayOfFileStatuses->statusArray[n]->targetSURL = soap_strdup(_soap, ((storm::CopySurl)*i).getDestinationSurl().c_str());
            this->builtResponse->arrayOfFileStatuses->statusArray[n]->status->statusCode = ((storm::CopySurl)*i).getStatus();
            this->builtResponse->arrayOfFileStatuses->statusArray[n]->status->explanation = soap_strdup(_soap, ((storm::CopySurl)*i).getExplanation().c_str());
        }
    } catch(std::invalid_argument x) {
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

void storm::copy::insert(struct srm_dbfd *db)
{
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
        query_s << "'" << r_token() <<"', ";
    }

    if (s_token().size() == 0) {
        query_s << "NULL, ";
    } else {
        query_s << "'"<<s_token()<<"', ";
    }

    query_s << status() << ", "<< _n_files;
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
    } catch(int e) {
        _response_errno = e;
        _response_error = "DB error inserting into request_queue a COPY request";
        _response_error += ". Errno: ";
        _response_error += e;
        storm_abort_tr(_db);
        throw _response_error;
    }
    // Insert into request_Copy using the requestID
    for (std::vector<storm::CopySurl>::const_iterator i = surls.begin();
        i != surls.end();
        ++i)
    {
        // DirOption
        int diroption_id;
        if (((storm::CopySurl)*i).hasDirOption()) {
            std::ostringstream query_d;
            query_d << "INSERT INTO request_DirOption (isSourceADirectory, allLevelRecursive, numOfLevels) values (";
            if (false == ((storm::CopySurl)*i).isDirectory())
                query_d << "0, ";
            else
                query_d << "1, ";

            if (((storm::CopySurl)*i).isAllLevelRecursive()) {
            	query_s << "1, ";
            } else {
            	query_s << "0, ";
			}

            if (((storm::CopySurl)*i).hasNumLevels()) {
            	query_s << ((storm::CopySurl)*i).getNumLevels() << ")";
			} else {
				query_s << "NULL)";
			}
            /*if (((storm::CopySurl)*i).n_levels != -1)
                query_d << ((storm::CopySurl)*i).n_levels << ")";
            else
                query_d << "NULL )";
                */
            set_savepoint(_db, "COPYFILE");
            try{
                diroption_id = storm_db::ID_exec_query(_db, query_d.str());
            } catch(int e) {

                _response_errno = 0;
                _response_error = "DB error inserting DirOption for surl "+((storm::CopySurl)*i).getSourceSurl();
                _response_error += "Error = ";
                _response_error += e;
                rollback_to_savepoint(_db, "COPYFILE");
                continue;
            }
        }


        std::ostringstream query_s;
        query_s << "INSERT INTO request_Copy (sourceSURL, targetSURL, request_queueID, request_DirOptionID) VALUES ";
        query_s << "('" << ((storm::CopySurl)*i).getSourceSurl()<< "', '" << ((storm::CopySurl)*i).getDestinationSurl() << "', ";
        query_s << request_id << ", ";
        if (((storm::CopySurl)*i).hasDirOption())
        {
            query_s << diroption_id<<")";
        }
        else
        {
            query_s << "NULL )";
        }
        int copy_id;
        set_savepoint(_db, "COPYFILE");
        try{
            copy_id = storm_db::ID_exec_query(_db, query_s.str());
        } catch(int e) {
            // Qua impostiamo l'errore per il surl e lo dobbiamo dedurre dal tipo di errore di mysql.
            // Poi dobbiamo impostare la stringa di errore.
            // Infine aggiorniamo _n_failed.
            // Dobbiamo fare un continue, sempre che funzioni, e continuare con le altre surl.

            _response_errno = e;
            _response_error = "DB error inserting surl " + ((storm::CopySurl)*i).getSourceSurl();
            _response_error += " into request_Copy. Errno: "+e;
            rollback_to_savepoint(_db, "COPYFILE");
            ++_n_failed;
            continue;
        }


        // Insert into status_Copy using the request_GetID
        std::ostringstream query2_s;
        query2_s << "INSERT INTO status_Copy (request_CopyID, statusCode) values (";
        query2_s << copy_id << ", " << SRM_USCOREREQUEST_USCOREQUEUED <<")";
        try{
            storm_db::ID_exec_query(_db, query2_s.str());
        } catch(int e) {
            _response_errno = e;
            _response_error = "DB error inserting into status_Copy";
            _response_error += ". Errno: "+e;
            rollback_to_savepoint(_db, "COPYFILE");
            ++_n_failed;
            continue;
        }
    }
    // Check the nr of successfully inserted qery.

    // Check the number of correctly inserted protocols.

    storm_end_tr(_db);
    // insert into retention policy, clientNetworks, extrainfo, VOMS
    // attribute using the requestID

}
