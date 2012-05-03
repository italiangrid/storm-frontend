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

#ifndef MONITOR_HPP_
#define MONITOR_HPP_

#include <string>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/thread/locks.hpp>

#include "srmlogit.h"

#include <math.h>

namespace storm {

const std::string SRM_ABORT_FILES_MONITOR_NAME("srm_abort_files_monitor");
const std::string SRM_LS_MONITOR_NAME("srm_ls_monitor");
const std::string SRM_ABORT_REQUEST_MONITOR_NAME("srm_abort_request_monitor");
const std::string SRM_CHANGE_SPACE_FOR_FILES_MONITOR_NAME("srm_change_space_for_files_monitor");
const std::string SRM_CHECK_PERMISSION_MONITOR_NAME("srm_check_permission_monitor");
const std::string SRM_EXTEND_FILE_LIFE_TIME_IN_SPACE_MONITOR_NAME("srm_extend_file_life_time_in_space_monitor");
const std::string SRM_EXTEND_FILE_LIFE_TIME_MONITOR_NAME("srm_extend_file_life_time_monitor");
const std::string SRM_GET_PERMISSION_MONITOR_NAME("srm_get_permission_monitor");
const std::string SRM_GET_REQUEST_SUMMARY_MONITOR_NAME("srm_get_request_summary_monitor");
const std::string SRM_GET_REQUEST_TOKENS_MONITOR_NAME("srm_get_request_tokens_monitor");
const std::string SRM_GET_SPACE_META_DATA_MONITOR_NAME("srm_get_space_meta_data_monitor");
const std::string SRM_GET_SPACE_TOKENS_MONITOR_NAME("srm_get_space_tokens_monitor");
const std::string SRM_GET_TRANSFER_PROTOCOLS_MONITOR_NAME("srm_get_transfer_protocols_monitor");
const std::string SRM_MKDIR_MONITOR_NAME("srm_mkdir_monitor");
const std::string SRM_MV_MONITOR_NAME("srm_mv_monitor");
const std::string SRM_PING_MONITOR_NAME("srm_ping_monitor");
const std::string SRM_PURGE_FROM_SPACE_MONITOR_NAME("srm_purge_from_space_monitor");
const std::string SRM_PUT_DONE_MONITOR_NAME("srm_put_done_monitor");
const std::string SRM_RELEASE_FILES_MONITOR_NAME("srm_release_files_monitor");
const std::string SRM_RELEASE_SPACE_MONITOR_NAME("srm_release_space_monitor");
const std::string SRM_RESERVE_SPACE_MONITOR_NAME("srm_reserve_space_monitor");
const std::string SRM_RESUME_REQUEST_MONITOR_NAME("srm_resume_request_monitor");
const std::string SRM_RMDIR_MONITOR_NAME("srm_rmdir_monitor");
const std::string SRM_RM_MONITOR_NAME("srm_rm_monitor");
const std::string SRM_SET_PERMISSION_MONITOR_NAME("srm_set_permission_monitor");
const std::string SRM_STATUS_OF_CHANGE_SPACE_FOR_FILES_REQUEST_MONITOR_NAME("srm_status_of_change_space_for_files_request_monitor");
const std::string SRM_STATUS_OF_LS_REQUEST_MONITOR_NAME("srm_status_of_ls_request_monitor");
const std::string SRM_STATUS_OF_RESERVE_SPACE_REQUEST_MONITOR_NAME("srm_status_of_reserve_space_request_monitor");
const std::string SRM_STATUS_OF_UPDATE_SPACE_REQUEST_MONITOR_NAME("srm_status_of_update_space_request_monitor");
const std::string SRM_SUSPEND_REQUEST_MONITOR_NAME("srm_suspend_request_monitor");
const std::string SRM_UPDATE_SPACE_MONITOR_NAME("srm_update_space_monitor");
const std::string SRM_PREPARE_TO_PUT_MONITOR_NAME("srm_prepare_to_put_monitor");
const std::string SRM_STATUS_OF_PUT_REQUEST_MONITOR_NAME("srm_status_of_put_request_monitor");
const std::string SRM_PREPARE_TO_GET_MONITOR_NAME("srm_prepare_to_get_monitor");
const std::string SRM_STATUS_OF_GET_REQUEST_MONITOR_NAME("srm_status_of_get_request_monitor");
const std::string SRM_COPY_MONITOR_NAME("srm_copy_monitor");
const std::string SRM_STATUS_OF_COPY_REQUEST_MONITOR_NAME("srm_status_of_copy_request_monitor");
const std::string SRM_BRING_ONLINE_MONITOR_NAME("srm_bring_online_monitor");
const std::string SRM_STATUS_OF_BRING_ONLINE_REQUEST_MONITOR_NAME("srm_status_of_bring_online_request_monitor");

class Monitor {
	public:
		enum OperationType
		{
			Synchronous = 0,
			Asynchronous = 1,
			Stub = 2
		};

		static std::string nameOfOperationType(OperationType type)
		{
			std::string name;
			switch (type)
			{
				case Synchronous:
					name = std::string("S");
					break;
				case Asynchronous:
					name = std::string("A");
					break;
				default:
					name = std::string("Unknown");
					break;
			}
			return name;
		}

		Monitor(std::string name, std::string friendlyName, OperationType type) : name(name) , friendlyName(friendlyName), type(type)
		{
			this->current_status.reset();
			this->aggregated_status.reset();
		};

		~Monitor() {};

		const std::string getName()
		{
			return this->name;
		}

		const std::string getFriendlyName()
		{
			return this->friendlyName;
		}

		const OperationType getType()
		{
			return this->type;
		}

		int getCompleted()
		{
			return this->aggregated_status.completed;
		}

		int getFailed()
		{
			return this->aggregated_status.failures;
		}

		int getErrors()
		{
			return this->aggregated_status.errors;
		}

		int getSuccessfull()
		{
			return (this->aggregated_status.completed - (this->aggregated_status.failures + this->aggregated_status.errors));
		}

		float getMaxTime()
		{
			return this->aggregated_status.max_exec_time;
		}

		float getMinTime()
		{
			return this->aggregated_status.min_exec_time;
		}

		float getTotalTime()
		{
			return this->aggregated_status.total_exec_time;
		}

		int getCompletedRound()
		{
			return this->current_status.completed;
		}

		int getFailedRound()
		{
			return this->current_status.failures;
		}

		int getErrorsRound()
		{
			return this->current_status.errors;
		}

		int getSuccessfullRound()
		{
			return (this->current_status.completed - (this->current_status.failures + this->current_status.errors));
		}

		float getMaxTimeRound()
		{
			return this->current_status.max_exec_time;
		}

		float getMinTimeRound()
		{
			return this->current_status.min_exec_time;
		}

		float getTotalTimeRound()
		{
			return this->current_status.total_exec_time;
		}

		void registerSuccess(long executionTimeInMills)
		{
			boost::lock_guard<boost::recursive_mutex> lock(mutex);
			this->updateTotalTime((float) executionTimeInMills / 1000);
			this->updateTimeLimits((float) executionTimeInMills / 1000);
			this->current_status.completed++;
		}

		void registerFailure(long executionTimeInMills)
		{
			boost::lock_guard<boost::recursive_mutex> lock(mutex);
			this->updateTotalTime((float) executionTimeInMills / 1000);
			this->updateTimeLimits((float) executionTimeInMills / 1000);
			this->current_status.completed++;
			this->current_status.failures++;
		}

		void registerError(long executionTimeInMills)
		{
			boost::lock_guard<boost::recursive_mutex> lock(mutex);
			this->updateTotalTime((float) executionTimeInMills / 1000);
			this->updateTimeLimits((float) executionTimeInMills / 1000);
			this->current_status.completed++;
			this->current_status.errors++;
		}

		bool isEmpty()
		{
			return this->current_status.isClean() && this->aggregated_status.isClean();
		}

		bool isUpdated()
		{
			return !this->current_status.isClean();
		}

		float computeExecTimeStandardDeviation()
		{
			boost::lock_guard<boost::recursive_mutex> lock(mutex);
			return computeStandardDeviation(this->aggregated_status.completed , this->aggregated_status.total_exec_time, this->aggregated_status.total_square_exec_time);
		}

		float computeExecTimeStandardDeviationRound()
		{
			boost::lock_guard<boost::recursive_mutex> lock(mutex);
			return computeStandardDeviation(this->current_status.completed , this->current_status.total_exec_time, this->current_status.total_square_exec_time);
		}

		float computeAverageExecTime()
		{
			boost::lock_guard<boost::recursive_mutex> lock(mutex);
			return computeAverage(this->aggregated_status.completed, this->aggregated_status.total_exec_time);
		}

		float computeAverageExecTimeRound()
		{
			boost::lock_guard<boost::recursive_mutex> lock(mutex);
			return computeAverage(this->current_status.completed, this->current_status.total_exec_time);
		}

		void digestRound()
		{
			boost::lock_guard<boost::recursive_mutex> lock(mutex);
			this->aggregated_status.mergeIn(this->current_status);
			this->current_status.reset();
		}

		void print()
		{
			char* _funcName = "print";
			srmlogit(
				STORM_LOG_DEBUG,
				_funcName,
				"Name %s Friendly Name %s | Type %s | Current Status: completed %u faileures - %u errors - %u tot time - %f tot square time - %f min time - %f max time - %f | Aggregated Status: completed %u faileures - %u errors - %u tot time - %f tot square time - %f min time - %f max time - %f\n",
				this->name.c_str(), this->friendlyName.c_str(),
				nameOfOperationType(this->type).c_str(),
				this->current_status.completed, this->current_status.failures,
				this->current_status.errors,
				this->current_status.total_exec_time,
				this->current_status.total_square_exec_time,
				this->current_status.min_exec_time,
				this->current_status.max_exec_time,
				this->aggregated_status.completed, this->aggregated_status.failures,
				this->aggregated_status.errors,
				this->aggregated_status.total_exec_time,
				this->aggregated_status.total_square_exec_time,
				this->aggregated_status.min_exec_time,
				this->aggregated_status.max_exec_time);
		}

	private:
		const std::string name;
		const std::string friendlyName;
		const OperationType type;
		boost::recursive_mutex mutex;

		class Status{
		public:

			boost::recursive_mutex mutex;
			volatile int completed;
			volatile int failures;
			volatile int errors;
			volatile float total_exec_time;
			volatile float total_square_exec_time;
			volatile float min_exec_time;
			volatile float max_exec_time;

			void reset()
			{
				boost::lock_guard<boost::recursive_mutex> lock(this->mutex);
				this->completed = 0;
				this->failures = 0;
				this->errors = 0;
				this->total_exec_time = 0;
				this->total_square_exec_time = 0;
				this->max_exec_time = 0;
				this->min_exec_time = 0;
			}

			bool isClean()
			{
				return(this->completed == 0);
			}

			void mergeIn(Status &other)
			{
				boost::lock_guard<boost::recursive_mutex> lock(this->mutex);
				this->checkUpdateMinTime(other.min_exec_time);
				this->checkUpdateMaxTime(other.max_exec_time);
				this->completed += other.completed;
				this->failures += other.failures;
				this->errors += other.errors;
				this->total_exec_time += other.total_exec_time;
				this->total_square_exec_time += other.total_square_exec_time;
			}

			void checkUpdateMinTime(float min_exec_time_candidate)
			{
				boost::lock_guard<boost::recursive_mutex> lock(this->mutex);
				if(this->isClean() || min_exec_time_candidate < this->min_exec_time)
				{
					this->min_exec_time = min_exec_time_candidate;
				}
			}

			void checkUpdateMaxTime(float max_exec_time_candidate)
			{
				boost::lock_guard<boost::recursive_mutex> lock(this->mutex);
				if(this->isClean() || max_exec_time_candidate > this->max_exec_time)
				{
					this->max_exec_time = max_exec_time_candidate;
				}
			}

			void updateTotalExecTime(float timeInSecs)
			{
				boost::lock_guard<boost::recursive_mutex> lock(this->mutex);
				this->total_exec_time+=timeInSecs;
			}

			void updateTotalSquareExecTime(float timeInSecs)
			{
				boost::lock_guard<boost::recursive_mutex> lock(this->mutex);
				this->total_square_exec_time+=pow(timeInSecs, 2);
			}

		};

		Status current_status;
		Status aggregated_status;

		void updateTotalTime(float timeInSecs)
		{
			boost::lock_guard<boost::recursive_mutex> lock(mutex);
			this->current_status.updateTotalExecTime(timeInSecs);
			this->current_status.updateTotalSquareExecTime(timeInSecs);
		}

		void updateTimeLimits(float timeInSec)
		{
			boost::lock_guard<boost::recursive_mutex> lock(mutex);
			this->current_status.checkUpdateMaxTime(timeInSec);
			this->current_status.checkUpdateMinTime(timeInSec);
		}

		static float computeStandardDeviation(int count, float total, float total_square)
		{
			char* _funcName = "computeStandardDeviation";
			srmlogit(STORM_LOG_DEBUG, _funcName,
				"Computing standard deviation. completed operations = %u, total time = %f total squares time = %f\n",
				count, total, total_square);
			if(count == 0 || count == 1)
			{
				return 0;
			}
			float reverseCount = ((float)1) / count;
			float countTimesSquareSum = (float) (count * total_square);
			float sumSquare = (float) pow(total ,2);
			if(countTimesSquareSum - sumSquare <= 0)
			{
				return 0;
			}
			float sqrtSquareSumsDiff = (float) sqrt(countTimesSquareSum - sumSquare);
			return reverseCount * sqrtSquareSumsDiff;
		}

		static float computeAverage(int count, float total)
		{
			char* _funcName = "computeAverage";
			if(count == 0)
			{
				return 0;
			}
			return total / count;
		}
	};
}
#endif /* MONITOR_HPP_ */

