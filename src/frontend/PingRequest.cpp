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


#include "PingRequest.hpp"
#include "srmlogit.h"

const std::string storm::PingRequest::NAME = "Ping";
const std::string storm::PingRequest::MONITOR_NAME = storm::SRM_PING_MONITOR_NAME;

void storm::PingRequest::load(ns1__srmPingRequest* request)
{
}

int storm::PingRequest::performXmlRpcCall(ns1__srmPingResponse_* response){
	int ret =  ns1__srmPing_impl(m_soapRequest, m_request, response);
	m_status = SRM_USCORESUCCESS;
	return ret;
}

int storm::PingRequest::buildResponse()
{
    srmlogit(STORM_LOG_DEBUG, "storm::PingRequest::buildResponse()", "called.\n");
    m_builtResponse = storm::soap_calloc<struct ns1__srmPingResponse>(m_soapRequest);
	if(m_builtResponse != NULL)
	{
		m_builtResponse->versionInfo = soap_strdup(m_soapRequest, m_explanation.c_str());
		return 0;
	}
	else
	{
		srmlogit(STORM_LOG_ERROR, "storm::PingRequest::buildResponse()", "Unable to allocate memory for the response\n");
		return SOAP_EOM;
	}
}
