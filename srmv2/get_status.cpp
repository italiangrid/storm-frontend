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
#include "get_status.hpp"
#include "srmlogit.h"

using namespace storm;

namespace storm {
        get_status::get_status(struct soap * soap) : file_status<ns1__srmStatusOfGetRequestResponse>(soap) {
    }

    void get_status::__fill_get_request()
    {
        // fill SURLs information
        // Loop on the requested SURLs checking if they were found in the result set
        for (std::vector<storm::get_status::surl_req_t>::const_iterator i = _surls_req.begin(); i != _surls_req.end(); ++i)
        {
			bool surl_found = false;
			
			// This nested loop is not so efficient... maybe think to another solution
			file_status_results_t::iterator j = _results.begin();
			for (; j != _results.end(); ++j)
			{
				if (i->source().compare((*j)["sourceSURL"]) == 0) {
					surl_found = true;
					break;
				}
			}
			get_status::surl_t *s = NULL;
			if (surl_found) {
                // check if overwriteOption is present.
                if ("1" == (*j)["isSourceADirectory"]) {
                    s = new get_status::surl_t((*j)["sourceSURL"], true);
                    s->allrecursive = atoi((*j)["allLevelRecursive"].c_str());
                    s->n_levels = atoi((*j)["numOfLevels"].c_str());
                } else if ("0" == (*j)["isSourceADirectory"]) {
                    s = new get_status::surl_t((*j)["sourceSURL"], false);
                    s->allrecursive = atoi((*j)["allLevelRecursive"].c_str());
                    s->n_levels = atoi((*j)["numOfLevels"].c_str());
                } else {
                    s = new get_status::surl_t((*j)["sourceSURL"]);
                }

                // Fill status information
                s->status = static_cast<ns1__TStatusCode>(atoi((*j)["statusCode"].c_str()));
                s->explanation = (*j)["explanation"];

				// TURL
                s->turl = (*j)["transferURL"];

                // filesize
                if(0 == (*j)["fileSize"].size())
                    s->fileSize = -1;
                else
                    s->fileSize = atoi((*j)["fileSize"].c_str());

                // Estimated Waiting Time
                if(0 == (*j)["estimatedWaitTime"].size())
                    s->estimatedWaitTime = -1;
                else
                    s->estimatedWaitTime = atoi((*j)["estimatedWaitTime"].c_str());

                /// Remaining Pin Time
                if(0 == (*j)["remainingPinTime"].size())
                    s->remainingPinLifetime = -1;
                else
                    s->remainingPinLifetime = atoi((*j)["remainingPinTime"].c_str());
            } else { // SURL not found in the result set
            	s = new get_status::surl_t(i->source());
            	s->status = SRM_USCOREINVALID_USCOREPATH;
            	s->explanation = "No information about this SURL";
            	s->fileSize = -1;
            	s->remainingPinLifetime = -1;
            	s->estimatedWaitTime = -1;
			}
			srmlogit(STORM_LOG_DEBUG, "get_status::__fill_get_request()", "Inserting  sourceSURL %s\n", s->source.c_str());
			_surls.push_back(*s);
			delete s;
        }
    }
    
    std::string get_status::__format_surl_request()
    {
        // Create a string from _surls_req array of surl_req_t (surl
        // requested by the client)
        std::string query;
        if(_surls_req.size()==0)
            return query;
        
        query=" AND (";
        for(std::vector<storm::get_status::surl_req_t>::const_iterator i = _surls_req.begin();
            i != _surls_req.end();
            ++i){
            if(i!=_surls_req.begin())
                query+=" OR ";
            query += "(c.sourceSURL = '"+i->source()+"')";
        }
        query+=")";
    }

//    void get_status::_load(struct srm_dbfd *db, const std::string &requestToken, const std::string &to_add){
//        _query = "select * from request_queue r JOIN "
//            "(request_Get c, status_Get s) ON "
//            "(c.request_queueID=r.ID AND s.request_GetID=c.ID) "
//            "left join request_DirOption d ON c.request_DirOptionID=d.ID "
//            "where r.r_token='"+requestToken+"'"+to_add;
//        file_status<ns1__srmStatusOfGetRequestResponse>::load(db, requestToken);
//    }

    void get_status::load(struct srm_dbfd *db, const std::string &requestToken)
    {
        srmlogit(STORM_LOG_DEBUG, "get_status::load", "R_token: %s\n",  requestToken.c_str());

        _empty_results();
        // Next line commented, we need to know if the request token is valid or not.
        // std::string to_add = __format_surl_request();
        // _load(db, requestToken, to_add);
        
        _query = "SELECT * FROM request_queue r JOIN (request_Get c, status_Get s) ON "
                 "(c.request_queueID=r.ID AND s.request_GetID=c.ID) "
                 "LEFT JOIN request_DirOption d ON c.request_DirOptionID=d.ID "
                 "WHERE r.r_token='" + requestToken + "'"; // +to_add;
		
        file_status<ns1__srmStatusOfGetRequestResponse>::load(db, requestToken);

        if (_results.size() == 0)
        	throw token_not_found();
    	
        // TEMPORARY SOLUTION! TODO: find another way to manage the case of no SURLs specified in the request.
        if (_surls_req.size() == 0) {
            file_status_results_t::iterator i = _results.begin();
            for (; i != _results.end(); ++i)
                add_requested_surl((*i)["sourceSURL"]);
        }
        
        __fill_get_request();
    }

	ns1__srmStatusOfGetRequestResponse* get_status::response()
	{
        srmlogit(STORM_LOG_DEBUG, "get_status::response()", "called.\n");
	
		// Allocate space, if needed.
		if(NULL == _response)
            _response = storm::soap_calloc<ns1__srmStatusOfGetRequestResponse>(_soap);
		
		if(NULL == _response->returnStatus)
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
        if (NULL == _response->arrayOfFileStatuses && _surls.size()>0) {
            try {
				_response->arrayOfFileStatuses = storm::soap_calloc<ns1__ArrayOfTGetRequestFileStatus>(_soap);
				_response->arrayOfFileStatuses->__sizestatusArray = _surls.size();
				_response->arrayOfFileStatuses->statusArray = storm::soap_calloc<ns1__TGetRequestFileStatus>(_soap, _surls.size());
				int n = 0;
				for (std::vector<storm::get_status::surl_t>::const_iterator i = _surls.begin(); i != _surls.end(); ++i) {
                    _response->arrayOfFileStatuses->statusArray[n] = storm::soap_calloc<ns1__TGetRequestFileStatus>(_soap);
		
                    // Source SURL
					if (i->source.length() > 0)
                        _response->arrayOfFileStatuses->statusArray[n]->sourceSURL = soap_strdup(_soap, i->source.c_str());
					else
                        _response->arrayOfFileStatuses->statusArray[n]->sourceSURL = NULL;
					 
					// TURL
					if (i->turl.length() > 0)
                        _response->arrayOfFileStatuses->statusArray[n]->transferURL = soap_strdup(_soap, i->turl.c_str());
					else
                        _response->arrayOfFileStatuses->statusArray[n]->transferURL = NULL;
					 
					 // Status of the surl
					 _response->arrayOfFileStatuses->statusArray[n]->status = storm::soap_calloc<ns1__TReturnStatus>(_soap);
					 _response->arrayOfFileStatuses->statusArray[n]->status->statusCode = i->status;
					 _response->arrayOfFileStatuses->statusArray[n]->status->explanation = soap_strdup(_soap, i->explanation.c_str());
					
					 // Filesize
					 if (-1 != i->fileSize) {
                        _response->arrayOfFileStatuses->statusArray[n]->fileSize = storm::soap_calloc<ULONG64>(_soap);
                        *_response->arrayOfFileStatuses->statusArray[n]->fileSize = i->fileSize;
                    }
					
                    // waiting time, if present
                    _response->arrayOfFileStatuses->statusArray[n]->estimatedWaitTime = storm::soap_calloc<int>(_soap);
                    *_response->arrayOfFileStatuses->statusArray[n]->estimatedWaitTime = i->estimatedWaitTime;
					
                    // pin lifetime, if present
                    if (-1 != i->remainingPinLifetime) {
                        _response->arrayOfFileStatuses->statusArray[n]->remainingPinTime = storm::soap_calloc<int>(_soap);
                        *_response->arrayOfFileStatuses->statusArray[n]->remainingPinTime = i->remainingPinLifetime;
                    }
                     
                    // transferProtocolInfo
                    ++n;
                }
            } catch(soap_bad_alloc x) {
                srmlogit(STORM_LOG_ERROR, "get_status::response()","Error allocating memory (%s). Continuing without filling SURLs informations.\n",x.what());
            }
        } else {
            srmlogit(STORM_LOG_ERROR, "get_status::response()", "INTERNAL ERROR: arrayOfFileStatuses already allocated! Continuing not filling-it! May the Force be with us!!\n");
        }
        return _response;
    }
}
