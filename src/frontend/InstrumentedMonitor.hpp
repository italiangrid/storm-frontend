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

#include <string.h>
#include "Monitor.hpp"
#include "MonitorHelper.hpp"

namespace storm {

class InstrumentedMonitor : public Monitor {
	public:


		InstrumentedMonitor(std::string name , OperationType type) : Monitor(name,type)
		{
		};

		~InstrumentedMonitor() {};


		void registerCompleted(long executionTimeInMills, int returnCode)
		{
			if(MonitorHelper::isSuccessfull(this->getName() , returnCode))
			{
				registerSuccess(executionTimeInMills);
			}
			else
			{
				registerFailure(executionTimeInMills);
			}
		}

	};
}
#endif /* INSTRUMENTED_MONITOR_HPP_ */
