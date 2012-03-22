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

#ifndef MONITOR_STUB_HPP_
#define MONITOR_STUB_HPP_

#include <string>

namespace storm {

class MonitorStub : public Monitor {
	public:

		static std::string nameOfOperationType(OperationType type)
		{
			return std::string("void");
		}

		MonitorStub() : Monitor(std::string("Stub"), std::string("Stub"), Stub)
		{
			M_EMPTY_VALUE = -1;
		};

		~MonitorStub() {};


		int getCompleted()
		{
			return M_EMPTY_VALUE;
		}

		int getFailed()
		{
			return M_EMPTY_VALUE;
		}

		int getErrors()
		{
			return M_EMPTY_VALUE;
		}

		float getMaxTime()
		{
			return M_EMPTY_VALUE;
		}

		float getMinTime()
		{
			return M_EMPTY_VALUE;
		}

		float getTotalTime()
		{
			return M_EMPTY_VALUE;
		}

		int getCompletedRound()
		{
			return M_EMPTY_VALUE;
		}

		int getFailedRound()
		{
			return M_EMPTY_VALUE;
		}

		int getErrorsRound()
		{
			return M_EMPTY_VALUE;
		}

		float getMaxTimeRound()
		{
			return M_EMPTY_VALUE;
		}

		float getMinTimeRound()
		{
			return M_EMPTY_VALUE;
		}

		float getTotalTimeRound()
		{
			return M_EMPTY_VALUE;
		}

		void registerSuccess(long executionTimeInMills)
		{
		}

		void registerFailure(long executionTimeInMills)
		{
		}

		void registerError(long executionTimeInMills)
		{
		}

		bool isEmpty()
		{
			return true;
		}

		bool isUpdated()
		{
			return false;
		}

		float computeExecTimeStandardDeviation()
		{
			return M_EMPTY_VALUE;
		}

		float computeExecTimeStandardDeviationRound()
		{
			return M_EMPTY_VALUE;
		}

		float computeAverageExecTime()
		{
			return M_EMPTY_VALUE;
		}

		float computeAverageExecTimeRound()
		{
			return M_EMPTY_VALUE;
		}

		void digestRound()
		{
		}

		void print()
		{
		}

	private:
		int M_EMPTY_VALUE;
	};
}
#endif /* MONITOR_STUB_HPP_ */

