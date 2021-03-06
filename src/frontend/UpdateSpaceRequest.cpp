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

#include "UpdateSpaceRequest.hpp"
#include "srmlogit.h"

const std::string storm::UpdateSpaceRequest::NAME = "Update space";
const std::string storm::UpdateSpaceRequest::MONITOR_NAME = storm::SRM_UPDATE_SPACE_MONITOR_NAME;

void storm::UpdateSpaceRequest::load(ns1__srmUpdateSpaceRequest* request)
{
	if(request->spaceToken == NULL)
	{
		throw storm::invalid_request("Received NULL spaceToken in the request");
	}

	m_spaceToken = std::string(request->spaceToken);
	validate_token(m_spaceToken);

	if(request->newSizeOfTotalSpaceDesired != NULL)
	{
		m_newSizeOfTotalSpaceDesired = *(request->newSizeOfTotalSpaceDesired);
	}
	if(request->newSizeOfGuaranteedSpaceDesired != NULL)
	{
		m_newSizeOfGuaranteedSpaceDesired = *(request->newSizeOfGuaranteedSpaceDesired);
	}
	if(request->newLifeTime != NULL)
	{
		m_newLifeTime = *(request->newLifeTime);
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

int storm::UpdateSpaceRequest::performXmlRpcCall(ns1__srmUpdateSpaceResponse_* response){

	int ret = ns1__srmUpdateSpace_impl(m_soapRequest, m_request, response);
	if(response->srmUpdateSpaceResponse != NULL && response->srmUpdateSpaceResponse->returnStatus != NULL )
	{
		m_status = response->srmUpdateSpaceResponse->returnStatus->statusCode;
		if(response->srmUpdateSpaceResponse->returnStatus->explanation != NULL)
		{
			m_explanation = std::string(response->srmUpdateSpaceResponse->returnStatus->explanation);
		}
	}
	else
	{
		srmlogit(STORM_LOG_ERROR, __func__, "Xmlrpc call returned with an empty returnStatus.\n");
	}
	return ret;
}

int storm::UpdateSpaceRequest::buildResponse()
{
    srmlogit(STORM_LOG_DEBUG, "storm::UpdateSpaceRequest::buildResponse()", "called.\n");
	if(m_builtResponse != NULL)
	{
		return 0;
	}
	try
	{
		m_builtResponse = storm::soap_calloc<ns1__srmUpdateSpaceResponse>(m_soapRequest);
		if(m_builtResponse == NULL)
		{
			srmlogit(STORM_LOG_ERROR, "storm::UpdateSpaceRequest::buildResponse()", "Unable to allocate memory for the response\n");
			return SOAP_EOM;
		}
		m_builtResponse->returnStatus = storm::soap_calloc<ns1__TReturnStatus>(m_soapRequest);
		if(m_builtResponse->returnStatus == NULL)
		{
			srmlogit(STORM_LOG_ERROR, "storm::UpdateSpaceRequest::buildResponse()", "Unable to allocate memory for the return status\n");
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
