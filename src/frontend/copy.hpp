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

#ifndef __COPY_HPP
#define __COPY_HPP

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
namespace storm{
class copy : public file_request<ns1__srmCopyRequest, ns1__srmCopyResponse> {
public:
    copy(struct soap *soap) : file_request<ns1__srmCopyRequest, ns1__srmCopyResponse>(soap){_response=NULL;};
    void insert(struct srm_dbfd *dbfd);
    void load(ns1__srmCopyRequest *req);
    ns1__srmCopyResponse * response();

private:
    class surl_t {
    public:
        surl_t(std::string src, std::string dst):source(src), target(dst),  has_diroption(false), status(SRM_USCOREREQUEST_USCOREQUEUED){};
        surl_t(std::string src, std::string dst, bool isdir):source(src), target(dst), has_diroption(true), status(SRM_USCOREREQUEST_USCOREQUEUED){isdirectory = isdir; n_levels = -1; allrecursive = -1;};
        virtual ~surl_t(){};
        sql_string source;
        sql_string target;
        bool has_diroption;
        bool isdirectory;
        int allrecursive; // -1 means not supplied
        int n_levels; // -1 means not supplied
        ns1__TStatusCode status;
        std::string explanation;
    };
    
    std::vector<copy::surl_t> _surls;
    struct ns1__srmCopyResponse * _response;
    
};
}
#endif // __COPY_HPP
