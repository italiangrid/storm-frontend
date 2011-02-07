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

#include <string>
#include "put_status.hpp"
#include "srmlogit.h"

using namespace storm;

namespace storm {
    
put_status::put_status(struct soap * soap)  : file_status<ns1__srmStatusOfPutRequestResponse>(soap) { }

void put_status::__fill_put_request()
{
    // fill SURLs information
	// Loop on the requested SURLs checking if they were found in the result set
    for (std::vector<storm::put_status::surl_req_t>::const_iterator i = _surls_req.begin(); i != _surls_req.end(); ++i)
    {
		bool surl_found = false;
		// This nested loop is not so efficient... maybe think to another solution
		file_status_results_t::iterator j = _results.begin();
		for (; j != _results.end(); ++j)
		{
			if (i->target().compare((*j)["targetSURL"]) == 0) {
				surl_found = true;
				break;
			}
		}
		// Instantiate the object
        put_status::surl_t *s = new put_status::surl_t(i->target());
		if (surl_found) {
            s->turl = (*j)["transferURL"];
            // Fill status information
            s->status = static_cast<ns1__TStatusCode>(atoi((*j)["statusCode"].c_str()));
            s->explanation = (*j)["explanation"];
            
            // filesize
            if(0 == (*j)["fileSize"].size())
                s->fileSize = -1;
            else
                s->fileSize = atoi((*j)["fileSize"].c_str());

            // filesize
            if(0 == (*j)["expectedFileSize"].size())
                s->expectedFileSize = -1;
            else
                s->expectedFileSize = atoi((*j)["expectedFileSize"].c_str());

            // Estimated Waiting Time
            if(0 == (*j)["estimatedWaitTime"].size())
                s->estimatedWaitTime = -1;
            else
                s->estimatedWaitTime = atoi((*j)["estimatedWaitTime"].c_str());

            /// Remaining File Time
            if(0 == (*j)["remainingFileTime"].size())
                s->remainingFileLifetime = -1;
            else
                s->remainingFileLifetime = atoi((*j)["remainingFileTime"].c_str());

            /// Remaining Pin Time
            if(0 == (*j)["remainingPinTime"].size())
                s->remainingPinLifetime = -1;
            else
                s->remainingPinLifetime = atoi((*j)["remainingPinTime"].c_str());

            srmlogit(STORM_LOG_DEBUG, "put_status::__fill_put_request()", "Inserting  targetSURL %s\n",
                     s->target.c_str());
        }
		else { // SURL not found in the result set
            s->status = SRM_USCOREINVALID_USCOREPATH;
            s->explanation = "No information about this SURL";
            s->fileSize = -1;
            s->remainingPinLifetime = -1;
			s->remainingFileLifetime = -1;
            s->estimatedWaitTime = -1;
		}
		_surls.push_back(*s);
        delete s;
	}
}

std::string put_status::__format_surl_request()
{
    // Create a string from _surls_req array of surl_req_t (surl
    // requested by the client)
    std::string query;
    if(_surls_req.size()==0)
        return query;
    
    query=" AND (";
    for(std::vector<storm::put_status::surl_req_t>::const_iterator i = _surls_req.begin();
        i != _surls_req.end();
        ++i){
        if(i!=_surls_req.begin())
            query+=" OR ";
        query += "(c.targetSURL = '"+i->target()+"')";
    }
    query+=")";
}


void put_status::load(struct srm_dbfd *db, const std::string &requestToken){

    srmlogit(STORM_LOG_DEBUG, "put_status::load", "R_token: %s\n",  requestToken.c_str());

    _empty_results();
	// Next line commented, we need to know if the request token is valid or not.
    // std::string to_add = __format_surl_request();
    _query = "SELECT * from request_queue r JOIN (request_Put c, status_Put s) ON "
             "(c.request_queueID=r.ID AND s.request_PutID=c.ID) "
             "WHERE r.r_token='" + requestToken + "'"; // + to_add;
    file_status<ns1__srmStatusOfPutRequestResponse>::load(db, requestToken);
    
    if (_results.size() == 0)
        throw token_not_found();
        
    // TEMPORARY SOLUTION! TODO: find another way to manage the case of no SURLs specified in the request.
    if (_surls_req.size() == 0) {
        file_status_results_t::iterator i = _results.begin();
        for (; i != _results.end(); ++i)
            add_requested_surl((*i)["targetSURL"]);
    }
    __fill_put_request();
}

ns1__srmStatusOfPutRequestResponse* put_status::response()
{
    srmlogit(STORM_LOG_DEBUG, "put_status::response()", "called.\n");

    // Allocate space, if needed.
    if (NULL == _response) {
        _response = storm::soap_calloc<ns1__srmStatusOfPutRequestResponse>(_soap);
    }

    if (NULL == _response->returnStatus) {
        _response->returnStatus = storm::soap_calloc<ns1__TReturnStatus>(_soap);
    }

    // Fill global status
    _response->returnStatus->statusCode = _status;
    _response->returnStatus->explanation = soap_strdup(_soap, _explanation.c_str());

    // Fill, if present, RemainingTotalRequestTime
    if (-1 != _remainingTime) {
        _response->remainingTotalRequestTime = storm::soap_calloc<int>(_soap);
        *_response->remainingTotalRequestTime = _remainingTime;
    }
    
    // BUG check
    if (_response->arrayOfFileStatuses != NULL) {
        srmlogit(STORM_LOG_ERROR, "put_status::response()",
                 "INTERNAL ERROR: arrayOfFileStatuses already allocated! Continuing not filling-it! May the Force be with us!!\n");
        return _response;
    }
    
    if (_surls.size()<=0) {
        srmlogit(STORM_LOG_ERROR, "put_status::response()",
                 "INTERNAL ERROR: Valid request token but no SURLs found!!!\n");
        return _response;
    }
    
    // Fill status for each surl.
    try {
        _response->arrayOfFileStatuses = storm::soap_calloc<ns1__ArrayOfTPutRequestFileStatus>(_soap);
        _response->arrayOfFileStatuses->__sizestatusArray = _surls.size();
        _response->arrayOfFileStatuses->statusArray = storm::soap_calloc<ns1__TPutRequestFileStatus>(_soap, _surls.size());
        int n=0;
        for (std::vector<storm::put_status::surl_t>::const_iterator i=_surls.begin(); i!=_surls.end(); ++i) {
            _response->arrayOfFileStatuses->statusArray[n] = storm::soap_calloc<ns1__TPutRequestFileStatus>(_soap);

            // Target SURL
            if(i->target.length()>0)
                _response->arrayOfFileStatuses->statusArray[n]->SURL = soap_strdup(_soap, i->target.c_str());
            else
                _response->arrayOfFileStatuses->statusArray[n]->SURL = NULL;
            
            // TURL
            if(i->turl.length()>0)
                _response->arrayOfFileStatuses->statusArray[n]->transferURL = soap_strdup(_soap, i->turl.c_str());
            else
                _response->arrayOfFileStatuses->statusArray[n]->transferURL = NULL;
             
            // Status of the surl
            _response->arrayOfFileStatuses->statusArray[n]->status = storm::soap_calloc<ns1__TReturnStatus>(_soap);
            _response->arrayOfFileStatuses->statusArray[n]->status->statusCode = i->status;
            _response->arrayOfFileStatuses->statusArray[n]->status->explanation = soap_strdup(_soap, i->explanation.c_str());

            // Filesize
            if (-1 != i->expectedFileSize) {
                // Yes it is right, values from "expectedFileSize" are assigned to "fileSize"
                _response->arrayOfFileStatuses->statusArray[n]->fileSize = storm::soap_calloc<ULONG64>(_soap);
                *_response->arrayOfFileStatuses->statusArray[n]->fileSize = i->expectedFileSize;
            }

            // waiting time, if present
            _response->arrayOfFileStatuses->statusArray[n]->estimatedWaitTime = storm::soap_calloc<int>(_soap);
            *_response->arrayOfFileStatuses->statusArray[n]->estimatedWaitTime = i->estimatedWaitTime;

            // filetime, if present
            if (-1 != i->remainingFileLifetime) {
                _response->arrayOfFileStatuses->statusArray[n]->remainingFileLifetime = storm::soap_calloc<int>(_soap);
                *_response->arrayOfFileStatuses->statusArray[n]->remainingFileLifetime = i->remainingFileLifetime;
            }

            // pin lifetime, if present
            if (-1 != i->remainingPinLifetime) {
                _response->arrayOfFileStatuses->statusArray[n]->remainingPinLifetime = storm::soap_calloc<int>(_soap);
                *_response->arrayOfFileStatuses->statusArray[n]->remainingPinLifetime = i->remainingPinLifetime;
            }
            // transferProtocolInfo
            ++n;
        }
    } catch(soap_bad_alloc x) {
        srmlogit(STORM_LOG_ERROR, "put_status::response()",
                 "Error allocating memory (%s). Continuing without filling SURLs informations.\n", x.what());
    }
    return _response;
}

} // namespace storm
