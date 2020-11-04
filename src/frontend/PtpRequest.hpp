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

#ifndef PTP_REQUEST_HPP
#define PTP_REQUEST_HPP

#include "file_request.hpp"
#include "PtpSurl.hpp"

namespace storm {

class PtpRequest: public file_request<ns1__srmPrepareToPutRequest, ns1__srmPrepareToPutResponse> {
public:
	PtpRequest(struct soap* soapRequest, struct ns1__srmPrepareToPutRequest* ptpRequest):
			file_request<ns1__srmPrepareToPutRequest, ns1__srmPrepareToPutResponse >(soapRequest) {
		m_requestType = DB_PUT_REQUEST;
		m_pinLifetime = -1;
		m_fileLifetime = -1;
		m_overwrite = DB_OVERWRITE_UNKNOWN;
		m_desiredFileStorageType = NULL;
		this->load(ptpRequest);
	}

    void insertIntoDB(struct srm_dbfd* dbfd);

    void load(ns1__srmPrepareToPutRequest* req);
    
    /*
     * Returns true if the ptp command supports the option for specify the preferred transfer protocol
     * */
    bool supportsProtocolSpecification();

    struct ns1__srmPrepareToPutResponse* buildResponse();

    static const std::string NAME;
    static const std::string MONITOR_NAME;

private:
    storm_time_t m_pinLifetime; // -1 = not specified
    storm_time_t m_fileLifetime; // -1 = not specified
    char m_overwrite;
    ns1__TFileStorageType* m_desiredFileStorageType; //ignored
};
}
#endif // PTP_REQUEST_HPP
