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

#include "BolStatusRequest.hpp"
#include "BolTurl.hpp"
#include "srmlogit.h"

const std::string storm::BolStatusRequest::NAME = "BoL Status";
const std::string storm::BolStatusRequest::MONITOR_NAME = storm::SRM_STATUS_OF_BRING_ONLINE_REQUEST_MONITOR_NAME;

void storm::BolStatusRequest::load(ns1__srmStatusOfBringOnlineRequestRequest* req)
{
	if(req->arrayOfSourceSURLs == NULL)
	{
		return;
	}
	for (int i = 0; i < req->arrayOfSourceSURLs->__sizeurlArray; ++i) {
		m_surls.insert(SurlPtr(new storm::Surl(req->arrayOfSourceSURLs->urlArray[i])));
	}
}

void storm::BolStatusRequest::loadFromDB(struct srm_dbfd* db){

    srmlogit(STORM_LOG_DEBUG, "storm::BolStatusRequest::loadFromDB", "R_token: %s\n",  m_requestToken.c_str());

    std::string query("");
    if(m_surls.size() > 0)
    {
    	query += "SELECT r.client_dn, r.status, r.errstring, r.remainingTotalTime, "
    			" c.sourceSURL , s.fileSize , s.estimatedWaitTime , "
    			" s.remainingPinTime , s.statusCode , s.explanation"
    			" FROM request_queue r JOIN (request_BoL c, status_BoL s) ON "
    			"(c.request_queueID=r.ID AND s.request_BoLID=c.ID) "
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
    }
    else
    {
		query += "SELECT r.client_dn, r.status, r.errstring, r.remainingTotalTime, "
    			" c.sourceSURL , s.fileSize , s.estimatedWaitTime , "
    			" s.remainingPinTime , s.statusCode , s.explanation"
    			" FROM request_queue r JOIN (request_BoL c, status_BoL s) ON "
    			"(c.request_queueID=r.ID AND s.request_BoLID=c.ID) "
    			"WHERE r.r_token=" + sqlFormat(m_requestToken);
    }
    file_status_results_t results = storm_db::vector_exec_query(db, query);
	if (results.size() == 0)
	{
		if(m_surls.size() > 0)
		{
			srmlogit(STORM_LOG_INFO, "storm::BolStatusRequest::loadFromDB()",
									 "No tokens found for token %s and the requested SURLs\n", m_requestToken.c_str());
			throw storm::token_not_found("No request found for token " + m_requestToken + " and the requested SURLs\n");
		}
		else
		{
			srmlogit(STORM_LOG_INFO, "storm::BolStatusRequest::loadFromDB()",
									 "No tokens found for token %s\n", m_requestToken.c_str());
			throw storm::token_not_found("No request found for token " + m_requestToken + "\n");
		}

	}
	fillCommonFields(results);

	std::vector<file_status_result_t>::const_iterator const vectorEnd = results.end();
	for (std::vector<file_status_result_t>::const_iterator i = results.begin(); i != vectorEnd; ++i) {
		file_status_result_t currentResutl = *i;
		storm::Surl surl(currentResutl["sourceSURL"].c_str());
		BolTurl* turl;
		if(currentResutl["fileSize"].empty())
		{
			turl = new BolTurl(surl);
		}
		else
		{
			turl = new BolTurl(surl, strtoull(currentResutl["fileSize"].c_str(),(char**)NULL,10));
		}
		if(currentResutl["statusCode"].empty())
		{
			srmlogit(STORM_LOG_ERROR, "storm::BolStatusRequest::loadFromDB()",
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
		if(!currentResutl["remainingPinTime"].empty())
		{
			turl->setRemainingPinLifetime(atoi(currentResutl["remainingPinTime"].c_str()));
		}
		m_turls.insert(TurlPtr(turl));
	}
}

ns1__srmStatusOfBringOnlineRequestResponse* storm::BolStatusRequest::buildResponse()
{
    srmlogit(STORM_LOG_DEBUG, "storm::BolStatusRequest::buildResponse()", "called.\n");

	if(m_builtResponse != NULL)
	{
		return m_builtResponse;
	}
	try
	{
		m_builtResponse = storm::soap_calloc<ns1__srmStatusOfBringOnlineRequestResponse>(m_soapRequest);
		m_builtResponse->returnStatus = storm::soap_calloc<ns1__TReturnStatus>(m_soapRequest);
	} catch (std::invalid_argument& exc) {
		throw std::logic_error("Unable to allocate memory for the response. invalid_argument Exception: "
				+ std::string(exc.what()));
	}
    if (this->hasRemainingTotalRequestTime()) {
    	m_builtResponse->remainingTotalRequestTime = storm::soap_calloc<int>(m_soapRequest);
        *m_builtResponse->remainingTotalRequestTime = m_remainingTotalRequestTime;
    }
    // Fill status for each surl.
    int fileStatusArraySize = (m_surls.empty() ? m_turls.size() : m_surls.size());
    if(fileStatusArraySize > 0)
    {
    	try
    	{
    		m_builtResponse->arrayOfFileStatuses = storm::soap_calloc<ns1__ArrayOfTBringOnlineRequestFileStatus>(m_soapRequest);
    		m_builtResponse->arrayOfFileStatuses->statusArray = storm::soap_calloc<ns1__TBringOnlineRequestFileStatus>(
    				m_soapRequest, fileStatusArraySize);
    	} catch (std::invalid_argument& exc) {
    		throw std::logic_error("Unable to allocate memory for the file status array. invalid_argument Exception: " + std::string(exc.what()));
    	}
    	m_builtResponse->arrayOfFileStatuses->__sizestatusArray = fileStatusArraySize;
    	int countSuccess = 0;
    	int countFailure = 0;
    	int index = 0;
    	std::set<TurlPtr>::const_iterator const vectorEnd = m_turls.end();
    	for (std::set<TurlPtr>::const_iterator i = m_turls.begin(); i != vectorEnd; ++i, ++index) {
    		ns1__TBringOnlineRequestFileStatus *fileStatus;
    		try
    		{
    			fileStatus = storm::soap_calloc<ns1__TBringOnlineRequestFileStatus>(m_soapRequest);
    		} catch (std::invalid_argument& exc) {
    			throw std::logic_error("Unable to allocate memory for a file status. invalid_argument Exception: " + std::string(exc.what()));
    		}
    		m_builtResponse->arrayOfFileStatuses->statusArray[index] = fileStatus;

    		storm::BolTurl* turl = dynamic_cast<storm::BolTurl*> (i->get());
    		if(!turl)
    		{
    			throw std::logic_error("Unable to cast TurlPtr to BolTurl, cast failure");
    		}
    		if (turl->getStatus() == SRM_USCOREREQUEST_USCOREINPROGRESS) {
    			/* Useful for tape enabled file systems. Tells the BE to check if the SURL is already
    			 * recalled from tame and in that case to update DB info (that will be available to the next
    			 * get status request).
    			 */
    			if (isSurlOnDisk(turl->getSurl().getSurl()))
    			{
    				turl->setStatus(SRM_USCORESUCCESS);
    				turl->setExplanation("File recalled from tape");
    				countSuccess++;
    			}
    		}
    		else
    		{
    			if (turl->getSurl().getStatus() == SRM_USCORESUCCESS
    					|| turl->getSurl().getStatus() == SRM_USCOREFILE_USCOREIN_USCORECACHE
    					|| turl->getSurl().getStatus() == SRM_USCORERELEASED)
    			{
    				countSuccess++;
    			}
    			else
    			{
    				if(turl->getSurl().getStatus() != SRM_USCOREREQUEST_USCOREQUEUED)
    				{
    					countFailure++;
    				}
    			}
    		}
    		fileStatus->sourceSURL = soap_strdup(m_soapRequest, turl->getSurl().getSurl().c_str());
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
    		if (turl->hasRemainingPinLifetime()) {
    			fileStatus->remainingPinTime = storm::soap_calloc<int>(m_soapRequest);
    			*fileStatus->remainingPinTime = turl->hasRemainingPinLifetime();
    		}
    		fileStatus->status->statusCode = turl->getStatus();
    		fileStatus->status->explanation = soap_strdup(m_soapRequest, turl->getExplanation().c_str());
    	}
    	if(this->hasMissingSurls())
    	{
    		this->addMissingSurls();
    	}

        if ((countSuccess + countFailure) == fileStatusArraySize) {
            if (countFailure == 0) {
                m_status = SRM_USCORESUCCESS;
            } else if (countSuccess == 0) {
                m_status = SRM_USCOREFAILURE;
            } else {
    			m_status = SRM_USCOREPARTIAL_USCORESUCCESS;
    		}
    	}
    }
	m_builtResponse->returnStatus->statusCode = m_status;

  	if (!m_explanation.empty()) {
  		m_builtResponse->returnStatus->explanation = soap_strdup(m_soapRequest, m_explanation.c_str());
  	}
	m_builtResponse->remainingDeferredStartTime = NULL;
    return m_builtResponse;
}

void storm::BolStatusRequest::addMissingSurls()
{
	int index = (m_turls.empty() ? 0 : m_turls.size() - 1);

	std::set<SurlPtr>::const_iterator const surlVectorEnd = m_surls.end();
	for (std::set<SurlPtr>::const_iterator i = m_surls.begin(); i != surlVectorEnd; ++i) {

		storm::Surl* current = i->get();

		if(!current)
		{
			throw std::logic_error("Unable to cast SurlPtr to Surl, cast failure");
		}
		if(this->checkSurl(current->getSurl()))
		{
			continue;
		}
		ns1__TBringOnlineRequestFileStatus *fileStatus;
		try
		{
			fileStatus = storm::soap_calloc<ns1__TBringOnlineRequestFileStatus>(m_soapRequest);
		} catch (std::invalid_argument& exc) {
				throw std::logic_error("Unable to allocate memory for a file status. invalid_argument Exception: " + std::string(exc.what()));
		}
		if(index >= m_builtResponse->arrayOfFileStatuses->__sizestatusArray)
		{
			throw std::logic_error("Attempt to add more Put Request File Status than allocated!");
		}
		m_builtResponse->arrayOfFileStatuses->statusArray[index] = fileStatus;

		fileStatus->fileSize = NULL;
		fileStatus->remainingPinTime = NULL;
		fileStatus->estimatedWaitTime = NULL;
		fileStatus->sourceSURL = soap_strdup(m_soapRequest, current->getSurl().c_str());
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
