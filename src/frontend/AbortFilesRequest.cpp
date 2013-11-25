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

#include "AbortFilesRequest.hpp"
#include "srmlogit.h"
#include "Surl.hpp"

const std::string storm::AbortFilesRequest::NAME = "Abort files";
const std::string storm::AbortFilesRequest::MONITOR_NAME = storm::SRM_ABORT_FILES_MONITOR_NAME;

void storm::AbortFilesRequest::load(ns1__srmAbortFilesRequest* request)
{
	if (request->requestToken == NULL)
	{
		throw storm::invalid_request("requestToken is NULL");
	}
	if (request->arrayOfSURLs == NULL || request->arrayOfSURLs->__sizeurlArray == 0 || request->arrayOfSURLs->urlArray == NULL) {
		throw storm::invalid_request("SURLs array is NULL");
	}

	m_requestToken = std::string(request->requestToken);
	validate_token(m_requestToken);

	for (int i = 0; i < request->arrayOfSURLs->__sizeurlArray; ++i) {
		m_surls.insert(storm::normalize_surl(std::string(request->arrayOfSURLs->urlArray[i])));
	}
}

int storm::AbortFilesRequest::performXmlRpcCall(ns1__srmAbortFilesResponse_* response){

	int ret = ns1__srmAbortFiles_impl(m_soapRequest, m_request, response);
	if(response->srmAbortFilesResponse != NULL && response->srmAbortFilesResponse->returnStatus != NULL )
	{
		m_status = response->srmAbortFilesResponse->returnStatus->statusCode;
		if(response->srmAbortFilesResponse->returnStatus->explanation != NULL)
		{
			m_explanation = std::string(response->srmAbortFilesResponse->returnStatus->explanation);
		}
	}
	else
	{
		srmlogit(STORM_LOG_ERROR, __func__, "Xmlrpc call returned with an empty returnStatus.\n");
	}
	return ret;
}

int storm::AbortFilesRequest::buildResponse()
{
    srmlogit(STORM_LOG_DEBUG, "storm::AbortFilesRequest::buildResponse()", "called.\n");
    if(m_builtResponse != NULL)
	{
		return 0;
	}
	try
	{
		m_builtResponse = storm::soap_calloc<ns1__srmAbortFilesResponse>(m_soapRequest);
		if(m_builtResponse == NULL)
		{
			srmlogit(STORM_LOG_ERROR, "storm::AbortFilesRequest::buildResponse()", "Unable to allocate memory for the response\n");
			return SOAP_EOM;
		}
		m_builtResponse->returnStatus = storm::soap_calloc<ns1__TReturnStatus>(m_soapRequest);
		if(m_builtResponse->returnStatus == NULL)
		{
			srmlogit(STORM_LOG_ERROR, "storm::AbortFilesRequest::buildResponse()", "Unable to allocate memory for the return status\n");
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
