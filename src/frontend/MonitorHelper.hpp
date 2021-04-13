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

#ifndef MONITOR_HELPER_HPP_
#define MONITOR_HELPER_HPP_

#include <string>
#include <utility>
#include <map>
#include "srmlogit.h"
#include "InstrumentedMonitor.hpp"
#include "Monitoring.hpp"

namespace storm {

class MonitorHelper{
	public:

		static void addSuccessStatusCodeMapping(std::string monitorName, int successReturnStatus)
		{
			std::string funcName("addSuccessStatusCodeMapping");
			srmlogit(STORM_LOG_DEBUG2, funcName.c_str(), "Adding code %u for monitor %s\n" , successReturnStatus , monitorName.c_str());
			((InstrumentedMonitor*)Monitoring::getInstance()->getMonitor(monitorName))->addStatusCodeMapping(successReturnStatus,true);
		}

		static void addSuccessStatusCodeMapping(InstrumentedMonitor* instrumentedMonitor, int successReturnStatus)
		{
			std::string funcName("addSuccessStatusCodeMapping");
			srmlogit(STORM_LOG_DEBUG2, funcName.c_str(), "Adding code %u for monitor %s\n" , successReturnStatus , instrumentedMonitor->getName().c_str());
			instrumentedMonitor->addStatusCodeMapping(successReturnStatus,true);
		}

		static bool isSuccessfull(std::string monitorName, int returnStatus)
		{
			return ((InstrumentedMonitor*)Monitoring::getInstance()->getMonitor(monitorName))->getStatusCodeMapping(returnStatus);
		}

		static void printMappings()
		{
			std::string funcName("printMappings");
			std::set<std::string> names = Monitoring::getInstance()->getMonitorNames();
			std::set<std::string>::const_iterator const setEnd = names.end();
			for(std::set<std::string>::const_iterator it = names.begin();
	   				it != setEnd; ++it)
			{
				srmlogit(STORM_LOG_DEBUG, funcName.c_str(), "Printing mappings for monitor %s :\n" , it->c_str());
				((InstrumentedMonitor*)Monitoring::getInstance()->getMonitor(*it))->printStatusCodeMappings();
			}
		}


	private:
		static bool testAddedMapping(std::string monitorName, int returnStatus, bool successful)
		{
			return ((InstrumentedMonitor*)Monitoring::getInstance()->getMonitor(monitorName))->getStatusCodeMapping(returnStatus) == successful;
		}

		static bool testAddedMapping(InstrumentedMonitor* instrumentedMonitor, int returnStatus, bool successful)
		{
			return instrumentedMonitor->getStatusCodeMapping(returnStatus) == successful;
		}
	};
}
#endif /* MONITOR_HELPER_HPP_ */

