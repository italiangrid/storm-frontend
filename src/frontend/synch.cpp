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

/* commented out. not available in first Argus integration prototype
#include "Credentials.hpp"
#include "Authorization.hpp"
*/
#include "boost/date_time/posix_time/posix_time.hpp"

extern "C" int ns1__srmPing_impl(struct soap* soap,
		struct ns1__srmPingRequest *req, struct ns1__srmPingResponse_ *rep);

/*
extern "C" int get_ns1__srmPingResponse(struct soap* soap,  struct ns1__srmPingResponse *repp);
*/
int ns1__srmPing(struct soap* soap, struct ns1__srmPingRequest *req,
		struct ns1__srmPingResponse_ *rep) {

	char *func = "ns1__srmPing()";
	boost::posix_time::ptime start_time = boost::posix_time::microsec_clock::local_time();

	/* commented out. not available in first Argus integration prototype
	bool black = false;
	storm::Credentials cred(soap);
	try
	{
		storm::Authorization auth((storm::Credentials*)&cred);
		black = auth.isBlacklisted();
	} catch (storm::AuthorizationException& e)
	{
		srmlogit(STORM_LOG_WARNING, func, "Received AuthorizationException during authorization authorization: %s\n" , e.what());
	}
    if(black)
    {
    	srmlogit(STORM_LOG_INFO, func, "The user is blacklisted\n");
    	struct ns1__srmPingResponse *repp;
    	int ret_val = get_ns1__srmPingResponse(soap, repp);
		repp->otherInfo = NULL;
		rep->srmPingResponse = repp;
		repp->versionInfo= "User not authorized to ping the service";
		return(SOAP_OK);
    }
    else
    {
    	srmlogit(STORM_LOG_DEBUG, func, "The user is not blacklisted\n");
    }
	 */

	/*
	cred.getCertChain();
	fprintf(stdout,"getCertChain chiamata\n");
	 */


	/*
	storm::Credentials cred(soap);
	 char* proxy = cred.getProxy();
	 */
	int ret_val = ns1__srmPing_impl(soap, req, rep);

	boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();

	boost::posix_time::time_duration et = (end_time - start_time);

	if (ret_val != SOAP_OK) {
		storm::Monitoring::getInstance()->notifyPingCompleted(-1, false);
	} else {
		storm::Monitoring::getInstance()->notifyPingCompleted(
				et.total_milliseconds(), true);
	}

	return ret_val;
}


