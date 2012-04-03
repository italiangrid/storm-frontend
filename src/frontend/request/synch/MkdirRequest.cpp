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


#include "MkdirRequest.hpp"
#include "srmlogit.h"

void storm::MkdirRequest::load(ns1__srmMkdirRequest* request) throw (storm::invalid_request)
{
	if(request->SURL == NULL)
	{
		throw storm::invalid_request("Received NULL surl in the request");
	}
	m_surls.insert(std::string(request->SURL));
	if(request->storageSystemInfo != NULL && request->storageSystemInfo->__sizeextraInfoArray > 0 && request->storageSystemInfo->extraInfoArray != NULL)
	{
		for(int i = 0; i < request->storageSystemInfo->__sizeextraInfoArray; ++i)
		{
			m_extraInfo.insert(std::make_pair(request->storageSystemInfo->extraInfoArray[i]->key,
					request->storageSystemInfo->extraInfoArray[i]->value));
		}
	}
}

int storm::MkdirRequest::performXmlRpcCall(ns1__srmMkdirResponse_* response){
	char *funcName = "MkdirRequest::performXmlRpcCall()";
	int ret = ns1__srmMkdir_impl(m_soapRequest, m_request, response);
	if(response->srmMkdirResponse != NULL && response->srmMkdirResponse->returnStatus != NULL )
	{
		m_status = response->srmMkdirResponse->returnStatus->statusCode;
		if(response->srmMkdirResponse->returnStatus->explanation != NULL)
		{
			m_explanation = std::string(response->srmMkdirResponse->returnStatus->explanation);
		}
	}
	else
	{
		srmlogit(STORM_LOG_ERROR, "storm::MkdirRequest::performXmlRpcCall()", "Xmlrpc call returned with an empty returnStatus.\n");
	}
	return ret;
}

void storm::MkdirRequest::buildResponse() throw (std::logic_error, storm::InvalidResponse)
{
    srmlogit(STORM_LOG_DEBUG, "storm::MkdirRequest::buildResponse()", "called.\n");
	if(m_builtResponse != NULL)
	{
		return;
	}
	try
	{
		m_builtResponse = storm::soap_calloc<ns1__srmMkdirResponse>(m_soapRequest);
		m_builtResponse->returnStatus = storm::soap_calloc<ns1__TReturnStatus>(m_soapRequest);
	} catch (std::invalid_argument& exc) {
		throw std::logic_error("Unable to allocate memory for the response. invalid_argument Exception: "
				+ std::string(exc.what()));
	}
	m_builtResponse->returnStatus->statusCode = m_status;
  	if (!m_explanation.empty()) {
  		m_builtResponse->returnStatus->explanation = soap_strdup(m_soapRequest, m_explanation.c_str());
  	}
}
