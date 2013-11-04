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

#include "CopyStatusRequest.hpp"
#include "CopySurl.hpp"
#include "CopyTurl.hpp"
#include "srmlogit.h"

const std::string storm::CopyStatusRequest::NAME = "Copy";
const std::string storm::CopyStatusRequest::MONITOR_NAME = storm::SRM_COPY_MONITOR_NAME;

void storm::CopyStatusRequest::load(ns1__srmStatusOfCopyRequestRequest* req)
{
	if(req->arrayOfSourceSURLs == NULL)
	{
		return;
	}
	for (int i = 0; i < req->arrayOfSourceSURLs->__sizeurlArray; ++i) {
		if(req->arrayOfTargetSURLs != NULL && req->arrayOfSourceSURLs->__sizeurlArray > i)
		{
			m_surls.insert(SurlPtr(new storm::CopySurl(
														req->arrayOfSourceSURLs->urlArray[i],
														req->arrayOfTargetSURLs->urlArray[i])));
		}
		else
		{
			m_surls.insert(SurlPtr(new storm::CopySurl(
											req->arrayOfSourceSURLs->urlArray[i])));
			m_allTargetSurlSpecified = false;
		}
	}
}

void storm::CopyStatusRequest::loadFromDB(struct srm_dbfd* db){

    srmlogit(STORM_LOG_DEBUG, "storm::CopyStatusRequest::loadFromDB", "R_token: %s\n",  m_requestToken.c_str());

    std::string query("");
    if(m_surls.size() > 0)
    {
    	query += "SELECT r.client_dn, r.status, r.errstring, r.remainingTotalTime, "
    			" c.sourceSURL, c.targetSURL , s.fileSize , s.estimatedWaitTime , "
    			" s.remainingFileTime , s.statusCode , s.explanation"
    			" FROM request_queue r JOIN (request_Copy c, status_Copy s) ON "
                 "(c.request_queueID=r.ID AND s.request_CopyID=c.ID)  "
    			"WHERE r.r_token=" + sqlFormat(m_requestToken) + " and c.sourceSURL in (";
		bool first = true;
		std::set<SurlPtr>::const_iterator const vectorEnd = m_surls.end();
		for (std::set<SurlPtr>::const_iterator i = m_surls.begin(); i != vectorEnd; ++i) {
			Surl* current = i->get();
			if(first)
			{
				first = false;
			}
			else
			{
				query += " , ";
			}
			query += sqlFormat(current->getSurl());
		}
		query += ")";
		if(m_allTargetSurlSpecified)
		{
			query += " and c.targetSURL in (";
			first = true;
			std::set<SurlPtr>::const_iterator const vectorEnd = m_surls.end();
			for (std::set<SurlPtr>::const_iterator i = m_surls.begin(); i != vectorEnd; ++i) {
				storm::CopySurl* current = dynamic_cast<storm::CopySurl*> (i->get());
				if(!current)
				{
					throw std::logic_error("Unable to cast SurlPtr to CopySurl, cast failure");
				}
				if(first)
				{
					first = false;
				}
				else
				{
					query += " , ";
				}
				query += sqlFormat(current->getDestinationSurl());
			}
			query += ")";
		}

    }
    else
    {
		query += "SELECT r.client_dn, r.status, r.errstring, r.remainingTotalTime, "
    			" c.sourceSURL, c.targetSURL , s.fileSize , s.estimatedWaitTime , "
    			" s.remainingFileTime , s.statusCode , s.explanation"
    			" FROM request_queue r JOIN (request_Copy c, status_Copy s) ON "
                 "(c.request_queueID=r.ID AND s.request_CopyID=c.ID)  "
    			"WHERE r.r_token=" + sqlFormat(m_requestToken);
    }
    file_status_results_t results;
    storm_db::vector_exec_query(db, query, results);
	if (results.size() == 0)
	{
		if(m_surls.size() > 0)
		{
			srmlogit(STORM_LOG_INFO, "storm::CopyStatusRequest::loadFromDB()",
									 "No tokens found for token %s and the requested SURLs\n", m_requestToken.c_str());
			throw storm::token_not_found("No request found for token " + m_requestToken + " and the requested SURLs\n");
		}
		else
		{
			srmlogit(STORM_LOG_INFO, "storm::CopyStatusRequest::loadFromDB()",
									 "No tokens found for token %s\n", m_requestToken.c_str());
			throw storm::token_not_found("No request found for token " + m_requestToken + "\n");
		}

	}
	fillCommonFields(results);

	std::vector<file_status_result_t>::const_iterator const vectorEnd = results.end();
	for (std::vector<file_status_result_t>::const_iterator i = results.begin(); i != vectorEnd; ++i) {
		file_status_result_t currentResutl = *i;
		storm::Surl surl(currentResutl["sourceSURL"].c_str());
		sql_string destinationSURL(currentResutl["targetSURL"].c_str());
		CopyTurl* turl;
		if(currentResutl["fileSize"].empty())
		{
			turl = new CopyTurl(surl, destinationSURL);
		}
		else
		{
			turl = new CopyTurl(surl, destinationSURL, strtoull(currentResutl["fileSize"].c_str(),(char**)NULL,10));
		}
		if(currentResutl["statusCode"].empty())
		{
			srmlogit(STORM_LOG_ERROR, "storm::CopyStatusRequest::loadFromDB()",
			                 "Error,status code for SURL %s is empty. Continuing without filling SURLs informations.\n", currentResutl["targetSURL"].c_str());
			delete turl;
			continue;
		}
		turl->setStatus(static_cast<ns1__TStatusCode>(atoi(currentResutl["statusCode"].c_str())));
		turl->setExplanation(currentResutl["explanation"]);
		if(!currentResutl["estimatedWaitTime"].empty())
		{
			turl->setEstimatedWaitTime(atoi(currentResutl["estimatedWaitTime"].c_str()));
		}
		if(!currentResutl["remainingFileTime"].empty())
		{
			turl->setRemainingFileLifetime(atoi(currentResutl["remainingFileTime"].c_str()));
		}
		m_turls.insert(TurlPtr(turl));
	}
}

ns1__srmStatusOfCopyRequestResponse* storm::CopyStatusRequest::buildResponse()
{
    srmlogit(STORM_LOG_DEBUG, "storm::CopyStatusRequest::buildResponse()", "called.\n");

	if(m_builtResponse != NULL)
	{
		return m_builtResponse;
	}
	try
	{
		m_builtResponse = storm::soap_calloc<ns1__srmStatusOfCopyRequestResponse>(m_soapRequest);
		m_builtResponse->returnStatus = storm::soap_calloc<ns1__TReturnStatus>(m_soapRequest);
	} catch (std::invalid_argument& exc) {
		throw std::logic_error("Unable to allocate memory for the response. invalid_argument Exception: "
				+ std::string(exc.what()));
	}

	m_builtResponse->returnStatus->statusCode = m_status;
	if (!m_explanation.empty()) {
		m_builtResponse->returnStatus->explanation = soap_strdup(m_soapRequest, m_explanation.c_str());
	}
    if (this->hasRemainingTotalRequestTime()) {
    	m_builtResponse->remainingTotalRequestTime = storm::soap_calloc<int>(m_soapRequest);
        *m_builtResponse->remainingTotalRequestTime = m_remainingTotalRequestTime;
    }
    // Fill status for each surl.
    int fileStatusArraySize = (m_surls.size() >  m_turls.size() ? m_surls.size() : m_turls.size());
    if(fileStatusArraySize > 0)
    {
		try
		{
			m_builtResponse->arrayOfFileStatuses = storm::soap_calloc<ns1__ArrayOfTCopyRequestFileStatus>(m_soapRequest);
			m_builtResponse->arrayOfFileStatuses->statusArray = storm::soap_calloc<ns1__TCopyRequestFileStatus>(
					m_soapRequest, fileStatusArraySize);
		} catch (std::invalid_argument& exc) {
			throw std::logic_error("Unable to allocate memory for the file status array. invalid_argument Exception: " + std::string(exc.what()));
		}
		m_builtResponse->arrayOfFileStatuses->__sizestatusArray = fileStatusArraySize;
		int index = 0;
		std::set<TurlPtr>::const_iterator const vectorEnd = m_turls.end();
		for (std::set<TurlPtr>::const_iterator i = m_turls.begin(); i != vectorEnd; ++i, ++index) {
			ns1__TCopyRequestFileStatus *fileStatus;
			try
			{
				fileStatus = storm::soap_calloc<ns1__TCopyRequestFileStatus>(m_soapRequest);
			} catch (std::invalid_argument& exc) {
					throw std::logic_error("Unable to allocate memory for a file status. invalid_argument Exception: " + std::string(exc.what()));
			}
			m_builtResponse->arrayOfFileStatuses->statusArray[index] = fileStatus;

			storm::CopyTurl* turl = dynamic_cast<storm::CopyTurl*> (i->get());
			if(!turl)
			{
				throw std::logic_error("Unable to cast TurlPtr to CopyTurl, cast failure");
			}
			fileStatus->sourceSURL = soap_strdup(m_soapRequest, turl->getSourceSurl().getSurl().c_str());
			fileStatus->targetSURL = soap_strdup(m_soapRequest, turl->getDestinationSurl().c_str());
			try
			{
				fileStatus->status = storm::soap_calloc<ns1__TReturnStatus>(m_soapRequest);
			} catch (std::invalid_argument& exc) {
					throw std::logic_error("Unable to allocate memory for a return status. invalid_argument Exception: " + std::string(exc.what()));
			}
			if (turl->hasFileSize()) {
				fileStatus->fileSize = storm::soap_calloc<ULONG64>(m_soapRequest);
				*fileStatus->fileSize = turl->getFileSize();
			}
			if (turl->hasEstimatedWaitTime()) {
				fileStatus->estimatedWaitTime = storm::soap_calloc<int>(m_soapRequest);
				*fileStatus->estimatedWaitTime = turl->getEstimatedWaitTime();
			}
			if (turl->hasRemainingFileLifetime()) {
				fileStatus->remainingFileLifetime= storm::soap_calloc<int>(m_soapRequest);
				*fileStatus->remainingFileLifetime = turl->hasRemainingFileLifetime();
			}
			fileStatus->status->statusCode = turl->getStatus();
			fileStatus->status->explanation = soap_strdup(m_soapRequest, turl->getExplanation().c_str());
		}
		if(this->hasMissingSurls())
		{
			this->addMissingSurls();
		}
    }
    return m_builtResponse;
}

void storm::CopyStatusRequest::addMissingSurls()
{
	int index = (m_turls.empty() ? 0 : m_turls.size() - 1);

	std::set<SurlPtr>::const_iterator const surlVectorEnd = m_surls.end();
	for (std::set<SurlPtr>::const_iterator i = m_surls.begin(); i != surlVectorEnd; ++i) {

		storm::CopySurl* current = dynamic_cast<storm::CopySurl*> (i->get());
		if(!current)
		{
			throw std::logic_error("Unable to cast SurlPtr to CopySurl, cast failure");
		}
		if(this->checkSurl(current->getSourceSurl()))
		{
			continue;
		}
		ns1__TCopyRequestFileStatus *fileStatus;
		try
		{
			fileStatus = storm::soap_calloc<ns1__TCopyRequestFileStatus>(m_soapRequest);
		} catch (std::invalid_argument& exc) {
				throw std::logic_error("Unable to allocate memory for a file status. invalid_argument Exception: " + std::string(exc.what()));
		}
		if(index >= m_builtResponse->arrayOfFileStatuses->__sizestatusArray)
		{
			throw std::logic_error("Attempt to add more Put Request File Status than allocated!");
		}
		m_builtResponse->arrayOfFileStatuses->statusArray[index] = fileStatus;

		fileStatus->fileSize = NULL;
		fileStatus->remainingFileLifetime = NULL;
		fileStatus->estimatedWaitTime = NULL;
		fileStatus->sourceSURL = soap_strdup(m_soapRequest, current->getSourceSurl().c_str());
		if(current->hasDestinationSurl())
		{
			fileStatus->targetSURL = soap_strdup(m_soapRequest, current->getDestinationSurl().c_str());
		}
		else
		{
			fileStatus->targetSURL = NULL;
		}
		try
		{
			fileStatus->status = storm::soap_calloc<ns1__TReturnStatus>(m_soapRequest);
		} catch (std::invalid_argument& exc) {
				throw std::logic_error("Unable to allocate memory for a return status. invalid_argument Exception: " + std::string(exc.what()));
		}
		fileStatus->status->statusCode = SRM_USCOREINVALID_USCOREPATH;
		fileStatus->status->explanation = soap_strdup(m_soapRequest, "No information about this SURL");
		++index;
	}
}
