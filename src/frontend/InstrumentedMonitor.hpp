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

#ifndef INSTRUMENTED_MONITOR_HPP_
#define INSTRUMENTED_MONITOR_HPP_

#include <string>
#include <map>
#include "Monitor.hpp"
#include "srmlogit.h"

namespace storm {

class InstrumentedMonitor : public Monitor {
	public:


		InstrumentedMonitor(std::string name , std::string friendlyName, OperationType type) : Monitor(name,friendlyName,type) , m_statusCodeMapping()
		{
		};

		void registerCompleted(long executionTimeInMills, int returnCode)
		{
			if(getStatusCodeMapping(returnCode))
			{
				registerSuccess(executionTimeInMills);
			}
			else
			{
				registerFailure(executionTimeInMills);
			}
		}

		void addStatusCodeMapping(int returnStatus, bool successfull)
		{
			if(m_statusCodeMapping.find(returnStatus) == m_statusCodeMapping.end())
			{
				m_statusCodeMapping[returnStatus] = successfull;
			}
			else
			{
				if(m_statusCodeMapping.find(returnStatus)->second != successfull)
				{
					srmlogit(STORM_LOG_WARNING, "storm::InstrumentedMonitor::addStatusCodeMapping()", "Attempt to modify already stored status code mapping for status %d\n" , returnStatus);
				}
			}
		}

		bool getStatusCodeMapping(int returnStatus)
		{
			bool mapping = false;
			if(m_statusCodeMapping.find(returnStatus) != m_statusCodeMapping.end())
			{
				mapping = m_statusCodeMapping.find(returnStatus)->second;
			}
			return mapping;
		}

		void printStatusCodeMappings()
		{
			std::map<int, bool>::const_iterator const mapEnd = m_statusCodeMapping.end();
			for(std::map<int, bool>::const_iterator it = m_statusCodeMapping.begin();
	   				it != mapEnd; ++it)
			{
				srmlogit(STORM_LOG_DEBUG, "storm::InstrumentedMonitor::printStatusCodeMapping()", "Code %d Successful %s\n" , it->first, (it->second? "true" : "false"));
			}
		}
	private:
		std::map<int, bool> m_statusCodeMapping;

	};
}
#endif /* INSTRUMENTED_MONITOR_HPP_ */
