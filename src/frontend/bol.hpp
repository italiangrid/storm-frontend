#ifndef __BOL_HPP
#define __BOL_HPP
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
class bol: public file_request<ns1__srmBringOnlineRequest, ns1__srmBringOnlineResponse> {
public:
    bol(struct soap *soap) :
        file_request<ns1__srmBringOnlineRequest, ns1__srmBringOnlineResponse> (soap) {
        _response = NULL;
    }
    ;
    void insert(struct srm_dbfd *dbfd);
    void load(ns1__srmBringOnlineRequest *req);
    ns1__srmBringOnlineResponse * response();

private:
    class surl_t {
    public:
//        surl_t(std::string src) :
//            source(src), has_diroption(false), status(SRM_USCOREREQUEST_USCOREQUEUED) {
//        };
//        surl_t(std::string src, bool isdir) :
//            source(src), has_diroption(true), status(SRM_USCOREREQUEST_USCOREQUEUED) {
//            isdirectory = isdir;
//            n_levels = -1;
//            allrecursive = -1;
//        };
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
            estimatedWaitTime = -1;
        };
        virtual ~surl_t() {
        }
        ;
        sql_string source;
        bool has_diroption;
        bool isdirectory;
        int allrecursive; // -1 means not supplied
        int n_levels; // -1 means not supplied
        unsigned long fileSize;
        int remainingPinTime;
        int estimatedWaitTime;
        ns1__TStatusCode status;
        std::string explanation;
    };

    int _remainingTotalRequestTime;
    int _deferredStartTime;
    std::vector<bol::surl_t> _surls;
    struct ns1__srmBringOnlineResponse * _response;

};
}
#endif // __BOL_HPP
