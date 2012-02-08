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
		Monitor(std::string name) : name(name)
		{
			this->reset();
		};

		~Monitor() {};

		std::string getName()
		{
			return this->name;
		}

		int getCompleted()
		{
			return this->completed;
		}

		int getFailed()
		{
			return this->failures;
		}

		int getErrors()
		{
			return this->errors;
		}

		float getAverageExecTime()
		{
			return this->average_exec_time;
		}

		void reset()
		{
			boost::lock_guard<boost::recursive_mutex> lock(mutex);
			this->completed = 0;
			this->failures = 0;
			this->errors = 0;
			this->average_exec_time = -1;
		}

		void registerSuccess(long executionTimeInMills)
		{
			boost::lock_guard<boost::recursive_mutex> lock(mutex);
			this->average_exec_time = this->computeAverageExecTime(executionTimeInMills);
			this->completed++;
		}

		void registerFailure(long executionTimeInMills)
		{
			boost::lock_guard<boost::recursive_mutex> lock(mutex);
			this->average_exec_time = this->computeAverageExecTime(executionTimeInMills);
			this->completed++;
			this->failures++;
		}

		void registerError(long executionTimeInMills)
		{
			boost::lock_guard<boost::recursive_mutex> lock(mutex);
			this->average_exec_time = this->computeAverageExecTime(executionTimeInMills);
			this->completed++;
			this->errors++;
		}

		float computeAverageExecTime(long executionTimeInMills)
		{
			if(! this->isClean()){
				float totalAccountedTimeInSeconds = this->getAverageExecTime() * this->completed;
				float totalUpdatedTimeInSeconds = totalAccountedTimeInSeconds + ((float) executionTimeInMills / 1000.0);
				return (totalUpdatedTimeInSeconds / (this->completed + 1));
			} else {
				return ((float) executionTimeInMills / 1000.0);
			}
		}

		bool isEmpty()
		{
			return (completed == 0);
		}

	private:
		std::string name;
		boost::recursive_mutex mutex;
		volatile int completed;
		volatile int failures;
		volatile int errors;
		volatile float average_exec_time;

		bool isClean()
		{
			return(average_exec_time < 0);
		}
	};
}
#endif /* MONITOR_HPP_ */

