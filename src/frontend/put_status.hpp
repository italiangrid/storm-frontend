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

#ifndef PUT_STATUS_HPP
#define PUT_STATUS_HPP

#include <string>
#include "file_status.hpp"

using namespace storm;

namespace storm{

    class put_status : public storm::file_status<ns1__srmStatusOfPutRequestResponse>
    {
    public:
        put_status(struct soap * soap);
        void load(struct srm_dbfd *db, const std::string& requestToken);
//         void load(struct srm_dbfd *db, const std::string &requestToken, const std::vector<const std::string> &surls);
        ns1__srmStatusOfPutRequestResponse * response();

        void add_requested_surl(const std::string &d){
            _surls_req.push_back(storm::put_status::surl_req_t(d));
        }

    protected:
        class surl_t {
        public:
            surl_t(std::string dst): target(dst){};
            virtual ~surl_t(){};
            std::string target;
            std::string turl;

            storm_size_t expectedFileSize;
            storm_size_t fileSize;
            storm_time_t estimatedWaitTime; // can be -1
            storm_time_t remainingPinLifetime; // can be -1
            storm_time_t remainingFileLifetime; // can be -1
            ns1__TStatusCode status;
            std::string explanation;
        };

        class surl_req_t {
        public:
            surl_req_t(const std::string &d) : dst(d){};
            const std::string &target() const {return dst;};
        private:
            sql_string dst;
        };
    private:
        void __fill_put_request();
        std::string __format_surl_request();

        std::vector<storm::put_status::surl_t> _surls;
        std::vector<storm::put_status::surl_req_t> _surls_req;
    };
}

#endif
