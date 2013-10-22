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
#include "mysql_query.hpp"
#include "storm_mysql.h"

#include "PtgRequest.hpp"

bool storm::PtgRequest::supportsProtocolSpecification()
{
	return true;
}

void storm::PtgRequest::load(ns1__srmPrepareToGetRequest *req)
{
    if (NULL == req) {
        throw storm::invalid_request("Request is NULL");
    }
    if (NULL == req->arrayOfFileRequests) {
        throw storm::invalid_request("SURLs array is NULL");
    }
    if (0 == req->arrayOfFileRequests->__sizerequestArray) {
        throw storm::invalid_request("SURLs array is empty");
    }

    if (this->getClientDN().size() == 0 && NULL != req->authorizationID) {
        m_credentials.setDN(req->authorizationID);
    }
    
    for (int i = 0; i < req->arrayOfFileRequests->__sizerequestArray; ++i) {
		m_surls.push_back(SurlPtr( new storm::PtgSurl(
						req->arrayOfFileRequests->requestArray[i]->sourceSURL,
						req->arrayOfFileRequests->requestArray[i]->dirOption)));
	}
    
    
    if (NULL != req->userRequestDescription && m_userToken.size() == 0) {
    	m_userToken = sql_string(req->userRequestDescription);
    }

    if (NULL != req->desiredFileStorageType) {
    	try
    	{
    		this->setFileStorageType(*req->desiredFileStorageType);
    	} catch(std::domain_error &e)
    	{
    		throw storm::invalid_request("Unable to load request file storage type. domain_error Exception: " + std::string(e.what()));
    	}
    }

    if (NULL != req->targetSpaceToken) {
    	m_spaceToken = req->targetSpaceToken;
    }

    if (NULL != req->transferParameters) {
		if (NULL != req->transferParameters->arrayOfTransferProtocols) {
			for (int i = 0; i < req->transferParameters->arrayOfTransferProtocols->__sizestringArray; ++i) {
				m_protocols.push_back(req->transferParameters->arrayOfTransferProtocols->stringArray[i]);
			}
		}
	}

    if (NULL != req->desiredPinLifeTime) {
        m_pinLifetime = *req->desiredPinLifeTime;
    }
}

/*
 * Builds the response given object's state. It builds the response just once.
 * */
struct ns1__srmPrepareToGetResponse* storm::PtgRequest::buildResponse() {

	if(m_builtResponse != NULL)
	{
		return m_builtResponse;
	}
	try
	{
		m_builtResponse = storm::soap_calloc<struct ns1__srmPrepareToGetResponse>(m_soapRequest);
		m_builtResponse->returnStatus = storm::soap_calloc<ns1__TReturnStatus>(m_soapRequest);
	} catch (std::invalid_argument& exc) {
		throw storm::storm_error("Unable to allocate memory for the response. invalid_argument Exception: " + std::string(exc.what()));
	}
	m_builtResponse->returnStatus->statusCode = m_status;

    if (!m_explanation.empty()) {
    	m_builtResponse->returnStatus->explanation = soap_strdup(m_soapRequest, m_explanation.c_str());
    }

    // Fill arrayOfFileStatuses
    try {
		m_builtResponse->arrayOfFileStatuses = storm::soap_calloc<ns1__ArrayOfTGetRequestFileStatus>(m_soapRequest);
        m_builtResponse->arrayOfFileStatuses->statusArray = storm::soap_calloc<ns1__TGetRequestFileStatus>(m_soapRequest, m_surls.size());
    } catch (std::invalid_argument& exc) {
		throw storm::storm_error("Unable to allocate memory for the file status array. invalid_argument Exception: " + std::string(exc.what()));
	}
	m_builtResponse->arrayOfFileStatuses->__sizestatusArray = m_surls.size();

	int index = 0;
	std::vector<SurlPtr>::const_iterator const vectorEnd = m_surls.end();
	for (std::vector<SurlPtr>::const_iterator i = m_surls.begin(); i != vectorEnd; ++i, ++index) {
		ns1__TGetRequestFileStatus* fileStatus;
		try {
			fileStatus = storm::soap_calloc<ns1__TGetRequestFileStatus>(m_soapRequest);
		} catch (std::invalid_argument &exc) {
			throw storm::storm_error("Unable to allocate memory for a file status. invalid_argument Exception: " + std::string(exc.what()));
		}
		m_builtResponse->arrayOfFileStatuses->statusArray[index] = fileStatus;
		storm::PtgSurl* surl = dynamic_cast<storm::PtgSurl*> (i->get());
		if(!surl)
		{
			throw std::logic_error("Unable to cast SurlPtr to PtgSurl, cast failure");
		}
		fileStatus->sourceSURL = soap_strdup(m_soapRequest, surl->getSurl().c_str());
		fileStatus->fileSize = NULL;
		fileStatus->estimatedWaitTime = NULL; //should be -1 --> unknown
		fileStatus->remainingPinTime = NULL;

		//those can be not null in case of synchronous PTG
		fileStatus->transferURL = NULL;
		fileStatus->transferProtocolInfo = NULL;
		//
		try
		{
			fileStatus->status = storm::soap_calloc<ns1__TReturnStatus>(m_soapRequest);
		} catch (std::invalid_argument& exc) {
				throw storm::storm_error("Unable to allocate memory for a return status. invalid_argument Exception: " + std::string(exc.what()));
		}
		fileStatus->status->statusCode = surl->getStatus();
		fileStatus->status->explanation = soap_strdup(m_soapRequest, surl->getExplanation().c_str());
	}
    
	if (m_requestToken.size() > 0) {
		m_builtResponse->requestToken = soap_strdup(m_soapRequest, m_requestToken.c_str());
	}

	m_builtResponse->remainingTotalRequestTime = NULL; //should be -1 --> try at least once
    return m_builtResponse;
}

void storm::PtgRequest::insertIntoDB(struct srm_dbfd* db){
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
    
    if (m_fileStorageType == DB_FILE_TYPE_UNKNOWN) {
        query_s << nullcomma;
    } else {
        query_s << sqlFormat(m_fileStorageType) << ", ";
    }

    query_s << sqlFormat(m_requestType) << ", ";
    query_s << sqlFormat(this->getClientDN()) << ", ";

    if (m_userToken.empty()) {
        query_s << nullcomma;
    } else {
        query_s << sqlFormat(m_userToken) << ", ";
    }

    if (m_pinLifetime == -1) {
        query_s << nullcomma;
    } else {
        query_s << m_pinLifetime << ", ";
    }

    if (m_requestToken.size() == 0) {
        throw std::logic_error("Unable to store PTG request into the DB: request token empty");
    } else {
        query_s << sqlFormat(m_requestToken) <<", ";
    }
    
    if (m_spaceToken.size() == 0) {
        query_s << nullcomma;
    } else {
        query_s << sqlFormat(m_spaceToken) << ", ";
    }

    query_s << m_status << ", "<< m_surls.size() << ", ";
    query_s << "0, "<< m_surls.size() << ", 0, ";
    
    // Temporary hack: using the proxy column to store FQANs
    if (m_credentials.getFQANsOneString().empty()) {
        query_s << nullcomma;
    } else {
        query_s << sqlFormat(m_credentials.getFQANsOneString()) << ", ";
    }
     
    query_s << "current_timestamp() )";

    storm_start_tr(0, db);
    // Insert into request_queue
    int request_id;
    try {
        request_id = storm_db::ID_exec_query(db, query_s.str());
    } catch (storm_db::mysql_exception& e) {
    	srmlogit(STORM_LOG_ERROR, "PtgRequest::insert()",
    	                "Error writing the request on StoRM DB. mysql_exception : %s , errno = %d\n", e.what() , e.get_mysql_errno());
        storm_abort_tr(db);
        failRequest("Error writing the request on StoRM DB");
        throw e;
    }
    
    unsigned int failedCount = 0;
    // Insert into request_Get using the requestID
    std::vector<SurlPtr>::const_iterator const vectorEnd = m_surls.end();
	for (std::vector<SurlPtr>::const_iterator i = m_surls.begin(); i != vectorEnd; ++i) {
    	storm::PtgSurl* surl = dynamic_cast<storm::PtgSurl*> (i->get());
		if(!surl)
		{
			throw std::logic_error("Unable to cast SurlPtr to PtgSurl, cast failure");
		}
    	set_savepoint(db, "GETFILE");
        int dirOptionId;
        if (surl->hasDirOption()) { // Insert into request_DirOption
            query_s.str("");
            query_s << "INSERT INTO request_DirOption (isSourceADirectory, allLevelRecursive, numOfLevels) VALUES (";
            query_s << sqlFormat(surl->isDirectory()) << ", " << sqlFormat(surl->isAllLevelRecursive()) << ", ";

            if (surl->hasNumLevels()) {
            	query_s << surl->getNumLevels() << ")";
			} else {
				query_s << "NULL)";
			}
            try {
                dirOptionId = storm_db::ID_exec_query(db, query_s.str());
            } catch (storm_db::mysql_exception& e) {
                srmlogit(STORM_LOG_ERROR, "ptg::insert()",
                         "Error %s inserting surl %s into request_Get. Errno = %d .Continuing\n",
                         e.what(), surl->getSurl().c_str() , e.get_mysql_errno());

                surl->setStatus(SRM_USCOREFAILURE);
				surl->setExplanation("Unable to store the request in StoRM DB");
                rollback_to_savepoint(db, "GETFILE");
                ++failedCount;
                continue;
            }
        }
        query_s.str("");
        query_s << "INSERT INTO request_Get (sourceSURL, request_DirOptionID, request_queueID) VALUES (";
        query_s << sqlFormat(surl->getSurl()) << ", ";
        if (surl->hasDirOption()) {
        	query_s << dirOptionId << ", ";
        } else {
        	query_s << nullcomma;
        }
        query_s << request_id << ")";
        int getId;
        try {
            getId = storm_db::ID_exec_query(db, query_s.str());
        } catch (storm_db::mysql_exception& e) {
            srmlogit(STORM_LOG_ERROR, "ptg::insert()",
                     "Error %s inserting surl %s into request_Get. Errno = %d .Continuing\n",
                     e.what(), surl->getSurl().c_str() , e.get_mysql_errno());

            surl->setStatus(SRM_USCOREFAILURE);
			surl->setExplanation("Unable to store the request in StoRM DB");
            rollback_to_savepoint(db, "GETFILE");
            ++failedCount;
            continue;
        }

        query_s.str("");
        query_s << "INSERT INTO status_Get (request_GetID, statusCode) values (";
        query_s << getId << ", " << SRM_USCOREREQUEST_USCOREQUEUED <<")";
        try {
            storm_db::ID_exec_query(db, query_s.str());
        } catch (storm_db::mysql_exception& e) {
            srmlogit(STORM_LOG_ERROR, "ptg::insert()",
                     "Error %s inserting surl %s into status_Get. Errno = %d .Continuing\n",
                     e.what(), surl->getSurl().c_str() , e.get_mysql_errno());

            surl->setStatus(SRM_USCOREFAILURE);
			surl->setExplanation("Unable to store the request in StoRM DB");
            rollback_to_savepoint(db, "GETFILE");
            ++failedCount;
            continue;
        }
    }

    if(failedCount == m_surls.size())
    {
    	srmlogit(STORM_LOG_ERROR, "ptg::insert()",
    			"No SURLs inserted. Request failed\n");
    	storm_abort_tr(db);
    	m_status = SRM_USCOREFAILURE;
    	m_explanation = "All files requests are failed";
    }
    else
    {
		// Insert into request_TransferProtocols using the request_ID
    	std::vector<sql_string>::const_iterator const protocolsEnd =  m_protocols.end();
		for (std::vector<sql_string>::const_iterator i = m_protocols.begin(); i != protocolsEnd; ++i) {
			query_s.str("");
			query_s << "INSERT into request_TransferProtocols (request_queueID, config_protocolsID) values (";
			query_s << request_id << ", " << sqlFormat(*i) << ")";
			try {
				storm_db::ID_exec_query(db, query_s.str());
			} catch (storm_db::mysql_exception& e) {
				srmlogit(STORM_LOG_ERROR, "ptg::insert()",
						 "Error %s inserting transfer protocol %s into DB. Errno = %d .Continuing\n",
						 e.what(), i->c_str() , e.get_mysql_errno());
				continue;
			}
		}
    }
    storm_end_tr(db);
    // insert into retention policy, clientNetworks, extrainfo, VOMS
    // attribute using the requestID
}
