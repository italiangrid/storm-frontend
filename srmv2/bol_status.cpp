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

using namespace storm;

namespace storm{
    bol_status::bol_status(struct soap * soap)  : file_status<ns1__srmStatusOfBringOnlineRequestResponse>(soap){
    }


    void bol_status::__fill_bol_request()
    {
        // Temporary Hack!
        _n_waiting=0;
        _n_failed=0;
        _n_completed=0;
        _n_aborted = 0;

        // fill SURLs information
        for(file_status_results_t::iterator i = _results.begin();
            i != _results.end();
            ++i)
            {
                bol_status::surl_t *s;
                // check if overwriteOption is present.
                if("1" == (*i)["isSourceADirectory"]){
                    s = new bol_status::surl_t((*i)["sourceSURL"], 
                            true);
                    s->allrecursive=atoi((*i)["allLevelRecursive"].c_str());
                    s->n_levels=atoi((*i)["numOfLevels"].c_str());
                }else if ("0" == (*i)["isSourceADirectory"]){
                    s = new bol_status::surl_t((*i)["sourceSURL"], 
                            false);
                    s->allrecursive=atoi((*i)["allLevelRecursive"].c_str());
                    s->n_levels=atoi((*i)["numOfLevels"].c_str());
                }else{
                    s = new bol_status::surl_t((*i)["sourceSURL"]);
                }
                s->status = static_cast<ns1__TStatusCode>(atoi((*i)["statusCode"].c_str()));
                s->explanation = (*i)["explanation"];

                // Temporary Hack! 
                switch(s->status){
                case SRM_USCORESPACE_USCOREAVAILABLE:
                case SRM_USCORESUCCESS:
                    ++_n_completed;
                    break;
                case SRM_USCOREREQUEST_USCOREQUEUED:
                case SRM_USCOREREQUEST_USCOREINPROGRESS:
                    ++_n_waiting;
                    break;
                case SRM_USCOREABORTED:
                    ++_n_aborted;
                    break;
                default:
                    ++_n_failed;
                    break;
                }

                // Fill status information
                s->status = static_cast<ns1__TStatusCode>(atoi((*i)["statusCode"].c_str()));
                s->explanation = (*i)["explanation"];

                // filesize
                if(0 == (*i)["fileSize"].size())
                    s->fileSize = -1;
                else
                    s->fileSize = atoi((*i)["fileSize"].c_str());

                // Estimated Waiting Time
                if(0 == (*i)["estimatedWaitTime"].size())
                    s->estimatedWaitTime = -1;
                else
                    s->estimatedWaitTime = atoi((*i)["estimatedWaitTime"].c_str());

                /// Remaining Pin Time
                if(0 == (*i)["remainingPinTime"].size())
                    s->remainingPinLifetime = -1;
                else
                    s->remainingPinLifetime = atoi((*i)["remainingPinTime"].c_str());

                srmlogit(STORM_LOG_DEBUG, "bol_status::__fill_bol_request()", 
                         "Inserting  sourceSURL %s\n",
                         s->source.c_str());
                _surls.push_back(*s);
                delete s;
            }      
        // Temporary Hack! 
        if (_n_waiting > 0)
            _status = SRM_USCOREREQUEST_USCOREINPROGRESS;
        else if (_n_failed == _n_files)
            _status = SRM_USCOREFAILURE;
        else if (_n_completed == _n_files)
            _status = SRM_USCORESUCCESS;
        else if (_n_aborted == _n_files)
            _status = SRM_USCOREABORTED;
        else 
            _status = SRM_USCOREPARTIAL_USCORESUCCESS;
    }
    
    std::string bol_status::__format_surl_request()
    {
        // Create a string from _surls_req array of surl_req_t (surl
        // requested by the client)
        std::string query;
        if(_surls_req.size()==0)
            return query;
        
        query=" AND (";
        for(std::vector<storm::bol_status::surl_req_t>::const_iterator i = _surls_req.begin();
            i != _surls_req.end();
            ++i){
            if(i!=_surls_req.begin())
                query+=" OR ";
            query += "(c.sourceSURL = '"+i->source()+"')";
        }
        query+=")";
    }


    void bol_status::load(struct srm_dbfd *db, const std::string &requestToken){

        srmlogit(STORM_LOG_DEBUG, "bol_status::load", "R_token: %s\n",  requestToken.c_str());

        _empty_results();
        std::string to_add = __format_surl_request();
        _query = "select * from request_queue r JOIN "
            "(request_BoL c, status_BoL s) ON "
            "(c.request_queueID=r.ID AND s.request_BoLID=c.ID) "
            "left join request_DirOption d ON c.request_DirOptionID=d.ID "
            "where r.r_token='"+requestToken+"'"+to_add;
        file_status<ns1__srmStatusOfBringOnlineRequestResponse>::load(db, requestToken);
        
        if(_results.size() == 0)
            throw token_not_found();

        __fill_bol_request();
    }

     ns1__srmStatusOfBringOnlineRequestResponse* bol_status::response()
     {
         srmlogit(STORM_LOG_DEBUG, "bol_status::response()", "called.\n");

         // Allocate space, if needed.
         if(NULL == _response)
             _response = storm::soap_calloc<ns1__srmStatusOfBringOnlineRequestResponse>(_soap);

         if(NULL == _response->returnStatus)
             _response->returnStatus = storm::soap_calloc<ns1__TReturnStatus>(_soap);

         // Fill global status
         _response->returnStatus->statusCode = _status;
         _response->returnStatus->explanation = soap_strdup(_soap, _explanation.c_str());

         // Fill, if present, RemainingTotalRequestTime
         if(-1 != _remainingTime){
             _response->remainingTotalRequestTime = storm::soap_calloc<int>(_soap);
             *_response->remainingTotalRequestTime = _remainingTime;
         }

         // Fill status for each surl.
         if(NULL == _response->arrayOfFileStatuses && _surls.size()>0){
             try{
                 _response->arrayOfFileStatuses = storm::soap_calloc<ns1__ArrayOfTBringOnlineRequestFileStatus>(_soap);
                 _response->arrayOfFileStatuses->__sizestatusArray = _surls.size();
                 _response->arrayOfFileStatuses->statusArray = storm::soap_calloc<ns1__TBringOnlineRequestFileStatus>(_soap, _surls.size());
                 int n=0;
                 for(std::vector<storm::bol_status::surl_t>::const_iterator i = _surls.begin();
                     i != _surls.end();
                     ++i){
                     _response->arrayOfFileStatuses->statusArray[n] = storm::soap_calloc<ns1__TBringOnlineRequestFileStatus>(_soap);

                     // Source SURL
                     if(i->source.length()>0)
                         _response->arrayOfFileStatuses->statusArray[n]->sourceSURL
                             = soap_strdup(_soap, i->source.c_str());
                     else
                         _response->arrayOfFileStatuses->statusArray[n]->sourceSURL = NULL;
                     
                     
                     // Status of the surl
                     _response->arrayOfFileStatuses->statusArray[n]->status 
                         = storm::soap_calloc<ns1__TReturnStatus>(_soap);
                     _response->arrayOfFileStatuses->statusArray[n]->status->statusCode = i->status;
                     _response->arrayOfFileStatuses->statusArray[n]->status->explanation 
                         = soap_strdup(_soap, i->explanation.c_str());

                     // Filesize
                     if(-1 != i->fileSize){
                         _response->arrayOfFileStatuses->statusArray[n]->fileSize 
                             = storm::soap_calloc<ULONG64>(_soap);
                         *_response->arrayOfFileStatuses->statusArray[n]->fileSize
                             = i->fileSize;
                     }

                     // waiting time, if present
                     _response->arrayOfFileStatuses->statusArray[n]->estimatedWaitTime 
                        = storm::soap_calloc<int>(_soap);
                     *_response->arrayOfFileStatuses->statusArray[n]->estimatedWaitTime
                        = i->estimatedWaitTime;

                     // pin lifetime, if present
                     if(-1 != i->remainingPinLifetime){
                         _response->arrayOfFileStatuses->statusArray[n]->remainingPinTime 
                             = storm::soap_calloc<int>(_soap);
                         *_response->arrayOfFileStatuses->statusArray[n]->remainingPinTime
                             = i->remainingPinLifetime;
                     }

                     // transferProtocolInfo
                     ++n;
                 }
             }catch(soap_bad_alloc x){
                 srmlogit(STORM_LOG_ERROR, "bol_status::response()","Error allocating memory (%s). Continuing without filling SURLs informations.\n",x.what());
             }


         }else{
             srmlogit(STORM_LOG_ERROR, "bol_status::response()", "INTERNAL ERROR: arrayOfFileStatuses already allocated! Continuing not filling-it! May the Force be with us!!\n");
         }
         return _response;
     }


}
