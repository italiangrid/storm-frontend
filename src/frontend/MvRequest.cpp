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


#include "MvRequest.hpp"
#include "srmlogit.h"
#include "Surl.hpp"

const std::string storm::MvRequest::NAME = "Mv";
const std::string storm::MvRequest::MONITOR_NAME = storm::SRM_MV_MONITOR_NAME;

std::string storm::MvRequest::from_surl() const{
	return m_fromSURL;
}

std::string storm::MvRequest::to_surl() const {
	return m_toSURL;
}

void storm::MvRequest::load(ns1__srmMvRequest* request)
{
	if (request->fromSURL == NULL) {
		throw storm::invalid_request("fromSURL is NULL");
	}
	if (request->toSURL == NULL) {
		throw storm::invalid_request("toSURL is NULL");
	}
	m_fromSURL = storm::normalize_surl(std::string(request->fromSURL));
	m_toSURL = storm::normalize_surl(std::string(request->toSURL));
	if(request->storageSystemInfo != NULL && request->storageSystemInfo->__sizeextraInfoArray > 0 && request->storageSystemInfo->extraInfoArray != NULL)
	{
		for(int i = 0; i < request->storageSystemInfo->__sizeextraInfoArray; ++i)
		{
			m_extraInfo.insert(std::make_pair(request->storageSystemInfo->extraInfoArray[i]->key,
					request->storageSystemInfo->extraInfoArray[i]->value));
		}
	}
}

int storm::MvRequest::performXmlRpcCall(ns1__srmMvResponse_* response){
	char *funcName = "MvRequest::performXmlRpcCall()";
	int ret = ns1__srmMv_impl(m_soapRequest, m_request, response);
	if(response->srmMvResponse != NULL && response->srmMvResponse->returnStatus != NULL )
	{
		m_status = response->srmMvResponse->returnStatus->statusCode;
		if(response->srmMvResponse->returnStatus->explanation != NULL)
		{
			m_explanation = std::string(response->srmMvResponse->returnStatus->explanation);
		}
	}
	else
	{
		srmlogit(STORM_LOG_ERROR, "storm::MvRequest::performXmlRpcCall()", "Xmlrpc call returned with an empty returnStatus.\n");
	}
	return ret;
}

int storm::MvRequest::buildResponse()
{
	srmlogit(STORM_LOG_DEBUG, "storm::MvRequest::buildResponse()", "called.\n");
	if(m_builtResponse != NULL)
	{
		return 0;
	}
	try
	{
		m_builtResponse = storm::soap_calloc<ns1__srmMvResponse>(m_soapRequest);
		if(m_builtResponse == NULL)
		{
			srmlogit(STORM_LOG_ERROR, "storm::MvRequest::buildResponse()", "Unable to allocate memory for the response\n");
			return SOAP_EOM;
		}
		m_builtResponse->returnStatus = storm::soap_calloc<ns1__TReturnStatus>(m_soapRequest);
		if(m_builtResponse->returnStatus == NULL)
		{
			srmlogit(STORM_LOG_ERROR, "storm::MvRequest::buildResponse()", "Unable to allocate memory for the return status\n");
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
