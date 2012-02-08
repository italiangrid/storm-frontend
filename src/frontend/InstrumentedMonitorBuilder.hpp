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

#ifndef INSTRUMENTED_MONITOR_BUILDER_HPP_
#define INSTRUMENTED_MONITOR_BUILDER_HPP_

#include <string>
#include <vector>
#include "InstrumentedMonitor.hpp"
#include <boost/assign/list_of.hpp>
#include "srmv2H.h"

namespace storm {

class InstrumentedMonitorBuilder{
	public:

		InstrumentedMonitorBuilder()
		{
		};

		~InstrumentedMonitorBuilder() {};

		static InstrumentedMonitor* buildAbortFiles()
		{
			std::vector<int> successfullReturnCodes = boost::assign::list_of(SRM_USCORESUCCESS)
				(SRM_USCOREPARTIAL_USCORESUCCESS);
			return new storm::InstrumentedMonitor(storm::SRM_ABORT_FILES_MONITOR_NAME, successfullReturnCodes);
		}

		static InstrumentedMonitor* buildAbortRequest()
		{
			std::vector<int> successfullReturnCodes = boost::assign::list_of(SRM_USCORESUCCESS)
				(SRM_USCOREPARTIAL_USCORESUCCESS);
			return new storm::InstrumentedMonitor(storm::SRM_ABORT_REQUEST_MONITOR_NAME, successfullReturnCodes);
		}

		static InstrumentedMonitor* buildBringOnline()
		{
			std::vector<int> successfullReturnCodes = boost::assign::list_of(SRM_USCORESUCCESS)
				(SRM_USCOREPARTIAL_USCORESUCCESS)(SRM_USCOREREQUEST_USCOREQUEUED)
				(SRM_USCOREREQUEST_USCOREINPROGRESS);
			return new storm::InstrumentedMonitor(storm::SRM_BRING_ONLINE_MONITOR_NAME, successfullReturnCodes);
		}

		static InstrumentedMonitor* buildChangeSpaceForFiles()
		{
			std::vector<int> successfullReturnCodes = boost::assign::list_of(SRM_USCORESUCCESS)
				(SRM_USCOREPARTIAL_USCORESUCCESS)(SRM_USCOREREQUEST_USCOREQUEUED)
				(SRM_USCOREREQUEST_USCOREINPROGRESS);
			return new storm::InstrumentedMonitor(storm::SRM_CHANGE_SPACE_FOR_FILES_MONITOR_NAME, successfullReturnCodes);
		}

		static InstrumentedMonitor* buildCheckPermission()
		{
			std::vector<int> successfullReturnCodes = boost::assign::list_of(SRM_USCORESUCCESS)
				(SRM_USCOREPARTIAL_USCORESUCCESS);
			return new storm::InstrumentedMonitor(storm::SRM_CHECK_PERMISSION_MONITOR_NAME, successfullReturnCodes);
		}

		static InstrumentedMonitor* buildCopy()
		{
			std::vector<int> successfullReturnCodes = boost::assign::list_of(SRM_USCORESUCCESS)
				(SRM_USCOREPARTIAL_USCORESUCCESS)(SRM_USCOREREQUEST_USCOREQUEUED)
				(SRM_USCOREREQUEST_USCOREINPROGRESS);
			return new storm::InstrumentedMonitor(storm::SRM_COPY_MONITOR_NAME, successfullReturnCodes);
		}

		static InstrumentedMonitor* buildExtendFileLifeTimeInSpace()
		{
			std::vector<int> successfullReturnCodes = boost::assign::list_of(SRM_USCORESUCCESS)
				(SRM_USCOREPARTIAL_USCORESUCCESS);
			return new storm::InstrumentedMonitor(storm::SRM_EXTEND_FILE_LIFE_TIME_IN_SPACE_MONITOR_NAME, successfullReturnCodes);
		}

		static InstrumentedMonitor* buildExtendFileLifeTime()
		{
			std::vector<int> successfullReturnCodes = boost::assign::list_of(SRM_USCORESUCCESS)
				(SRM_USCOREPARTIAL_USCORESUCCESS);
			return new storm::InstrumentedMonitor(storm::SRM_EXTEND_FILE_LIFE_TIME_MONITOR_NAME, successfullReturnCodes);
		}

		static InstrumentedMonitor* buildGetPermission()
		{
			std::vector<int> successfullReturnCodes = boost::assign::list_of(SRM_USCORESUCCESS)
				(SRM_USCOREPARTIAL_USCORESUCCESS);
			return new storm::InstrumentedMonitor(storm::SRM_GET_PERMISSION_MONITOR_NAME, successfullReturnCodes);
		}

		static InstrumentedMonitor* buildGetRequestSummary()
		{
			std::vector<int> successfullReturnCodes = boost::assign::list_of(SRM_USCORESUCCESS)
				(SRM_USCOREPARTIAL_USCORESUCCESS);
			return new storm::InstrumentedMonitor(storm::SRM_GET_REQUEST_SUMMARY_MONITOR_NAME, successfullReturnCodes);
		}

		static InstrumentedMonitor* buildGetRequestTokens()
		{
			std::vector<int> successfullReturnCodes = boost::assign::list_of(SRM_USCORESUCCESS);
			return new storm::InstrumentedMonitor(storm::SRM_GET_REQUEST_TOKENS_MONITOR_NAME, successfullReturnCodes);
		}

		static InstrumentedMonitor* buildGetSpaceMetaData()
		{
			std::vector<int> successfullReturnCodes = boost::assign::list_of(SRM_USCORESUCCESS)
				(SRM_USCOREPARTIAL_USCORESUCCESS);
			return new storm::InstrumentedMonitor(storm::SRM_GET_SPACE_META_DATA_MONITOR_NAME, successfullReturnCodes);
		}

		static InstrumentedMonitor* buildGetSpaceTokens()
		{
			std::vector<int> successfullReturnCodes = boost::assign::list_of(SRM_USCORESUCCESS);
			return new storm::InstrumentedMonitor(storm::SRM_GET_SPACE_TOKENS_MONITOR_NAME, successfullReturnCodes);
		}

		static InstrumentedMonitor* buildGetTransferProtocols()
		{
			std::vector<int> successfullReturnCodes = boost::assign::list_of(SRM_USCORESUCCESS);
			return new storm::InstrumentedMonitor(storm::SRM_GET_TRANSFER_PROTOCOLS_MONITOR_NAME, successfullReturnCodes);
		}

		static InstrumentedMonitor* buildLs()
		{
			std::vector<int> successfullReturnCodes = boost::assign::list_of(SRM_USCORESUCCESS)
				(SRM_USCOREPARTIAL_USCORESUCCESS)(SRM_USCOREREQUEST_USCOREQUEUED)
				(SRM_USCOREREQUEST_USCOREINPROGRESS);
			return new storm::InstrumentedMonitor(storm::SRM_LS_MONITOR_NAME, successfullReturnCodes);
		}

		static InstrumentedMonitor* buildMkdir()
		{
			std::vector<int> successfullReturnCodes = boost::assign::list_of(SRM_USCORESUCCESS);
			return new storm::InstrumentedMonitor(storm::SRM_MKDIR_MONITOR_NAME, successfullReturnCodes);
		}

		static InstrumentedMonitor* buildMv()
		{
			std::vector<int> successfullReturnCodes = boost::assign::list_of(SRM_USCORESUCCESS);
			return new storm::InstrumentedMonitor(storm::SRM_MV_MONITOR_NAME, successfullReturnCodes);
		}

		static InstrumentedMonitor* buildPing()
		{
			std::vector<int> successfullReturnCodes = boost::assign::list_of(SRM_USCORESUCCESS);
			return new storm::InstrumentedMonitor(storm::SRM_PING_MONITOR_NAME, successfullReturnCodes);
		}

		static InstrumentedMonitor* buildPrepareToGet()
		{
			std::vector<int> successfullReturnCodes = boost::assign::list_of(SRM_USCORESUCCESS)
				(SRM_USCOREPARTIAL_USCORESUCCESS)(SRM_USCOREREQUEST_USCOREQUEUED)
				(SRM_USCOREREQUEST_USCOREINPROGRESS);
			return new storm::InstrumentedMonitor(storm::SRM_PREPARE_TO_GET_MONITOR_NAME, successfullReturnCodes);
		}

		static InstrumentedMonitor* buildPrepareToPut()
		{
			std::vector<int> successfullReturnCodes = boost::assign::list_of(SRM_USCORESUCCESS)
				(SRM_USCOREPARTIAL_USCORESUCCESS)(SRM_USCOREREQUEST_USCOREQUEUED)
				(SRM_USCOREREQUEST_USCOREINPROGRESS);
			return new storm::InstrumentedMonitor(storm::SRM_PREPARE_TO_PUT_MONITOR_NAME, successfullReturnCodes);
		}

		static InstrumentedMonitor* buildPurgeFromSpace()
		{
			std::vector<int> successfullReturnCodes = boost::assign::list_of(SRM_USCORESUCCESS)
				(SRM_USCOREPARTIAL_USCORESUCCESS);
			return new storm::InstrumentedMonitor(storm::SRM_PURGE_FROM_SPACE_MONITOR_NAME, successfullReturnCodes);
		}

		static InstrumentedMonitor* buildPutDone()
		{
			std::vector<int> successfullReturnCodes = boost::assign::list_of(SRM_USCORESUCCESS)
				(SRM_USCOREPARTIAL_USCORESUCCESS);
			return new storm::InstrumentedMonitor(storm::SRM_PUT_DONE_MONITOR_NAME, successfullReturnCodes);
		}

		static InstrumentedMonitor* buildReleaseFiles()
		{
			std::vector<int> successfullReturnCodes = boost::assign::list_of(SRM_USCORESUCCESS)
				(SRM_USCOREPARTIAL_USCORESUCCESS);
			return new storm::InstrumentedMonitor(storm::SRM_RELEASE_FILES_MONITOR_NAME, successfullReturnCodes);
		}

		static InstrumentedMonitor* buildReleaseSpace()
		{
			std::vector<int> successfullReturnCodes = boost::assign::list_of(SRM_USCORESUCCESS);
			return new storm::InstrumentedMonitor(storm::SRM_RELEASE_SPACE_MONITOR_NAME, successfullReturnCodes);
		}

		static InstrumentedMonitor* buildReserveSpace()
		{
			std::vector<int> successfullReturnCodes = boost::assign::list_of(SRM_USCORESUCCESS)
				(SRM_USCOREREQUEST_USCOREQUEUED)(SRM_USCOREREQUEST_USCOREINPROGRESS)
				(SRM_USCORELOWER_USCORESPACE_USCOREGRANTED);
			return new storm::InstrumentedMonitor(storm::SRM_RESERVE_SPACE_MONITOR_NAME, successfullReturnCodes);
		}

		static InstrumentedMonitor* buildResumeRequest()
		{
			std::vector<int> successfullReturnCodes = boost::assign::list_of(SRM_USCORESUCCESS);
			return new storm::InstrumentedMonitor(storm::SRM_RESUME_REQUEST_MONITOR_NAME, successfullReturnCodes);
		}

		static InstrumentedMonitor* buildRmdir()
		{
			std::vector<int> successfullReturnCodes = boost::assign::list_of(SRM_USCORESUCCESS);
			return new storm::InstrumentedMonitor(storm::SRM_RMDIR_MONITOR_NAME, successfullReturnCodes);
		}

		static InstrumentedMonitor* buildRm()
		{
			std::vector<int> successfullReturnCodes = boost::assign::list_of(SRM_USCORESUCCESS)
				(SRM_USCOREPARTIAL_USCORESUCCESS);
			return new storm::InstrumentedMonitor(storm::SRM_RM_MONITOR_NAME, successfullReturnCodes);
		}

		static InstrumentedMonitor* buildSetPermission()
		{
			std::vector<int> successfullReturnCodes = boost::assign::list_of(SRM_USCORESUCCESS);
			return new storm::InstrumentedMonitor(storm::SRM_SET_PERMISSION_MONITOR_NAME, successfullReturnCodes);
		}

		static InstrumentedMonitor* buildStatusOfBringOnlineRequest()
		{
			std::vector<int> successfullReturnCodes = boost::assign::list_of(SRM_USCORESUCCESS)
				(SRM_USCOREPARTIAL_USCORESUCCESS)(SRM_USCOREREQUEST_USCOREQUEUED)
				(SRM_USCOREREQUEST_USCOREINPROGRESS)(SRM_USCOREREQUEST_USCORESUSPENDED);
			return new storm::InstrumentedMonitor(storm::SRM_STATUS_OF_BRING_ONLINE_REQUEST_MONITOR_NAME, successfullReturnCodes);
		}

		static InstrumentedMonitor* buildStatusOfChangeSpaceForFilesRequest()
		{
			std::vector<int> successfullReturnCodes = boost::assign::list_of(SRM_USCORESUCCESS)
				(SRM_USCOREPARTIAL_USCORESUCCESS)(SRM_USCOREREQUEST_USCOREQUEUED)
				(SRM_USCOREREQUEST_USCOREINPROGRESS)(SRM_USCOREREQUEST_USCORESUSPENDED);
			return new storm::InstrumentedMonitor(storm::SRM_STATUS_OF_CHANGE_SPACE_FOR_FILES_REQUEST_MONITOR_NAME, successfullReturnCodes);
		}

		static InstrumentedMonitor* buildStatusOfCopyRequest()
		{
			std::vector<int> successfullReturnCodes = boost::assign::list_of(SRM_USCORESUCCESS)
				(SRM_USCOREPARTIAL_USCORESUCCESS)(SRM_USCOREREQUEST_USCOREQUEUED)
				(SRM_USCOREREQUEST_USCOREINPROGRESS)(SRM_USCOREREQUEST_USCORESUSPENDED);
			return new storm::InstrumentedMonitor(storm::SRM_STATUS_OF_COPY_REQUEST_MONITOR_NAME, successfullReturnCodes);
		}

		static InstrumentedMonitor* buildStatusOfGetRequest()
		{
			std::vector<int> successfullReturnCodes = boost::assign::list_of(SRM_USCORESUCCESS)
				(SRM_USCOREPARTIAL_USCORESUCCESS)(SRM_USCOREREQUEST_USCOREQUEUED)
				(SRM_USCOREREQUEST_USCOREINPROGRESS)(SRM_USCOREREQUEST_USCORESUSPENDED);
			return new storm::InstrumentedMonitor(storm::SRM_STATUS_OF_GET_REQUEST_MONITOR_NAME, successfullReturnCodes);
		}

		static InstrumentedMonitor* buildStatusOfLsRequest()
		{
			std::vector<int> successfullReturnCodes = boost::assign::list_of(SRM_USCORESUCCESS)
				(SRM_USCOREPARTIAL_USCORESUCCESS)(SRM_USCOREREQUEST_USCOREQUEUED)
				(SRM_USCOREREQUEST_USCOREINPROGRESS);
			return new storm::InstrumentedMonitor(storm::SRM_STATUS_OF_LS_REQUEST_MONITOR_NAME, successfullReturnCodes);
		}

		static InstrumentedMonitor* buildStatusOfPutRequest()
		{
			std::vector<int> successfullReturnCodes = boost::assign::list_of(SRM_USCORESUCCESS)
				(SRM_USCOREPARTIAL_USCORESUCCESS)(SRM_USCOREREQUEST_USCOREQUEUED)
				(SRM_USCOREREQUEST_USCOREINPROGRESS)(SRM_USCOREREQUEST_USCORESUSPENDED);
			return new storm::InstrumentedMonitor(storm::SRM_STATUS_OF_PUT_REQUEST_MONITOR_NAME, successfullReturnCodes);
		}

		static InstrumentedMonitor* buildStatusOfReserveSpaceRequest()
		{
			std::vector<int> successfullReturnCodes = boost::assign::list_of(SRM_USCORESUCCESS)
				(SRM_USCOREREQUEST_USCOREQUEUED)(SRM_USCOREREQUEST_USCOREINPROGRESS)
				(SRM_USCOREREQUEST_USCORESUSPENDED)(SRM_USCORELOWER_USCORESPACE_USCOREGRANTED);
			return new storm::InstrumentedMonitor(storm::SRM_STATUS_OF_RESERVE_SPACE_REQUEST_MONITOR_NAME, successfullReturnCodes);
		}

		static InstrumentedMonitor* buildStatusOfUpdateSpaceRequest()
		{
			std::vector<int> successfullReturnCodes = boost::assign::list_of(SRM_USCORESUCCESS)
				(SRM_USCOREREQUEST_USCOREQUEUED)(SRM_USCOREREQUEST_USCOREINPROGRESS)
				(SRM_USCOREREQUEST_USCORESUSPENDED)(SRM_USCORELOWER_USCORESPACE_USCOREGRANTED);
			return new storm::InstrumentedMonitor(storm::SRM_STATUS_OF_UPDATE_SPACE_REQUEST_MONITOR_NAME, successfullReturnCodes);
		}

		static InstrumentedMonitor* buildSuspendRequest()
		{
			std::vector<int> successfullReturnCodes = boost::assign::list_of(SRM_USCORESUCCESS);
			return new storm::InstrumentedMonitor(storm::SRM_SUSPEND_REQUEST_MONITOR_NAME, successfullReturnCodes);
		}

		static InstrumentedMonitor* buildUpdateSpace()
		{
			std::vector<int> successfullReturnCodes = boost::assign::list_of(SRM_USCORESUCCESS)
				(SRM_USCOREREQUEST_USCOREQUEUED)(SRM_USCORELOWER_USCORESPACE_USCOREGRANTED);
			return new storm::InstrumentedMonitor(storm::SRM_UPDATE_SPACE_MONITOR_NAME, successfullReturnCodes);
		}
	};
}
#endif /* INSTRUMENTED_MONITOR_BUILDER_HPP_ */












