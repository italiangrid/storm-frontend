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

#include <sstream>

#include "CopyRequest.hpp"
#include "mysql_query.hpp"
#include "storm_mysql.h"

#include "srmlogit.h"

const std::string storm::CopyRequest::NAME = "Copy";
const std::string storm::CopyRequest::MONITOR_NAME = storm::SRM_COPY_MONITOR_NAME;

bool storm::CopyRequest::supportsProtocolSpecification()
{
	return false;
}

void storm::CopyRequest::load(struct ns1__srmCopyRequest* req)
{
    if (NULL == req) {
        throw storm::invalid_request("Request is NULL");
    }

    srmlogit(STORM_LOG_DEBUG, "storm::CopyRequest::load", "empty cause srmCopy is not supported");
}


ns1__srmCopyResponse* storm::CopyRequest::buildResponse()
{
	srmlogit(STORM_LOG_DEBUG, "storm::CopyRequest::buildResponse", "called");

	try
	{
		m_builtResponse = storm::soap_calloc<ns1__srmCopyResponse>(m_soapRequest);
		m_builtResponse->returnStatus = storm::soap_calloc<ns1__TReturnStatus>(m_soapRequest);
	} catch (std::invalid_argument& exc) {
		throw storm::storm_error("Unable to allocate memory for the response. invalid_argument Exception: " + std::string(exc.what()));
	}
    m_builtResponse->returnStatus->statusCode = SRM_USCORENOT_USCORESUPPORTED;
    m_builtResponse->returnStatus->explanation = const_cast<char*>("srmCopy operation is not supported");

    return m_builtResponse;
}

void storm::CopyRequest::insertIntoDB(struct srm_dbfd *)
{
	srmlogit(STORM_LOG_DEBUG, "storm::CopyRequest::insertIntoDB", "empty cause srmCopy is not supported");
}

std::string storm::CopyRequest::getSurlsList()
{
	srmlogit(STORM_LOG_DEBUG, "storm::CopyRequest::getSurlsList", "empty cause srmCopy is not supported");

	return "";
}
