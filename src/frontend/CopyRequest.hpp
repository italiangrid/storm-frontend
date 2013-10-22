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

#ifndef COPY_REQUEST_HPP
#define COPY_REQUEST_HPP

#include "file_request.hpp"
#include "CopySurl.hpp"

namespace storm {

class CopyRequest : public file_request<ns1__srmCopyRequest, ns1__srmCopyResponse> {
public:
	CopyRequest(struct soap* soapRequest, struct ns1__srmCopyRequest* copyRequest):
			file_request<ns1__srmCopyRequest, ns1__srmCopyResponse >(soapRequest) {
		m_requestType = DB_COPY_REQUEST;
		m_fileLifetime = -1;
		m_overwrite = DB_OVERWRITE_UNKNOWN;
		m_targetFileStorageType = NULL;
		m_targetStorageSystemInfo = NULL;
		this->load(copyRequest);
	};

    void insertIntoDB(struct srm_dbfd *dbfd);;

    void load(ns1__srmCopyRequest *req);

    bool supportsProtocolSpecification();

	struct ns1__srmCopyResponse* buildResponse();

	std::string getSurlsList();

private:
    storm_time_t m_fileLifetime; // -1 = not specified --> stands for desiredTargetSURLLifeTime
    char m_overwrite;
    ns1__TFileStorageType* m_targetFileStorageType; //ignored
    ns1__ArrayOfTExtraInfo* m_targetStorageSystemInfo; //ignored
};
}
#endif // COPY_REQUEST_HPP
