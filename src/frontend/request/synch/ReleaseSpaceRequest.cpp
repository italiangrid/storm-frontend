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


#include "ReleaseSpaceRequest.hpp"
#include "srmlogit.h"

void storm::ReleaseSpaceRequest::load(ns1__srmReleaseSpaceRequest* request) throw (storm::invalid_request)
{
	if(request->spaceToken == NULL)
	{
		throw storm::invalid_request("Received NULL spaceToken in the request");
	}
	m_spaceToken = std::string(request->spaceToken);
	if(request->forceFileRelease != NULL)
	{
		 m_forceFileRelease = convertBoolean(*(request->forceFileRelease));
	}
	if(request->storageSystemInfo != NULL && request->storageSystemInfo->__sizeextraInfoArray > 0 && request->storageSystemInfo->extraInfoArray != NULL)
	{
		for(int i = 0; i < request->storageSystemInfo->__sizeextraInfoArray; ++i)
		{
			m_extraInfo.insert(std::make_pair(request->storageSystemInfo->extraInfoArray[i]->key,
					request->storageSystemInfo->extraInfoArray[i]->value));
		}
	}
}

int storm::ReleaseSpaceRequest::performXmlRpcCall(ns1__srmReleaseSpaceResponse_* response){
	char *funcName = "ReleaseSpaceRequest::performXmlRpcCall()";
	int ret = ns1__srmReleaseSpace_impl(m_soapRequest, m_request, response);
	if(response->srmReleaseSpaceResponse != NULL && response->srmReleaseSpaceResponse->returnStatus != NULL )
	{
		m_status = response->srmReleaseSpaceResponse->returnStatus->statusCode;
		if(response->srmReleaseSpaceResponse->returnStatus->explanation != NULL)
		{
			m_explanation = std::string(response->srmReleaseSpaceResponse->returnStatus->explanation);
		}
	}
	else
	{
		srmlogit(STORM_LOG_ERROR, "storm::ReleaseSpaceRequest::performXmlRpcCall()", "Xmlrpc call returned with an empty returnStatus.\n");
	}
	return ret;
}

void storm::ReleaseSpaceRequest::buildResponse() throw (std::logic_error, storm::InvalidResponse)
{
    srmlogit(STORM_LOG_DEBUG, "storm::ReleaseSpaceRequest::buildResponse()", "called.\n");
	if(m_builtResponse != NULL)
	{
		return;
	}
	try
	{
		m_builtResponse = storm::soap_calloc<ns1__srmReleaseSpaceResponse>(m_soapRequest);
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