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

#ifndef BOL_STATUS_HPP
#define BOL_STATUS_HPP

#include <string>
#include "file_status.hpp"

using namespace storm;

namespace storm{

    class bol_status : public storm::file_status<ns1__srmStatusOfBringOnlineRequestResponse>
    {
    public:
        bol_status(struct soap * soap);
        void load(struct srm_dbfd *db, const std::string& requestToken);
        ns1__srmStatusOfBringOnlineRequestResponse * response();

        void add_requested_surl(const std::string &d){
            _surls_req.push_back(storm::bol_status::surl_req_t(d));
        }

    protected:
        class surl_t {
        public:
            surl_t(std::string src): source(src){};
            surl_t(std::string src, bool isdir): source(src), has_diroption(true){isdirectory = isdir; n_levels = -1; allrecursive = -1;};
            virtual ~surl_t(){};
            std::string source;
            std::string turl;

            bool has_diroption;
            bool isdirectory;
            int allrecursive; // -1 means not supplied
            int n_levels; // -1 means not supplied

            storm_size_t fileSize;
            storm_time_t estimatedWaitTime; // can be -1
            storm_time_t remainingPinLifetime; // can be -1
            ns1__TStatusCode status;
            std::string explanation;
        };

        class surl_req_t {
        public:
            surl_req_t(const std::string &d) : src(d){};
            const std::string &source() const {return src;};
        private:
            sql_string src;
        };
    private:
        void __fill_bol_request();
        std::string __format_surl_request();

        std::vector<storm::bol_status::surl_t> _surls;
        std::vector<storm::bol_status::surl_req_t> _surls_req;
    };
}

#endif
