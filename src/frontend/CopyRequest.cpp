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

#include "CopyRequest.hpp"
#include "mysql_query.hpp"
#include "storm_mysql.h"

#include "srmlogit.h"

const std::string storm::CopyRequest::NAME = "Copy";
const std::string storm::CopyRequest::MONITOR_NAME = storm::SRM_COPY_MONITOR_NAME;

bool storm::CopyRequest::supportsProtocolSpecification()
{
	return false;
}

void storm::CopyRequest::load(struct ns1__srmCopyRequest* req)
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

    if (getClientDN().size() == 0 && NULL != req->authorizationID) {
        m_credentials.setDN(req->authorizationID);
    }

    for (int i = 0; i < req->arrayOfFileRequests->__sizerequestArray; ++i) {
		m_surls.push_back(SurlPtr( new storm::CopySurl(
						req->arrayOfFileRequests->requestArray[i]->sourceSURL,
						req->arrayOfFileRequests->requestArray[i]->targetSURL,
						req->arrayOfFileRequests->requestArray[i]->dirOption)));
	}

    if (NULL != req->userRequestDescription && m_userToken.size() == 0) {
    	storm::validate_request_description(std::string(req->userRequestDescription));
    	m_userToken = sql_string(req->userRequestDescription);
    }

    if (NULL != req->targetFileStorageType) {
    	try
		{
			this->setFileStorageType(*req->targetFileStorageType);
		} catch(storm::storm_error& e)
		{
			throw storm::invalid_request("Unable to load request file storage type. domain_error Exception: " + std::string(e.what()));
		}
    }

    if (NULL != req->targetSpaceToken) {
    	m_spaceToken = req->targetSpaceToken;
    }

    if (NULL != req->overwriteOption) {
        switch(*req->overwriteOption) {
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
        	throw storm::invalid_request("Unable to load Copy request. Invalid overwriteOption");
        }
    }

    if (req->desiredTargetSURLLifeTime != NULL) {
    	m_fileLifetime = *req->desiredTargetSURLLifeTime;
    }

    // Temporary hack: proxy saved as a file. TODO: insert the proxy into the DB.
}


ns1__srmCopyResponse* storm::CopyRequest::buildResponse()
{

	if(m_builtResponse != NULL)
	{
		return m_builtResponse;
	}
	try
	{
		m_builtResponse = storm::soap_calloc<ns1__srmCopyResponse>(m_soapRequest);
		m_builtResponse->returnStatus = storm::soap_calloc<ns1__TReturnStatus>(m_soapRequest);
	} catch (std::invalid_argument& exc) {
		throw storm::storm_error("Unable to allocate memory for the response. invalid_argument Exception: " + std::string(exc.what()));
	}
    m_builtResponse->returnStatus->statusCode = m_status;

    if (!m_explanation.empty()) {
    	m_builtResponse->returnStatus->explanation = soap_strdup(m_soapRequest, m_explanation.c_str());
    }

    // Fill per-surl info.
    try
    {
    	m_builtResponse->arrayOfFileStatuses = storm::soap_calloc<ns1__ArrayOfTCopyRequestFileStatus>(m_soapRequest);
    	m_builtResponse->arrayOfFileStatuses->statusArray = storm::soap_calloc<ns1__TCopyRequestFileStatus>(
    			m_soapRequest, m_surls.size());
    } catch (std::invalid_argument& exc) {
    	throw storm::storm_error("Unable to allocate memory for the file status array. invalid_argument Exception: " + std::string(exc.what()));
    }
	m_builtResponse->arrayOfFileStatuses->__sizestatusArray = m_surls.size();

	int index=0;
	std::vector<SurlPtr>::const_iterator const vectorEnd = m_surls.end();
	for (std::vector<SurlPtr>::const_iterator i = m_surls.begin(); i != vectorEnd; ++i, ++index) {
		ns1__TCopyRequestFileStatus* fileStatus;
		try
		{
			fileStatus = storm::soap_calloc<ns1__TCopyRequestFileStatus>(m_soapRequest);
		} catch (std::invalid_argument &exc) {
			throw storm::storm_error("Unable to allocate memory for a file status. invalid_argument Exception: " + std::string(exc.what()));
		}
		m_builtResponse->arrayOfFileStatuses->statusArray[index] = fileStatus;
		storm::CopySurl* surl = dynamic_cast<storm::CopySurl*> (i->get());
		if(!surl)
		{
			throw std::logic_error("Unable to cast SurlPtr to CopySurl, cast failure");
		}
		fileStatus->sourceSURL = soap_strdup(m_soapRequest, surl->getSourceSurl().c_str());
		fileStatus->targetSURL = soap_strdup(m_soapRequest, surl->getDestinationSurl().c_str());
		fileStatus->fileSize = NULL;
		fileStatus->estimatedWaitTime = NULL; //should be -1 --> unknown
		fileStatus->remainingFileLifetime = NULL;
		try
		{
			fileStatus->status = storm::soap_calloc<ns1__TReturnStatus>(m_soapRequest);
		} catch (std::invalid_argument &exc) {
				throw storm::storm_error("Unable to allocate memory for a return status. invalid_argument Exception: " + std::string(exc.what()));
		}
		fileStatus->status->statusCode = SRM_USCORENOT_USCORESUPPORTED;
		fileStatus->status->explanation = "srmCopy operation is not supported";
	}
    if (m_requestToken.size() > 0) {
    	m_builtResponse->requestToken = soap_strdup(m_soapRequest, m_requestToken.c_str());
    }

    m_builtResponse->remainingTotalRequestTime = NULL;  //should be -1 --> try at least once

    return m_builtResponse;
}

void storm::CopyRequest::insertIntoDB(struct srm_dbfd *db) {
    std::string nullcomma("NULL, ");
    std::ostringstream query_s;

    std::string q("INSERT INTO request_queue ("
                "  config_FileStorageTypeID"
                ", config_OverwriteID"
                ", config_RequestTypeID"
                ", client_dn"
                ", u_token"
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

    if (m_fileLifetime == -1) {
        query_s << nullcomma;
    } else {
        query_s << m_fileLifetime << ", ";
    }

    if (m_requestToken.size() == 0) {
    	throw std::logic_error("Unable to store COPY request into the DB: request token empty");
    } else {
        query_s << sqlFormat(m_requestToken) <<", ";
    }

    if (m_spaceToken.size() == 0) {
        query_s << nullcomma;
    } else {
        query_s << sqlFormat(m_spaceToken) <<", ";
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

    storm_start_tr(0, db);
    // Insert into request_queue
    int request_id;
    try {
        request_id = storm_db::ID_exec_query(db, query_s.str());
    } catch(storm_db::mysql_exception& e) {
    	srmlogit(STORM_LOG_ERROR, "CopyRequest::insert()",
    	                "Error writing the request on StoRM DB. mysql_exception : %s , errno = %d\n", e.what() , e.get_mysql_errno());
        storm_abort_tr(db);
		failRequest("Error writing the request on StoRM DB");
		throw e;
    }
    int failedCount = 0;
    // Insert into request_Copy using the requestID
    std::vector<SurlPtr>::const_iterator const vectorEnd = m_surls.end();
    for (std::vector<SurlPtr>::const_iterator i = m_surls.begin(); i != vectorEnd; ++i) {
    	storm::CopySurl* surl = dynamic_cast<storm::CopySurl*> (i->get());
		if(!surl)
		{
			throw std::logic_error("Unable to cast SurlPtr to CopySurl, cast failure");
		}
    	set_savepoint(db, "COPYFILE");
    	int diroption_id;
        if (surl->hasDirOption()) {
            std::ostringstream query_d;
            query_d << "INSERT INTO request_DirOption (isSourceADirectory, allLevelRecursive, numOfLevels) values (";
            query_d << sqlFormat(surl->isDirectory()) << ", " << sqlFormat(surl->isAllLevelRecursive()) << ", ";
            if (surl->hasNumLevels()) {
            	query_s << surl->getNumLevels() << ")";
			} else {
				query_s << "NULL)";
			}
            try{
                diroption_id = storm_db::ID_exec_query(db, query_d.str());
            } catch(storm_db::mysql_exception& e) {
            	srmlogit(STORM_LOG_ERROR, "CopyRequest::insert()",
						"Error %s inserting surl %s into request_DirOption. Errno = %d .Continuing\n", e.what(),
						surl->getSurl().c_str() , e.get_mysql_errno());

				surl->setStatus(SRM_USCOREFAILURE);
				surl->setExplanation("Unable to store the request in StoRM DB");
				rollback_to_savepoint(db, "COPYFILE");
				++failedCount;
                continue;
            }
        }

        std::ostringstream query_s;
        query_s << "INSERT INTO request_Copy (sourceSURL, targetSURL, request_queueID, request_DirOptionID) VALUES ";
        query_s << "(" << sqlFormat(surl->getSourceSurl()) << ", " << sqlFormat(surl->getDestinationSurl()) << ", ";
        query_s << request_id << ", ";
        if (surl->hasDirOption())
        {
            query_s << diroption_id<<")";
        }
        else
        {
            query_s << "NULL )";
        }
        int copy_id;
        try{
            copy_id = storm_db::ID_exec_query(db, query_s.str());
        } catch(storm_db::mysql_exception& e) {
        	srmlogit(STORM_LOG_ERROR, "CopyRequest::insert()",
        			"Error %s inserting surl %s into request_Copy. Errno = %d .Continuing\n", e.what(),
					surl->getSurl().c_str() , e.get_mysql_errno());

			surl->setStatus(SRM_USCOREFAILURE);
			surl->setExplanation("Unable to store the request in StoRM DB");
			rollback_to_savepoint(db, "COPYFILE");
			++failedCount;
            continue;
        }

        std::ostringstream query2_s;
        query2_s << "INSERT INTO status_Copy (request_CopyID, statusCode) values (";
        query2_s << copy_id << ", " << SRM_USCOREREQUEST_USCOREQUEUED <<")";
        try{
            storm_db::ID_exec_query(db, query2_s.str());
        } catch(storm_db::mysql_exception& e) {
            srmlogit(STORM_LOG_ERROR, "CopyRequest::insert()",
                    "Error %s inserting surl %s into status_Copy. Errno = %d .Continuing\n", e.what(),
                    surl->getSurl().c_str() , e.get_mysql_errno());

			surl->setStatus(SRM_USCOREFAILURE);
			surl->setExplanation("Unable to store the request in StoRM DB");
			rollback_to_savepoint(db, "COPYFILE");
			++failedCount;
            continue;
        }
    }
    if(failedCount == m_surls.size())
    {
    	srmlogit(STORM_LOG_ERROR, "CopyRequest::insert()",
    			"No SURLs inserted. Request failed\n");
    	storm_abort_tr(db);
    	m_status = SRM_USCOREFAILURE;
    	m_explanation = "All files requests are failed";
    }
    else
    {
    	storm_end_tr(db);
    }

    saveProxy();
    // insert into retention policy, clientNetworks, extrainfo, VOMS
    // attribute using the requestID

}
std::string storm::CopyRequest::getSurlsList()
{
	std::string builtList;
	std::vector<SurlPtr>::const_iterator const vectorEnd = m_surls.end();
	bool first = true;
	for (std::vector<SurlPtr>::const_iterator i = m_surls.begin(); i != vectorEnd; ++i) {
		storm::CopySurl* surl = dynamic_cast<storm::CopySurl*> (i->get());
		if(!surl)
		{
			throw std::logic_error("Unable to cast SurlPtr to CopySurl, cast failure");
		}
		if(first)
		{
			first = false;
		}
		else
		{
			builtList += ' ';
		}
		builtList += surl->getSourceSurl();
		builtList += " -> ";
		builtList += surl->getDestinationSurl();

	}
	return builtList;
}
