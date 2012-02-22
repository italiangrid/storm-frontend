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

namespace storm {

class MonitorHelper{
	public:
		MonitorHelper() {
		};

		~MonitorHelper() {};

		static void addSuccessStatusCodeMapping(std::string monitorName, int successReturnStatus)
		{
			std::string funcName("addSuccessStatusCodeMapping");
			srmlogit(STORM_LOG_DEBUG2, funcName.c_str(), "Adding code %u for monitor %s\n" , successReturnStatus , monitorName.c_str());
			std::map < int , bool > * monitor_map = getMonitorMap(monitorName);
			if(monitor_map != NULL)
			{
				checkUpdateSuccessStatusCodeMapping(successReturnStatus, monitor_map);
			}
			else
			{
				std::pair< std::string , std::map < int , bool > * > newStatusMapping = buildSuccessStatusCodeMap(monitorName, successReturnStatus);
				status_code_map.insert(newStatusMapping);
			}
		}

		static bool isSuccessfull(std::string monitorName, int successReturnStatus)
		{
			bool successfull = false;
			if(status_code_map.find(monitorName) != status_code_map.end())
			{
				if(status_code_map.find(monitorName)->second->find(successReturnStatus) != status_code_map.find(monitorName)->second->end())
				{
					successfull = status_code_map.find(monitorName)->second->find(successReturnStatus)->second;
				}
			}
			return successfull;
		}


		static void printMappings()
		{
			std::string funcName("printMappings");
			std::map< std::string, std::map < int , bool > * >::iterator ite;
			std::map < int , bool >::iterator innerIte;
			for(ite=status_code_map.begin();ite!=status_code_map.end(); ite++)
			{
				srmlogit(STORM_LOG_DEBUG, funcName.c_str(), "Printing mappings for monitor %s :\n" , ite->first.c_str());
				for(innerIte=ite->second->begin();innerIte!=ite->second->end();innerIte++)
				{
					srmlogit(STORM_LOG_DEBUG, funcName.c_str(), "Code %d\n" , innerIte->first);
				}
			}
		}


	private:
		static std::map< std::string, std::map < int , bool > * > status_code_map;

		static std::map < int , bool > * getMonitorMap(std::string monitorName)
		{
			std::map < int , bool > * statusMappings = NULL;
			if(status_code_map.find(monitorName) != status_code_map.end())
			{
				statusMappings = status_code_map.find(monitorName)->second;
			}
			return statusMappings;
		}

		static void checkUpdateSuccessStatusCodeMapping(int successReturnStatus, std::map < int , bool > * statusCodeMap)
		{
			if(statusCodeMap->find(successReturnStatus) != statusCodeMap->end())
			{
				//already in
			}
			else
			{
				statusCodeMap->insert(std::make_pair(successReturnStatus, true));
			}
		}

		static std::pair< std::string , std::map < int , bool > * > buildSuccessStatusCodeMap(std::string monitorName, int successReturnStatus)
		{
			std::map < int , bool > * statusCodeMap =  new std::map < int , bool >();
			statusCodeMap->insert(std::make_pair(successReturnStatus, true));
			std::pair < std::string , std::map < int , bool > * > statusMapping = std::make_pair(monitorName, statusCodeMap);
			return statusMapping;
		}

		static bool testAddedMapping(std::string monitorName, int successReturnStatus)
		{
			bool found = false;
			if(status_code_map.find(monitorName) != status_code_map.end())
			{
				if(status_code_map.find(monitorName)->second->find(successReturnStatus) != status_code_map.find(monitorName)->second->end())
				{
					found = true;
				}
			}
			return found;
		}
	};
}
#endif /* MONITOR_HELPER_HPP_ */

