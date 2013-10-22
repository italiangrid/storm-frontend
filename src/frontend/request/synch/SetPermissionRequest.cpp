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

#include "SetPermissionRequest.hpp"
#include "srmlogit.h"
#include "Surl.hpp"

void storm::SetPermissionRequest::load(ns1__srmSetPermissionRequest* request)
{
	if(request->SURL == NULL)
	{
		throw storm::invalid_request("Received NULL surl in the request");
	}
	m_surls.insert(storm::normalize_surl(std::string(request->SURL)));
	m_permissionType = request->permissionType;
	if(request->ownerPermission != NULL)
	{
		m_ownerPermission = *request->ownerPermission;
	}
	if(request->otherPermission != NULL)
	{
		m_otherPermission = *request->otherPermission;
	}
	if(request->arrayOfGroupPermissions != NULL && request->arrayOfGroupPermissions->__sizegroupPermissionArray > 0)
	{
		for(int i = 0; i < request->arrayOfGroupPermissions->__sizegroupPermissionArray; ++i)
		{
			std::pair<std::string,ns1__TPermissionMode> permission =
					std::make_pair(std::string(request->arrayOfGroupPermissions->groupPermissionArray[i]->groupID), request->arrayOfGroupPermissions->groupPermissionArray[i]->mode);
			m_groupPermissions.insert(permission);
		}
	}
	if(request->arrayOfUserPermissions != NULL && request->arrayOfUserPermissions->__sizeuserPermissionArray > 0)
	{
		for(int i = 0; i < request->arrayOfUserPermissions->__sizeuserPermissionArray; ++i)
		{
			std::pair<std::string,ns1__TPermissionMode> permission =
					std::make_pair(std::string(request->arrayOfUserPermissions->userPermissionArray[i]->userID), request->arrayOfUserPermissions->userPermissionArray[i]->mode);
			m_userPermissions.insert(permission);
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

int storm::SetPermissionRequest::performXmlRpcCall(ns1__srmSetPermissionResponse_* response){
	char *funcName = "SetPermissionRequest::performXmlRpcCall()";
	int ret = ns1__srmSetPermission_impl(m_soapRequest, m_request, response);
	if(response->srmSetPermissionResponse != NULL && response->srmSetPermissionResponse->returnStatus != NULL )
	{
		m_status = response->srmSetPermissionResponse->returnStatus->statusCode;
		if(response->srmSetPermissionResponse->returnStatus->explanation != NULL)
		{
			m_explanation = std::string(response->srmSetPermissionResponse->returnStatus->explanation);
		}
	}
	else
	{
		srmlogit(STORM_LOG_ERROR, "storm::SetPermissionRequest::performXmlRpcCall()", "Xmlrpc call returned with an empty returnStatus.\n");
	}
	return ret;
}

int storm::SetPermissionRequest::buildResponse()
{
    srmlogit(STORM_LOG_DEBUG, "storm::SetPermissionRequest::buildResponse()", "called.\n");
	if(m_builtResponse != NULL)
	{
		return 0;
	}
	try
	{
		m_builtResponse = storm::soap_calloc<ns1__srmSetPermissionResponse>(m_soapRequest);
		if(m_builtResponse == NULL)
		{
			srmlogit(STORM_LOG_ERROR, "storm::SetPermissionRequest::buildResponse()", "Unable to allocate memory for the response\n");
			return SOAP_EOM;
		}
		m_builtResponse->returnStatus = storm::soap_calloc<ns1__TReturnStatus>(m_soapRequest);
		if(m_builtResponse->returnStatus == NULL)
		{
			srmlogit(STORM_LOG_ERROR, "storm::SetPermissionRequest::buildResponse()", "Unable to allocate memory for the return status\n");
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
