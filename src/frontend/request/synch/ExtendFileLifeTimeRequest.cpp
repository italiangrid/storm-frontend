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

#include "ExtendFileLifeTimeRequest.hpp"
#include "srmlogit.h"
#include "Surl.hpp"

const std::string storm::ExtendFileLifeTimeRequest::NAME = "Extend file lifetime";
const std::string storm::ExtendFileLifeTimeRequest::MONITOR_NAME = storm::SRM_EXTEND_FILE_LIFE_TIME_MONITOR_NAME;

void storm::ExtendFileLifeTimeRequest::load(ns1__srmExtendFileLifeTimeRequest* request)
{
	//complex parameter checks by srm specs
	if (request->newPinLifeTime == NULL && request->newFileLifeTime == NULL)
	{
		throw storm::invalid_request("newPinLifeTime and newFileLifeTime are NULL");
	}
	if (request->newPinLifeTime != NULL && request->newFileLifeTime != NULL)
	{
		throw storm::invalid_request("newPinLifeTime and newFileLifeTime are NOT NULL");
	}
	if (request->arrayOfSURLs == NULL || request->arrayOfSURLs->__sizeurlArray == 0 || request->arrayOfSURLs->urlArray == NULL) {
		throw storm::invalid_request("SURLs array is NULL");
	}
	if (request->requestToken != NULL && request->newPinLifeTime == NULL)
	{
		throw storm::invalid_request("requestToken is NOT NULL and newPinLifeTime is NULL");
	}
	if ((request->arrayOfSURLs != NULL || request->arrayOfSURLs->__sizeurlArray > 0 || request->arrayOfSURLs->urlArray != NULL) && request->newFileLifeTime != NULL && request->requestToken != NULL)
	{

		throw storm::invalid_request("SURLs array is NOT NULL, requestToken is NOT NULL and newFileLifeTime is NOT NULL");
	}
	if (request->arrayOfSURLs != NULL && request->arrayOfSURLs->__sizeurlArray > 0 && request->arrayOfSURLs->urlArray != NULL) {
		for (int i = 0; i < request->arrayOfSURLs->__sizeurlArray; ++i) {
			m_surls.insert(storm::normalize_surl(std::string(request->arrayOfSURLs->urlArray[i])));
		}
	}
	if (request->requestToken != NULL)
	{
		m_requestToken = std::string(request->requestToken);
		validate_token(m_requestToken);
	}
	if (request->requestToken == NULL)
	{
		if(request->newFileLifeTime != NULL)
		{
			m_newFileLifeTime = *request->newFileLifeTime;
		}
	}
	if (request->newPinLifeTime != NULL)
	{
		m_newPinLifeTime = *request->newPinLifeTime;
	}
}

int storm::ExtendFileLifeTimeRequest::performXmlRpcCall(ns1__srmExtendFileLifeTimeResponse_* response){
	char *funcName = "ExtendFileLifeTimeRequest::performXmlRpcCall()";
	int ret = ns1__srmExtendFileLifeTime_impl(m_soapRequest, m_request, response);
	if(response->srmExtendFileLifeTimeResponse != NULL && response->srmExtendFileLifeTimeResponse->returnStatus != NULL )
	{
		m_status = response->srmExtendFileLifeTimeResponse->returnStatus->statusCode;
		if(response->srmExtendFileLifeTimeResponse->returnStatus->explanation != NULL)
		{
			m_explanation = std::string(response->srmExtendFileLifeTimeResponse->returnStatus->explanation);
		}
	}
	else
	{
		srmlogit(STORM_LOG_ERROR, "storm::ExtendFileLifeTimeRequest::performXmlRpcCall()", "Xmlrpc call returned with an empty returnStatus.\n");
	}
	return ret;
}

int storm::ExtendFileLifeTimeRequest::buildResponse()
{
    srmlogit(STORM_LOG_DEBUG, "storm::ExtendFileLifeTimeRequest::buildResponse()", "called.\n");
	if(m_builtResponse != NULL)
	{
		return 0;
	}
	try
	{
		m_builtResponse = storm::soap_calloc<ns1__srmExtendFileLifeTimeResponse>(m_soapRequest);
		if(m_builtResponse == NULL)
		{
			srmlogit(STORM_LOG_ERROR, "storm::ExtendFileLifeTimeRequest::buildResponse()", "Unable to allocate memory for the response\n");
			return SOAP_EOM;
		}
		m_builtResponse->returnStatus = storm::soap_calloc<ns1__TReturnStatus>(m_soapRequest);
		if(m_builtResponse->returnStatus == NULL)
		{
			srmlogit(STORM_LOG_ERROR, "storm::ExtendFileLifeTimeRequest::buildResponse()", "Unable to allocate memory for the return status\n");
			return SOAP_EOM;
		}
	} catch (std::invalid_argument& exc) {
		throw std::logic_error("Unable to allocate memory for the response. invalid_argument Exception: "
				+ std::string(exc.what()));
	}
	m_builtResponse->returnStatus->statusCode = m_status;
  	if (!m_explanation.empty()) {
  		m_builtResponse->returnStatus->explanation = soap_strdup(m_soapRequest, m_explanation.c_str());
  	}
    return 0;
}
