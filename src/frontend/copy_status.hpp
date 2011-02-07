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

#ifndef COPY_STATUS_HPP
#define COPY_STATUS_HPP

#include <string>
#include "file_status.hpp"

using namespace storm;

namespace storm{

    class copy_status : public storm::file_status<ns1__srmStatusOfCopyRequestResponse>
    {
    public:
        copy_status(struct soap * soap);
        void load(struct srm_dbfd *db, const std::string& requestToken);
//         void load(struct srm_dbfd *db, const std::string &requestToken, const std::vector<const std::string> &surls);
        ns1__srmStatusOfCopyRequestResponse * response();

        void add_requested_surl(const std::string &s, const std::string &d){
            _surls_req.push_back(storm::copy_status::surl_req_t(s,d));
        }

    protected:
        class surl_t {
        public:
            surl_t(std::string src, std::string dst):source(src), target(dst),  has_diroption(false){};
            surl_t(std::string src, std::string dst, bool isdir):source(src), target(dst), has_diroption(true){isdirectory = isdir; n_levels = -1; allrecursive = -1;};
            virtual ~surl_t(){};
            std::string source;
            std::string target;
            bool has_diroption;
            bool isdirectory;
            int allrecursive; // -1 means not supplied
            int n_levels; // -1 means not supplied

            storm_size_t fileSize;
            storm_time_t estimatedWaitTime; // can be -1
            storm_time_t remainingFileLifetime; // can be -1
            ns1__TStatusCode status;
            std::string explanation;
        };

        class surl_req_t {
        public:
            surl_req_t(const std::string s, const std::string &d) : src(s), dst(d){};
            const std::string &source() const {return src;};
            const std::string &target() const {return dst;};
        private:
            sql_string src;
            sql_string dst;
        };
    private:
        void __fill_copy_request();
        std::string __format_surl_request();

        std::vector<storm::copy_status::surl_t> _surls;
        std::vector<storm::copy_status::surl_req_t> _surls_req;
    };
}

#endif
