#ifndef __PTP_HPP
#define __PTP_HPP
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
