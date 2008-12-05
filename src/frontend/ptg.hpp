#ifndef PTG_HPP_
#define PTG_HPP_

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

namespace storm {
    
class ptg : public storm::file_request<ns1__srmPrepareToGetRequest, ns1__srmPrepareToGetResponse> {
public:
    ptg(struct soap *soap) : storm::file_request<ns1__srmPrepareToGetRequest, ns1__srmPrepareToGetResponse>(soap) { _response=NULL; };
    void insert(struct srm_dbfd *dbfd);
    void load(ns1__srmPrepareToGetRequest *req);
    struct ns1__srmPrepareToGetResponse* response();

private:
    class surl_t {
    public:
        surl_t(std::string src, ns1__TDirOption *dirOption) : sourceSURL(src), status(SRM_USCOREREQUEST_USCOREQUEUED) {
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
        };
        virtual ~surl_t() {};
        
        sql_string sourceSURL;
        bool has_diroption;
        bool isdirectory;
        int allLevelRecursive; // -1 means not supplied
        int n_levels; // -1 means not supplied
        ns1__TStatusCode status;
        std::string explanation;
    };
    
    std::vector<ptg::surl_t> _surls;
    struct ns1__srmPrepareToGetResponse * _response;
    std::vector<sql_string> _protocols;
    
};

}
#endif /*PTG_HPP_*/
