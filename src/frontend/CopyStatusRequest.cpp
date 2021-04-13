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

#include "CopyStatusRequest.hpp"
#include "CopySurl.hpp"
#include "CopyTurl.hpp"
#include "srmlogit.h"

const std::string storm::CopyStatusRequest::NAME = "Copy Status";
const std::string storm::CopyStatusRequest::MONITOR_NAME = storm::SRM_COPY_MONITOR_NAME;

void storm::CopyStatusRequest::load(ns1__srmStatusOfCopyRequestRequest*)
{
    srmlogit(STORM_LOG_DEBUG, "storm::CopyStatusRequest::load", "empty cause srmStatusOfCopy is not supported");

}

void storm::CopyStatusRequest::loadFromDB(struct srm_dbfd*){

    srmlogit(STORM_LOG_DEBUG, "storm::CopyStatusRequest::loadFromDB", "empty cause srmStatusOfCopy is not supported");

}

ns1__srmStatusOfCopyRequestResponse* storm::CopyStatusRequest::buildResponse()
{
    srmlogit(STORM_LOG_DEBUG, "storm::CopyStatusRequest::buildResponse()", "called.\n");

	try
	{
		m_builtResponse = storm::soap_calloc<ns1__srmStatusOfCopyRequestResponse>(m_soapRequest);
		m_builtResponse->returnStatus = storm::soap_calloc<ns1__TReturnStatus>(m_soapRequest);
	} catch (std::invalid_argument& exc) {
		throw std::logic_error("Unable to allocate memory for the response. invalid_argument Exception: "
				+ std::string(exc.what()));
	}

	m_builtResponse->returnStatus->statusCode = SRM_USCORENOT_USCORESUPPORTED;
	m_builtResponse->returnStatus->explanation = const_cast<char*>("srmStatusCopy operation is not supported");

    return m_builtResponse;
}

void storm::CopyStatusRequest::addMissingSurls()
{
	srmlogit(STORM_LOG_DEBUG, "storm::CopyStatusRequest::addMissingSurls", "empty cause srmStatusOfCopy is not supported");
}
