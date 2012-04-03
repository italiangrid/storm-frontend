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

#include "PutDoneRequest.hpp"
#include "srmlogit.h"

void storm::PutDoneRequest::load(ns1__srmPutDoneRequest* request) throw (storm::invalid_request)
{
	if (request->requestToken == NULL)
	{
		throw storm::invalid_request("requestToken is NULL");
	}
	if (request->arrayOfSURLs == NULL || request->arrayOfSURLs->__sizeurlArray == 0 || request->arrayOfSURLs->urlArray == NULL) {
		throw storm::invalid_request("SURLs array is NULL");
	}
	m_requestToken = std::string(request->requestToken);
	for (int i = 0; i < request->arrayOfSURLs->__sizeurlArray; ++i) {
		m_surls.insert(std::string(request->arrayOfSURLs->urlArray[i]));
	}
}

int storm::PutDoneRequest::performXmlRpcCall(ns1__srmPutDoneResponse_* response){
	char *funcName = "PutDoneRequest::performXmlRpcCall()";
	int ret = ns1__srmPutDone_impl(m_soapRequest, m_request, response);
	if(response->srmPutDoneResponse != NULL && response->srmPutDoneResponse->returnStatus != NULL )
	{
		m_status = response->srmPutDoneResponse->returnStatus->statusCode;
		if(response->srmPutDoneResponse->returnStatus->explanation != NULL)
		{
			m_explanation = std::string(response->srmPutDoneResponse->returnStatus->explanation);
		}
	}
	else
	{
		srmlogit(STORM_LOG_ERROR, "storm::PutDoneRequest::performXmlRpcCall()", "Xmlrpc call returned with an empty returnStatus.\n");
	}
	return ret;
}

void storm::PutDoneRequest::buildResponse() throw (std::logic_error, storm::InvalidResponse)
{
    srmlogit(STORM_LOG_DEBUG, "storm::PutDoneRequest::buildResponse()", "called.\n");
	//fake
}
