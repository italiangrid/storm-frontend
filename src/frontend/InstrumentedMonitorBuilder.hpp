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
			InstrumentedMonitor* monitor = new InstrumentedMonitor(SRM_ABORT_FILES_MONITOR_NAME, "Abort files", Monitor::Synchronous);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCORESUCCESS);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCOREPARTIAL_USCORESUCCESS);
			return monitor;
		}

		static InstrumentedMonitor* buildAbortRequest()
		{
			InstrumentedMonitor* monitor = InstrumentedMonitor(SRM_ABORT_REQUEST_MONITOR_NAME, "Abort request", Monitor::Synchronous);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCORESUCCESS);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCOREPARTIAL_USCORESUCCESS);
			return monitor;
		}

		static InstrumentedMonitor* buildBringOnline()
		{
			InstrumentedMonitor* monitor = InstrumentedMonitor(SRM_BRING_ONLINE_MONITOR_NAME, "BOL" , Monitor::Asynchronous);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCORESUCCESS);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCOREPARTIAL_USCORESUCCESS);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCOREREQUEST_USCOREQUEUED);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCOREREQUEST_USCOREINPROGRESS);
			return monitor;
		}

		static InstrumentedMonitor* buildChangeSpaceForFiles()
		{
			InstrumentedMonitor* monitor = InstrumentedMonitor(SRM_CHANGE_SPACE_FOR_FILES_MONITOR_NAME, "Change space for files", Monitor::Synchronous);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCORESUCCESS);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCOREPARTIAL_USCORESUCCESS);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCOREREQUEST_USCOREQUEUED);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCOREREQUEST_USCOREINPROGRESS);
			return monitor;
		}

		static InstrumentedMonitor* buildCheckPermission()
		{
			InstrumentedMonitor* monitor = InstrumentedMonitor(SRM_CHECK_PERMISSION_MONITOR_NAME,"Check permission" , Monitor::Synchronous);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCORESUCCESS);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCOREPARTIAL_USCORESUCCESS);
			return monitor;
		}

		static InstrumentedMonitor* buildCopy()
		{
			InstrumentedMonitor* monitor = InstrumentedMonitor(SRM_COPY_MONITOR_NAME, "Cp" ,Monitor::Asynchronous);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCORESUCCESS);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCOREPARTIAL_USCORESUCCESS);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCOREREQUEST_USCOREQUEUED);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCOREREQUEST_USCOREINPROGRESS);
			return monitor;
		}

		static InstrumentedMonitor* buildExtendFileLifeTimeInSpace()
		{
			InstrumentedMonitor* monitor = InstrumentedMonitor(SRM_EXTEND_FILE_LIFE_TIME_IN_SPACE_MONITOR_NAME, "Extend file life time in space" ,Monitor::Synchronous);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCORESUCCESS);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCOREPARTIAL_USCORESUCCESS);
			return monitor;
		}

		static InstrumentedMonitor* buildExtendFileLifeTime()
		{
			InstrumentedMonitor* monitor = InstrumentedMonitor(SRM_EXTEND_FILE_LIFE_TIME_MONITOR_NAME, "Extend file life time" , Monitor::Synchronous);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCORESUCCESS);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCOREPARTIAL_USCORESUCCESS);
			return monitor;
		}

		static InstrumentedMonitor* buildGetPermission()
		{
			InstrumentedMonitor* monitor = InstrumentedMonitor(SRM_GET_PERMISSION_MONITOR_NAME, "Get permission", Monitor::Synchronous);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCORESUCCESS);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCOREPARTIAL_USCORESUCCESS);
			return monitor;
		}

		static InstrumentedMonitor* buildGetRequestSummary()
		{
			InstrumentedMonitor* monitor = InstrumentedMonitor(SRM_GET_REQUEST_SUMMARY_MONITOR_NAME, "Get request summary", Monitor::Synchronous);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCORESUCCESS);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCOREPARTIAL_USCORESUCCESS);
			return monitor;
		}

		static InstrumentedMonitor* buildGetRequestTokens()
		{
			InstrumentedMonitor* monitor = InstrumentedMonitor(SRM_GET_REQUEST_TOKENS_MONITOR_NAME, "Get request tokens", Monitor::Synchronous);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCORESUCCESS);
			return monitor;
		}

		static InstrumentedMonitor* buildGetSpaceMetaData()
		{
			InstrumentedMonitor* monitor = InstrumentedMonitor(SRM_GET_SPACE_META_DATA_MONITOR_NAME, "Get space meta data", Monitor::Synchronous);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCORESUCCESS);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCOREPARTIAL_USCORESUCCESS);
			return monitor;
		}

		static InstrumentedMonitor* buildGetSpaceTokens()
		{
			InstrumentedMonitor* monitor = InstrumentedMonitor(SRM_GET_SPACE_TOKENS_MONITOR_NAME, "Get space tokens", Monitor::Synchronous);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCORESUCCESS);
			return monitor;
		}

		static InstrumentedMonitor* buildGetTransferProtocols()
		{
			InstrumentedMonitor* monitor = InstrumentedMonitor(SRM_GET_TRANSFER_PROTOCOLS_MONITOR_NAME, "Get transfer protocols", Monitor::Synchronous);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCORESUCCESS);
			return monitor;
		}

		static InstrumentedMonitor* buildLs()
		{
			InstrumentedMonitor* monitor = InstrumentedMonitor(SRM_LS_MONITOR_NAME, "Ls", Monitor::Synchronous);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCORESUCCESS);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCOREPARTIAL_USCORESUCCESS);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCOREREQUEST_USCOREQUEUED);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCOREREQUEST_USCOREINPROGRESS);
			return monitor;
		}

		static InstrumentedMonitor* buildMkdir()
		{
			InstrumentedMonitor* monitor = InstrumentedMonitor(SRM_MKDIR_MONITOR_NAME, "Mkdir", Monitor::Synchronous);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCORESUCCESS);
			return monitor;
		}

		static InstrumentedMonitor* buildMv()
		{
			InstrumentedMonitor* monitor = InstrumentedMonitor(SRM_MV_MONITOR_NAME, "Mv", Monitor::Synchronous);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCORESUCCESS);
			return monitor;
		}

		static InstrumentedMonitor* buildPing()
		{
			InstrumentedMonitor* monitor = InstrumentedMonitor(SRM_PING_MONITOR_NAME, "Ping", Monitor::Synchronous);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCORESUCCESS);
			return monitor;
		}

		static InstrumentedMonitor* buildPrepareToGet()
		{
			InstrumentedMonitor* monitor = InstrumentedMonitor(SRM_PREPARE_TO_GET_MONITOR_NAME, "PTG", Monitor::Asynchronous);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCORESUCCESS);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCOREPARTIAL_USCORESUCCESS);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCOREREQUEST_USCOREQUEUED);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCOREREQUEST_USCOREINPROGRESS);
			return monitor;
		}

		static InstrumentedMonitor* buildPrepareToPut()
		{
			InstrumentedMonitor* monitor = InstrumentedMonitor(SRM_PREPARE_TO_PUT_MONITOR_NAME, "PTP", Monitor::Asynchronous);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCORESUCCESS);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCOREPARTIAL_USCORESUCCESS);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCOREREQUEST_USCOREQUEUED);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCOREREQUEST_USCOREINPROGRESS);
			return monitor;
		}

		static InstrumentedMonitor* buildPurgeFromSpace()
		{
			InstrumentedMonitor* monitor = InstrumentedMonitor(SRM_PURGE_FROM_SPACE_MONITOR_NAME, "Purge from Space", Monitor::Synchronous);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCORESUCCESS);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCOREPARTIAL_USCORESUCCESS);
			return monitor;
		}

		static InstrumentedMonitor* buildPutDone()
		{
			InstrumentedMonitor* monitor = InstrumentedMonitor(SRM_PUT_DONE_MONITOR_NAME, "Put done", Monitor::Synchronous);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCORESUCCESS);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCOREPARTIAL_USCORESUCCESS);
			return monitor;
		}

		static InstrumentedMonitor* buildReleaseFiles()
		{
			InstrumentedMonitor* monitor = InstrumentedMonitor(SRM_RELEASE_FILES_MONITOR_NAME, "Release files", Monitor::Synchronous);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCORESUCCESS);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCOREPARTIAL_USCORESUCCESS);
			return monitor;
		}

		static InstrumentedMonitor* buildReleaseSpace()
		{
			InstrumentedMonitor* monitor = InstrumentedMonitor(SRM_RELEASE_SPACE_MONITOR_NAME, "Release space", Monitor::Synchronous);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCORESUCCESS);
			return monitor;
		}

		static InstrumentedMonitor* buildReserveSpace()
		{
			InstrumentedMonitor* monitor = InstrumentedMonitor(SRM_RESERVE_SPACE_MONITOR_NAME, "Reserve space", Monitor::Synchronous);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCORESUCCESS);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCOREREQUEST_USCOREQUEUED);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCOREREQUEST_USCOREINPROGRESS);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCORELOWER_USCORESPACE_USCOREGRANTED);
			return monitor;
		}

		static InstrumentedMonitor* buildResumeRequest()
		{
			InstrumentedMonitor* monitor = InstrumentedMonitor(SRM_RESUME_REQUEST_MONITOR_NAME, "Resume request", Monitor::Synchronous);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCORESUCCESS);
			return monitor;
		}

		static InstrumentedMonitor* buildRmdir()
		{
			InstrumentedMonitor* monitor = InstrumentedMonitor(SRM_RMDIR_MONITOR_NAME, "Rmdir", Monitor::Synchronous);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCORESUCCESS);
			return monitor;
		}

		static InstrumentedMonitor* buildRm()
		{
			InstrumentedMonitor* monitor = InstrumentedMonitor(SRM_RM_MONITOR_NAME, "Rm", Monitor::Synchronous);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCORESUCCESS);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCOREPARTIAL_USCORESUCCESS);
			return monitor;
		}

		static InstrumentedMonitor* buildSetPermission()
		{
			InstrumentedMonitor* monitor = InstrumentedMonitor(SRM_SET_PERMISSION_MONITOR_NAME, "Set permission", Monitor::Synchronous);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCORESUCCESS);
			return monitor;
		}

		static InstrumentedMonitor* buildStatusOfBringOnlineRequest()
		{
			InstrumentedMonitor* monitor = InstrumentedMonitor(SRM_STATUS_OF_BRING_ONLINE_REQUEST_MONITOR_NAME, "Status BOL", Monitor::Synchronous);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCORESUCCESS);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCOREPARTIAL_USCORESUCCESS);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCOREREQUEST_USCOREQUEUED);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCOREREQUEST_USCOREINPROGRESS);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCOREREQUEST_USCORESUSPENDED);
			return monitor;
		}

		static InstrumentedMonitor* buildStatusOfChangeSpaceForFilesRequest()
		{
			InstrumentedMonitor* monitor = InstrumentedMonitor(SRM_STATUS_OF_CHANGE_SPACE_FOR_FILES_REQUEST_MONITOR_NAME, "Status change space for files", Monitor::Synchronous);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCORESUCCESS);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCOREPARTIAL_USCORESUCCESS);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCOREREQUEST_USCOREQUEUED);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCOREREQUEST_USCOREINPROGRESS);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCOREREQUEST_USCORESUSPENDED);
			return monitor;
		}

		static InstrumentedMonitor* buildStatusOfCopyRequest()
		{
			InstrumentedMonitor* monitor = InstrumentedMonitor(SRM_STATUS_OF_COPY_REQUEST_MONITOR_NAME, "Status cp", Monitor::Synchronous);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCORESUCCESS);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCOREPARTIAL_USCORESUCCESS);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCOREREQUEST_USCOREQUEUED);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCOREREQUEST_USCOREINPROGRESS);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCOREREQUEST_USCORESUSPENDED);
			return monitor;
		}

		static InstrumentedMonitor* buildStatusOfGetRequest()
		{
			InstrumentedMonitor* monitor = InstrumentedMonitor(SRM_STATUS_OF_GET_REQUEST_MONITOR_NAME, "Status PTG",  Monitor::Synchronous);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCORESUCCESS);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCOREPARTIAL_USCORESUCCESS);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCOREREQUEST_USCOREQUEUED);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCOREREQUEST_USCOREINPROGRESS);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCOREREQUEST_USCORESUSPENDED);
			return monitor;
		}

		static InstrumentedMonitor* buildStatusOfLsRequest()
		{
			InstrumentedMonitor* monitor = InstrumentedMonitor(SRM_STATUS_OF_LS_REQUEST_MONITOR_NAME, "Status ls", Monitor::Synchronous);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCORESUCCESS);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCOREPARTIAL_USCORESUCCESS);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCOREREQUEST_USCOREQUEUED);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCOREREQUEST_USCOREINPROGRESS);
			return monitor;
		}

		static InstrumentedMonitor* buildStatusOfPutRequest()
		{
			InstrumentedMonitor* monitor = InstrumentedMonitor(SRM_STATUS_OF_PUT_REQUEST_MONITOR_NAME, "Status PTP", Monitor::Synchronous);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCORESUCCESS);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCOREPARTIAL_USCORESUCCESS);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCOREREQUEST_USCOREQUEUED);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCOREREQUEST_USCOREINPROGRESS);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCOREREQUEST_USCORESUSPENDED);
			return monitor;
		}

		static InstrumentedMonitor* buildStatusOfReserveSpaceRequest()
		{
			InstrumentedMonitor* monitor = InstrumentedMonitor(SRM_STATUS_OF_RESERVE_SPACE_REQUEST_MONITOR_NAME, "Status reserve space", Monitor::Synchronous);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCORESUCCESS);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCOREREQUEST_USCOREQUEUED);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCOREREQUEST_USCOREINPROGRESS);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCOREREQUEST_USCORESUSPENDED);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCORELOWER_USCORESPACE_USCOREGRANTED);
			return monitor;
		}

		static InstrumentedMonitor* buildStatusOfUpdateSpaceRequest()
		{
			InstrumentedMonitor* monitor = InstrumentedMonitor(SRM_STATUS_OF_UPDATE_SPACE_REQUEST_MONITOR_NAME, "Status update space", Monitor::Synchronous);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCORESUCCESS);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCOREREQUEST_USCOREQUEUED);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCOREREQUEST_USCOREINPROGRESS);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCOREREQUEST_USCORESUSPENDED);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCORELOWER_USCORESPACE_USCOREGRANTED);
			return monitor;
		}

		static InstrumentedMonitor* buildSuspendRequest()
		{
			InstrumentedMonitor* monitor = InstrumentedMonitor(SRM_SUSPEND_REQUEST_MONITOR_NAME, "Suspend request", Monitor::Synchronous);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCORESUCCESS);
			return monitor;
		}

		static InstrumentedMonitor* buildUpdateSpace()
		{
			InstrumentedMonitor* monitor = InstrumentedMonitor(SRM_UPDATE_SPACE_MONITOR_NAME, "Update space" , Monitor::Synchronous);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCORESUCCESS);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCOREREQUEST_USCOREQUEUED);
			MonitorHelper::addSuccessStatusCodeMapping(monitor,SRM_USCORELOWER_USCORESPACE_USCOREGRANTED);
			return monitor;
		}
	};
}
#endif /* INSTRUMENTED_MONITOR_BUILDER_HPP_ */
