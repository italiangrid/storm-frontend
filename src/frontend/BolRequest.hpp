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

#ifndef BOL_REQUEST_HPP
#define BOL_REQUEST_HPP

#include "file_request.hpp"
#include "BolSurl.hpp"

namespace storm {

class BolRequest: public file_request<ns1__srmBringOnlineRequest, ns1__srmBringOnlineResponse> {
public:
	BolRequest(struct soap* soapRequest, struct ns1__srmBringOnlineRequest* bolRequest):
			file_request<ns1__srmBringOnlineRequest, ns1__srmBringOnlineResponse > (soapRequest) {
        m_requestType = DB_BOL_REQUEST;
		m_deferredStartTime = -1;
		m_pinLifetime = -1;
		m_desiredFileStorageType = NULL;
		this->load(bolRequest);
    }

    void insertIntoDB(struct srm_dbfd* dbfd);

    void load(ns1__srmBringOnlineRequest* req);

    /*
     * Returns true if the bol command supports the option for specify the preferred transfer protocol
     * */
    bool supportsProtocolSpecification();
    
	struct ns1__srmBringOnlineResponse* buildResponse();

	static const std::string NAME;
	static const std::string MONITOR_NAME;

private:
    storm_time_t m_deferredStartTime; // -1 = not specified
	storm_time_t m_pinLifetime; // -1 = not specified
	ns1__TFileStorageType* m_desiredFileStorageType; //ignored
};
}
#endif // BOL_REQUEST_HPP
