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

const std::string storm::CopyStatusRequest::NAME = "Copy Status";
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

    srmlogit(STORM_LOG_DEBUG, "storm::CopyStatusRequest::loadFromDB", "disabled");

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

	m_builtResponse->returnStatus->statusCode = SRM_USCORENOT_USCORESUPPORTED;
	m_builtResponse->returnStatus->explanation = "srmStatusCopy operation is not supported";

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
			fileStatus->status->statusCode = SRM_USCORENOT_USCORESUPPORTED;
			fileStatus->status->explanation = "srmStatusCopy operation is not supported";
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
