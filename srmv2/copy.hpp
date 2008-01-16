#ifndef __COPY_HPP
#define __COPY_HPP
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
#include "storm_functions.h"
#include "serrno.h"
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
