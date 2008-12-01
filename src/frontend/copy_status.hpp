#ifndef COPY_STATUS_HPP
#define COPY_STATUS_HPP
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
