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

#include "ReserveSpaceRequest.hpp"
#include "srmlogit.h"
#include "token_validator.hpp"

void storm::ReserveSpaceRequest::load(ns1__srmReserveSpaceRequest* request)
{
	if (request->retentionPolicyInfo == NULL) {
		throw storm::invalid_request("retentionPolicyInfo is NULL");
	}
	if (request->desiredSizeOfTotalSpace == NULL) {
		throw storm::invalid_request("desiredSizeOfTotalSpace array is empty");
	}
	m_retentionPolicy = request->retentionPolicyInfo->retentionPolicy;
	m_desiredSizeOfTotalSpace = *(request->desiredSizeOfTotalSpace);
	if (request->retentionPolicyInfo->accessLatency != NULL) {
		m_accessLatency = *(request->retentionPolicyInfo->accessLatency);
	}
	if (request->userSpaceTokenDescription != NULL) {
		storm::validate_token_description(std::string(request->userSpaceTokenDescription));
		m_userSpaceTokenDescription = *(request->userSpaceTokenDescription);
	}
	m_desiredSizeOfGuaranteedSpace = request->desiredSizeOfGuaranteedSpace;
	if (request->desiredLifetimeOfReservedSpace != NULL) {
		m_desiredLifetimeOfReservedSpace = *(request->desiredLifetimeOfReservedSpace);
	}
	if (request->transferParameters != NULL) {
		if (request->transferParameters->accessPattern != NULL) {
			m_accessPattern = *(request->transferParameters->accessPattern);
		}
		if (request->transferParameters->connectionType != NULL) {
			m_connectionType = *(request->transferParameters->connectionType);
		}
		if (request->transferParameters->arrayOfClientNetworks != NULL
				&& request->transferParameters->arrayOfClientNetworks->__sizestringArray
						> 0 && request->transferParameters->arrayOfClientNetworks->stringArray != NULL) {
			for(int i = 0 ; i < request->transferParameters->arrayOfClientNetworks->__sizestringArray ; ++i)
			{
				m_arrayOfClientNetworks.push_back(std::string(request->transferParameters->arrayOfClientNetworks->stringArray[i]));
			}
		}
		if (request->transferParameters->arrayOfTransferProtocols != NULL
				&& request->transferParameters->arrayOfTransferProtocols->__sizestringArray
				> 0 && request->transferParameters->arrayOfTransferProtocols->stringArray != NULL) {
			for(int i = 0 ; i < request->transferParameters->arrayOfTransferProtocols->__sizestringArray ; ++i)
			{
				m_arrayOfTransferProtocols.push_back(std::string(request->transferParameters->arrayOfTransferProtocols->stringArray[i]));
			}
		}
	}
	if (request->arrayOfExpectedFileSizes != NULL
			&& request->arrayOfExpectedFileSizes->__sizeunsignedLongArray
			> 0 && request->arrayOfExpectedFileSizes->unsignedLongArray != NULL) {
		for(int i = 0 ; i < request->arrayOfExpectedFileSizes->__sizeunsignedLongArray ; ++i)
		{
			m_arrayOfExpectedFileSizes.push_back(request->arrayOfExpectedFileSizes->unsignedLongArray[i]);
		}
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

int storm::ReserveSpaceRequest::performXmlRpcCall(ns1__srmReserveSpaceResponse_* response){
	char *funcName = "ReserveSpaceRequest::performXmlRpcCall()";
	int ret = ns1__srmReserveSpace_impl(m_soapRequest, m_request, response);
	if(response->srmReserveSpaceResponse != NULL && response->srmReserveSpaceResponse->returnStatus != NULL )
	{
		m_status = response->srmReserveSpaceResponse->returnStatus->statusCode;
		if(response->srmReserveSpaceResponse->returnStatus->explanation != NULL)
		{
			m_explanation = std::string(response->srmReserveSpaceResponse->returnStatus->explanation);
		}
	}
	else
	{
		srmlogit(STORM_LOG_ERROR, "storm::ReserveSpaceRequest::performXmlRpcCall()", "Xmlrpc call returned with an empty returnStatus.\n");
	}
	return ret;
}

int storm::ReserveSpaceRequest::buildResponse()
{
    srmlogit(STORM_LOG_DEBUG, "storm::ReserveSpaceRequest::buildResponse()", "called.\n");
	if(m_builtResponse != NULL)
	{
		return 0;
	}
	try
	{
		m_builtResponse = storm::soap_calloc<ns1__srmReserveSpaceResponse>(m_soapRequest);
		if(m_builtResponse == NULL)
		{
			srmlogit(STORM_LOG_ERROR, "storm::ReserveSpaceRequest::buildResponse()", "Unable to allocate memory for the response\n");
			return SOAP_EOM;
		}
		m_builtResponse->returnStatus = storm::soap_calloc<ns1__TReturnStatus>(m_soapRequest);
		if(m_builtResponse->returnStatus == NULL)
		{
			srmlogit(STORM_LOG_ERROR, "storm::ReserveSpaceRequest::buildResponse()", "Unable to allocate memory for the return status\n");
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
