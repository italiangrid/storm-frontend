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

#include "StatusReserveSpaceRequest.hpp"
#include "srmlogit.h"

const std::string storm::StatusReserveSpaceRequest::NAME = "Status reserve space";
const std::string storm::StatusReserveSpaceRequest::MONITOR_NAME = storm::SRM_STATUS_OF_RESERVE_SPACE_REQUEST_MONITOR_NAME;

void storm::StatusReserveSpaceRequest::load(ns1__srmStatusOfReserveSpaceRequestRequest* request)
{
	if(request->requestToken == NULL)
	{
		throw storm::invalid_request("Received NULL requestToken in the request");
	}

	m_requestToken = std::string(request->requestToken);
	validate_token(m_requestToken);
}

int storm::StatusReserveSpaceRequest::performXmlRpcCall(ns1__srmStatusOfReserveSpaceRequestResponse_* response){
	char *funcName = "StatusReserveSpaceRequest::performXmlRpcCall()";
	int ret = ns1__srmStatusOfReserveSpaceRequest_impl(m_soapRequest, m_request, response);
	if(response->srmStatusOfReserveSpaceRequestResponse != NULL && response->srmStatusOfReserveSpaceRequestResponse->returnStatus != NULL )
	{
		m_status = response->srmStatusOfReserveSpaceRequestResponse->returnStatus->statusCode;
		if(response->srmStatusOfReserveSpaceRequestResponse->returnStatus->explanation != NULL)
		{
			m_explanation = std::string(response->srmStatusOfReserveSpaceRequestResponse->returnStatus->explanation);
		}
	}
	else
	{
		srmlogit(STORM_LOG_ERROR, "storm::StatusReserveSpaceRequest::performXmlRpcCall()", "Xmlrpc call returned with an empty returnStatus.\n");
	}
	return ret;
}

int storm::StatusReserveSpaceRequest::buildResponse()
{
    srmlogit(STORM_LOG_DEBUG, "storm::StatusReserveSpaceRequest::buildResponse()", "called.\n");
	if(m_builtResponse != NULL)
	{
		return 0;
	}
	try
	{
		m_builtResponse = storm::soap_calloc<ns1__srmStatusOfReserveSpaceRequestResponse>(m_soapRequest);
		if(m_builtResponse == NULL)
		{
			srmlogit(STORM_LOG_ERROR, "storm::StatusReserveSpaceRequest::buildResponse()", "Unable to allocate memory for the response\n");
			return SOAP_EOM;
		}
		m_builtResponse->returnStatus = storm::soap_calloc<ns1__TReturnStatus>(m_soapRequest);
		if(m_builtResponse->returnStatus == NULL)
		{
			srmlogit(STORM_LOG_ERROR, "storm::StatusReserveSpaceRequest::buildResponse()", "Unable to allocate memory for the return status\n");
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
