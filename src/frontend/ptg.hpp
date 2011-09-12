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

#ifndef PTG_HPP_
#define PTG_HPP_

// C includes
#include "srm_server.h"
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

class ptg: public storm::file_request<ns1__srmPrepareToGetRequest, ns1__srmPrepareToGetResponse> {
public:
    ptg(struct soap *soap) :
        storm::file_request<ns1__srmPrepareToGetRequest, ns1__srmPrepareToGetResponse>(soap) {
        _response = NULL;
    }
    ;
    void insert(struct srm_dbfd *dbfd);
    void load(ns1__srmPrepareToGetRequest *req);
    
    /*
     * Returns true if the ptg command supports the option for specify the preferred transfer protocol
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
    struct ns1__srmPrepareToGetResponse* response();

private:
    class surl_t {
    public:
        surl_t(std::string src, ns1__TDirOption *dirOption) :
            sourceSURL(src), status(SRM_USCOREREQUEST_USCOREQUEUED) {

            if (dirOption == NULL) {
                has_diroption = false;
            } else {
                has_diroption = true;
                isdirectory = dirOption->isSourceADirectory;
                if (dirOption->allLevelRecursive == NULL) {
                    allLevelRecursive = -1;
                } else {
                    if (*(dirOption->allLevelRecursive)) {
                        allLevelRecursive = 1;
                    } else {
                        allLevelRecursive = 0;
                    }
                }
                if (dirOption->numOfLevels == NULL) {
                    n_levels = -1;
                } else {
                    n_levels = *(dirOption->numOfLevels);
                }
            }
        };
        virtual ~surl_t() {};

        sql_string sourceSURL;
        bool has_diroption;
        bool isdirectory;
        int allLevelRecursive; // -1 means not supplied
        int n_levels; // -1 means not supplied
        ns1__TStatusCode status;
        std::string explanation;
    };

    std::vector<ptg::surl_t> _surls;
    struct ns1__srmPrepareToGetResponse * _response;
    std::vector<sql_string> _protocols;

};

}
#endif /*PTG_HPP_*/
