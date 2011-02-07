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
#include "copy_status.hpp"
#include "srmlogit.h"

using namespace storm;

namespace storm{
    copy_status::copy_status(struct soap * soap)  : file_status<ns1__srmStatusOfCopyRequestResponse>(soap){
    }


    void copy_status::__fill_copy_request()
    {
        // fill SURLs information
        // Loop on the requested SURLs checking if they were found in the result set
        for (std::vector<storm::copy_status::surl_req_t>::const_iterator i = _surls_req.begin(); i != _surls_req.end(); ++i)
        {
            bool surl_found = false;
            std::string sourceSURL = i->source();
            std::string targetSURL = i->target();
            // This nested loop is not so efficient... maybe think to another solution
            file_status_results_t::iterator j = _results.begin();
            for (; j != _results.end(); ++j)
            {
                if ((sourceSURL.compare((*j)["sourceSURL"]) == 0) && (targetSURL.compare((*j)["targetSURL"]) == 0)) {
                    surl_found = true;
                    break;
                }
            }
            
            copy_status::surl_t *s;
            if (surl_found) {
                // check if overwriteOption is present.
                if ("1" == (*j)["isSourceADirectory"]) {
                    s = new copy_status::surl_t(sourceSURL, targetSURL, true);
                    s->allrecursive = atoi((*j)["allLevelRecursive"].c_str());
                    s->n_levels = atoi((*j)["numOfLevels"].c_str());
                } else if ("0" == (*j)["isSourceADirectory"]){
                    s = new copy_status::surl_t(sourceSURL, targetSURL, false);
                    s->allrecursive = atoi((*j)["allLevelRecursive"].c_str());
                    s->n_levels = atoi((*j)["numOfLevels"].c_str());
                } else {
                    s = new copy_status::surl_t(sourceSURL, targetSURL);                     
                }
                s->status = static_cast<ns1__TStatusCode>(atoi((*j)["statusCode"].c_str()));
                s->explanation = (*j)["explanation"];

                // filesize
                if (0 == (*j)["fileSize"].size())
                    s->fileSize = -1;
                else
                    s->fileSize = atoi((*j)["fileSize"].c_str());

                // Estimated Waiting Time
                if (0 == (*j)["estimatedWaitTime"].size())
                    s->estimatedWaitTime = -1;
                else
                    s->estimatedWaitTime = atoi((*j)["estimatedWaitTime"].c_str());

                /// Remaining File Time
                if (0 == (*j)["remainingFileTime"].size())
                    s->remainingFileLifetime = -1;
                else
                    s->remainingFileLifetime = atoi((*j)["remainingFileTime"].c_str());
            } else { // SURL not found in the result set
                s = new copy_status::surl_t(sourceSURL, targetSURL); 
                s->status = SRM_USCOREINVALID_USCOREPATH;
                s->explanation = "No information about sourceSURL or targetSURL";
                s->fileSize = -1;
                s->remainingFileLifetime = -1;
                s->estimatedWaitTime = -1;
            }
            srmlogit(STORM_LOG_DEBUG, "copy_status::__fill_copy_request()", "Inserting sourceSURL %s and targetSURL %s\n",
                     sourceSURL.c_str(), targetSURL.c_str());
            _surls.push_back(*s);
            delete s;        
        }
    }
    
    std::string copy_status::__format_surl_request()
    {
        // Create a string from _surls_req array of surl_req_t (surl
        // requested by the client)
        std::string query;
        if(_surls_req.size()==0)
            return query;
        
        query=" AND (";
        for(std::vector<storm::copy_status::surl_req_t>::const_iterator i = _surls_req.begin();
            i != _surls_req.end();
            ++i){
            if(i!=_surls_req.begin())
                query+=" OR ";
            query += "(c.sourceSURL = '"+i->source() +"' AND c.targetSURL = '"+i->target()+"')";
        }
        query+=")";
    }


    void copy_status::load(struct srm_dbfd *db, const std::string &requestToken){

        srmlogit(STORM_LOG_DEBUG, "copy_status::load", "R_token: %s\n",  requestToken.c_str());

        _empty_results();
        // std::string to_add = __format_surl_request();
        _query = "SELECT * FROM request_queue r JOIN (request_Copy c, status_Copy s) ON "
                 "(c.request_queueID=r.ID AND s.request_CopyID=c.ID) "
                 "LEFT JOIN request_DirOption d ON c.request_DirOptionID=d.ID "
                 "WHERE r.r_token='" + requestToken + "'"; // + to_add;
                 
        file_status<ns1__srmStatusOfCopyRequestResponse>::load(db, requestToken);
        
        if (_results.size() == 0)
            throw token_not_found();
            
        // TEMPORARY SOLUTION! TODO: find another way to manage the case of no SURLs specified in the request.
        if (_surls_req.size() == 0) {
            for (file_status_results_t::iterator i = _results.begin(); i != _results.end(); ++i)
                add_requested_surl((*i)["sourceSURL"], (*i)["targetSURL"]);
        }

        __fill_copy_request();
    }

     ns1__srmStatusOfCopyRequestResponse* copy_status::response()
     {
         srmlogit(STORM_LOG_DEBUG, "copy_status::response()", "called.\n");

         // Allocate space, if needed.
         if(NULL == _response)
             _response = storm::soap_calloc<ns1__srmStatusOfCopyRequestResponse>(_soap);

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
                 _response->arrayOfFileStatuses = storm::soap_calloc<ns1__ArrayOfTCopyRequestFileStatus>(_soap);
                 _response->arrayOfFileStatuses->__sizestatusArray = _surls.size();
                 _response->arrayOfFileStatuses->statusArray = storm::soap_calloc<ns1__TCopyRequestFileStatus>(_soap, _surls.size());
                 int n=0;
                 for(std::vector<storm::copy_status::surl_t>::const_iterator i = _surls.begin();
                     i != _surls.end();
                     ++i){
                     _response->arrayOfFileStatuses->statusArray[n] = storm::soap_calloc<ns1__TCopyRequestFileStatus>(_soap);

                     if(i->source.length()>0)
                         _response->arrayOfFileStatuses->statusArray[n]->sourceSURL
                             = soap_strdup(_soap, i->source.c_str());
                     else
                         _response->arrayOfFileStatuses->statusArray[n]->sourceSURL = NULL;

                     if(i->target.length()>0)
                         _response->arrayOfFileStatuses->statusArray[n]->targetSURL
                             = soap_strdup(_soap, i->target.c_str());
                     else
                         _response->arrayOfFileStatuses->statusArray[n]->targetSURL = NULL;

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

                     // filetime, if present
                     if(-1 != i->remainingFileLifetime){
                         _response->arrayOfFileStatuses->statusArray[n]->remainingFileLifetime 
                             = storm::soap_calloc<int>(_soap);
                         *_response->arrayOfFileStatuses->statusArray[n]->remainingFileLifetime
                             = i->remainingFileLifetime;
                     }
                     ++n;
                 }
             }catch(soap_bad_alloc x){
                 srmlogit(STORM_LOG_ERROR, "copy_status::response()","Error allocating memory (%s). Continuing without filling SURLs informations.\n",x.what());
             }


         }else{
             srmlogit(STORM_LOG_ERROR, "copy_status::response()", "INTERNAL ERROR: arrayOfFileStatuses already allocated! Continuing not filling-it! May the Force be with us!!\n");
         }
         return _response;
     }


}
