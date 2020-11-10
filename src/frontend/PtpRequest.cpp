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

#include "mysql_query.hpp"
#include "storm_mysql.h"
#include "srmlogit.h"

#include "PtpRequest.hpp"

const std::string storm::PtpRequest::NAME = "Prepare to put";
const std::string storm::PtpRequest::MONITOR_NAME = storm::SRM_PREPARE_TO_PUT_MONITOR_NAME;

bool storm::PtpRequest::supportsProtocolSpecification()
{
	return true;
}

void storm::PtpRequest::load(ns1__srmPrepareToPutRequest* req)
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
		if (NULL != req->arrayOfFileRequests->requestArray[i]->expectedFileSize) {
			m_surls.push_back(SurlPtr( new storm::PtpSurl(
							req->arrayOfFileRequests->requestArray[i]->targetSURL,
							*req->arrayOfFileRequests->requestArray[i]->expectedFileSize)));
			srmlogit(STORM_LOG_DEBUG,"PtpRequest::load()","Debug: expectedFileSize!=NULL, value= %l\n",req->arrayOfFileRequests->requestArray[i]->expectedFileSize);
		} else {
			m_surls.push_back(SurlPtr( new storm::PtpSurl(
							req->arrayOfFileRequests->requestArray[i]->targetSURL)));
		}
	}

    if (NULL != req->userRequestDescription && m_userToken.size() == 0) {
    	storm::validate_request_description(req->userRequestDescription);
    	m_userToken = sql_string(req->userRequestDescription);
    }

    if (NULL != req->desiredFileStorageType) {
    	try
		{
			this->setFileStorageType(*req->desiredFileStorageType);
		} catch(std::domain_error& e)
		{
			throw storm::invalid_request("Unable to load request file storage type. domain_error Exception:  " + std::string(e.what()));
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

    if (NULL != req->overwriteOption) {
        switch (*req->overwriteOption) {
        case NEVER:
        	m_overwrite = DB_OVERWRITE_NEVER;
            break;
        case ALWAYS:
        	m_overwrite = DB_OVERWRITE_ALWAYS;
            break;
        case WHEN_USCOREFILES_USCOREARE_USCOREDIFFERENT:
        	m_overwrite = DB_OVERWRITE_IF_DIFFERENT;
            break;
        default:
            throw storm::invalid_request("Unable to load PTP request. Invalid overwriteOption");
        }
    }

    if (NULL != req->desiredFileLifeTime) {
        m_fileLifetime = *req->desiredFileLifeTime;
    }
}

ns1__srmPrepareToPutResponse * storm::PtpRequest::buildResponse(){

	if(m_builtResponse != NULL)
	{
		return m_builtResponse;
	}
	try
	{
    	m_builtResponse = storm::soap_calloc<ns1__srmPrepareToPutResponse>(m_soapRequest);
    	m_builtResponse->returnStatus = storm::soap_calloc<ns1__TReturnStatus>(m_soapRequest);
	} catch (std::invalid_argument& exc) {
		throw storm::invalid_response("Unable to allocate memory for the response. invalid_argument Exception: "
				+ std::string(exc.what()));
	}
    m_builtResponse->returnStatus->statusCode = m_status;

    if (!m_explanation.empty()) {
		m_builtResponse->returnStatus->explanation = soap_strdup(m_soapRequest, m_explanation.c_str());
	}

    try
    {
		m_builtResponse->arrayOfFileStatuses = storm::soap_calloc<ns1__ArrayOfTPutRequestFileStatus>(m_soapRequest);
        m_builtResponse->arrayOfFileStatuses->statusArray = storm::soap_calloc<ns1__TPutRequestFileStatus>(
        		m_soapRequest, m_surls.size());
    } catch (std::invalid_argument& exc) {
		throw storm::storm_error("Unable to allocate memory for the file status array. invalid_argument Exception: " + std::string(exc.what()));
	}
	m_builtResponse->arrayOfFileStatuses->__sizestatusArray = m_surls.size();

	// Fill per-surl info.
	int index = 0;
	std::vector<SurlPtr>::const_iterator const vectorEnd = m_surls.end();
	for (std::vector<SurlPtr>::const_iterator i = m_surls.begin(); i != vectorEnd; ++i, ++index) {
		ns1__TPutRequestFileStatus *fileStatus;
		try
		{
			fileStatus = storm::soap_calloc<ns1__TPutRequestFileStatus>(m_soapRequest);
		} catch (std::invalid_argument& exc) {
				throw storm::storm_error("Unable to allocate memory for a file status. invalid_argument Exception: " + std::string(exc.what()));
		}
		m_builtResponse->arrayOfFileStatuses->statusArray[index] = fileStatus;
		storm::PtpSurl* surl = dynamic_cast<storm::PtpSurl*> (i->get());
		if(!surl)
		{
			throw std::logic_error("Unable to cast SurlPtr to PtpSurl, cast failure");
		}
		fileStatus->SURL = soap_strdup(m_soapRequest, surl->getSurl().c_str());
		fileStatus->fileSize = NULL;
		fileStatus->estimatedWaitTime = NULL; //should be -1 --> unknown
		fileStatus->remainingPinLifetime = NULL;
		fileStatus->remainingFileLifetime = NULL;
		fileStatus->transferURL = NULL;
		fileStatus->transferProtocolInfo = NULL;
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

void storm::PtpRequest::insertIntoDB(struct srm_dbfd* db) {
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

    if (m_fileStorageType == DB_FILE_TYPE_UNKNOWN) {
        query_s << nullcomma;
    } else {
        query_s << sqlFormat(m_fileStorageType) << ", ";
    }

    if (m_overwrite == DB_OVERWRITE_UNKNOWN) {
        query_s << nullcomma;
    } else {
        query_s << sqlFormat(m_overwrite) << ", ";
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

    if (m_fileLifetime == -1) {
        query_s << nullcomma;
    } else {
        query_s << m_fileLifetime << ", ";
    }

    if (m_requestToken.size() == 0) {
    	throw std::logic_error("Unable to store PTP request into the DB: request token empty");
    } else {
    	query_s << sqlFormat(m_requestToken) << ", ";
    }

    if (m_spaceToken.size() == 0) {
        query_s << nullcomma;
    } else {
        query_s << sqlFormat(m_spaceToken) << ", ";
    }

    query_s << m_status << ", " << m_surls.size() << ", ";
    query_s << "0, " << m_surls.size() << ", 0, ";

    // Temporary hack: using the proxy column to store FQANs
    if (m_credentials.getFQANsOneString().empty()) {
        query_s << nullcomma;
    } else {
        query_s << sqlFormat(m_credentials.getFQANsOneString()) << ", ";
    }

    query_s << "current_timestamp() )";

    // Start DB transaction
    storm_start_tr(0, db);

    int request_id;
    try {
        request_id = storm_db::ID_exec_query(db, query_s.str());
    } catch (storm_db::mysql_exception& e) {
    	srmlogit(STORM_LOG_ERROR, "PtpRequest::insert()",
    	                "Error writing the request on StoRM DB. mysql_exception : %s , errno = %d\n", e.what() , e.get_mysql_errno());
        storm_abort_tr(db);
        failRequest("Error writing the request on StoRM DB");
        throw e;
    }

    int failedCount = 0;
    // Insert into request_Put using the requestID
	std::vector<SurlPtr>::const_iterator const vectorEnd = m_surls.end();
	for (std::vector<SurlPtr>::const_iterator i = m_surls.begin(); i != vectorEnd; ++i) {
		storm::PtpSurl* surl = dynamic_cast<storm::PtpSurl*> (i->get());
		if(!surl)
		{
			throw std::logic_error("Unable to cast SurlPtr to PtpSurl, cast failure");
		}
        std::ostringstream query_s;
        query_s << "INSERT INTO request_Put (targetSURL, expectedFileSize, request_queueID) VALUES ";
        query_s << "(" << sqlFormat(surl->getSurl()) << ", ";
        if (surl->hasExpected_size())
        {
            query_s << surl->getExpected_size() << ", ";
        }
        else
        {
            query_s << nullcomma;
        }
        query_s  << request_id << ")";
        int put_id;
        set_savepoint(db, "PUTFILE");
        try {
            put_id = storm_db::ID_exec_query(db, query_s.str());
        } catch (storm_db::mysql_exception& e) {
            srmlogit(STORM_LOG_ERROR, "ptp::insert()",
                    "Error %s inserting surl %s into request_Put. Errno = %d Continuing\n", e.what(),
                    surl->getSurl().c_str() , e.get_mysql_errno());

        	surl->setStatus(SRM_USCOREFAILURE);
			surl->setExplanation("Unable to store the request in StoRM DB");
            rollback_to_savepoint(db, "PUTFILE");
            ++failedCount;
            continue;
        }

        // Insert into status_Put using the request_PutID
        std::ostringstream query2_s;
        query2_s << "INSERT INTO status_Put (request_PutID, statusCode) values (";
        query2_s << put_id << ", " << SRM_USCOREREQUEST_USCOREQUEUED << ")";
        try {
            storm_db::ID_exec_query(db, query2_s.str());
        } catch (storm_db::mysql_exception& e) {
            srmlogit(STORM_LOG_ERROR, "ptp::insert()",
                    "Error %s inserting surl %s into status_Put. Errno = %d Continuing\n", e.what(),
                    surl->getSurl().c_str() , e.get_mysql_errno());

        	surl->setStatus(SRM_USCOREFAILURE);
			surl->setExplanation("Unable to store the request in StoRM DB");
            rollback_to_savepoint(db, "PUTFILE");
            ++failedCount;
            continue;
        }
    }
    if(failedCount == static_cast<int>(m_surls.size()))
    {
    	srmlogit(STORM_LOG_ERROR, "PtpRequest::insert()",
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
				srmlogit(STORM_LOG_ERROR, "PtpRequest::insert()",
						"Error %s inserting transfer protocol %s into DB. Errno = %d Continuing\n", e.what(), i->c_str() , e.get_mysql_errno());
				continue;
			}
		}
    }
    storm_end_tr(db);
    // insert into retention policy, clientNetworks, extrainfo, VOMS
    // attribute using the requestID

}
