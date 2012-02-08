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
#include <vector>
#include <algorithm>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/thread/locks.hpp>
#include "Monitor.hpp"
#include "srmlogit.h"

namespace storm {

class InstrumentedMonitor : public Monitor {
	public:

		InstrumentedMonitor(std::string name , std::vector<int> success) : Monitor(name)
		{
			this->_successCode = success;
		};

		~InstrumentedMonitor() {};


		void registerCompleted(long executionTimeInMills, int returnCode)
		{
			if(std::find(this->_successCode.begin(), this->_successCode.end(), returnCode) != this->_successCode.end())
			{
				registerSuccess(executionTimeInMills);
			}
			else
			{
				registerFailure(executionTimeInMills);
			}
		}

	private:
		std::vector<int> _successCode;
	};
}
#endif /* INSTRUMENTED_MONITOR_HPP_ */

