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

#ifndef PTG_REQUEST_HPP
#define PTG_REQUEST_HPP

#include "file_request.hpp"
#include "PtgSurl.hpp"

namespace storm {

class PtgRequest: public file_request<ns1__srmPrepareToGetRequest, ns1__srmPrepareToGetResponse> {
public:
	PtgRequest(struct soap* soapRequest, struct ns1__srmPrepareToGetRequest* ptgRequest) throw (invalid_request) :
        	storm::file_request<ns1__srmPrepareToGetRequest, ns1__srmPrepareToGetResponse >(soapRequest) {
		m_requestType = DB_GET_REQUEST;
		m_pinLifetime = -1;
		m_desiredFileStorageType = NULL;
		this->load(ptgRequest);
    }

    void insertIntoDB(struct srm_dbfd* dbfd) throw (std::logic_error , storm_db::mysql_exception);

    void load(ns1__srmPrepareToGetRequest* req) throw (invalid_request) ;
    
    /*
     * Returns true if the ptg command supports the option for specify the preferred transfer protocol
     * */
    bool supportsProtocolSpecification();

    struct ns1__srmPrepareToGetResponse* buildResponse() throw (std::logic_error , storm::InvalidResponse);

private:
    storm_time_t m_pinLifetime; // -1 = not specified
    ns1__TFileStorageType* m_desiredFileStorageType; //ignored
};
}
#endif // PTG_REQUEST_HPP
