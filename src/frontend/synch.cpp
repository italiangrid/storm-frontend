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

#include "Monitoring.hpp"
#include "srmv2H.h"

#include "boost/date_time/posix_time/posix_time.hpp"

extern "C" int ns1__srmPing_impl(struct soap* soap, struct ns1__srmPingRequest *req,
        struct ns1__srmPingResponse_ *rep);

using namespace boost::posix_time;

int ns1__srmPing(struct soap* soap, struct ns1__srmPingRequest *req, struct ns1__srmPingResponse_ *rep) {

    ptime start_time = microsec_clock::local_time();

    int ret_val = ns1__srmPing_impl(soap, req, rep);

    ptime end_time = microsec_clock::local_time();

    time_duration et = (end_time - start_time);

    if (ret_val != SOAP_OK) {
        storm::Monitoring::getInstance()->notifyPingCompleted(-1, false);
    } else {
        storm::Monitoring::getInstance()->notifyPingCompleted(et.total_milliseconds(), true);
    }

    return ret_val;
}

