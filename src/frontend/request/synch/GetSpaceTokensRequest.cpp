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

#include "GetSpaceTokensRequest.hpp"
#include "srmlogit.h"

void storm::GetSpaceTokensRequest::load(ns1__srmGetSpaceTokensRequest* request)
{
	if(request->userSpaceTokenDescription != NULL)
	{
		storm::validate_token_description(std::string(request->userSpaceTokenDescription));
		m_userSpaceTokenDescription = request->userSpaceTokenDescription;
	}
}

int storm::GetSpaceTokensRequest::performXmlRpcCall(ns1__srmGetSpaceTokensResponse_* response){
	char *funcName = "GetSpaceTokensRequest::performXmlRpcCall()";
	int ret = ns1__srmGetSpaceTokens_impl(m_soapRequest, m_request, response);
	if(response->srmGetSpaceTokensResponse != NULL && response->srmGetSpaceTokensResponse->returnStatus != NULL )
	{
		m_status = response->srmGetSpaceTokensResponse->returnStatus->statusCode;
		if(response->srmGetSpaceTokensResponse->returnStatus->explanation != NULL)
		{
			m_explanation = std::string(response->srmGetSpaceTokensResponse->returnStatus->explanation);
		}
	}
	else
	{
		srmlogit(STORM_LOG_ERROR, "storm::GetSpaceTokensRequest::performXmlRpcCall()", "Xmlrpc call returned with an empty returnStatus.\n");
	}
	return ret;
}

int storm::GetSpaceTokensRequest::buildResponse()
{
    srmlogit(STORM_LOG_DEBUG, "storm::GetSpaceTokensRequest::buildResponse()", "called.\n");
	if(m_builtResponse != NULL)
	{
		return 0;
	}
	try
	{
		m_builtResponse = storm::soap_calloc<ns1__srmGetSpaceTokensResponse>(m_soapRequest);
		if(m_builtResponse == NULL)
		{
			srmlogit(STORM_LOG_ERROR, "storm::GetSpaceTokensRequest::buildResponse()", "Unable to allocate memory for the response\n");
			return SOAP_EOM;
		}
		m_builtResponse->returnStatus = storm::soap_calloc<ns1__TReturnStatus>(m_soapRequest);
		if(m_builtResponse->returnStatus == NULL)
		{
			srmlogit(STORM_LOG_ERROR, "storm::GetSpaceTokensRequest::buildResponse()", "Unable to allocate memory for the return status\n");
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
