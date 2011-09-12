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

#ifndef __PTP_HPP
#define __PTP_HPP

// C includes
#include "srmv2H.h"
#include "storm_util.h"
#include "srmlogit.h"

// STL includes
#include <string>
#include <vector>
#include <map>

// storm_db include
#include "mysql_query.hpp"

// parent
#include "file_request.hpp"

namespace storm {
class ptp : public storm::file_request<ns1__srmPrepareToPutRequest, ns1__srmPrepareToPutResponse> {
public:
    ptp(struct soap *soap) : storm::file_request<ns1__srmPrepareToPutRequest, ns1__srmPrepareToPutResponse>(soap){_response=NULL;};
    void insert(struct srm_dbfd *dbfd);
    void load(ns1__srmPrepareToPutRequest *req);
    
    /*
     * Returns true if the ptp command supports the option for specify the preferred transfer protocol
     * */
    bool supportsProtocolSpecification();
    
    /*
     * Returns the array of requested preferred transfer protocols
     * */
    vector<sql_string>* getRequestedProtocols();
    
    /*
     * Sets the array of requested preferred transfer protocols to the provided one
     * */
	void setProtocolVector(vector<sql_string>* protocolVector);
	/*
	 * Set the status code at SURL level to SRM_FAILURE to all requested SURLs
	 */
	void setGenericFailureSurls();
    struct ns1__srmPrepareToPutResponse * response();

private:
    class surl_t {
    public:
        surl_t(std::string s):surl(s), has_expected_size(false), status(SRM_USCOREREQUEST_USCOREQUEUED){};
        surl_t(std::string s, storm_size_t e):surl(s), has_expected_size(true), status(SRM_USCOREREQUEST_USCOREQUEUED){ expected_size=e;};
        virtual ~surl_t(){};
        sql_string surl;
        bool has_expected_size;
        storm_size_t expected_size;
        ns1__TStatusCode status;
        std::string explanation;
    };

    void set_surl_status(ns1__TStatusCode status);

    std::vector<ptp::surl_t> _surls;
    struct ns1__srmPrepareToPutResponse * _response;
    std::vector<sql_string> _protocols;

};
}
#endif // __PTP_HPP
