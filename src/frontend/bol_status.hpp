#ifndef BOL_STATUS_HPP
#define BOL_STATUS_HPP
// Copyright (C) 2006 by Antonio Messina <antonio.messina@ictp.it> for the ICTP project EGRID.
/*
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by the
 *  Free Software Foundation; either version 2 of the License, or (at your
 *  option) any later version.
 *
 *  This program is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

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
        long _recalltablePort;
        std::string _recalltableHost;

        std::vector<storm::bol_status::surl_t> _surls;
        std::vector<storm::bol_status::surl_req_t> _surls_req;
    };
}

#endif
