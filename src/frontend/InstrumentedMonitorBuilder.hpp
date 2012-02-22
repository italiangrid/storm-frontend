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

#include "InstrumentedMonitor.hpp"
#include "MonitorHelper.hpp"
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
			MonitorHelper::addSuccessStatusCodeMapping(SRM_ABORT_FILES_MONITOR_NAME,SRM_USCORESUCCESS);
			MonitorHelper::addSuccessStatusCodeMapping(SRM_ABORT_FILES_MONITOR_NAME,SRM_USCOREPARTIAL_USCORESUCCESS);
			return new InstrumentedMonitor(SRM_ABORT_FILES_MONITOR_NAME, "Abort files", Monitor::Synchronous);
		}

		static InstrumentedMonitor* buildAbortRequest()
		{
			MonitorHelper::addSuccessStatusCodeMapping(SRM_ABORT_REQUEST_MONITOR_NAME,SRM_USCORESUCCESS);
			MonitorHelper::addSuccessStatusCodeMapping(SRM_ABORT_REQUEST_MONITOR_NAME,SRM_USCOREPARTIAL_USCORESUCCESS);
			return new InstrumentedMonitor(SRM_ABORT_REQUEST_MONITOR_NAME, "Abort request", Monitor::Synchronous);
		}

		static InstrumentedMonitor* buildBringOnline()
		{
			MonitorHelper::addSuccessStatusCodeMapping(SRM_BRING_ONLINE_MONITOR_NAME,SRM_USCORESUCCESS);
			MonitorHelper::addSuccessStatusCodeMapping(SRM_BRING_ONLINE_MONITOR_NAME,SRM_USCOREPARTIAL_USCORESUCCESS);
			MonitorHelper::addSuccessStatusCodeMapping(SRM_BRING_ONLINE_MONITOR_NAME,SRM_USCOREREQUEST_USCOREQUEUED);
			MonitorHelper::addSuccessStatusCodeMapping(SRM_BRING_ONLINE_MONITOR_NAME,SRM_USCOREREQUEST_USCOREINPROGRESS);
			return new InstrumentedMonitor(SRM_BRING_ONLINE_MONITOR_NAME, "BOL" , Monitor::Asynchronous);
		}

		static InstrumentedMonitor* buildChangeSpaceForFiles()
		{
			MonitorHelper::addSuccessStatusCodeMapping(SRM_CHANGE_SPACE_FOR_FILES_MONITOR_NAME,SRM_USCORESUCCESS);
			MonitorHelper::addSuccessStatusCodeMapping(SRM_CHANGE_SPACE_FOR_FILES_MONITOR_NAME,SRM_USCOREPARTIAL_USCORESUCCESS);
			MonitorHelper::addSuccessStatusCodeMapping(SRM_CHANGE_SPACE_FOR_FILES_MONITOR_NAME,SRM_USCOREREQUEST_USCOREQUEUED);
			MonitorHelper::addSuccessStatusCodeMapping(SRM_CHANGE_SPACE_FOR_FILES_MONITOR_NAME,SRM_USCOREREQUEST_USCOREINPROGRESS);
			return new InstrumentedMonitor(SRM_CHANGE_SPACE_FOR_FILES_MONITOR_NAME, "Change space for files", Monitor::Synchronous);
		}

		static InstrumentedMonitor* buildCheckPermission()
		{
			MonitorHelper::addSuccessStatusCodeMapping(SRM_CHECK_PERMISSION_MONITOR_NAME,SRM_USCORESUCCESS);
			MonitorHelper::addSuccessStatusCodeMapping(SRM_CHECK_PERMISSION_MONITOR_NAME,SRM_USCOREPARTIAL_USCORESUCCESS);
			return new InstrumentedMonitor(SRM_CHECK_PERMISSION_MONITOR_NAME,"Check permission" , Monitor::Synchronous);
		}

		static InstrumentedMonitor* buildCopy()
		{
			MonitorHelper::addSuccessStatusCodeMapping(SRM_COPY_MONITOR_NAME,SRM_USCORESUCCESS);
			MonitorHelper::addSuccessStatusCodeMapping(SRM_COPY_MONITOR_NAME,SRM_USCOREPARTIAL_USCORESUCCESS);
			MonitorHelper::addSuccessStatusCodeMapping(SRM_COPY_MONITOR_NAME,SRM_USCOREREQUEST_USCOREQUEUED);
			MonitorHelper::addSuccessStatusCodeMapping(SRM_COPY_MONITOR_NAME,SRM_USCOREREQUEST_USCOREINPROGRESS);
			return new InstrumentedMonitor(SRM_COPY_MONITOR_NAME, "Cp" ,Monitor::Asynchronous);
		}

		static InstrumentedMonitor* buildExtendFileLifeTimeInSpace()
		{
			MonitorHelper::addSuccessStatusCodeMapping(SRM_EXTEND_FILE_LIFE_TIME_IN_SPACE_MONITOR_NAME,SRM_USCORESUCCESS);
			MonitorHelper::addSuccessStatusCodeMapping(SRM_EXTEND_FILE_LIFE_TIME_IN_SPACE_MONITOR_NAME,SRM_USCOREPARTIAL_USCORESUCCESS);
			return new InstrumentedMonitor(SRM_EXTEND_FILE_LIFE_TIME_IN_SPACE_MONITOR_NAME, "Extend file life time in space" ,Monitor::Synchronous);
		}

		static InstrumentedMonitor* buildExtendFileLifeTime()
		{
			MonitorHelper::addSuccessStatusCodeMapping(SRM_EXTEND_FILE_LIFE_TIME_MONITOR_NAME,SRM_USCORESUCCESS);
			MonitorHelper::addSuccessStatusCodeMapping(SRM_EXTEND_FILE_LIFE_TIME_MONITOR_NAME,SRM_USCOREPARTIAL_USCORESUCCESS);
			return new InstrumentedMonitor(SRM_EXTEND_FILE_LIFE_TIME_MONITOR_NAME, "Extend file life time" , Monitor::Synchronous);
		}

		static InstrumentedMonitor* buildGetPermission()
		{
			MonitorHelper::addSuccessStatusCodeMapping(SRM_GET_PERMISSION_MONITOR_NAME,SRM_USCORESUCCESS);
			MonitorHelper::addSuccessStatusCodeMapping(SRM_GET_PERMISSION_MONITOR_NAME,SRM_USCOREPARTIAL_USCORESUCCESS);
			return new InstrumentedMonitor(SRM_GET_PERMISSION_MONITOR_NAME, "Get permission", Monitor::Synchronous);
		}

		static InstrumentedMonitor* buildGetRequestSummary()
		{
			MonitorHelper::addSuccessStatusCodeMapping(SRM_GET_REQUEST_SUMMARY_MONITOR_NAME,SRM_USCORESUCCESS);
			MonitorHelper::addSuccessStatusCodeMapping(SRM_GET_REQUEST_SUMMARY_MONITOR_NAME,SRM_USCOREPARTIAL_USCORESUCCESS);
			return new InstrumentedMonitor(SRM_GET_REQUEST_SUMMARY_MONITOR_NAME, "Get request summary", Monitor::Synchronous);
		}

		static InstrumentedMonitor* buildGetRequestTokens()
		{
			MonitorHelper::addSuccessStatusCodeMapping(SRM_GET_REQUEST_TOKENS_MONITOR_NAME,SRM_USCORESUCCESS);
			return new InstrumentedMonitor(SRM_GET_REQUEST_TOKENS_MONITOR_NAME, "Get request tokens", Monitor::Synchronous);
		}

		static InstrumentedMonitor* buildGetSpaceMetaData()
		{
			MonitorHelper::addSuccessStatusCodeMapping(SRM_GET_SPACE_META_DATA_MONITOR_NAME,SRM_USCORESUCCESS);
			MonitorHelper::addSuccessStatusCodeMapping(SRM_GET_SPACE_META_DATA_MONITOR_NAME,SRM_USCOREPARTIAL_USCORESUCCESS);
			return new InstrumentedMonitor(SRM_GET_SPACE_META_DATA_MONITOR_NAME, "Get space meta data", Monitor::Synchronous);
		}

		static InstrumentedMonitor* buildGetSpaceTokens()
		{
			MonitorHelper::addSuccessStatusCodeMapping(SRM_GET_SPACE_TOKENS_MONITOR_NAME,SRM_USCORESUCCESS);
			return new InstrumentedMonitor(SRM_GET_SPACE_TOKENS_MONITOR_NAME, "Get space tokens", Monitor::Synchronous);
		}

		static InstrumentedMonitor* buildGetTransferProtocols()
		{
			MonitorHelper::addSuccessStatusCodeMapping(SRM_GET_TRANSFER_PROTOCOLS_MONITOR_NAME,SRM_USCORESUCCESS);
			return new InstrumentedMonitor(SRM_GET_TRANSFER_PROTOCOLS_MONITOR_NAME, "Get transfer protocols", Monitor::Synchronous);
		}

		static InstrumentedMonitor* buildLs()
		{
			MonitorHelper::addSuccessStatusCodeMapping(SRM_LS_MONITOR_NAME,SRM_USCORESUCCESS);
			MonitorHelper::addSuccessStatusCodeMapping(SRM_LS_MONITOR_NAME,SRM_USCOREPARTIAL_USCORESUCCESS);
			MonitorHelper::addSuccessStatusCodeMapping(SRM_LS_MONITOR_NAME,SRM_USCOREREQUEST_USCOREQUEUED);
			MonitorHelper::addSuccessStatusCodeMapping(SRM_LS_MONITOR_NAME,SRM_USCOREREQUEST_USCOREINPROGRESS);
			return new InstrumentedMonitor(SRM_LS_MONITOR_NAME, "Ls", Monitor::Synchronous);
		}

		static InstrumentedMonitor* buildMkdir()
		{
			MonitorHelper::addSuccessStatusCodeMapping(SRM_MKDIR_MONITOR_NAME,SRM_USCORESUCCESS);
			return new InstrumentedMonitor(SRM_MKDIR_MONITOR_NAME, "Mkdir", Monitor::Synchronous);
		}

		static InstrumentedMonitor* buildMv()
		{
			MonitorHelper::addSuccessStatusCodeMapping(SRM_MV_MONITOR_NAME,SRM_USCORESUCCESS);
			return new InstrumentedMonitor(SRM_MV_MONITOR_NAME, "Mv", Monitor::Synchronous);
		}

		static InstrumentedMonitor* buildPing()
		{
			MonitorHelper::addSuccessStatusCodeMapping(SRM_PING_MONITOR_NAME,SRM_USCORESUCCESS);
			return new InstrumentedMonitor(SRM_PING_MONITOR_NAME, "Ping", Monitor::Synchronous);
		}

		static InstrumentedMonitor* buildPrepareToGet()
		{
			MonitorHelper::addSuccessStatusCodeMapping(SRM_PREPARE_TO_GET_MONITOR_NAME,SRM_USCORESUCCESS);
			MonitorHelper::addSuccessStatusCodeMapping(SRM_PREPARE_TO_GET_MONITOR_NAME,SRM_USCOREPARTIAL_USCORESUCCESS);
			MonitorHelper::addSuccessStatusCodeMapping(SRM_PREPARE_TO_GET_MONITOR_NAME,SRM_USCOREREQUEST_USCOREQUEUED);
			MonitorHelper::addSuccessStatusCodeMapping(SRM_PREPARE_TO_GET_MONITOR_NAME,SRM_USCOREREQUEST_USCOREINPROGRESS);
			return new InstrumentedMonitor(SRM_PREPARE_TO_GET_MONITOR_NAME, "PTG", Monitor::Asynchronous);
		}

		static InstrumentedMonitor* buildPrepareToPut()
		{
			MonitorHelper::addSuccessStatusCodeMapping(SRM_PREPARE_TO_PUT_MONITOR_NAME,SRM_USCORESUCCESS);
			MonitorHelper::addSuccessStatusCodeMapping(SRM_PREPARE_TO_PUT_MONITOR_NAME,SRM_USCOREPARTIAL_USCORESUCCESS);
			MonitorHelper::addSuccessStatusCodeMapping(SRM_PREPARE_TO_PUT_MONITOR_NAME,SRM_USCOREREQUEST_USCOREQUEUED);
			MonitorHelper::addSuccessStatusCodeMapping(SRM_PREPARE_TO_PUT_MONITOR_NAME,SRM_USCOREREQUEST_USCOREINPROGRESS);
			return new InstrumentedMonitor(SRM_PREPARE_TO_PUT_MONITOR_NAME, "PTP", Monitor::Asynchronous);
		}

		static InstrumentedMonitor* buildPurgeFromSpace()
		{
			MonitorHelper::addSuccessStatusCodeMapping(SRM_PURGE_FROM_SPACE_MONITOR_NAME,SRM_USCORESUCCESS);
			MonitorHelper::addSuccessStatusCodeMapping(SRM_PURGE_FROM_SPACE_MONITOR_NAME,SRM_USCOREPARTIAL_USCORESUCCESS);
			return new InstrumentedMonitor(SRM_PURGE_FROM_SPACE_MONITOR_NAME, "Purge from Space", Monitor::Synchronous);
		}

		static InstrumentedMonitor* buildPutDone()
		{
			MonitorHelper::addSuccessStatusCodeMapping(SRM_PUT_DONE_MONITOR_NAME,SRM_USCORESUCCESS);
			MonitorHelper::addSuccessStatusCodeMapping(SRM_PUT_DONE_MONITOR_NAME,SRM_USCOREPARTIAL_USCORESUCCESS);
			return new InstrumentedMonitor(SRM_PUT_DONE_MONITOR_NAME, "Put done", Monitor::Synchronous);
		}

		static InstrumentedMonitor* buildReleaseFiles()
		{
			MonitorHelper::addSuccessStatusCodeMapping(SRM_RELEASE_FILES_MONITOR_NAME,SRM_USCORESUCCESS);
			MonitorHelper::addSuccessStatusCodeMapping(SRM_RELEASE_FILES_MONITOR_NAME,SRM_USCOREPARTIAL_USCORESUCCESS);
			return new InstrumentedMonitor(SRM_RELEASE_FILES_MONITOR_NAME, "Release files", Monitor::Synchronous);
		}

		static InstrumentedMonitor* buildReleaseSpace()
		{
			MonitorHelper::addSuccessStatusCodeMapping(SRM_RELEASE_SPACE_MONITOR_NAME,SRM_USCORESUCCESS);
			return new InstrumentedMonitor(SRM_RELEASE_SPACE_MONITOR_NAME, "Release space", Monitor::Synchronous);
		}

		static InstrumentedMonitor* buildReserveSpace()
		{
			MonitorHelper::addSuccessStatusCodeMapping(SRM_RESERVE_SPACE_MONITOR_NAME,SRM_USCORESUCCESS);
			MonitorHelper::addSuccessStatusCodeMapping(SRM_RESERVE_SPACE_MONITOR_NAME,SRM_USCOREREQUEST_USCOREQUEUED);
			MonitorHelper::addSuccessStatusCodeMapping(SRM_RESERVE_SPACE_MONITOR_NAME,SRM_USCOREREQUEST_USCOREINPROGRESS);
			MonitorHelper::addSuccessStatusCodeMapping(SRM_RESERVE_SPACE_MONITOR_NAME,SRM_USCORELOWER_USCORESPACE_USCOREGRANTED);
			return new InstrumentedMonitor(SRM_RESERVE_SPACE_MONITOR_NAME, "Reserve space", Monitor::Synchronous);
		}

		static InstrumentedMonitor* buildResumeRequest()
		{
			MonitorHelper::addSuccessStatusCodeMapping(SRM_RESUME_REQUEST_MONITOR_NAME,SRM_USCORESUCCESS);
			return new InstrumentedMonitor(SRM_RESUME_REQUEST_MONITOR_NAME, "Resume request", Monitor::Synchronous);
		}

		static InstrumentedMonitor* buildRmdir()
		{
			MonitorHelper::addSuccessStatusCodeMapping(SRM_RMDIR_MONITOR_NAME,SRM_USCORESUCCESS);
			return new InstrumentedMonitor(SRM_RMDIR_MONITOR_NAME, "Rmdir", Monitor::Synchronous);
		}

		static InstrumentedMonitor* buildRm()
		{
			MonitorHelper::addSuccessStatusCodeMapping(SRM_RM_MONITOR_NAME,SRM_USCORESUCCESS);
			MonitorHelper::addSuccessStatusCodeMapping(SRM_RM_MONITOR_NAME,SRM_USCOREPARTIAL_USCORESUCCESS);
			return new InstrumentedMonitor(SRM_RM_MONITOR_NAME, "Rm", Monitor::Synchronous);
		}

		static InstrumentedMonitor* buildSetPermission()
		{
			MonitorHelper::addSuccessStatusCodeMapping(SRM_SET_PERMISSION_MONITOR_NAME,SRM_USCORESUCCESS);
			return new InstrumentedMonitor(SRM_SET_PERMISSION_MONITOR_NAME, "Set permission", Monitor::Synchronous);
		}

		static InstrumentedMonitor* buildStatusOfBringOnlineRequest()
		{
			MonitorHelper::addSuccessStatusCodeMapping(SRM_STATUS_OF_BRING_ONLINE_REQUEST_MONITOR_NAME,SRM_USCORESUCCESS);
			MonitorHelper::addSuccessStatusCodeMapping(SRM_STATUS_OF_BRING_ONLINE_REQUEST_MONITOR_NAME,SRM_USCOREPARTIAL_USCORESUCCESS);
			MonitorHelper::addSuccessStatusCodeMapping(SRM_STATUS_OF_BRING_ONLINE_REQUEST_MONITOR_NAME,SRM_USCOREREQUEST_USCOREQUEUED);
			MonitorHelper::addSuccessStatusCodeMapping(SRM_STATUS_OF_BRING_ONLINE_REQUEST_MONITOR_NAME,SRM_USCOREREQUEST_USCOREINPROGRESS);
			MonitorHelper::addSuccessStatusCodeMapping(SRM_STATUS_OF_BRING_ONLINE_REQUEST_MONITOR_NAME,SRM_USCOREREQUEST_USCORESUSPENDED);
			return new InstrumentedMonitor(SRM_STATUS_OF_BRING_ONLINE_REQUEST_MONITOR_NAME, "Status BOL", Monitor::Synchronous);
		}

		static InstrumentedMonitor* buildStatusOfChangeSpaceForFilesRequest()
		{
			MonitorHelper::addSuccessStatusCodeMapping(SRM_STATUS_OF_CHANGE_SPACE_FOR_FILES_REQUEST_MONITOR_NAME,SRM_USCORESUCCESS);
			MonitorHelper::addSuccessStatusCodeMapping(SRM_STATUS_OF_CHANGE_SPACE_FOR_FILES_REQUEST_MONITOR_NAME,SRM_USCOREPARTIAL_USCORESUCCESS);
			MonitorHelper::addSuccessStatusCodeMapping(SRM_STATUS_OF_CHANGE_SPACE_FOR_FILES_REQUEST_MONITOR_NAME,SRM_USCOREREQUEST_USCOREQUEUED);
			MonitorHelper::addSuccessStatusCodeMapping(SRM_STATUS_OF_CHANGE_SPACE_FOR_FILES_REQUEST_MONITOR_NAME,SRM_USCOREREQUEST_USCOREINPROGRESS);
			MonitorHelper::addSuccessStatusCodeMapping(SRM_STATUS_OF_CHANGE_SPACE_FOR_FILES_REQUEST_MONITOR_NAME,SRM_USCOREREQUEST_USCORESUSPENDED);
			return new InstrumentedMonitor(SRM_STATUS_OF_CHANGE_SPACE_FOR_FILES_REQUEST_MONITOR_NAME, "Status change space for files", Monitor::Synchronous);
		}

		static InstrumentedMonitor* buildStatusOfCopyRequest()
		{
			MonitorHelper::addSuccessStatusCodeMapping(SRM_STATUS_OF_COPY_REQUEST_MONITOR_NAME,SRM_USCORESUCCESS);
			MonitorHelper::addSuccessStatusCodeMapping(SRM_STATUS_OF_COPY_REQUEST_MONITOR_NAME,SRM_USCOREPARTIAL_USCORESUCCESS);
			MonitorHelper::addSuccessStatusCodeMapping(SRM_STATUS_OF_COPY_REQUEST_MONITOR_NAME,SRM_USCOREREQUEST_USCOREQUEUED);
			MonitorHelper::addSuccessStatusCodeMapping(SRM_STATUS_OF_COPY_REQUEST_MONITOR_NAME,SRM_USCOREREQUEST_USCOREINPROGRESS);
			MonitorHelper::addSuccessStatusCodeMapping(SRM_STATUS_OF_COPY_REQUEST_MONITOR_NAME,SRM_USCOREREQUEST_USCORESUSPENDED);
			return new InstrumentedMonitor(SRM_STATUS_OF_COPY_REQUEST_MONITOR_NAME, "Status cp", Monitor::Synchronous);
		}

		static InstrumentedMonitor* buildStatusOfGetRequest()
		{
			MonitorHelper::addSuccessStatusCodeMapping(SRM_STATUS_OF_GET_REQUEST_MONITOR_NAME,SRM_USCORESUCCESS);
			MonitorHelper::addSuccessStatusCodeMapping(SRM_STATUS_OF_GET_REQUEST_MONITOR_NAME,SRM_USCOREPARTIAL_USCORESUCCESS);
			MonitorHelper::addSuccessStatusCodeMapping(SRM_STATUS_OF_GET_REQUEST_MONITOR_NAME,SRM_USCOREREQUEST_USCOREQUEUED);
			MonitorHelper::addSuccessStatusCodeMapping(SRM_STATUS_OF_GET_REQUEST_MONITOR_NAME,SRM_USCOREREQUEST_USCOREINPROGRESS);
			MonitorHelper::addSuccessStatusCodeMapping(SRM_STATUS_OF_GET_REQUEST_MONITOR_NAME,SRM_USCOREREQUEST_USCORESUSPENDED);
			return new InstrumentedMonitor(SRM_STATUS_OF_GET_REQUEST_MONITOR_NAME, "Status PTG",  Monitor::Synchronous);
		}

		static InstrumentedMonitor* buildStatusOfLsRequest()
		{
			MonitorHelper::addSuccessStatusCodeMapping(SRM_STATUS_OF_LS_REQUEST_MONITOR_NAME,SRM_USCORESUCCESS);
			MonitorHelper::addSuccessStatusCodeMapping(SRM_STATUS_OF_LS_REQUEST_MONITOR_NAME,SRM_USCOREPARTIAL_USCORESUCCESS);
			MonitorHelper::addSuccessStatusCodeMapping(SRM_STATUS_OF_LS_REQUEST_MONITOR_NAME,SRM_USCOREREQUEST_USCOREQUEUED);
			MonitorHelper::addSuccessStatusCodeMapping(SRM_STATUS_OF_LS_REQUEST_MONITOR_NAME,SRM_USCOREREQUEST_USCOREINPROGRESS);
			return new InstrumentedMonitor(SRM_STATUS_OF_LS_REQUEST_MONITOR_NAME, "Status ls", Monitor::Synchronous);
		}

		static InstrumentedMonitor* buildStatusOfPutRequest()
		{
			MonitorHelper::addSuccessStatusCodeMapping(SRM_STATUS_OF_PUT_REQUEST_MONITOR_NAME,SRM_USCORESUCCESS);
			MonitorHelper::addSuccessStatusCodeMapping(SRM_STATUS_OF_PUT_REQUEST_MONITOR_NAME,SRM_USCOREPARTIAL_USCORESUCCESS);
			MonitorHelper::addSuccessStatusCodeMapping(SRM_STATUS_OF_PUT_REQUEST_MONITOR_NAME,SRM_USCOREREQUEST_USCOREQUEUED);
			MonitorHelper::addSuccessStatusCodeMapping(SRM_STATUS_OF_PUT_REQUEST_MONITOR_NAME,SRM_USCOREREQUEST_USCOREINPROGRESS);
			MonitorHelper::addSuccessStatusCodeMapping(SRM_STATUS_OF_PUT_REQUEST_MONITOR_NAME,SRM_USCOREREQUEST_USCORESUSPENDED);
			return new InstrumentedMonitor(SRM_STATUS_OF_PUT_REQUEST_MONITOR_NAME, "Status PTP", Monitor::Synchronous);
		}

		static InstrumentedMonitor* buildStatusOfReserveSpaceRequest()
		{
			MonitorHelper::addSuccessStatusCodeMapping(SRM_STATUS_OF_RESERVE_SPACE_REQUEST_MONITOR_NAME,SRM_USCORESUCCESS);
			MonitorHelper::addSuccessStatusCodeMapping(SRM_STATUS_OF_RESERVE_SPACE_REQUEST_MONITOR_NAME,SRM_USCOREREQUEST_USCOREQUEUED);
			MonitorHelper::addSuccessStatusCodeMapping(SRM_STATUS_OF_RESERVE_SPACE_REQUEST_MONITOR_NAME,SRM_USCOREREQUEST_USCOREINPROGRESS);
			MonitorHelper::addSuccessStatusCodeMapping(SRM_STATUS_OF_RESERVE_SPACE_REQUEST_MONITOR_NAME,SRM_USCOREREQUEST_USCORESUSPENDED);
			MonitorHelper::addSuccessStatusCodeMapping(SRM_STATUS_OF_RESERVE_SPACE_REQUEST_MONITOR_NAME,SRM_USCORELOWER_USCORESPACE_USCOREGRANTED);
			return new InstrumentedMonitor(SRM_STATUS_OF_RESERVE_SPACE_REQUEST_MONITOR_NAME, "Status reserve space", Monitor::Synchronous);
		}

		static InstrumentedMonitor* buildStatusOfUpdateSpaceRequest()
		{
			MonitorHelper::addSuccessStatusCodeMapping(SRM_STATUS_OF_UPDATE_SPACE_REQUEST_MONITOR_NAME,SRM_USCORESUCCESS);
			MonitorHelper::addSuccessStatusCodeMapping(SRM_STATUS_OF_UPDATE_SPACE_REQUEST_MONITOR_NAME,SRM_USCOREREQUEST_USCOREQUEUED);
			MonitorHelper::addSuccessStatusCodeMapping(SRM_STATUS_OF_UPDATE_SPACE_REQUEST_MONITOR_NAME,SRM_USCOREREQUEST_USCOREINPROGRESS);
			MonitorHelper::addSuccessStatusCodeMapping(SRM_STATUS_OF_UPDATE_SPACE_REQUEST_MONITOR_NAME,SRM_USCOREREQUEST_USCORESUSPENDED);
			MonitorHelper::addSuccessStatusCodeMapping(SRM_STATUS_OF_UPDATE_SPACE_REQUEST_MONITOR_NAME,SRM_USCORELOWER_USCORESPACE_USCOREGRANTED);
			return new InstrumentedMonitor(SRM_STATUS_OF_UPDATE_SPACE_REQUEST_MONITOR_NAME, "Status update space", Monitor::Synchronous);
		}

		static InstrumentedMonitor* buildSuspendRequest()
		{
			MonitorHelper::addSuccessStatusCodeMapping(SRM_SUSPEND_REQUEST_MONITOR_NAME,SRM_USCORESUCCESS);
			return new InstrumentedMonitor(SRM_SUSPEND_REQUEST_MONITOR_NAME, "Suspend request", Monitor::Synchronous);
		}

		static InstrumentedMonitor* buildUpdateSpace()
		{
			MonitorHelper::addSuccessStatusCodeMapping(SRM_UPDATE_SPACE_MONITOR_NAME,SRM_USCORESUCCESS);
			MonitorHelper::addSuccessStatusCodeMapping(SRM_UPDATE_SPACE_MONITOR_NAME,SRM_USCOREREQUEST_USCOREQUEUED);
			MonitorHelper::addSuccessStatusCodeMapping(SRM_UPDATE_SPACE_MONITOR_NAME,SRM_USCORELOWER_USCORESPACE_USCOREGRANTED);
			return new InstrumentedMonitor(SRM_UPDATE_SPACE_MONITOR_NAME, "Update space" , Monitor::Synchronous);
		}
	};
}
#endif /* INSTRUMENTED_MONITOR_BUILDER_HPP_ */
