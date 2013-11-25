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

#ifndef MONITORING_HELPER_HPP_
#define MONITORING_HELPER_HPP_

#include "boost/date_time/posix_time/posix_time.hpp"

#include <string>

#include "srmlogit.h"

#include "Monitoring.hpp"
#include "InstrumentedMonitor.hpp"
#include "MonitorNotEnabledException.hpp"

#include "srmv2H.h"
#include <stdsoap2.h>

namespace storm {

class MonitoringHelper {
    public:

    static void registerOperation(boost::posix_time::ptime start_time, int returnValue, std::string monitorName, ns1__TStatusCode statusCode)
    {
    	boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
    	boost::posix_time::time_duration et = (end_time - start_time);

    	if (returnValue != SOAP_OK)
    	{
    		try
    		{
    			Monitoring::getInstance()->getMonitor(monitorName)->registerFailure(et.total_milliseconds());
    		}catch(storm::MonitorNotEnabledException& exc)
    		{
    			srmlogit(STORM_LOG_ERROR, __func__, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc.what());
    		}
    	}
    	else
    	{
    		try
    		{
    			((InstrumentedMonitor*) Monitoring::getInstance()->getMonitor(monitorName))->registerCompleted(
    					et.total_milliseconds(),statusCode);
    		}catch(storm::MonitorNotEnabledException& exc)
    		{
    			srmlogit(STORM_LOG_ERROR, __func__, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc.what());
    		}
    	}
    }

    static void registerOperation(boost::posix_time::ptime start_time, std::string monitorName, ns1__TStatusCode statusCode)
	{
		boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
		boost::posix_time::time_duration et = (end_time - start_time);

		try
		{
			((InstrumentedMonitor*) Monitoring::getInstance()->getMonitor(monitorName))->registerCompleted(
					et.total_milliseconds(),statusCode);
		}catch(storm::MonitorNotEnabledException& exc)
		{
			srmlogit(STORM_LOG_ERROR, __func__, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc.what());
		}
	}

    static void registerOperationFailure(boost::posix_time::ptime start_time, std::string monitorName)
	{
		boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
		boost::posix_time::time_duration et = (end_time - start_time);

		try
		{
			((InstrumentedMonitor*) Monitoring::getInstance()->getMonitor(monitorName))->registerFailure(
					et.total_milliseconds());
		}catch(storm::MonitorNotEnabledException& exc)
		{
			srmlogit(STORM_LOG_ERROR, __func__, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc.what());
		}
	}

    static void registerOperationError(boost::posix_time::ptime start_time, std::string monitorName)
   	{
   		boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
   		boost::posix_time::time_duration et = (end_time - start_time);

   		try
   		{
   			((InstrumentedMonitor*) Monitoring::getInstance()->getMonitor(monitorName))->registerError(
   					et.total_milliseconds());
   		}catch(storm::MonitorNotEnabledException& exc)
   		{
   			srmlogit(STORM_LOG_ERROR, __func__, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc.what());
   		}
   	}
};
}
#endif /* MONITORING_HELPER_HPP_ */
