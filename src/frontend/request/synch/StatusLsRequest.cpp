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


#include "StatusLsRequest.hpp"
#include "srmlogit.h"
#include "token_validator.hpp"

const std::string storm::StatusLsRequest::NAME = "Status ls";
const std::string storm::StatusLsRequest::MONITOR_NAME = storm::SRM_STATUS_OF_LS_REQUEST_MONITOR_NAME;

void storm::StatusLsRequest::load(ns1__srmStatusOfLsRequestRequest* request)
{
	if(request->requestToken == NULL)
	{
		throw storm::invalid_request("Received NULL requestToken in the request");
	}
	// Ensure token is a UUID
	if (!storm::token::valid(std::string(request->requestToken))){
		throw storm::invalid_request("Invalid request token");
	}
	m_requestToken = std::string(request->requestToken);
	validate_token(m_requestToken);

	if(request->count != NULL)
	{
		m_count = *request->count;
	}
	if(request->offset != NULL)
	{
		m_offset = *request->offset;
	}
}

int storm::StatusLsRequest::performXmlRpcCall(ns1__srmStatusOfLsRequestResponse_* response){
	char *funcName = "StatusLsRequest::performXmlRpcCall()";
	int ret = ns1__srmStatusOfLsRequest_impl(m_soapRequest, m_request, response);
	if(response->srmStatusOfLsRequestResponse != NULL && response->srmStatusOfLsRequestResponse->returnStatus != NULL )
	{
		m_status = response->srmStatusOfLsRequestResponse->returnStatus->statusCode;
		if(response->srmStatusOfLsRequestResponse->returnStatus->explanation != NULL)
		{
			m_explanation = std::string(response->srmStatusOfLsRequestResponse->returnStatus->explanation);
		}
	}
	else
	{
		srmlogit(STORM_LOG_ERROR, "storm::StatusLsRequest::performXmlRpcCall()", "Xmlrpc call returned with an empty returnStatus.\n");
	}
	return ret;
}

int storm::StatusLsRequest::buildResponse()
{
    srmlogit(STORM_LOG_DEBUG, "storm::StatusLsRequest::buildResponse()", "called.\n");
	if(m_builtResponse != NULL)
	{
		return 0;
	}
	try
	{
		m_builtResponse = storm::soap_calloc<ns1__srmStatusOfLsRequestResponse>(m_soapRequest);
		if(m_builtResponse == NULL)
		{
			srmlogit(STORM_LOG_ERROR, "storm::StatusLsRequest::buildResponse()", "Unable to allocate memory for the response\n");
			return SOAP_EOM;
		}
		m_builtResponse->returnStatus = storm::soap_calloc<ns1__TReturnStatus>(m_soapRequest);
		if(m_builtResponse->returnStatus == NULL)
		{
			srmlogit(STORM_LOG_ERROR, "storm::StatusLsRequest::buildResponse()", "Unable to allocate memory for the return status\n");
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
