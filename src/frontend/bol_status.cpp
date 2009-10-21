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
#include "bol_status.hpp"
#include "srmlogit.h"
#include "FrontendConfiguration.hpp"
#include "HttpPostClient.h"

using namespace storm;

namespace storm {
bol_status::bol_status(struct soap * soap) :
    file_status<ns1__srmStatusOfBringOnlineRequestResponse> (soap) {

    _recalltableEnabled = FrontendConfiguration::getInstance()->isRecaltableEnabled();

    if (_recalltableEnabled) {
        _recalltablePort = (long) FrontendConfiguration::getInstance()->getRecalltablePort();
        _recalltableHost = FrontendConfiguration::getInstance()->getXmlRpcHost();
    }
}

void bol_status::load(struct srm_dbfd *db, const std::string &requestToken) {

    srmlogit(STORM_LOG_DEBUG, "bol_status::load", "R_token: %s\n", requestToken.c_str());

    _empty_results();

    //std::string to_add = __format_surl_request();

    _query = "SELECT * FROM request_queue r JOIN "
        "(request_BoL c, status_BoL s) ON "
        "(c.request_queueID=r.ID AND s.request_BoLID=c.ID) "
        "LEFT JOIN request_DirOption d ON c.request_DirOptionID=d.ID "
        "WHERE r.r_token='" + requestToken + "'";//+to_add;

    file_status<ns1__srmStatusOfBringOnlineRequestResponse>::load(db, requestToken);

    if (_results.size() == 0) {
        throw token_not_found();
    }

    // TEMPORARY SOLUTION! TODO: find another way to manage the case of no SURLs specified in the request.
    if (_surls_req.size() == 0) {
        file_status_results_t::iterator i = _results.begin();
        for (; i != _results.end(); ++i)
            add_requested_surl((*i)["sourceSURL"]);
    }

    __fill_bol_request();
}

ns1__srmStatusOfBringOnlineRequestResponse* bol_status::response() {
    srmlogit(STORM_LOG_DEBUG, "bol_status::response()", "called.\n");

    // Allocate space, if needed.
    _response = storm::soap_calloc<ns1__srmStatusOfBringOnlineRequestResponse>(_soap);

    _response->returnStatus = storm::soap_calloc<ns1__TReturnStatus>(_soap);

    // Fill global status
    _response->returnStatus->statusCode = _status;
    _response->returnStatus->explanation = soap_strdup(_soap, _explanation.c_str());

    // Fill, if present, RemainingTotalRequestTime
    if (-1 != _remainingTime) {
        _response->remainingTotalRequestTime = storm::soap_calloc<int>(_soap);
        *_response->remainingTotalRequestTime = _remainingTime;
    }

    // Fill status for each surl.
    if (_surls.size() > 0) {

        try {
            _response->arrayOfFileStatuses = storm::soap_calloc<ns1__ArrayOfTBringOnlineRequestFileStatus>(
                    _soap);

            _response->arrayOfFileStatuses->__sizestatusArray = _surls.size();
            _response->arrayOfFileStatuses->statusArray = storm::soap_calloc<
                    ns1__TBringOnlineRequestFileStatus>(_soap, _surls.size());

            int n = 0;
            for (std::vector<storm::bol_status::surl_t>::const_iterator i = _surls.begin(); i != _surls.end(); ++i) {

                _response->arrayOfFileStatuses->statusArray[n] = storm::soap_calloc<
                        ns1__TBringOnlineRequestFileStatus>(_soap);

                // Source SURL
                std::string surl = i->source;
                if (surl.length() > 0) {
                    _response->arrayOfFileStatuses->statusArray[n]->sourceSURL = soap_strdup(_soap,
                            surl.c_str());
                } else {
                    _response->arrayOfFileStatuses->statusArray[n]->sourceSURL = NULL;
                }

                // Status of the surl
                ns1__TStatusCode status = i->status;
                const char* explanation = i->explanation.c_str();
                if ((_recalltableEnabled) && (status == SRM_USCOREREQUEST_USCOREINPROGRESS)) {
                    if (isSurlOnDisk(_r_token, surl)) {
                        status = SRM_USCORESUCCESS;
                        explanation = "File recalled from tape";
                    }
                }
                _response->arrayOfFileStatuses->statusArray[n]->status = storm::soap_calloc<
                        ns1__TReturnStatus>(_soap);
                _response->arrayOfFileStatuses->statusArray[n]->status->statusCode = status;
                _response->arrayOfFileStatuses->statusArray[n]->status->explanation = soap_strdup(_soap,
                        explanation);

                // Filesize
                if (-1 != i->fileSize) {
                    _response->arrayOfFileStatuses->statusArray[n]->fileSize = storm::soap_calloc<ULONG64>(
                            _soap);
                    *_response->arrayOfFileStatuses->statusArray[n]->fileSize = i->fileSize;
                }

                // waiting time, if present
                _response->arrayOfFileStatuses->statusArray[n]->estimatedWaitTime = storm::soap_calloc<int>(
                        _soap);
                *_response->arrayOfFileStatuses->statusArray[n]->estimatedWaitTime = i->estimatedWaitTime;

                // pin lifetime, if present
                if (-1 != i->remainingPinLifetime) {
                    _response->arrayOfFileStatuses->statusArray[n]->remainingPinTime
                            = storm::soap_calloc<int>(_soap);
                    *_response->arrayOfFileStatuses->statusArray[n]->remainingPinTime
                            = i->remainingPinLifetime;
                }

                ++n;
            }

        } catch (soap_bad_alloc x) {
            srmlogit(STORM_LOG_ERROR, "bol_status::response()",
                    "Error allocating memory (%s). Continuing without filling SURLs informations.\n",
                    x.what());
        }

    } else {
        srmlogit(STORM_LOG_ERROR, "bol_status::response()", "No SURLs found\n");
    }
    return _response;
}

void bol_status::__fill_bol_request() {
    // fill SURLs information
    // Loop on the requested SURLs checking if they were found in the result set
    for (std::vector<storm::bol_status::surl_req_t>::const_iterator i = _surls_req.begin(); i
            != _surls_req.end(); ++i) {
        bool surl_found = false;

        // This nested loop is not so efficient... maybe think to another solution
        file_status_results_t::iterator j = _results.begin();
        for (; j != _results.end(); ++j) {
            if (i->source().compare((*j)["sourceSURL"]) == 0) {
                surl_found = true;
                break;
            }
        }

        bol_status::surl_t *s = NULL;

        if (surl_found) {
            // check if overwriteOption is present.
            if ("1" == (*j)["isSourceADirectory"]) {

                s = new bol_status::surl_t((*j)["sourceSURL"], true);
                s->allrecursive = atoi((*j)["allLevelRecursive"].c_str());
                s->n_levels = atoi((*j)["numOfLevels"].c_str());

            } else if ("0" == (*j)["isSourceADirectory"]) {

                s = new bol_status::surl_t((*j)["sourceSURL"], false);
                s->allrecursive = atoi((*j)["allLevelRecursive"].c_str());
                s->n_levels = atoi((*j)["numOfLevels"].c_str());

            } else {
                s = new bol_status::surl_t((*j)["sourceSURL"]);
            }

            // Fill status information
            s->status = static_cast<ns1__TStatusCode> (atoi((*j)["statusCode"].c_str()));
            s->explanation = (*j)["explanation"];

            // filesize
            if (0 == (*j)["fileSize"].size()) {
                s->fileSize = -1;
            } else {
                s->fileSize = atoll((*j)["fileSize"].c_str());
            }

            // Estimated Waiting Time
            if (0 == (*j)["estimatedWaitTime"].size()) {
                s->estimatedWaitTime = -1;
            } else {
                s->estimatedWaitTime = atoll((*j)["estimatedWaitTime"].c_str());
            }

            /// Remaining Pin Time
            if (0 == (*j)["remainingPinTime"].size()) {
                s->remainingPinLifetime = -1;
            } else {
                s->remainingPinLifetime = atoll((*j)["remainingPinTime"].c_str());
            }

        } else { // SURL not found in the result set
            s = new bol_status::surl_t(i->source());
            s->status = SRM_USCOREINVALID_USCOREPATH;
            s->explanation = "No information about this SURL";
            s->fileSize = -1;
            s->remainingPinLifetime = -1;
            s->estimatedWaitTime = -1;
        }

        srmlogit(STORM_LOG_DEBUG, "bol_status::__fill_bol_request()", "Inserting  sourceSURL %s\n",
                s->source.c_str());

        _surls.push_back(*s);
        delete s;
    }
}

std::string bol_status::__format_surl_request() {
    // Create a string from _surls_req array of surl_req_t (surl requested by the client)
    std::string query;
    if (_surls_req.size() == 0)
        return query;

    query = " AND (";
    for (std::vector<storm::bol_status::surl_req_t>::const_iterator i = _surls_req.begin(); i
            != _surls_req.end(); ++i) {
        if (i != _surls_req.begin())
            query += " OR ";
        query += "(c.sourceSURL = '" + i->source() + "')";
    }
    query += ")";
}

bool bol_status::isSurlOnDisk(std::string requestToken, std::string surl) {

    bool result = false;

    try {

        HttpPostClient client;

        client.setHostname(_recalltableHost);
        client.setPort(_recalltablePort);
        std::string data = "requestToken=" + requestToken + "\nsurl=" + surl;
        client.callService(data);

        long responseCode = client.getHttpResponseCode();
        srmlogit(STORM_LOG_DEBUG, "bol_status::isSurlOnDisk()", "Response code: %d\n", responseCode);

        if (responseCode == 200) {

            std::string response = client.getResponse();

            if (response.compare("true") == 0) {

                result = true;

                srmlogit(STORM_LOG_DEBUG2, "bol_status::isSurlOnDisk()", "Response=true for surl=%s\n",
                        surl.c_str());

            } else {

                result = false;

                srmlogit(STORM_LOG_DEBUG2, "bol_status::isSurlOnDisk()", "Response=false for surl=%s\n",
                        surl.c_str());

            }
        }

    } catch (exception& e) {
        srmlogit(STORM_LOG_ERROR, "bol_status::isSurlOnDisk()",
                "Curl: cannot create handle for HTTP client.\n");
        return false;
    }

    return result;
}

}
