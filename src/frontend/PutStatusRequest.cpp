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

#include "PutStatusRequest.hpp"
#include "PtpTurl.hpp"
#include "srmlogit.h"
#include <bits/stl_pair.h>

void storm::PutStatusRequest::load(ns1__srmStatusOfPutRequestRequest* req)
{
	if(req->arrayOfTargetSURLs == NULL)
	{
		return;
	}
	for (int i = 0; i < req->arrayOfTargetSURLs->__sizeurlArray; ++i) {
		/*storm::Surl surl(
					req->arrayOfTargetSURLs->urlArray[i]);
		m_surls.insert(surl);*/
		m_surls.insert(SurlPtr(new storm::Surl(req->arrayOfTargetSURLs->urlArray[i])));
	}
}

void storm::PutStatusRequest::loadFromDB(struct srm_dbfd* db) throw (storm::TokenNotFound){

    srmlogit(STORM_LOG_DEBUG, "storm::PutStatusRequest::loadFromDB", "R_token: %s\n",  m_requestToken.c_str());

    /*std::string query = "SELECT r.client_dn, r.status, r.errstring, r.remainingTotalTime, r.pinLifetime, r.fileLifetime, "
    		"from request_queue r JOIN (request_Put c, status_Put s) ON "
             "(c.request_queueID=r.ID AND s.request_PutID=c.ID) "
             "WHERE r.r_token='" + requestToken + "'";*/
    std::string query("");
    if(m_surls.size() > 0)
    {
		query += "SELECT r.client_dn, r.status, r.errstring, r.remainingTotalTime,"
						" c.targetSURL , s.transferURL ,  s.fileSize , s.estimatedWaitTime , s.remainingPinTime , s.remainingFileTime , s.statusCode , s.explanation"
						" from request_queue r JOIN (request_Put c, status_Put s) ON "
						"(c.request_queueID=r.ID AND s.request_PutID=c.ID) WHERE r.r_token="
						+ sqlFormat(m_requestToken) + " and "
						"c.targetSURL in (";
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
		query += "SELECT r.client_dn, r.status, r.errstring, r.remainingTotalTime,"
						" c.targetSURL , s.transferURL , s.fileSize , s.estimatedWaitTime , s.remainingPinTime , s.remainingFileTime , s.statusCode , s.explanation"
						" from request_queue r JOIN (request_Put c, status_Put s) ON "
						"(c.request_queueID=r.ID AND s.request_PutID=c.ID) WHERE r.r_token="
						+ sqlFormat(m_requestToken);
    }
    file_status_results_t results;
    storm_db::vector_exec_query(db, query, results);
	if (results.size() == 0)
	{
		if(m_surls.size() > 0)
		{
			srmlogit(STORM_LOG_INFO, "storm::PutStatusRequest::loadFromDB()",
									 "No requests found for token %s and the requested SURLs\n", m_requestToken.c_str());
			throw storm::TokenNotFound("No requests found for token " + m_requestToken + " and the requested SURLs\n");
		}
		else
		{
			srmlogit(STORM_LOG_INFO, "storm::PutStatusRequest::loadFromDB()",
									 "No requests found for token %s\n", m_requestToken.c_str());
			throw storm::TokenNotFound("No requests found for token " + m_requestToken + "\n");
		}

	}
	fillCommonFields(results);

	std::vector<file_status_result_t>::const_iterator const vectorEnd = results.end();
	for (std::vector<file_status_result_t>::const_iterator i = results.begin(); i != vectorEnd; ++i) {
		file_status_result_t currentResutl = *i;
		std::string turlString = currentResutl["transferURL"];
		storm::Surl surl(currentResutl["targetSURL"].c_str());
		PtpTurl* turl;
		if(!turlString.empty())
		{
			if(currentResutl["fileSize"].empty())
			{
				turl = new storm::PtpTurl(turlString, surl);
			}
			else
			{
				turl = new storm::PtpTurl(turlString,surl, atoi(currentResutl["fileSize"].c_str()));
			}
		}
		else
		{
			if(currentResutl["fileSize"].empty())
			{
				turl = new storm::PtpTurl(surl);
			}
			else
			{
				turl = new storm::PtpTurl(surl, atoi(currentResutl["fileSize"].c_str()));
			}
		}
		if(currentResutl["statusCode"].empty())
		{
			srmlogit(STORM_LOG_ERROR, "storm::PutStatusRequest::loadFromDB()",
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
		if(!currentResutl["remainingFileTime"].empty())
		{
			turl->setRemainingFileLifetime(atoi(currentResutl["remainingFileTime"].c_str()));
		}
		m_turls.insert(TurlPtr(turl));
	}
}

ns1__srmStatusOfPutRequestResponse* storm::PutStatusRequest::buildResponse() throw (std::logic_error)
{
    srmlogit(STORM_LOG_DEBUG, "storm::PutStatusRequest::buildResponse()", "called.\n");

	if(m_builtResponse != NULL)
	{
		return m_builtResponse;
	}
	try
	{
		m_builtResponse = storm::soap_calloc<ns1__srmStatusOfPutRequestResponse>(m_soapRequest);
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
    int fileStatusArraySize = (m_surls.empty() ? m_turls.size() : m_surls.size());
    if(fileStatusArraySize > 0)
    {
    	try
    	{
    		m_builtResponse->arrayOfFileStatuses = storm::soap_calloc<ns1__ArrayOfTPutRequestFileStatus>(m_soapRequest);
    		m_builtResponse->arrayOfFileStatuses->statusArray = storm::soap_calloc<ns1__TPutRequestFileStatus>(
    				m_soapRequest, fileStatusArraySize);
    	} catch (std::invalid_argument& exc) {
    		throw std::logic_error("Unable to allocate memory for the file status array. invalid_argument Exception: " + std::string(exc.what()));
    	}
    	m_builtResponse->arrayOfFileStatuses->__sizestatusArray = fileStatusArraySize;
    	int index = 0;
    	std::set<TurlPtr>::const_iterator const vectorEnd = m_turls.end();
    	for (std::set<TurlPtr>::const_iterator i = m_turls.begin(); i != vectorEnd; ++i, ++index) {
    		ns1__TPutRequestFileStatus *fileStatus;
    		try
    		{
    			fileStatus = storm::soap_calloc<ns1__TPutRequestFileStatus>(m_soapRequest);
    		} catch (std::invalid_argument& exc) {
    			throw std::logic_error("Unable to allocate memory for a file status. invalid_argument Exception: " + std::string(exc.what()));
    		}
    		m_builtResponse->arrayOfFileStatuses->statusArray[index] = fileStatus;

    		storm::PtpTurl* turl = dynamic_cast<storm::PtpTurl*> (i->get());
    		if(!turl)
    		{
    			throw std::logic_error("Unable to cast TurlPtr to PtpTurl, cast failure");
    		}
    		fileStatus->SURL = soap_strdup(m_soapRequest, turl->getSurl().getSurl().c_str());
    		if(!turl->isEmpty())
    		{
    			fileStatus->transferURL = soap_strdup(m_soapRequest, turl->getTurl().c_str());
    		}
    		else
    		{
    			fileStatus->transferURL = NULL;
    		}
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
    		else
    		{
    			fileStatus->fileSize = NULL;
    		}
    		if (turl->hasEstimatedWaitTime()) {
    			fileStatus->estimatedWaitTime = storm::soap_calloc<int>(m_soapRequest);
    			*fileStatus->estimatedWaitTime = turl->getEstimatedWaitTime();
    		}
    		else
    		{
    			fileStatus->estimatedWaitTime = NULL;
    		}
    		if (turl->hasRemainingFileLifetime()) {
    			fileStatus->remainingFileLifetime = storm::soap_calloc<int>(m_soapRequest);
    			*fileStatus->remainingFileLifetime = turl->getRemainingFileLifetime();
    		}
    		else
    		{
    			fileStatus->remainingFileLifetime = NULL;
    		}
    		if (turl->hasRemainingPinLifetime()) {
    			fileStatus->remainingPinLifetime = storm::soap_calloc<int>(m_soapRequest);
    			*fileStatus->remainingPinLifetime = turl->getRemainingPinLifetime();
    		}
    		else
    		{
    			fileStatus->remainingPinLifetime = NULL;
    		}
    		fileStatus->status->statusCode = turl->getStatus();
    		fileStatus->status->explanation = soap_strdup(m_soapRequest, turl->getExplanation().c_str());
    		fileStatus->transferProtocolInfo = NULL;
    	}
    	if(this->hasMissingSurls())
    	{
    		this->addMissingSurls();
    	}
   }
    return m_builtResponse;
}

void storm::PutStatusRequest::addMissingSurls() throw (std::logic_error)
{
	int index = (m_turls.empty() ? 0 : m_turls.size() - 1);

	std::set<SurlPtr>::const_iterator const surlVectorEnd = m_surls.end();
	for (std::set<SurlPtr>::const_iterator i = m_surls.begin(); i != surlVectorEnd; ++i) {

		storm::Surl* current = dynamic_cast<storm::Surl*> (i->get());
		if(!current)
		{
			throw std::logic_error("Unable to cast SurlPtr to Surl, cast failure");
		}
		if(this->checkSurl(current->getSurl()))
		{
			continue;
		}
		ns1__TPutRequestFileStatus *fileStatus;
		try
		{
			fileStatus = storm::soap_calloc<ns1__TPutRequestFileStatus>(m_soapRequest);
		} catch (std::invalid_argument& exc) {
				throw std::logic_error("Unable to allocate memory for a file status. invalid_argument Exception: " + std::string(exc.what()));
		}
		if(index >= m_builtResponse->arrayOfFileStatuses->__sizestatusArray)
		{
			throw std::logic_error("Attempt to add more Put Request File Status than allocated!");
		}
		m_builtResponse->arrayOfFileStatuses->statusArray[index] = fileStatus;

		fileStatus->fileSize = NULL;
		fileStatus->remainingPinLifetime = NULL;
		fileStatus->remainingFileLifetime = NULL;
		fileStatus->estimatedWaitTime = NULL;
		fileStatus->SURL = soap_strdup(m_soapRequest, current->getSurl().c_str());
		fileStatus->transferURL = NULL;
		try
		{
			fileStatus->status = storm::soap_calloc<ns1__TReturnStatus>(m_soapRequest);
		} catch (std::invalid_argument& exc) {
				throw std::logic_error("Unable to allocate memory for a return status. invalid_argument Exception: " + std::string(exc.what()));
		}
		fileStatus->status->statusCode = SRM_USCOREINVALID_USCOREPATH;
		fileStatus->status->explanation = soap_strdup(m_soapRequest, "No information about this SURL");
		fileStatus->transferProtocolInfo = NULL;
		++index;
	}

}
