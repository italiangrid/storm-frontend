#ifndef PUT_STATUS_HPP
#define PUT_STATUS_HPP
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
