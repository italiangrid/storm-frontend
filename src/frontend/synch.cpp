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

#include "Monitoring.hpp"
#include "InstrumentedMonitor.hpp"
#include "MonitorNotEnabledException.hpp"
#include "srmv2H.h"

/* commented out. not available in first Argus integration prototype
#include "Credentials.hpp"
#include "Authorization.hpp"
*/
#include "boost/date_time/posix_time/posix_time.hpp"

#include "srmlogit.h"
#include "get_socket_info.hpp"
#include "Authorization.hpp"
#include <stdsoap2.h>
#include "soap_util.hpp"
#include "synch.hpp"

//Directory Functions

int ns1__srmMkdir(struct soap* soap, struct ns1__srmMkdirRequest *req,
		struct ns1__srmMkdirResponse_ *rep) {

	char *func = "ns1__srmMkdir()";
	srmLogRequest("Mkdir",get_ip(soap).c_str(),storm::Credentials(soap).getDN().c_str());
	boost::posix_time::ptime start_time = boost::posix_time::microsec_clock::local_time();

	if(storm::Authorization::checkBlacklist(soap))
	{
		srmlogit(STORM_LOG_INFO, func, "The user is blacklisted\n");
		rep->srmMkdirResponse = storm::soap_calloc<struct ns1__srmMkdirResponse>(soap);
		if(rep->srmMkdirResponse != NULL)
		{
			rep->srmMkdirResponse->returnStatus = storm::soap_calloc<struct ns1__TReturnStatus>(soap);
			if (rep->srmMkdirResponse->returnStatus != NULL)
			{
				rep->srmMkdirResponse->returnStatus->explanation =
						"User not authorized";
				rep->srmMkdirResponse->returnStatus->statusCode =
						SRM_USCOREAUTHORIZATION_USCOREFAILURE;
				boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
				boost::posix_time::time_duration et = (end_time - start_time);
				try
				{
					storm::Monitoring::getInstance()->getMonitor(
							storm::SRM_MKDIR_MONITOR_NAME)->registerFailure(et.total_milliseconds());
				}catch(storm::MonitorNotEnabledException *exc)
				{
					srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
				}
				return (SOAP_OK);
			} else {
				srmlogit(STORM_LOG_ERROR, func, "Error in response allocation\n");
				boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
				boost::posix_time::time_duration et = (end_time - start_time);
				try
				{
					storm::Monitoring::getInstance()->getMonitor(storm::SRM_MKDIR_MONITOR_NAME)->registerError(et.total_milliseconds());
				}catch(storm::MonitorNotEnabledException *exc)
				{
					srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
				}
				return SOAP_EOM;
			}
		}
		else
		{
			srmlogit(STORM_LOG_ERROR, func, "Error in response allocation\n");
			boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
			boost::posix_time::time_duration et = (end_time - start_time);
			try
			{
				storm::Monitoring::getInstance()->getMonitor(storm::SRM_MKDIR_MONITOR_NAME)->registerError(et.total_milliseconds());
			}catch(storm::MonitorNotEnabledException *exc)
			{
				srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
			}
			return SOAP_EOM;
		}
	}
	else
	{
		srmlogit(STORM_LOG_DEBUG, func, "The user is not blacklisted\n");
	}

	int ret_val = ns1__srmMkdir_impl(soap, req, rep);

	boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
	boost::posix_time::time_duration et = (end_time - start_time);

	if (ret_val != SOAP_OK)
	{
		boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
		boost::posix_time::time_duration et = (end_time - start_time);
		try
		{
			storm::Monitoring::getInstance()->getMonitor(storm::SRM_MKDIR_MONITOR_NAME)->registerFailure(et.total_milliseconds());
		}catch(storm::MonitorNotEnabledException *exc)
		{
			srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
		}
	}
	else
	{
		try
		{
			((storm::InstrumentedMonitor*) storm::Monitoring::getInstance()->getMonitor(
					storm::SRM_MKDIR_MONITOR_NAME))->registerCompleted(
					et.total_milliseconds(),
					rep->srmMkdirResponse->returnStatus->statusCode);
		}catch(storm::MonitorNotEnabledException *exc)
		{
			srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
		}
	}

	return ret_val;
}

int ns1__srmRmdir(struct soap* soap, struct ns1__srmRmdirRequest *req,
		struct ns1__srmRmdirResponse_ *rep) {

	char *func = "ns1__srmRmdir()";
	srmLogRequest("Rmdir",get_ip(soap).c_str(),storm::Credentials(soap).getDN().c_str());
	boost::posix_time::ptime start_time = boost::posix_time::microsec_clock::local_time();

	if(storm::Authorization::checkBlacklist(soap))
	{
		srmlogit(STORM_LOG_INFO, func, "The user is blacklisted\n");
		rep->srmRmdirResponse = storm::soap_calloc<struct ns1__srmRmdirResponse>(soap);
		if(rep->srmRmdirResponse != NULL)
		{
			rep->srmRmdirResponse->returnStatus = storm::soap_calloc<struct ns1__TReturnStatus>(soap);
			if (rep->srmRmdirResponse->returnStatus != NULL)
			{
				rep->srmRmdirResponse->returnStatus->explanation =
						"User not authorized";
				rep->srmRmdirResponse->returnStatus->statusCode =
						SRM_USCOREAUTHORIZATION_USCOREFAILURE;
				boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
				boost::posix_time::time_duration et = (end_time - start_time);
				try
				{
					storm::Monitoring::getInstance()->getMonitor(
							storm::SRM_RMDIR_MONITOR_NAME)->registerFailure(et.total_milliseconds());
				}catch(storm::MonitorNotEnabledException *exc)
				{
					srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
				}
				return (SOAP_OK);
			} else {
				srmlogit(STORM_LOG_ERROR, func, "Error in response allocation\n");
				boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
				boost::posix_time::time_duration et = (end_time - start_time);
				try
				{
					storm::Monitoring::getInstance()->getMonitor(storm::SRM_RMDIR_MONITOR_NAME)->registerError(et.total_milliseconds());
				}catch(storm::MonitorNotEnabledException *exc)
				{
					srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
				}
				return SOAP_EOM;
			}
		}
		else
		{
			srmlogit(STORM_LOG_ERROR, func, "Error in response allocation\n");
			boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
			boost::posix_time::time_duration et = (end_time - start_time);
			try
			{
				storm::Monitoring::getInstance()->getMonitor(storm::SRM_RMDIR_MONITOR_NAME)->registerError(et.total_milliseconds());
			}catch(storm::MonitorNotEnabledException *exc)
			{
				srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
			}
			return SOAP_EOM;
		}
	}
	else
	{
		srmlogit(STORM_LOG_DEBUG, func, "The user is not blacklisted\n");
	}

	int ret_val = ns1__srmRmdir_impl(soap, req, rep);

	boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();

	boost::posix_time::time_duration et = (end_time - start_time);

	if (ret_val != SOAP_OK)
	{
		try
		{
			storm::Monitoring::getInstance()->getMonitor(storm::SRM_RMDIR_MONITOR_NAME)->registerFailure(et.total_milliseconds());
		}catch(storm::MonitorNotEnabledException *exc)
		{
			srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
		}
	}
	else
	{
		try
		{
			((storm::InstrumentedMonitor*) storm::Monitoring::getInstance()->getMonitor(
					storm::SRM_RMDIR_MONITOR_NAME))->registerCompleted(
					et.total_milliseconds(),
					rep->srmRmdirResponse->returnStatus->statusCode);
		}catch(storm::MonitorNotEnabledException *exc)
		{
			srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
		}
	}

	return ret_val;
}

int ns1__srmRm(struct soap* soap, struct ns1__srmRmRequest *req,
		struct ns1__srmRmResponse_ *rep) {

	char *func = "ns1__srmRm()";
	srmLogRequest("Rm",get_ip(soap).c_str(),storm::Credentials(soap).getDN().c_str());
	boost::posix_time::ptime start_time = boost::posix_time::microsec_clock::local_time();

	if(storm::Authorization::checkBlacklist(soap))
	{
		srmlogit(STORM_LOG_INFO, func, "The user is blacklisted\n");
		rep->srmRmResponse = storm::soap_calloc<struct ns1__srmRmResponse>(soap);
		if(rep->srmRmResponse != NULL)
		{
			rep->srmRmResponse->returnStatus = storm::soap_calloc<struct ns1__TReturnStatus>(soap);
			if (rep->srmRmResponse->returnStatus != NULL)
			{
				rep->srmRmResponse->returnStatus->explanation =
						"User not authorized";
				rep->srmRmResponse->returnStatus->statusCode =
						SRM_USCOREAUTHORIZATION_USCOREFAILURE;
				boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
				boost::posix_time::time_duration et = (end_time - start_time);
				try
				{
					storm::Monitoring::getInstance()->getMonitor(
							storm::SRM_RM_MONITOR_NAME)->registerFailure(et.total_milliseconds());
				}catch(storm::MonitorNotEnabledException *exc)
				{
					srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
				}
				return (SOAP_OK);
			} else {
				srmlogit(STORM_LOG_ERROR, func, "Error in response allocation\n");
				boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
				boost::posix_time::time_duration et = (end_time - start_time);
				try
				{
					storm::Monitoring::getInstance()->getMonitor(storm::SRM_RM_MONITOR_NAME)->registerError(et.total_milliseconds());
				}catch(storm::MonitorNotEnabledException *exc)
				{
					srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
				}
				return SOAP_EOM;
			}
		}
		else
		{
			srmlogit(STORM_LOG_ERROR, func, "Error in response allocation\n");
			boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
			boost::posix_time::time_duration et = (end_time - start_time);
			try
			{
				storm::Monitoring::getInstance()->getMonitor(storm::SRM_RM_MONITOR_NAME)->registerError(et.total_milliseconds());
			}catch(storm::MonitorNotEnabledException *exc)
			{
				srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
			}
			return SOAP_EOM;
		}
	}
	else
	{
		srmlogit(STORM_LOG_DEBUG, func, "The user is not blacklisted\n");
	}

	int ret_val = ns1__srmRm_impl(soap, req, rep);

	boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();

	boost::posix_time::time_duration et = (end_time - start_time);

	if (ret_val != SOAP_OK)
	{
		try
		{
			storm::Monitoring::getInstance()->getMonitor(storm::SRM_RM_MONITOR_NAME)->registerFailure(et.total_milliseconds());
		}catch(storm::MonitorNotEnabledException *exc)
		{
			srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
		}
	}
	else
	{
		try
		{
			((storm::InstrumentedMonitor*) storm::Monitoring::getInstance()->getMonitor(
					storm::SRM_RM_MONITOR_NAME))->registerCompleted(
					et.total_milliseconds(),
					rep->srmRmResponse->returnStatus->statusCode);
		}catch(storm::MonitorNotEnabledException *exc)
		{
			srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
		}
	}

	return ret_val;
}

int ns1__srmLs(struct soap* soap, struct ns1__srmLsRequest *req,
		struct ns1__srmLsResponse_ *rep) {

	char *func = "ns1__srmLs()";
	srmLogRequest("Ls",get_ip(soap).c_str(),storm::Credentials(soap).getDN().c_str());
	boost::posix_time::ptime start_time = boost::posix_time::microsec_clock::local_time();

	if(storm::Authorization::checkBlacklist(soap))
	{
		srmlogit(STORM_LOG_INFO, func, "The user is blacklisted\n");
		rep->srmLsResponse = storm::soap_calloc<struct ns1__srmLsResponse>(soap);
		if(rep->srmLsResponse != NULL)
		{
			rep->srmLsResponse->returnStatus = storm::soap_calloc<struct ns1__TReturnStatus>(soap);
			if (rep->srmLsResponse->returnStatus != NULL)
			{
				rep->srmLsResponse->returnStatus->explanation = "User not authorized";
				rep->srmLsResponse->returnStatus->statusCode = SRM_USCOREAUTHORIZATION_USCOREFAILURE;
				boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
				boost::posix_time::time_duration et = (end_time - start_time);
				try
				{
					storm::Monitoring::getInstance()->getMonitor(
							storm::SRM_LS_MONITOR_NAME)->registerFailure(et.total_milliseconds());
				}catch(storm::MonitorNotEnabledException *exc)
				{
					srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
				}
				return (SOAP_OK);
			}
			else
			{
				srmlogit(STORM_LOG_ERROR, func, "Error in response allocation\n");
				boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
				boost::posix_time::time_duration et = (end_time - start_time);
				try
				{
					storm::Monitoring::getInstance()->getMonitor(storm::SRM_LS_MONITOR_NAME)->registerError(et.total_milliseconds());
				}catch(storm::MonitorNotEnabledException *exc)
				{
					srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
				}
				return SOAP_EOM;
			}
		}
		else
		{
			srmlogit(STORM_LOG_ERROR, func, "Error in response allocation\n");
			boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
			boost::posix_time::time_duration et = (end_time - start_time);
			try
			{
				storm::Monitoring::getInstance()->getMonitor(storm::SRM_LS_MONITOR_NAME)->registerError(et.total_milliseconds());
			}catch(storm::MonitorNotEnabledException *exc)
			{
				srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
			}
			return SOAP_EOM;
		}
	}
	else
	{
		srmlogit(STORM_LOG_DEBUG, func, "The user is not blacklisted\n");
	}

	int ret_val = ns1__srmLs_impl(soap, req, rep);

	boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
	boost::posix_time::time_duration et = (end_time - start_time);


	if (ret_val != SOAP_OK)
	{
		try
		{
			storm::Monitoring::getInstance()->getMonitor(storm::SRM_LS_MONITOR_NAME)->registerFailure(et.total_milliseconds());
		}catch(storm::MonitorNotEnabledException *exc)
		{
			srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
		}
	}
	else
	{
		try
		{
			((storm::InstrumentedMonitor*) storm::Monitoring::getInstance()->getMonitor(
					storm::SRM_LS_MONITOR_NAME))->registerCompleted(
					et.total_milliseconds(),
					rep->srmLsResponse->returnStatus->statusCode);
		}catch(storm::MonitorNotEnabledException *exc)
		{
			srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
		}
	}
	return ret_val;
}

int ns1__srmStatusOfLsRequest(struct soap* soap, struct ns1__srmStatusOfLsRequestRequest *req,
		struct ns1__srmStatusOfLsRequestResponse_ *rep) {

	char *func = "ns1__srmStatusOfLsRequest()";
	srmLogRequest("Ls status",get_ip(soap).c_str(),storm::Credentials(soap).getDN().c_str());
	boost::posix_time::ptime start_time = boost::posix_time::microsec_clock::local_time();

	if(storm::Authorization::checkBlacklist(soap))
	{
		srmlogit(STORM_LOG_INFO, func, "The user is blacklisted\n");
		rep->srmStatusOfLsRequestResponse = storm::soap_calloc<struct ns1__srmStatusOfLsRequestResponse>(soap);
		if(rep->srmStatusOfLsRequestResponse != NULL)
		{
			rep->srmStatusOfLsRequestResponse->returnStatus = storm::soap_calloc<struct ns1__TReturnStatus>(soap);
			if (rep->srmStatusOfLsRequestResponse->returnStatus != NULL)
			{
				rep->srmStatusOfLsRequestResponse->returnStatus->explanation =
						"User not authorized";
				rep->srmStatusOfLsRequestResponse->returnStatus->statusCode =
						SRM_USCOREAUTHORIZATION_USCOREFAILURE;
				boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
				boost::posix_time::time_duration et = (end_time - start_time);
				try
				{
					storm::Monitoring::getInstance()->getMonitor(
							storm::SRM_STATUS_OF_LS_REQUEST_MONITOR_NAME)->registerFailure(et.total_milliseconds());
				}catch(storm::MonitorNotEnabledException *exc)
				{
					srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
				}
				return (SOAP_OK);
			} else {
				srmlogit(STORM_LOG_ERROR, func, "Error in response allocation\n");
				boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
				boost::posix_time::time_duration et = (end_time - start_time);
				try
				{
					storm::Monitoring::getInstance()->getMonitor(storm::SRM_STATUS_OF_LS_REQUEST_MONITOR_NAME)->registerError(et.total_milliseconds());
				}catch(storm::MonitorNotEnabledException *exc)
				{
					srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
				}
				return SOAP_EOM;
			}
		}
		else
		{
			srmlogit(STORM_LOG_ERROR, func, "Error in response allocation\n");
			boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
			boost::posix_time::time_duration et = (end_time - start_time);
			try
			{
				storm::Monitoring::getInstance()->getMonitor(storm::SRM_STATUS_OF_LS_REQUEST_MONITOR_NAME)->registerError(et.total_milliseconds());
			}catch(storm::MonitorNotEnabledException *exc)
			{
				srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
			}
			return SOAP_EOM;
		}
	}
	else
	{
		srmlogit(STORM_LOG_DEBUG, func, "The user is not blacklisted\n");
	}

	int ret_val = ns1__srmStatusOfLsRequest_impl(soap, req, rep);

	boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();

	boost::posix_time::time_duration et = (end_time - start_time);

	if (ret_val != SOAP_OK)
	{
		try
		{
			storm::Monitoring::getInstance()->getMonitor(storm::SRM_STATUS_OF_LS_REQUEST_MONITOR_NAME)->registerFailure(et.total_milliseconds());
		}catch(storm::MonitorNotEnabledException *exc)
		{
			srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
		}
	}
	else
	{
		try
		{
			((storm::InstrumentedMonitor*) storm::Monitoring::getInstance()->getMonitor(
					storm::SRM_STATUS_OF_LS_REQUEST_MONITOR_NAME))->registerCompleted(
					et.total_milliseconds(),
					rep->srmStatusOfLsRequestResponse->returnStatus->statusCode);
		}catch(storm::MonitorNotEnabledException *exc)
		{
			srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
		}
	}

	return ret_val;
}

int ns1__srmMv(struct soap* soap, struct ns1__srmMvRequest *req,
		struct ns1__srmMvResponse_ *rep) {

	char *func = "ns1__srmMv()";
	srmLogRequest("Mv",get_ip(soap).c_str(),storm::Credentials(soap).getDN().c_str());
	boost::posix_time::ptime start_time = boost::posix_time::microsec_clock::local_time();

	if(storm::Authorization::checkBlacklist(soap))
	{
		srmlogit(STORM_LOG_INFO, func, "The user is blacklisted\n");
		rep->srmMvResponse = storm::soap_calloc<struct ns1__srmMvResponse>(soap);
		if(rep->srmMvResponse != NULL)
		{
			rep->srmMvResponse->returnStatus = storm::soap_calloc<struct ns1__TReturnStatus>(soap);
			if (rep->srmMvResponse->returnStatus != NULL)
			{
				rep->srmMvResponse->returnStatus->explanation =
						"User not authorized";
				rep->srmMvResponse->returnStatus->statusCode =
						SRM_USCOREAUTHORIZATION_USCOREFAILURE;
				boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
				boost::posix_time::time_duration et = (end_time - start_time);
				try
				{
					storm::Monitoring::getInstance()->getMonitor(
							storm::SRM_MV_MONITOR_NAME)->registerFailure(et.total_milliseconds());
				}catch(storm::MonitorNotEnabledException *exc)
				{
					srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
				}
				return (SOAP_OK);
			} else {
				srmlogit(STORM_LOG_ERROR, func, "Error in response allocation\n");
				boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
				boost::posix_time::time_duration et = (end_time - start_time);
				try
				{
					storm::Monitoring::getInstance()->getMonitor(storm::SRM_MV_MONITOR_NAME)->registerError(et.total_milliseconds());
				}catch(storm::MonitorNotEnabledException *exc)
				{
					srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
				}
				return SOAP_EOM;
			}
		}
		else
		{
			srmlogit(STORM_LOG_ERROR, func, "Error in response allocation\n");
			boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
			boost::posix_time::time_duration et = (end_time - start_time);
			try
			{
				storm::Monitoring::getInstance()->getMonitor(storm::SRM_MV_MONITOR_NAME)->registerError(et.total_milliseconds());
			}catch(storm::MonitorNotEnabledException *exc)
			{
				srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
			}
			return SOAP_EOM;
		}
	}
	else
	{
		srmlogit(STORM_LOG_DEBUG, func, "The user is not blacklisted\n");
	}

	int ret_val = ns1__srmMv_impl(soap, req, rep);

	boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();

	boost::posix_time::time_duration et = (end_time - start_time);

	if (ret_val != SOAP_OK)
	{
		try
		{
			storm::Monitoring::getInstance()->getMonitor(storm::SRM_MV_MONITOR_NAME)->registerFailure(et.total_milliseconds());
		}catch(storm::MonitorNotEnabledException *exc)
		{
			srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
		}
	}
	else
	{
		try
		{
			((storm::InstrumentedMonitor*) storm::Monitoring::getInstance()->getMonitor(
					storm::SRM_MV_MONITOR_NAME))->registerCompleted(
					et.total_milliseconds(),
					rep->srmMvResponse->returnStatus->statusCode);
		}catch(storm::MonitorNotEnabledException *exc)
		{
			srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
		}
	}

	return ret_val;
}

//Permission Functions

int ns1__srmSetPermission(struct soap* soap, struct ns1__srmSetPermissionRequest *req,
		struct ns1__srmSetPermissionResponse_ *rep) {

	char *func = "ns1__srmSetPermission()";
	srmLogRequest("Set permission",get_ip(soap).c_str(),storm::Credentials(soap).getDN().c_str());
	boost::posix_time::ptime start_time = boost::posix_time::microsec_clock::local_time();

	if(storm::Authorization::checkBlacklist(soap))
	{
		srmlogit(STORM_LOG_INFO, func, "The user is blacklisted\n");
		rep->srmSetPermissionResponse = storm::soap_calloc<struct ns1__srmSetPermissionResponse>(soap);
		if(rep->srmSetPermissionResponse != NULL)
		{
			rep->srmSetPermissionResponse->returnStatus = storm::soap_calloc<struct ns1__TReturnStatus>(soap);
			if (rep->srmSetPermissionResponse->returnStatus != NULL)
			{
				rep->srmSetPermissionResponse->returnStatus->explanation =
						"User not authorized";
				rep->srmSetPermissionResponse->returnStatus->statusCode =
						SRM_USCOREAUTHORIZATION_USCOREFAILURE;
				boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
				boost::posix_time::time_duration et = (end_time - start_time);
				try
				{
					storm::Monitoring::getInstance()->getMonitor(
							storm::SRM_SET_PERMISSION_MONITOR_NAME)->registerFailure(et.total_milliseconds());
				}catch(storm::MonitorNotEnabledException *exc)
				{
					srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
				}
				return (SOAP_OK);
			} else {
				srmlogit(STORM_LOG_ERROR, func, "Error in response allocation\n");
				boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
				boost::posix_time::time_duration et = (end_time - start_time);
				try
				{
					storm::Monitoring::getInstance()->getMonitor(storm::SRM_SET_PERMISSION_MONITOR_NAME)->registerError(et.total_milliseconds());
				}catch(storm::MonitorNotEnabledException *exc)
				{
					srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
				}
				return SOAP_EOM;
			}
		}
		else
		{
			srmlogit(STORM_LOG_ERROR, func, "Error in response allocation\n");
			boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
			boost::posix_time::time_duration et = (end_time - start_time);
			try
			{
				storm::Monitoring::getInstance()->getMonitor(storm::SRM_SET_PERMISSION_MONITOR_NAME)->registerError(et.total_milliseconds());
			}catch(storm::MonitorNotEnabledException *exc)
			{
				srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
			}
			return SOAP_EOM;
		}
	}
	else
	{
		srmlogit(STORM_LOG_DEBUG, func, "The user is not blacklisted\n");
	}

	int ret_val = ns1__srmSetPermission_impl(soap, req, rep);

	boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();

	boost::posix_time::time_duration et = (end_time - start_time);

	if (ret_val != SOAP_OK)
	{
		try
		{
			storm::Monitoring::getInstance()->getMonitor(storm::SRM_SET_PERMISSION_MONITOR_NAME)->registerFailure(et.total_milliseconds());
		}catch(storm::MonitorNotEnabledException *exc)
		{
			srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
		}
	}
	else
	{
		try
		{
			((storm::InstrumentedMonitor*) storm::Monitoring::getInstance()->getMonitor(
					storm::SRM_SET_PERMISSION_MONITOR_NAME))->registerCompleted(
					et.total_milliseconds(),
					rep->srmSetPermissionResponse->returnStatus->statusCode);
		}catch(storm::MonitorNotEnabledException *exc)
		{
			srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
		}
	}

	return ret_val;
}

int ns1__srmCheckPermission(struct soap* soap, struct ns1__srmCheckPermissionRequest *req,
		struct ns1__srmCheckPermissionResponse_ *rep) {

	char *func = "ns1__srmCheckPermission()";
	srmLogRequest("Check permission",get_ip(soap).c_str(),storm::Credentials(soap).getDN().c_str());
	boost::posix_time::ptime start_time = boost::posix_time::microsec_clock::local_time();

	if(storm::Authorization::checkBlacklist(soap))
	{
		srmlogit(STORM_LOG_INFO, func, "The user is blacklisted\n");
		rep->srmCheckPermissionResponse = storm::soap_calloc<struct ns1__srmCheckPermissionResponse>(soap);
		if(rep->srmCheckPermissionResponse != NULL)
		{
			rep->srmCheckPermissionResponse->returnStatus = storm::soap_calloc<struct ns1__TReturnStatus>(soap);
			if (rep->srmCheckPermissionResponse->returnStatus != NULL)
			{
				rep->srmCheckPermissionResponse->returnStatus->explanation =
						"User not authorized";
				rep->srmCheckPermissionResponse->returnStatus->statusCode =
						SRM_USCOREAUTHORIZATION_USCOREFAILURE;
				boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
				boost::posix_time::time_duration et = (end_time - start_time);
				try
				{
					storm::Monitoring::getInstance()->getMonitor(
							storm::SRM_CHECK_PERMISSION_MONITOR_NAME)->registerFailure(et.total_milliseconds());
				}catch(storm::MonitorNotEnabledException *exc)
				{
					srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
				}
				return (SOAP_OK);
			} else {
				srmlogit(STORM_LOG_ERROR, func, "Error in response allocation\n");
				boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
				boost::posix_time::time_duration et = (end_time - start_time);
				try
				{
					storm::Monitoring::getInstance()->getMonitor(storm::SRM_CHECK_PERMISSION_MONITOR_NAME)->registerError(et.total_milliseconds());
				}catch(storm::MonitorNotEnabledException *exc)
				{
					srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
				}
				return SOAP_EOM;
			}
		}
		else
		{
			srmlogit(STORM_LOG_ERROR, func, "Error in response allocation\n");
			boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
			boost::posix_time::time_duration et = (end_time - start_time);
			try
			{
				storm::Monitoring::getInstance()->getMonitor(storm::SRM_CHECK_PERMISSION_MONITOR_NAME)->registerError(et.total_milliseconds());
			}catch(storm::MonitorNotEnabledException *exc)
			{
				srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
			}
			return SOAP_EOM;
		}
	}
	else
	{
		srmlogit(STORM_LOG_DEBUG, func, "The user is not blacklisted\n");
	}

	int ret_val = ns1__srmCheckPermission_impl(soap, req, rep);

	boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();

	boost::posix_time::time_duration et = (end_time - start_time);

	if (ret_val != SOAP_OK)
	{
		try
		{
			storm::Monitoring::getInstance()->getMonitor(storm::SRM_CHECK_PERMISSION_MONITOR_NAME)->registerFailure(et.total_milliseconds());
		}catch(storm::MonitorNotEnabledException *exc)
		{
			srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
		}
	}
	else
	{
		try
		{
			((storm::InstrumentedMonitor*) storm::Monitoring::getInstance()->getMonitor(
					storm::SRM_CHECK_PERMISSION_MONITOR_NAME))->registerCompleted(
					et.total_milliseconds(),
					rep->srmCheckPermissionResponse->returnStatus->statusCode);
		}catch(storm::MonitorNotEnabledException *exc)
		{
			srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
		}
	}

	return ret_val;
}

int ns1__srmGetPermission(struct soap* soap, struct ns1__srmGetPermissionRequest *req,
		struct ns1__srmGetPermissionResponse_ *rep) {

	char *func = "ns1__srmGetPermission()";
	srmLogRequest("Get permission",get_ip(soap).c_str(),storm::Credentials(soap).getDN().c_str());
	boost::posix_time::ptime start_time = boost::posix_time::microsec_clock::local_time();

	if(storm::Authorization::checkBlacklist(soap))
	{
		srmlogit(STORM_LOG_INFO, func, "The user is blacklisted\n");
		rep->srmGetPermissionResponse = storm::soap_calloc<struct ns1__srmGetPermissionResponse>(soap);
		if(rep->srmGetPermissionResponse != NULL)
		{
			rep->srmGetPermissionResponse->returnStatus = storm::soap_calloc<struct ns1__TReturnStatus>(soap);
			if (rep->srmGetPermissionResponse->returnStatus != NULL)
			{
				rep->srmGetPermissionResponse->returnStatus->explanation =
						"User not authorized";
				rep->srmGetPermissionResponse->returnStatus->statusCode =
						SRM_USCOREAUTHORIZATION_USCOREFAILURE;
				boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
				boost::posix_time::time_duration et = (end_time - start_time);
				try
				{
					storm::Monitoring::getInstance()->getMonitor(
							storm::SRM_GET_PERMISSION_MONITOR_NAME)->registerFailure(et.total_milliseconds());
				}catch(storm::MonitorNotEnabledException *exc)
				{
					srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
				}
				return (SOAP_OK);
			} else {
				srmlogit(STORM_LOG_ERROR, func, "Error in response allocation\n");
				boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
				boost::posix_time::time_duration et = (end_time - start_time);
				try
				{
					storm::Monitoring::getInstance()->getMonitor(storm::SRM_GET_PERMISSION_MONITOR_NAME)->registerError(et.total_milliseconds());
				}catch(storm::MonitorNotEnabledException *exc)
				{
					srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
				}
				return SOAP_EOM;
			}
		}
		else
		{
			srmlogit(STORM_LOG_ERROR, func, "Error in response allocation\n");
			boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
			boost::posix_time::time_duration et = (end_time - start_time);
			try
			{
				storm::Monitoring::getInstance()->getMonitor(storm::SRM_GET_PERMISSION_MONITOR_NAME)->registerError(et.total_milliseconds());
			}catch(storm::MonitorNotEnabledException *exc)
			{
				srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
			}
			return SOAP_EOM;
		}
	}
	else
	{
		srmlogit(STORM_LOG_DEBUG, func, "The user is not blacklisted\n");
	}

	int ret_val = ns1__srmGetPermission_impl(soap, req, rep);

	boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();

	boost::posix_time::time_duration et = (end_time - start_time);

	if (ret_val != SOAP_OK)
	{
		try
		{
			storm::Monitoring::getInstance()->getMonitor(storm::SRM_GET_PERMISSION_MONITOR_NAME)->registerFailure(et.total_milliseconds());
		}catch(storm::MonitorNotEnabledException *exc)
		{
			srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
		}
	}
	else
	{
		try
		{
			((storm::InstrumentedMonitor*) storm::Monitoring::getInstance()->getMonitor(
					storm::SRM_GET_PERMISSION_MONITOR_NAME))->registerCompleted(
					et.total_milliseconds(),
					rep->srmGetPermissionResponse->returnStatus->statusCode);
		}catch(storm::MonitorNotEnabledException *exc)
		{
			srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
		}
	}

	return ret_val;
}

//Space Management Functions

int ns1__srmReserveSpace(struct soap* soap, struct ns1__srmReserveSpaceRequest *req,
		struct ns1__srmReserveSpaceResponse_ *rep) {

	char *func = "ns1__srmReserveSpace()";
	srmLogRequest("Reserve space",get_ip(soap).c_str(),storm::Credentials(soap).getDN().c_str());
	boost::posix_time::ptime start_time = boost::posix_time::microsec_clock::local_time();

	if(storm::Authorization::checkBlacklist(soap))
	{
		srmlogit(STORM_LOG_INFO, func, "The user is blacklisted\n");
		rep->srmReserveSpaceResponse = storm::soap_calloc<struct ns1__srmReserveSpaceResponse>(soap);
		if(rep->srmReserveSpaceResponse != NULL)
		{
			rep->srmReserveSpaceResponse->returnStatus = storm::soap_calloc<struct ns1__TReturnStatus>(soap);
			if (rep->srmReserveSpaceResponse->returnStatus != NULL)
			{
				rep->srmReserveSpaceResponse->returnStatus->explanation =
						"User not authorized";
				rep->srmReserveSpaceResponse->returnStatus->statusCode =
						SRM_USCOREAUTHORIZATION_USCOREFAILURE;
				boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
				boost::posix_time::time_duration et = (end_time - start_time);
				try
				{
					storm::Monitoring::getInstance()->getMonitor(
							storm::SRM_RESERVE_SPACE_MONITOR_NAME)->registerFailure(et.total_milliseconds());
				}catch(storm::MonitorNotEnabledException *exc)
				{
					srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
				}
				return (SOAP_OK);
			} else {
				srmlogit(STORM_LOG_ERROR, func, "Error in response allocation\n");
				boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
				boost::posix_time::time_duration et = (end_time - start_time);
				try
				{
					storm::Monitoring::getInstance()->getMonitor(storm::SRM_RESERVE_SPACE_MONITOR_NAME)->registerError(et.total_milliseconds());
				}catch(storm::MonitorNotEnabledException *exc)
				{
					srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
				}
				return SOAP_EOM;
			}
		}
		else
		{
			srmlogit(STORM_LOG_ERROR, func, "Error in response allocation\n");
			boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
			boost::posix_time::time_duration et = (end_time - start_time);
			try
			{
				storm::Monitoring::getInstance()->getMonitor(storm::SRM_RESERVE_SPACE_MONITOR_NAME)->registerError(et.total_milliseconds());
			}catch(storm::MonitorNotEnabledException *exc)
			{
				srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
			}
			return SOAP_EOM;
		}
	}
	else
	{
		srmlogit(STORM_LOG_DEBUG, func, "The user is not blacklisted\n");
	}

	int ret_val = ns1__srmReserveSpace_impl(soap, req, rep);

	boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();

	boost::posix_time::time_duration et = (end_time - start_time);

	if (ret_val != SOAP_OK)
	{
		try
		{
			storm::Monitoring::getInstance()->getMonitor(storm::SRM_RESERVE_SPACE_MONITOR_NAME)->registerFailure(et.total_milliseconds());
		}catch(storm::MonitorNotEnabledException *exc)
		{
			srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
		}
	}
	else
	{
		try
		{
			((storm::InstrumentedMonitor*) storm::Monitoring::getInstance()->getMonitor(
					storm::SRM_RESERVE_SPACE_MONITOR_NAME))->registerCompleted(
					et.total_milliseconds(),
					rep->srmReserveSpaceResponse->returnStatus->statusCode);
		}catch(storm::MonitorNotEnabledException *exc)
		{
			srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
		}
	}

	return ret_val;
}

int ns1__srmStatusOfReserveSpaceRequest(struct soap* soap, struct ns1__srmStatusOfReserveSpaceRequestRequest *req,
		struct ns1__srmStatusOfReserveSpaceRequestResponse_ *rep) {

	char *func = "ns1__srmStatusOfReserveSpaceRequest()";
	srmLogRequest("Reserve space status",get_ip(soap).c_str(),storm::Credentials(soap).getDN().c_str());
	boost::posix_time::ptime start_time = boost::posix_time::microsec_clock::local_time();

	if(storm::Authorization::checkBlacklist(soap))
	{
		srmlogit(STORM_LOG_INFO, func, "The user is blacklisted\n");
		rep->srmStatusOfReserveSpaceRequestResponse = storm::soap_calloc<struct ns1__srmStatusOfReserveSpaceRequestResponse>(soap);
		if(rep->srmStatusOfReserveSpaceRequestResponse != NULL)
		{
			rep->srmStatusOfReserveSpaceRequestResponse->returnStatus = storm::soap_calloc<struct ns1__TReturnStatus>(soap);
			if (rep->srmStatusOfReserveSpaceRequestResponse->returnStatus != NULL)
			{
				rep->srmStatusOfReserveSpaceRequestResponse->returnStatus->explanation =
						"User not authorized";
				rep->srmStatusOfReserveSpaceRequestResponse->returnStatus->statusCode =
						SRM_USCOREAUTHORIZATION_USCOREFAILURE;
				boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
				boost::posix_time::time_duration et = (end_time - start_time);
				try
				{
					storm::Monitoring::getInstance()->getMonitor(
							storm::SRM_STATUS_OF_RESERVE_SPACE_REQUEST_MONITOR_NAME)->registerFailure(et.total_milliseconds());
				}catch(storm::MonitorNotEnabledException *exc)
				{
					srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
				}
				return (SOAP_OK);
			} else {
				srmlogit(STORM_LOG_ERROR, func, "Error in response allocation\n");
				boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
				boost::posix_time::time_duration et = (end_time - start_time);
				try
				{
					storm::Monitoring::getInstance()->getMonitor(storm::SRM_STATUS_OF_RESERVE_SPACE_REQUEST_MONITOR_NAME)->registerError(et.total_milliseconds());
				}catch(storm::MonitorNotEnabledException *exc)
				{
					srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
				}
				return SOAP_EOM;
			}
		}
		else
		{
			srmlogit(STORM_LOG_ERROR, func, "Error in response allocation\n");
			boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
			boost::posix_time::time_duration et = (end_time - start_time);
			try
			{
				storm::Monitoring::getInstance()->getMonitor(storm::SRM_STATUS_OF_RESERVE_SPACE_REQUEST_MONITOR_NAME)->registerError(et.total_milliseconds());
			}catch(storm::MonitorNotEnabledException *exc)
			{
				srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
			}
			return SOAP_EOM;
		}
	}
	else
	{
		srmlogit(STORM_LOG_DEBUG, func, "The user is not blacklisted\n");
	}

	int ret_val = ns1__srmStatusOfReserveSpaceRequest_impl(soap, req, rep);

	boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();

	boost::posix_time::time_duration et = (end_time - start_time);

	if (ret_val != SOAP_OK)
	{
		try
		{
			storm::Monitoring::getInstance()->getMonitor(storm::SRM_STATUS_OF_RESERVE_SPACE_REQUEST_MONITOR_NAME)->registerFailure(et.total_milliseconds());
		}catch(storm::MonitorNotEnabledException *exc)
		{
			srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
		}
	}
	else
	{
		try
		{
			((storm::InstrumentedMonitor*) storm::Monitoring::getInstance()->getMonitor(
					storm::SRM_STATUS_OF_RESERVE_SPACE_REQUEST_MONITOR_NAME))->registerCompleted(
					et.total_milliseconds(),
					rep->srmStatusOfReserveSpaceRequestResponse->returnStatus->statusCode);
		}catch(storm::MonitorNotEnabledException *exc)
		{
			srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
		}
	}

	return ret_val;
}

int ns1__srmReleaseSpace(struct soap* soap, struct ns1__srmReleaseSpaceRequest *req,
		struct ns1__srmReleaseSpaceResponse_ *rep) {

	char *func = "ns1__srmReleaseSpace()";
	srmLogRequest("Release space",get_ip(soap).c_str(),storm::Credentials(soap).getDN().c_str());
	boost::posix_time::ptime start_time = boost::posix_time::microsec_clock::local_time();

	if(storm::Authorization::checkBlacklist(soap))
	{
		srmlogit(STORM_LOG_INFO, func, "The user is blacklisted\n");
		rep->srmReleaseSpaceResponse = storm::soap_calloc<struct ns1__srmReleaseSpaceResponse>(soap);
		if(rep->srmReleaseSpaceResponse != NULL)
		{
			rep->srmReleaseSpaceResponse->returnStatus = storm::soap_calloc<struct ns1__TReturnStatus>(soap);
			if (rep->srmReleaseSpaceResponse->returnStatus != NULL)
			{
				rep->srmReleaseSpaceResponse->returnStatus->explanation =
						"User not authorized";
				rep->srmReleaseSpaceResponse->returnStatus->statusCode =
						SRM_USCOREAUTHORIZATION_USCOREFAILURE;
				boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
				boost::posix_time::time_duration et = (end_time - start_time);
				try
				{
					storm::Monitoring::getInstance()->getMonitor(
							storm::SRM_RELEASE_SPACE_MONITOR_NAME)->registerFailure(et.total_milliseconds());
				}catch(storm::MonitorNotEnabledException *exc)
				{
					srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
				}
				return (SOAP_OK);
			} else {
				srmlogit(STORM_LOG_ERROR, func, "Error in response allocation\n");
				boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
				boost::posix_time::time_duration et = (end_time - start_time);
				try
				{
					storm::Monitoring::getInstance()->getMonitor(storm::SRM_RELEASE_SPACE_MONITOR_NAME)->registerError(et.total_milliseconds());
				}catch(storm::MonitorNotEnabledException *exc)
				{
					srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
				}
				return SOAP_EOM;
			}
		}
		else
		{
			srmlogit(STORM_LOG_ERROR, func, "Error in response allocation\n");
			boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
			boost::posix_time::time_duration et = (end_time - start_time);
			try
			{
				storm::Monitoring::getInstance()->getMonitor(storm::SRM_RELEASE_SPACE_MONITOR_NAME)->registerError(et.total_milliseconds());
			}catch(storm::MonitorNotEnabledException *exc)
			{
				srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
			}
			return SOAP_EOM;
		}
	}
	else
	{
		srmlogit(STORM_LOG_DEBUG, func, "The user is not blacklisted\n");
	}

	int ret_val = ns1__srmReleaseSpace_impl(soap, req, rep);

	boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();

	boost::posix_time::time_duration et = (end_time - start_time);

	if (ret_val != SOAP_OK)
	{
		try
		{
			storm::Monitoring::getInstance()->getMonitor(storm::SRM_RELEASE_SPACE_MONITOR_NAME)->registerFailure(et.total_milliseconds());
		}catch(storm::MonitorNotEnabledException *exc)
		{
			srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
		}
	}
	else
	{
		try
		{
			((storm::InstrumentedMonitor*) storm::Monitoring::getInstance()->getMonitor(
					storm::SRM_RELEASE_SPACE_MONITOR_NAME))->registerCompleted(
					et.total_milliseconds(),
					rep->srmReleaseSpaceResponse->returnStatus->statusCode);
		}catch(storm::MonitorNotEnabledException *exc)
		{
			srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
		}
	}

	return ret_val;
}

int ns1__srmUpdateSpace(struct soap* soap, struct ns1__srmUpdateSpaceRequest *req,
		struct ns1__srmUpdateSpaceResponse_ *rep) {

	char *func = "ns1__srmUpdateSpace()";
	srmLogRequest("Update space",get_ip(soap).c_str(),storm::Credentials(soap).getDN().c_str());
	boost::posix_time::ptime start_time = boost::posix_time::microsec_clock::local_time();

	if(storm::Authorization::checkBlacklist(soap))
	{
		srmlogit(STORM_LOG_INFO, func, "The user is blacklisted\n");
		rep->srmUpdateSpaceResponse = storm::soap_calloc<struct ns1__srmUpdateSpaceResponse>(soap);
		if(rep->srmUpdateSpaceResponse != NULL)
		{
			rep->srmUpdateSpaceResponse->returnStatus = storm::soap_calloc<struct ns1__TReturnStatus>(soap);
			if (rep->srmUpdateSpaceResponse->returnStatus != NULL)
			{
				rep->srmUpdateSpaceResponse->returnStatus->explanation =
						"User not authorized";
				rep->srmUpdateSpaceResponse->returnStatus->statusCode =
						SRM_USCOREAUTHORIZATION_USCOREFAILURE;
				boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
				boost::posix_time::time_duration et = (end_time - start_time);
				try
				{
					storm::Monitoring::getInstance()->getMonitor(
							storm::SRM_UPDATE_SPACE_MONITOR_NAME)->registerFailure(et.total_milliseconds());
				}catch(storm::MonitorNotEnabledException *exc)
				{
					srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
				}
				return (SOAP_OK);
			} else {
				srmlogit(STORM_LOG_ERROR, func, "Error in response allocation\n");
				boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
				boost::posix_time::time_duration et = (end_time - start_time);
				try
				{
					storm::Monitoring::getInstance()->getMonitor(storm::SRM_UPDATE_SPACE_MONITOR_NAME)->registerError(et.total_milliseconds());
				}catch(storm::MonitorNotEnabledException *exc)
				{
					srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
				}
				return SOAP_EOM;
			}
		}
		else
		{
			srmlogit(STORM_LOG_ERROR, func, "Error in response allocation\n");
			boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
			boost::posix_time::time_duration et = (end_time - start_time);
			try
			{
				storm::Monitoring::getInstance()->getMonitor(storm::SRM_UPDATE_SPACE_MONITOR_NAME)->registerError(et.total_milliseconds());
			}catch(storm::MonitorNotEnabledException *exc)
			{
				srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
			}
			return SOAP_EOM;
		}
	}
	else
	{
		srmlogit(STORM_LOG_DEBUG, func, "The user is not blacklisted\n");
	}

	int ret_val = ns1__srmUpdateSpace_impl(soap, req, rep);

	boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();

	boost::posix_time::time_duration et = (end_time - start_time);

	if (ret_val != SOAP_OK)
	{
		try
		{
			storm::Monitoring::getInstance()->getMonitor(storm::SRM_UPDATE_SPACE_MONITOR_NAME)->registerFailure(et.total_milliseconds());
		}catch(storm::MonitorNotEnabledException *exc)
		{
			srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
		}
	}
	else
	{
		try
		{
			((storm::InstrumentedMonitor*) storm::Monitoring::getInstance()->getMonitor(
					storm::SRM_UPDATE_SPACE_MONITOR_NAME))->registerCompleted(
					et.total_milliseconds(),
					rep->srmUpdateSpaceResponse->returnStatus->statusCode);
		}catch(storm::MonitorNotEnabledException *exc)
		{
			srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
		}
	}

	return ret_val;
}

int ns1__srmStatusOfUpdateSpaceRequest(struct soap* soap, struct ns1__srmStatusOfUpdateSpaceRequestRequest *req,
		struct ns1__srmStatusOfUpdateSpaceRequestResponse_ *rep) {

	char *func = "ns1__srmStatusOfUpdateSpaceRequest()";
	srmLogRequest("Update space status",get_ip(soap).c_str(),storm::Credentials(soap).getDN().c_str());
	boost::posix_time::ptime start_time = boost::posix_time::microsec_clock::local_time();

	if(storm::Authorization::checkBlacklist(soap))
	{
		srmlogit(STORM_LOG_INFO, func, "The user is blacklisted\n");
		rep->srmStatusOfUpdateSpaceRequestResponse = storm::soap_calloc<struct ns1__srmStatusOfUpdateSpaceRequestResponse>(soap);
		if(rep->srmStatusOfUpdateSpaceRequestResponse != NULL)
		{
			rep->srmStatusOfUpdateSpaceRequestResponse->returnStatus = storm::soap_calloc<struct ns1__TReturnStatus>(soap);
			if (rep->srmStatusOfUpdateSpaceRequestResponse->returnStatus != NULL)
			{
				rep->srmStatusOfUpdateSpaceRequestResponse->returnStatus->explanation =
						"User not authorized";
				rep->srmStatusOfUpdateSpaceRequestResponse->returnStatus->statusCode =
						SRM_USCOREAUTHORIZATION_USCOREFAILURE;
				boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
				boost::posix_time::time_duration et = (end_time - start_time);
				try
				{
					storm::Monitoring::getInstance()->getMonitor(
							storm::SRM_STATUS_OF_UPDATE_SPACE_REQUEST_MONITOR_NAME)->registerFailure(et.total_milliseconds());
				}catch(storm::MonitorNotEnabledException *exc)
				{
					srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
				}
				return (SOAP_OK);
			} else {
				srmlogit(STORM_LOG_ERROR, func, "Error in response allocation\n");
				boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
				boost::posix_time::time_duration et = (end_time - start_time);
				try
				{
					storm::Monitoring::getInstance()->getMonitor(storm::SRM_STATUS_OF_UPDATE_SPACE_REQUEST_MONITOR_NAME)->registerError(et.total_milliseconds());
				}catch(storm::MonitorNotEnabledException *exc)
				{
					srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
				}
				return SOAP_EOM;
			}
		}
		else
		{
			srmlogit(STORM_LOG_ERROR, func, "Error in response allocation\n");
			boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
			boost::posix_time::time_duration et = (end_time - start_time);
			try
			{
				storm::Monitoring::getInstance()->getMonitor(storm::SRM_STATUS_OF_UPDATE_SPACE_REQUEST_MONITOR_NAME)->registerError(et.total_milliseconds());
			}catch(storm::MonitorNotEnabledException *exc)
			{
				srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
			}
			return SOAP_EOM;
		}
	}
	else
	{
		srmlogit(STORM_LOG_DEBUG, func, "The user is not blacklisted\n");
	}

	int ret_val = ns1__srmStatusOfUpdateSpaceRequest_impl(soap, req, rep);

	boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();

	boost::posix_time::time_duration et = (end_time - start_time);

	if (ret_val != SOAP_OK)
	{
		try
		{
			storm::Monitoring::getInstance()->getMonitor(storm::SRM_STATUS_OF_UPDATE_SPACE_REQUEST_MONITOR_NAME)->registerFailure(et.total_milliseconds());
		}catch(storm::MonitorNotEnabledException *exc)
		{
			srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
		}
	}
	else
	{
		try
		{
			((storm::InstrumentedMonitor*) storm::Monitoring::getInstance()->getMonitor(
					storm::SRM_STATUS_OF_UPDATE_SPACE_REQUEST_MONITOR_NAME))->registerCompleted(
					et.total_milliseconds(),
					rep->srmStatusOfUpdateSpaceRequestResponse->returnStatus->statusCode);
		}catch(storm::MonitorNotEnabledException *exc)
		{
			srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
		}
	}

	return ret_val;
}

int ns1__srmGetSpaceMetaData(struct soap* soap, struct ns1__srmGetSpaceMetaDataRequest *req,
		struct ns1__srmGetSpaceMetaDataResponse_ *rep) {

	char *func = "ns1__srmGetSpaceMetaData()";
	srmLogRequest("Get space metadata",get_ip(soap).c_str(),storm::Credentials(soap).getDN().c_str());
	boost::posix_time::ptime start_time = boost::posix_time::microsec_clock::local_time();

	if(storm::Authorization::checkBlacklist(soap))
	{
		srmlogit(STORM_LOG_INFO, func, "The user is blacklisted\n");
		rep->srmGetSpaceMetaDataResponse = storm::soap_calloc<struct ns1__srmGetSpaceMetaDataResponse>(soap);
		if(rep->srmGetSpaceMetaDataResponse != NULL)
		{
			rep->srmGetSpaceMetaDataResponse->returnStatus = storm::soap_calloc<struct ns1__TReturnStatus>(soap);
			if (rep->srmGetSpaceMetaDataResponse->returnStatus != NULL)
			{
				rep->srmGetSpaceMetaDataResponse->returnStatus->explanation =
						"User not authorized";
				rep->srmGetSpaceMetaDataResponse->returnStatus->statusCode =
						SRM_USCOREAUTHORIZATION_USCOREFAILURE;
				boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
				boost::posix_time::time_duration et = (end_time - start_time);
				try
				{
					storm::Monitoring::getInstance()->getMonitor(
							storm::SRM_GET_SPACE_META_DATA_MONITOR_NAME)->registerFailure(et.total_milliseconds());
				}catch(storm::MonitorNotEnabledException *exc)
				{
					srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
				}
				return (SOAP_OK);
			} else {
				srmlogit(STORM_LOG_ERROR, func, "Error in response allocation\n");
				boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
				boost::posix_time::time_duration et = (end_time - start_time);
				try
				{
					storm::Monitoring::getInstance()->getMonitor(storm::SRM_GET_SPACE_META_DATA_MONITOR_NAME)->registerError(et.total_milliseconds());
				}catch(storm::MonitorNotEnabledException *exc)
				{
					srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
				}
				return SOAP_EOM;
			}
		}
		else
		{
			srmlogit(STORM_LOG_ERROR, func, "Error in response allocation\n");
			boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
			boost::posix_time::time_duration et = (end_time - start_time);
			try
			{
				storm::Monitoring::getInstance()->getMonitor(storm::SRM_GET_SPACE_META_DATA_MONITOR_NAME)->registerError(et.total_milliseconds());
			}catch(storm::MonitorNotEnabledException *exc)
			{
				srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
			}
			return SOAP_EOM;
		}
	}
	else
	{
		srmlogit(STORM_LOG_DEBUG, func, "The user is not blacklisted\n");
	}

	int ret_val = ns1__srmGetSpaceMetaData_impl(soap, req, rep);

	boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();

	boost::posix_time::time_duration et = (end_time - start_time);

	if (ret_val != SOAP_OK)
	{
		try
		{
			storm::Monitoring::getInstance()->getMonitor(storm::SRM_GET_SPACE_META_DATA_MONITOR_NAME)->registerFailure(et.total_milliseconds());
		}catch(storm::MonitorNotEnabledException *exc)
		{
			srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
		}
	}
	else
	{
		try
		{
			((storm::InstrumentedMonitor*) storm::Monitoring::getInstance()->getMonitor(
					storm::SRM_GET_SPACE_META_DATA_MONITOR_NAME))->registerCompleted(
					et.total_milliseconds(),
					rep->srmGetSpaceMetaDataResponse->returnStatus->statusCode);
		}catch(storm::MonitorNotEnabledException *exc)
		{
			srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
		}
	}

	return ret_val;
}

int ns1__srmGetSpaceTokens(struct soap* soap, struct ns1__srmGetSpaceTokensRequest *req,
		struct ns1__srmGetSpaceTokensResponse_ *rep) {

	char *func = "ns1__srmGetSpaceTokens()";
	srmLogRequest("Get space tokens",get_ip(soap).c_str(),storm::Credentials(soap).getDN().c_str());
	boost::posix_time::ptime start_time = boost::posix_time::microsec_clock::local_time();

	if(storm::Authorization::checkBlacklist(soap))
	{
		srmlogit(STORM_LOG_INFO, func, "The user is blacklisted\n");
		rep->srmGetSpaceTokensResponse = storm::soap_calloc<struct ns1__srmGetSpaceTokensResponse>(soap);
		if(rep->srmGetSpaceTokensResponse != NULL)
		{
			rep->srmGetSpaceTokensResponse->returnStatus = storm::soap_calloc<struct ns1__TReturnStatus>(soap);
			if (rep->srmGetSpaceTokensResponse->returnStatus != NULL)
			{
				rep->srmGetSpaceTokensResponse->returnStatus->explanation =
						"User not authorized";
				rep->srmGetSpaceTokensResponse->returnStatus->statusCode =
						SRM_USCOREAUTHORIZATION_USCOREFAILURE;
				boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
				boost::posix_time::time_duration et = (end_time - start_time);
				try
				{
					storm::Monitoring::getInstance()->getMonitor(
							storm::SRM_GET_SPACE_TOKENS_MONITOR_NAME)->registerFailure(et.total_milliseconds());
				}catch(storm::MonitorNotEnabledException *exc)
				{
					srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
				}
				return (SOAP_OK);
			} else {
				srmlogit(STORM_LOG_ERROR, func, "Error in response allocation\n");
				boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
				boost::posix_time::time_duration et = (end_time - start_time);
				try
				{
					storm::Monitoring::getInstance()->getMonitor(storm::SRM_GET_SPACE_TOKENS_MONITOR_NAME)->registerError(et.total_milliseconds());
				}catch(storm::MonitorNotEnabledException *exc)
				{
					srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
				}
				return SOAP_EOM;
			}
		}
		else
		{
			srmlogit(STORM_LOG_ERROR, func, "Error in response allocation\n");
			boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
			boost::posix_time::time_duration et = (end_time - start_time);
			try
			{
				storm::Monitoring::getInstance()->getMonitor(storm::SRM_GET_SPACE_TOKENS_MONITOR_NAME)->registerError(et.total_milliseconds());
			}catch(storm::MonitorNotEnabledException *exc)
			{
				srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
			}
			return SOAP_EOM;
		}
	}
	else
	{
		srmlogit(STORM_LOG_DEBUG, func, "The user is not blacklisted\n");
	}

	int ret_val = ns1__srmGetSpaceTokens_impl(soap, req, rep);

	boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();

	boost::posix_time::time_duration et = (end_time - start_time);

	if (ret_val != SOAP_OK)
	{
		try
		{
			storm::Monitoring::getInstance()->getMonitor(storm::SRM_GET_SPACE_TOKENS_MONITOR_NAME)->registerFailure(et.total_milliseconds());
		}catch(storm::MonitorNotEnabledException *exc)
		{
			srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
		}
	}
	else
	{
		try
		{
			((storm::InstrumentedMonitor*) storm::Monitoring::getInstance()->getMonitor(
					storm::SRM_GET_SPACE_TOKENS_MONITOR_NAME))->registerCompleted(
					et.total_milliseconds(),
					rep->srmGetSpaceTokensResponse->returnStatus->statusCode);
		}catch(storm::MonitorNotEnabledException *exc)
		{
			srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
		}
	}

	return ret_val;
}

int ns1__srmChangeSpaceForFiles(struct soap* soap, struct ns1__srmChangeSpaceForFilesRequest *req,
		struct ns1__srmChangeSpaceForFilesResponse_ *rep) {

	char *func = "ns1__srmChangeSpaceForFiles()";
	srmLogRequest("Change space for files",get_ip(soap).c_str(),storm::Credentials(soap).getDN().c_str());
	boost::posix_time::ptime start_time = boost::posix_time::microsec_clock::local_time();

	if(storm::Authorization::checkBlacklist(soap))
	{
		srmlogit(STORM_LOG_INFO, func, "The user is blacklisted\n");
		rep->srmChangeSpaceForFilesResponse = storm::soap_calloc<struct ns1__srmChangeSpaceForFilesResponse>(soap);
		if(rep->srmChangeSpaceForFilesResponse != NULL)
		{
			rep->srmChangeSpaceForFilesResponse->returnStatus = storm::soap_calloc<struct ns1__TReturnStatus>(soap);
			if (rep->srmChangeSpaceForFilesResponse->returnStatus != NULL)
			{
				rep->srmChangeSpaceForFilesResponse->returnStatus->explanation =
						"User not authorized";
				rep->srmChangeSpaceForFilesResponse->returnStatus->statusCode =
						SRM_USCOREAUTHORIZATION_USCOREFAILURE;
				boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
				boost::posix_time::time_duration et = (end_time - start_time);
				try
				{
					storm::Monitoring::getInstance()->getMonitor(
							storm::SRM_CHANGE_SPACE_FOR_FILES_MONITOR_NAME)->registerFailure(et.total_milliseconds());
				}catch(storm::MonitorNotEnabledException *exc)
				{
					srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
				}
				return (SOAP_OK);
			} else {
				srmlogit(STORM_LOG_ERROR, func, "Error in response allocation\n");
				boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
				boost::posix_time::time_duration et = (end_time - start_time);
				try
				{
					storm::Monitoring::getInstance()->getMonitor(storm::SRM_CHANGE_SPACE_FOR_FILES_MONITOR_NAME)->registerError(et.total_milliseconds());
				}catch(storm::MonitorNotEnabledException *exc)
				{
					srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
				}
				return SOAP_EOM;
			}
		}
		else
		{
			srmlogit(STORM_LOG_ERROR, func, "Error in response allocation\n");
			boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
			boost::posix_time::time_duration et = (end_time - start_time);
			try
			{
				storm::Monitoring::getInstance()->getMonitor(storm::SRM_CHANGE_SPACE_FOR_FILES_MONITOR_NAME)->registerError(et.total_milliseconds());
			}catch(storm::MonitorNotEnabledException *exc)
			{
				srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
			}
			return SOAP_EOM;
		}
	}
	else
	{
		srmlogit(STORM_LOG_DEBUG, func, "The user is not blacklisted\n");
	}

	int ret_val = ns1__srmChangeSpaceForFiles_impl(soap, req, rep);

	boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();

	boost::posix_time::time_duration et = (end_time - start_time);

	if (ret_val != SOAP_OK)
	{
		try
		{
			storm::Monitoring::getInstance()->getMonitor(storm::SRM_CHANGE_SPACE_FOR_FILES_MONITOR_NAME)->registerFailure(et.total_milliseconds());
		}catch(storm::MonitorNotEnabledException *exc)
		{
			srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
		}
	}
	else
	{
		try
		{
			((storm::InstrumentedMonitor*) storm::Monitoring::getInstance()->getMonitor(
					storm::SRM_CHANGE_SPACE_FOR_FILES_MONITOR_NAME))->registerCompleted(
					et.total_milliseconds(),
					rep->srmChangeSpaceForFilesResponse->returnStatus->statusCode);
		}catch(storm::MonitorNotEnabledException *exc)
		{
			srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
		}
	}

	return ret_val;
}

int ns1__srmStatusOfChangeSpaceForFilesRequest(struct soap* soap, struct ns1__srmStatusOfChangeSpaceForFilesRequestRequest *req,
		struct ns1__srmStatusOfChangeSpaceForFilesRequestResponse_ *rep) {

	char *func = "ns1__srmStatusOfChangeSpaceForFilesRequest()";
	srmLogRequest("Change space for files status",get_ip(soap).c_str(),storm::Credentials(soap).getDN().c_str());
	boost::posix_time::ptime start_time = boost::posix_time::microsec_clock::local_time();

	if(storm::Authorization::checkBlacklist(soap))
	{
		srmlogit(STORM_LOG_INFO, func, "The user is blacklisted\n");
		rep->srmStatusOfChangeSpaceForFilesRequestResponse = storm::soap_calloc<struct ns1__srmStatusOfChangeSpaceForFilesRequestResponse>(soap);
		if(rep->srmStatusOfChangeSpaceForFilesRequestResponse != NULL)
		{
			rep->srmStatusOfChangeSpaceForFilesRequestResponse->returnStatus = storm::soap_calloc<struct ns1__TReturnStatus>(soap);
			if (rep->srmStatusOfChangeSpaceForFilesRequestResponse->returnStatus != NULL)
			{
				rep->srmStatusOfChangeSpaceForFilesRequestResponse->returnStatus->explanation =
						"User not authorized";
				rep->srmStatusOfChangeSpaceForFilesRequestResponse->returnStatus->statusCode =
						SRM_USCOREAUTHORIZATION_USCOREFAILURE;
				boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
				boost::posix_time::time_duration et = (end_time - start_time);
				try
				{
					storm::Monitoring::getInstance()->getMonitor(
							storm::SRM_STATUS_OF_CHANGE_SPACE_FOR_FILES_REQUEST_MONITOR_NAME)->registerFailure(et.total_milliseconds());
				}catch(storm::MonitorNotEnabledException *exc)
				{
					srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
				}
				return (SOAP_OK);
			} else {
				srmlogit(STORM_LOG_ERROR, func, "Error in response allocation\n");
				boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
				boost::posix_time::time_duration et = (end_time - start_time);
				try
				{
					storm::Monitoring::getInstance()->getMonitor(storm::SRM_STATUS_OF_CHANGE_SPACE_FOR_FILES_REQUEST_MONITOR_NAME)->registerError(et.total_milliseconds());
				}catch(storm::MonitorNotEnabledException *exc)
				{
					srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
				}
				return SOAP_EOM;
			}
		}
		else
		{
			srmlogit(STORM_LOG_ERROR, func, "Error in response allocation\n");
			boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
			boost::posix_time::time_duration et = (end_time - start_time);
			try
			{
				storm::Monitoring::getInstance()->getMonitor(storm::SRM_STATUS_OF_CHANGE_SPACE_FOR_FILES_REQUEST_MONITOR_NAME)->registerError(et.total_milliseconds());
			}catch(storm::MonitorNotEnabledException *exc)
			{
				srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
			}
			return SOAP_EOM;
		}
	}
	else
	{
		srmlogit(STORM_LOG_DEBUG, func, "The user is not blacklisted\n");
	}

	int ret_val = ns1__srmStatusOfChangeSpaceForFilesRequest_impl(soap, req, rep);

	boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();

	boost::posix_time::time_duration et = (end_time - start_time);

	if (ret_val != SOAP_OK)
	{
		try
		{
			storm::Monitoring::getInstance()->getMonitor(storm::SRM_STATUS_OF_CHANGE_SPACE_FOR_FILES_REQUEST_MONITOR_NAME)->registerFailure(et.total_milliseconds());
		}catch(storm::MonitorNotEnabledException *exc)
		{
			srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
		}
	}
	else
	{
		try
		{
			((storm::InstrumentedMonitor*) storm::Monitoring::getInstance()->getMonitor(
					storm::SRM_STATUS_OF_CHANGE_SPACE_FOR_FILES_REQUEST_MONITOR_NAME))->registerCompleted(
					et.total_milliseconds(),
					rep->srmStatusOfChangeSpaceForFilesRequestResponse->returnStatus->statusCode);
		}catch(storm::MonitorNotEnabledException *exc)
		{
			srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
		}
	}

	return ret_val;
}

int ns1__srmExtendFileLifeTimeInSpace(struct soap* soap, struct ns1__srmExtendFileLifeTimeInSpaceRequest *req,
		struct ns1__srmExtendFileLifeTimeInSpaceResponse_ *rep) {

	char *func = "ns1__srmExtendFileLifeTimeInSpace()";
	srmLogRequest("Extend file life time in space",get_ip(soap).c_str(),storm::Credentials(soap).getDN().c_str());
	boost::posix_time::ptime start_time = boost::posix_time::microsec_clock::local_time();

	if(storm::Authorization::checkBlacklist(soap))
	{
		srmlogit(STORM_LOG_INFO, func, "The user is blacklisted\n");
		rep->srmExtendFileLifeTimeInSpaceResponse = storm::soap_calloc<struct ns1__srmExtendFileLifeTimeInSpaceResponse>(soap);
		if(rep->srmExtendFileLifeTimeInSpaceResponse != NULL)
		{
			rep->srmExtendFileLifeTimeInSpaceResponse->returnStatus = storm::soap_calloc<struct ns1__TReturnStatus>(soap);
			if (rep->srmExtendFileLifeTimeInSpaceResponse->returnStatus != NULL)
			{
				rep->srmExtendFileLifeTimeInSpaceResponse->returnStatus->explanation =
						"User not authorized";
				rep->srmExtendFileLifeTimeInSpaceResponse->returnStatus->statusCode =
						SRM_USCOREAUTHORIZATION_USCOREFAILURE;
				boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
				boost::posix_time::time_duration et = (end_time - start_time);
				try
				{
					storm::Monitoring::getInstance()->getMonitor(
							storm::SRM_EXTEND_FILE_LIFE_TIME_IN_SPACE_MONITOR_NAME)->registerFailure(et.total_milliseconds());
				}catch(storm::MonitorNotEnabledException *exc)
				{
					srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
				}
				return (SOAP_OK);
			} else {
				srmlogit(STORM_LOG_ERROR, func, "Error in response allocation\n");
				boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
				boost::posix_time::time_duration et = (end_time - start_time);
				try
				{
					storm::Monitoring::getInstance()->getMonitor(storm::SRM_EXTEND_FILE_LIFE_TIME_IN_SPACE_MONITOR_NAME)->registerError(et.total_milliseconds());
				}catch(storm::MonitorNotEnabledException *exc)
				{
					srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
				}
				return SOAP_EOM;
			}
		}
		else
		{
			srmlogit(STORM_LOG_ERROR, func, "Error in response allocation\n");
			boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
			boost::posix_time::time_duration et = (end_time - start_time);
			try
			{
				storm::Monitoring::getInstance()->getMonitor(storm::SRM_EXTEND_FILE_LIFE_TIME_IN_SPACE_MONITOR_NAME)->registerError(et.total_milliseconds());
			}catch(storm::MonitorNotEnabledException *exc)
			{
				srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
			}
			return SOAP_EOM;
		}
	}
	else
	{
		srmlogit(STORM_LOG_DEBUG, func, "The user is not blacklisted\n");
	}

	int ret_val = ns1__srmExtendFileLifeTimeInSpace_impl(soap, req, rep);

	boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();

	boost::posix_time::time_duration et = (end_time - start_time);

	if (ret_val != SOAP_OK)
	{
		try
		{
			storm::Monitoring::getInstance()->getMonitor(storm::SRM_EXTEND_FILE_LIFE_TIME_IN_SPACE_MONITOR_NAME)->registerFailure(et.total_milliseconds());
		}catch(storm::MonitorNotEnabledException *exc)
		{
			srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
		}
	}
	else
	{
		try
		{
			((storm::InstrumentedMonitor*) storm::Monitoring::getInstance()->getMonitor(
					storm::SRM_EXTEND_FILE_LIFE_TIME_IN_SPACE_MONITOR_NAME))->registerCompleted(
					et.total_milliseconds(),
					rep->srmExtendFileLifeTimeInSpaceResponse->returnStatus->statusCode);
		}catch(storm::MonitorNotEnabledException *exc)
		{
			srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
		}
	}

	return ret_val;
}

int ns1__srmPurgeFromSpace(struct soap* soap, struct ns1__srmPurgeFromSpaceRequest *req,
		struct ns1__srmPurgeFromSpaceResponse_ *rep) {

	char *func = "ns1__srmPurgeFromSpace()";
	srmLogRequest("Purge from space",get_ip(soap).c_str(),storm::Credentials(soap).getDN().c_str());
	boost::posix_time::ptime start_time = boost::posix_time::microsec_clock::local_time();

	if(storm::Authorization::checkBlacklist(soap))
	{
		srmlogit(STORM_LOG_INFO, func, "The user is blacklisted\n");
		rep->srmPurgeFromSpaceResponse = storm::soap_calloc<struct ns1__srmPurgeFromSpaceResponse>(soap);
		if(rep->srmPurgeFromSpaceResponse != NULL)
		{
			rep->srmPurgeFromSpaceResponse->returnStatus = storm::soap_calloc<struct ns1__TReturnStatus>(soap);
			if (rep->srmPurgeFromSpaceResponse->returnStatus != NULL)
			{
				rep->srmPurgeFromSpaceResponse->returnStatus->explanation =
						"User not authorized";
				rep->srmPurgeFromSpaceResponse->returnStatus->statusCode =
						SRM_USCOREAUTHORIZATION_USCOREFAILURE;
				boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
				boost::posix_time::time_duration et = (end_time - start_time);
				try
				{
					storm::Monitoring::getInstance()->getMonitor(
							storm::SRM_PURGE_FROM_SPACE_MONITOR_NAME)->registerFailure(et.total_milliseconds());
				}catch(storm::MonitorNotEnabledException *exc)
				{
					srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
				}
				return (SOAP_OK);
			} else {
				srmlogit(STORM_LOG_ERROR, func, "Error in response allocation\n");
				boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
				boost::posix_time::time_duration et = (end_time - start_time);
				try
				{
					storm::Monitoring::getInstance()->getMonitor(storm::SRM_PURGE_FROM_SPACE_MONITOR_NAME)->registerError(et.total_milliseconds());
				}catch(storm::MonitorNotEnabledException *exc)
				{
					srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
				}
				return SOAP_EOM;
			}
		}
		else
		{
			srmlogit(STORM_LOG_ERROR, func, "Error in response allocation\n");
			boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
			boost::posix_time::time_duration et = (end_time - start_time);
			try
			{
				storm::Monitoring::getInstance()->getMonitor(storm::SRM_PURGE_FROM_SPACE_MONITOR_NAME)->registerError(et.total_milliseconds());
			}catch(storm::MonitorNotEnabledException *exc)
			{
				srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
			}
			return SOAP_EOM;
		}
	}
	else
	{
		srmlogit(STORM_LOG_DEBUG, func, "The user is not blacklisted\n");
	}

	int ret_val = ns1__srmPurgeFromSpace_impl(soap, req, rep);

	boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();

	boost::posix_time::time_duration et = (end_time - start_time);

	if (ret_val != SOAP_OK)
	{
		try
		{
			storm::Monitoring::getInstance()->getMonitor(storm::SRM_PURGE_FROM_SPACE_MONITOR_NAME)->registerFailure(et.total_milliseconds());
		}catch(storm::MonitorNotEnabledException *exc)
		{
			srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
		}
	}
	else
	{
		try
		{
			((storm::InstrumentedMonitor*) storm::Monitoring::getInstance()->getMonitor(
					storm::SRM_PURGE_FROM_SPACE_MONITOR_NAME))->registerCompleted(
					et.total_milliseconds(),
					rep->srmPurgeFromSpaceResponse->returnStatus->statusCode);
		}catch(storm::MonitorNotEnabledException *exc)
		{
			srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
		}
	}

	return ret_val;
}

//Data Transfer Functions

int ns1__srmReleaseFiles(struct soap* soap, struct ns1__srmReleaseFilesRequest *req,
		struct ns1__srmReleaseFilesResponse_ *rep) {

	char *func = "ns1__srmReleaseFiles()";
	srmLogRequest("Release files",get_ip(soap).c_str(),storm::Credentials(soap).getDN().c_str());
	boost::posix_time::ptime start_time = boost::posix_time::microsec_clock::local_time();

	if(storm::Authorization::checkBlacklist(soap))
	{
		srmlogit(STORM_LOG_INFO, func, "The user is blacklisted\n");
		rep->srmReleaseFilesResponse = storm::soap_calloc<struct ns1__srmReleaseFilesResponse>(soap);
		if(rep->srmReleaseFilesResponse != NULL)
		{
			rep->srmReleaseFilesResponse->returnStatus = storm::soap_calloc<struct ns1__TReturnStatus>(soap);
			if (rep->srmReleaseFilesResponse->returnStatus != NULL)
			{
				rep->srmReleaseFilesResponse->returnStatus->explanation =
						"User not authorized";
				rep->srmReleaseFilesResponse->returnStatus->statusCode =
						SRM_USCOREAUTHORIZATION_USCOREFAILURE;
				boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
				boost::posix_time::time_duration et = (end_time - start_time);
				try
				{
					storm::Monitoring::getInstance()->getMonitor(
							storm::SRM_RELEASE_FILES_MONITOR_NAME)->registerFailure(et.total_milliseconds());
				}catch(storm::MonitorNotEnabledException *exc)
				{
					srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
				}
				return (SOAP_OK);
			} else {
				srmlogit(STORM_LOG_ERROR, func, "Error in response allocation\n");
				boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
				boost::posix_time::time_duration et = (end_time - start_time);
				try
				{
					storm::Monitoring::getInstance()->getMonitor(storm::SRM_RELEASE_FILES_MONITOR_NAME)->registerError(et.total_milliseconds());
				}catch(storm::MonitorNotEnabledException *exc)
				{
					srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
				}
				return SOAP_EOM;
			}
		}
		else
		{
			srmlogit(STORM_LOG_ERROR, func, "Error in response allocation\n");
			boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
			boost::posix_time::time_duration et = (end_time - start_time);
			try
			{
				storm::Monitoring::getInstance()->getMonitor(storm::SRM_RELEASE_FILES_MONITOR_NAME)->registerError(et.total_milliseconds());
			}catch(storm::MonitorNotEnabledException *exc)
			{
				srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
			}
			return SOAP_EOM;
		}
	}
	else
	{
		srmlogit(STORM_LOG_DEBUG, func, "The user is not blacklisted\n");
	}

	int ret_val = ns1__srmReleaseFiles_impl(soap, req, rep);

	boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();

	boost::posix_time::time_duration et = (end_time - start_time);

	if (ret_val != SOAP_OK)
	{
		try
		{
			storm::Monitoring::getInstance()->getMonitor(storm::SRM_RELEASE_FILES_MONITOR_NAME)->registerFailure(et.total_milliseconds());
		}catch(storm::MonitorNotEnabledException *exc)
		{
			srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
		}
	}
	else
	{
		try
		{
			((storm::InstrumentedMonitor*) storm::Monitoring::getInstance()->getMonitor(
					storm::SRM_RELEASE_FILES_MONITOR_NAME))->registerCompleted(
					et.total_milliseconds(),
					rep->srmReleaseFilesResponse->returnStatus->statusCode);
		}catch(storm::MonitorNotEnabledException *exc)
		{
			srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
		}
	}

	return ret_val;
}

int ns1__srmPutDone(struct soap* soap, struct ns1__srmPutDoneRequest *req,
		struct ns1__srmPutDoneResponse_ *rep) {

	char *func = "ns1__srmPutDone()";
	srmLogRequest("Put done",get_ip(soap).c_str(),storm::Credentials(soap).getDN().c_str());
	boost::posix_time::ptime start_time = boost::posix_time::microsec_clock::local_time();

	if(storm::Authorization::checkBlacklist(soap))
	{
		srmlogit(STORM_LOG_INFO, func, "The user is blacklisted\n");
		rep->srmPutDoneResponse = storm::soap_calloc<struct ns1__srmPutDoneResponse>(soap);
		if(rep->srmPutDoneResponse != NULL)
		{
			rep->srmPutDoneResponse->returnStatus = storm::soap_calloc<struct ns1__TReturnStatus>(soap);
			if (rep->srmPutDoneResponse->returnStatus != NULL)
			{
				rep->srmPutDoneResponse->returnStatus->explanation =
						"User not authorized";
				rep->srmPutDoneResponse->returnStatus->statusCode =
						SRM_USCOREAUTHORIZATION_USCOREFAILURE;
				boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
				boost::posix_time::time_duration et = (end_time - start_time);
				try
				{
					storm::Monitoring::getInstance()->getMonitor(
							storm::SRM_PUT_DONE_MONITOR_NAME)->registerFailure(et.total_milliseconds());
				}catch(storm::MonitorNotEnabledException *exc)
				{
					srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
				}
				return (SOAP_OK);
			} else {
				srmlogit(STORM_LOG_ERROR, func, "Error in response allocation\n");
				boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
				boost::posix_time::time_duration et = (end_time - start_time);
				try
				{
					storm::Monitoring::getInstance()->getMonitor(storm::SRM_PUT_DONE_MONITOR_NAME)->registerError(et.total_milliseconds());
				}catch(storm::MonitorNotEnabledException *exc)
				{
					srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
				}
				return SOAP_EOM;
			}
		}
		else
		{
			srmlogit(STORM_LOG_ERROR, func, "Error in response allocation\n");
			boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
			boost::posix_time::time_duration et = (end_time - start_time);
			try
			{
				storm::Monitoring::getInstance()->getMonitor(storm::SRM_PUT_DONE_MONITOR_NAME)->registerError(et.total_milliseconds());
			}catch(storm::MonitorNotEnabledException *exc)
			{
				srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
			}
			return SOAP_EOM;
		}
	}
	else
	{
		srmlogit(STORM_LOG_DEBUG, func, "The user is not blacklisted\n");
	}

	int ret_val = ns1__srmPutDone_impl(soap, req, rep);

	boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();

	boost::posix_time::time_duration et = (end_time - start_time);

	if (ret_val != SOAP_OK)
	{
		try
		{
			storm::Monitoring::getInstance()->getMonitor(storm::SRM_PUT_DONE_MONITOR_NAME)->registerFailure(et.total_milliseconds());
		}catch(storm::MonitorNotEnabledException *exc)
		{
			srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
		}
	}
	else
	{
		try
		{
			((storm::InstrumentedMonitor*) storm::Monitoring::getInstance()->getMonitor(
					storm::SRM_PUT_DONE_MONITOR_NAME))->registerCompleted(
					et.total_milliseconds(),
					rep->srmPutDoneResponse->returnStatus->statusCode);
		}catch(storm::MonitorNotEnabledException *exc)
		{
			srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
		}
	}
	return ret_val;
}

int ns1__srmAbortRequest(struct soap* soap, struct ns1__srmAbortRequestRequest *req,
		struct ns1__srmAbortRequestResponse_ *rep) {

	char *func = "ns1__srmAbortRequest()";
	srmLogRequest("Abort request",get_ip(soap).c_str(),storm::Credentials(soap).getDN().c_str());
	boost::posix_time::ptime start_time = boost::posix_time::microsec_clock::local_time();

	if(storm::Authorization::checkBlacklist(soap))
	{
		srmlogit(STORM_LOG_INFO, func, "The user is blacklisted\n");
		rep->srmAbortRequestResponse = storm::soap_calloc<struct ns1__srmAbortRequestResponse>(soap);
		if(rep->srmAbortRequestResponse != NULL)
		{
			rep->srmAbortRequestResponse->returnStatus = storm::soap_calloc<struct ns1__TReturnStatus>(soap);
			if (rep->srmAbortRequestResponse->returnStatus != NULL)
			{
				rep->srmAbortRequestResponse->returnStatus->explanation =
						"User not authorized";
				rep->srmAbortRequestResponse->returnStatus->statusCode =
						SRM_USCOREAUTHORIZATION_USCOREFAILURE;
				boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
				boost::posix_time::time_duration et = (end_time - start_time);
				try
				{
					storm::Monitoring::getInstance()->getMonitor(
							storm::SRM_ABORT_REQUEST_MONITOR_NAME)->registerFailure(et.total_milliseconds());
				}catch(storm::MonitorNotEnabledException *exc)
				{
					srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
				}
				return (SOAP_OK);
			} else {
				srmlogit(STORM_LOG_ERROR, func, "Error in response allocation\n");
				boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
				boost::posix_time::time_duration et = (end_time - start_time);
				try
				{
					storm::Monitoring::getInstance()->getMonitor(storm::SRM_ABORT_REQUEST_MONITOR_NAME)->registerError(et.total_milliseconds());
				}catch(storm::MonitorNotEnabledException *exc)
				{
					srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
				}
				return SOAP_EOM;
			}
		}
		else
		{
			srmlogit(STORM_LOG_ERROR, func, "Error in response allocation\n");
			boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
			boost::posix_time::time_duration et = (end_time - start_time);
			try
			{
				storm::Monitoring::getInstance()->getMonitor(storm::SRM_ABORT_REQUEST_MONITOR_NAME)->registerError(et.total_milliseconds());
			}catch(storm::MonitorNotEnabledException *exc)
			{
				srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
			}
			return SOAP_EOM;
		}
	}
	else
	{
		srmlogit(STORM_LOG_DEBUG, func, "The user is not blacklisted\n");
	}

	int ret_val = ns1__srmAbortRequest_impl(soap, req, rep);

	boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();

	boost::posix_time::time_duration et = (end_time - start_time);

	if (ret_val != SOAP_OK)
	{
		try
		{
			storm::Monitoring::getInstance()->getMonitor(storm::SRM_ABORT_REQUEST_MONITOR_NAME)->registerFailure(et.total_milliseconds());
		}catch(storm::MonitorNotEnabledException *exc)
		{
			srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
		}
	}
	else
	{
		try
		{
			((storm::InstrumentedMonitor*) storm::Monitoring::getInstance()->getMonitor(
					storm::SRM_ABORT_REQUEST_MONITOR_NAME))->registerCompleted(
					et.total_milliseconds(),
					rep->srmAbortRequestResponse->returnStatus->statusCode);
		}catch(storm::MonitorNotEnabledException *exc)
		{
			srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
		}
	}

	return ret_val;
}

int ns1__srmAbortFiles(struct soap* soap, struct ns1__srmAbortFilesRequest *req,
		struct ns1__srmAbortFilesResponse_ *rep) {

	char *func = "ns1__srmAbortFiles()";
	srmLogRequest("Abort files",get_ip(soap).c_str(),storm::Credentials(soap).getDN().c_str());
	boost::posix_time::ptime start_time = boost::posix_time::microsec_clock::local_time();

	if(storm::Authorization::checkBlacklist(soap))
	{
		srmlogit(STORM_LOG_INFO, func, "The user is blacklisted\n");
		rep->srmAbortFilesResponse = storm::soap_calloc<struct ns1__srmAbortFilesResponse>(soap);
		if(rep->srmAbortFilesResponse != NULL)
		{
			rep->srmAbortFilesResponse->returnStatus = storm::soap_calloc<struct ns1__TReturnStatus>(soap);
			if (rep->srmAbortFilesResponse->returnStatus != NULL)
			{
				rep->srmAbortFilesResponse->returnStatus->explanation =
						"User not authorized";
				rep->srmAbortFilesResponse->returnStatus->statusCode =
						SRM_USCOREAUTHORIZATION_USCOREFAILURE;
				boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
				boost::posix_time::time_duration et = (end_time - start_time);
				try
				{
					storm::Monitoring::getInstance()->getMonitor(
							storm::SRM_ABORT_FILES_MONITOR_NAME)->registerFailure(et.total_milliseconds());
				}catch(storm::MonitorNotEnabledException *exc)
				{
					srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
				}
				return (SOAP_OK);
			} else {
				srmlogit(STORM_LOG_ERROR, func, "Error in response allocation\n");
				boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
				boost::posix_time::time_duration et = (end_time - start_time);
				try
				{
					storm::Monitoring::getInstance()->getMonitor(storm::SRM_ABORT_FILES_MONITOR_NAME)->registerError(et.total_milliseconds());
				}catch(storm::MonitorNotEnabledException *exc)
				{
					srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
				}
				return SOAP_EOM;
			}
		}
		else
		{
			srmlogit(STORM_LOG_ERROR, func, "Error in response allocation\n");
			boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
			boost::posix_time::time_duration et = (end_time - start_time);
			try
			{
				storm::Monitoring::getInstance()->getMonitor(storm::SRM_ABORT_FILES_MONITOR_NAME)->registerError(et.total_milliseconds());
			}catch(storm::MonitorNotEnabledException *exc)
			{
				srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
			}
			return SOAP_EOM;
		}
	}
	else
	{
		srmlogit(STORM_LOG_DEBUG, func, "The user is not blacklisted\n");
	}

	int ret_val = ns1__srmAbortFiles_impl(soap, req, rep);

	boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();

	boost::posix_time::time_duration et = (end_time - start_time);

	if (ret_val != SOAP_OK)
	{
		try
		{
			storm::Monitoring::getInstance()->getMonitor(storm::SRM_ABORT_FILES_MONITOR_NAME)->registerFailure(et.total_milliseconds());
		}catch(storm::MonitorNotEnabledException *exc)
		{
			srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
		}
	}
	else
	{
		try
		{
			((storm::InstrumentedMonitor*) storm::Monitoring::getInstance()->getMonitor(
					storm::SRM_ABORT_FILES_MONITOR_NAME))->registerCompleted(
					et.total_milliseconds(),
					rep->srmAbortFilesResponse->returnStatus->statusCode);
		}catch(storm::MonitorNotEnabledException *exc)
		{
			srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
		}
	}

	return ret_val;
}

int ns1__srmSuspendRequest(struct soap* soap, struct ns1__srmSuspendRequestRequest *req,
		struct ns1__srmSuspendRequestResponse_ *rep) {

	char *func = "ns1__srmSuspendRequest()";
	srmLogRequest("Suspend request",get_ip(soap).c_str(),storm::Credentials(soap).getDN().c_str());
	boost::posix_time::ptime start_time = boost::posix_time::microsec_clock::local_time();

	if(storm::Authorization::checkBlacklist(soap))
	{
		srmlogit(STORM_LOG_INFO, func, "The user is blacklisted\n");
		rep->srmSuspendRequestResponse = storm::soap_calloc<struct ns1__srmSuspendRequestResponse>(soap);
		if(rep->srmSuspendRequestResponse != NULL)
		{
			rep->srmSuspendRequestResponse->returnStatus = storm::soap_calloc<struct ns1__TReturnStatus>(soap);
			if (rep->srmSuspendRequestResponse->returnStatus != NULL)
			{
				rep->srmSuspendRequestResponse->returnStatus->explanation =
						"User not authorized";
				rep->srmSuspendRequestResponse->returnStatus->statusCode =
						SRM_USCOREAUTHORIZATION_USCOREFAILURE;
				boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
				boost::posix_time::time_duration et = (end_time - start_time);
				try
				{
					storm::Monitoring::getInstance()->getMonitor(
							storm::SRM_SUSPEND_REQUEST_MONITOR_NAME)->registerFailure(et.total_milliseconds());
				}catch(storm::MonitorNotEnabledException *exc)
				{
					srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
				}
				return (SOAP_OK);
			} else {
				srmlogit(STORM_LOG_ERROR, func, "Error in response allocation\n");
				boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
				boost::posix_time::time_duration et = (end_time - start_time);
				try
				{
					storm::Monitoring::getInstance()->getMonitor(storm::SRM_SUSPEND_REQUEST_MONITOR_NAME)->registerError(et.total_milliseconds());
				}catch(storm::MonitorNotEnabledException *exc)
				{
					srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
				}
				return SOAP_EOM;
			}
		}
		else
		{
			srmlogit(STORM_LOG_ERROR, func, "Error in response allocation\n");
			boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
			boost::posix_time::time_duration et = (end_time - start_time);
			try
			{
				storm::Monitoring::getInstance()->getMonitor(storm::SRM_SUSPEND_REQUEST_MONITOR_NAME)->registerError(et.total_milliseconds());
			}catch(storm::MonitorNotEnabledException *exc)
			{
				srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
			}
			return SOAP_EOM;
		}
	}
	else
	{
		srmlogit(STORM_LOG_DEBUG, func, "The user is not blacklisted\n");
	}

	int ret_val = ns1__srmSuspendRequest_impl(soap, req, rep);

	boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();

	boost::posix_time::time_duration et = (end_time - start_time);

	if (ret_val != SOAP_OK)
	{
		try
		{
			storm::Monitoring::getInstance()->getMonitor(storm::SRM_SUSPEND_REQUEST_MONITOR_NAME)->registerFailure(et.total_milliseconds());
		}catch(storm::MonitorNotEnabledException *exc)
		{
			srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
		}
	}
	else
	{
		try
		{
			((storm::InstrumentedMonitor*) storm::Monitoring::getInstance()->getMonitor(
					storm::SRM_SUSPEND_REQUEST_MONITOR_NAME))->registerCompleted(
					et.total_milliseconds(),
					rep->srmSuspendRequestResponse->returnStatus->statusCode);
		}catch(storm::MonitorNotEnabledException *exc)
		{
			srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
		}
	}

	return ret_val;
}

int ns1__srmResumeRequest(struct soap* soap, struct ns1__srmResumeRequestRequest *req,
		struct ns1__srmResumeRequestResponse_ *rep) {

	char *func = "ns1__srmResumeRequest()";
	srmLogRequest("Resume request",get_ip(soap).c_str(),storm::Credentials(soap).getDN().c_str());
	boost::posix_time::ptime start_time = boost::posix_time::microsec_clock::local_time();

	if(storm::Authorization::checkBlacklist(soap))
	{
		srmlogit(STORM_LOG_INFO, func, "The user is blacklisted\n");
		rep->srmResumeRequestResponse = storm::soap_calloc<struct ns1__srmResumeRequestResponse>(soap);
		if(rep->srmResumeRequestResponse != NULL)
		{
			rep->srmResumeRequestResponse->returnStatus = storm::soap_calloc<struct ns1__TReturnStatus>(soap);
			if (rep->srmResumeRequestResponse->returnStatus != NULL)
			{
				rep->srmResumeRequestResponse->returnStatus->explanation =
						"User not authorized";
				rep->srmResumeRequestResponse->returnStatus->statusCode =
						SRM_USCOREAUTHORIZATION_USCOREFAILURE;
				boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
				boost::posix_time::time_duration et = (end_time - start_time);
				try
				{
					storm::Monitoring::getInstance()->getMonitor(
							storm::SRM_RESUME_REQUEST_MONITOR_NAME)->registerFailure(et.total_milliseconds());
				}catch(storm::MonitorNotEnabledException *exc)
				{
					srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
				}
				return (SOAP_OK);
			} else {
				srmlogit(STORM_LOG_ERROR, func, "Error in response allocation\n");
				boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
				boost::posix_time::time_duration et = (end_time - start_time);
				try
				{
					storm::Monitoring::getInstance()->getMonitor(storm::SRM_RESUME_REQUEST_MONITOR_NAME)->registerError(et.total_milliseconds());
				}catch(storm::MonitorNotEnabledException *exc)
				{
					srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
				}
				return SOAP_EOM;
			}
		}
		else
		{
			srmlogit(STORM_LOG_ERROR, func, "Error in response allocation\n");
			boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
			boost::posix_time::time_duration et = (end_time - start_time);
			try
			{
				storm::Monitoring::getInstance()->getMonitor(storm::SRM_RESUME_REQUEST_MONITOR_NAME)->registerError(et.total_milliseconds());
			}catch(storm::MonitorNotEnabledException *exc)
			{
				srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
			}
			return SOAP_EOM;
		}
	}
	else
	{
		srmlogit(STORM_LOG_DEBUG, func, "The user is not blacklisted\n");
	}

	int ret_val = ns1__srmResumeRequest_impl(soap, req, rep);

	boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();

	boost::posix_time::time_duration et = (end_time - start_time);

	if (ret_val != SOAP_OK)
	{
		try
		{
			storm::Monitoring::getInstance()->getMonitor(storm::SRM_RESUME_REQUEST_MONITOR_NAME)->registerFailure(et.total_milliseconds());
		}catch(storm::MonitorNotEnabledException *exc)
		{
			srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
		}
	}
	else
	{
		try
		{
			((storm::InstrumentedMonitor*) storm::Monitoring::getInstance()->getMonitor(
					storm::SRM_RESUME_REQUEST_MONITOR_NAME))->registerCompleted(
					et.total_milliseconds(),
					rep->srmResumeRequestResponse->returnStatus->statusCode);
		}catch(storm::MonitorNotEnabledException *exc)
		{
			srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
		}
	}

	return ret_val;
}

int ns1__srmExtendFileLifeTime(struct soap* soap, struct ns1__srmExtendFileLifeTimeRequest *req,
		struct ns1__srmExtendFileLifeTimeResponse_ *rep) {

	char *func = "ns1__srmExtendFileLifeTime()";
	srmLogRequest("Extend file life time",get_ip(soap).c_str(),storm::Credentials(soap).getDN().c_str());
	boost::posix_time::ptime start_time = boost::posix_time::microsec_clock::local_time();

	if(storm::Authorization::checkBlacklist(soap))
	{
		srmlogit(STORM_LOG_INFO, func, "The user is blacklisted\n");
		rep->srmExtendFileLifeTimeResponse = storm::soap_calloc<struct ns1__srmExtendFileLifeTimeResponse>(soap);
		if(rep->srmExtendFileLifeTimeResponse != NULL)
		{
			rep->srmExtendFileLifeTimeResponse->returnStatus = storm::soap_calloc<struct ns1__TReturnStatus>(soap);
			if (rep->srmExtendFileLifeTimeResponse->returnStatus != NULL)
			{
				rep->srmExtendFileLifeTimeResponse->returnStatus->explanation =
						"User not authorized";
				rep->srmExtendFileLifeTimeResponse->returnStatus->statusCode =
						SRM_USCOREAUTHORIZATION_USCOREFAILURE;
				boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
				boost::posix_time::time_duration et = (end_time - start_time);
				try
				{
					storm::Monitoring::getInstance()->getMonitor(
							storm::SRM_EXTEND_FILE_LIFE_TIME_MONITOR_NAME)->registerFailure(et.total_milliseconds());
				}catch(storm::MonitorNotEnabledException *exc)
				{
					srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
				}
				return (SOAP_OK);
			} else {
				srmlogit(STORM_LOG_ERROR, func, "Error in response allocation\n");
				boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
				boost::posix_time::time_duration et = (end_time - start_time);
				try
				{
					storm::Monitoring::getInstance()->getMonitor(storm::SRM_EXTEND_FILE_LIFE_TIME_MONITOR_NAME)->registerError(et.total_milliseconds());
				}catch(storm::MonitorNotEnabledException *exc)
				{
					srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
				}
				return SOAP_EOM;
			}
		}
		else
		{
			srmlogit(STORM_LOG_ERROR, func, "Error in response allocation\n");
			boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
			boost::posix_time::time_duration et = (end_time - start_time);
			try
			{
				storm::Monitoring::getInstance()->getMonitor(storm::SRM_EXTEND_FILE_LIFE_TIME_MONITOR_NAME)->registerError(et.total_milliseconds());
			}catch(storm::MonitorNotEnabledException *exc)
			{
				srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
			}
			return SOAP_EOM;
		}
	}
	else
	{
		srmlogit(STORM_LOG_DEBUG, func, "The user is not blacklisted\n");
	}

	int ret_val = ns1__srmExtendFileLifeTime_impl(soap, req, rep);

	boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();

	boost::posix_time::time_duration et = (end_time - start_time);

	if (ret_val != SOAP_OK)
	{
		try
		{
			storm::Monitoring::getInstance()->getMonitor(storm::SRM_EXTEND_FILE_LIFE_TIME_MONITOR_NAME)->registerFailure(et.total_milliseconds());
		}catch(storm::MonitorNotEnabledException *exc)
		{
			srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
		}
	}
	else
	{
		try
		{
			((storm::InstrumentedMonitor*) storm::Monitoring::getInstance()->getMonitor(
					storm::SRM_EXTEND_FILE_LIFE_TIME_MONITOR_NAME))->registerCompleted(
					et.total_milliseconds(),
					rep->srmExtendFileLifeTimeResponse->returnStatus->statusCode);
		}catch(storm::MonitorNotEnabledException *exc)
		{
			srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
		}
	}

	return ret_val;
}

int ns1__srmGetTransferProtocols(struct soap* soap, struct ns1__srmGetTransferProtocolsRequest *req,
		struct ns1__srmGetTransferProtocolsResponse_ *rep) {

	char *func = "ns1__srmGetTransferProtocols()";
	srmLogRequest("Get transfer protocols",get_ip(soap).c_str(),storm::Credentials(soap).getDN().c_str());
	boost::posix_time::ptime start_time = boost::posix_time::microsec_clock::local_time();

	if(storm::Authorization::checkBlacklist(soap))
	{
		srmlogit(STORM_LOG_INFO, func, "The user is blacklisted\n");
		rep->srmGetTransferProtocolsResponse = storm::soap_calloc<struct ns1__srmGetTransferProtocolsResponse>(soap);
		if(rep->srmGetTransferProtocolsResponse != NULL)
		{
			rep->srmGetTransferProtocolsResponse->returnStatus = storm::soap_calloc<struct ns1__TReturnStatus>(soap);
			if (rep->srmGetTransferProtocolsResponse->returnStatus != NULL)
			{
				rep->srmGetTransferProtocolsResponse->returnStatus->explanation =
						"User not authorized";
				rep->srmGetTransferProtocolsResponse->returnStatus->statusCode =
						SRM_USCOREAUTHORIZATION_USCOREFAILURE;
				boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
				boost::posix_time::time_duration et = (end_time - start_time);
				try
				{
					storm::Monitoring::getInstance()->getMonitor(
							storm::SRM_GET_TRANSFER_PROTOCOLS_MONITOR_NAME)->registerFailure(et.total_milliseconds());
				}catch(storm::MonitorNotEnabledException *exc)
				{
					srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
				}
				return (SOAP_OK);
			} else {
				srmlogit(STORM_LOG_ERROR, func, "Error in response allocation\n");
				boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
				boost::posix_time::time_duration et = (end_time - start_time);
				try
				{
					storm::Monitoring::getInstance()->getMonitor(storm::SRM_GET_TRANSFER_PROTOCOLS_MONITOR_NAME)->registerError(et.total_milliseconds());
				}catch(storm::MonitorNotEnabledException *exc)
				{
					srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
				}
				return SOAP_EOM;
			}
		}
		else
		{
			srmlogit(STORM_LOG_ERROR, func, "Error in response allocation\n");
			boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
			boost::posix_time::time_duration et = (end_time - start_time);
			try
			{
				storm::Monitoring::getInstance()->getMonitor(storm::SRM_GET_TRANSFER_PROTOCOLS_MONITOR_NAME)->registerError(et.total_milliseconds());
			}catch(storm::MonitorNotEnabledException *exc)
			{
				srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
			}
			return SOAP_EOM;
		}
	}
	else
	{
		srmlogit(STORM_LOG_DEBUG, func, "The user is not blacklisted\n");
	}

	int ret_val = ns1__srmGetTransferProtocols_impl(soap, req, rep);

	boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();

	boost::posix_time::time_duration et = (end_time - start_time);

	if (ret_val != SOAP_OK)
	{
		try
		{
			storm::Monitoring::getInstance()->getMonitor(storm::SRM_GET_TRANSFER_PROTOCOLS_MONITOR_NAME)->registerFailure(et.total_milliseconds());
		}catch(storm::MonitorNotEnabledException *exc)
		{
			srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
		}
	}
	else
	{
		try
		{
			((storm::InstrumentedMonitor*) storm::Monitoring::getInstance()->getMonitor(
					storm::SRM_GET_TRANSFER_PROTOCOLS_MONITOR_NAME))->registerCompleted(
					et.total_milliseconds(),
					rep->srmGetTransferProtocolsResponse->returnStatus->statusCode);
		}catch(storm::MonitorNotEnabledException *exc)
		{
			srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
		}
	}

	return ret_val;
}

int ns1__srmPing(struct soap* soap, struct ns1__srmPingRequest *req,
		struct ns1__srmPingResponse_ *rep) {

	char *func = "ns1__srmPing()";
	srmLogRequest("Ping",get_ip(soap).c_str(),storm::Credentials(soap).getDN().c_str());
	boost::posix_time::ptime start_time = boost::posix_time::microsec_clock::local_time();
	if(storm::Authorization::checkBlacklist(soap))
	{
		srmlogit(STORM_LOG_INFO, func, "The user is blacklisted\n");
		rep->srmPingResponse = storm::soap_calloc<struct ns1__srmPingResponse>(soap);
		if(rep->srmPingResponse != NULL)
		{
			rep->srmPingResponse->versionInfo =
					"User not authorized";
			boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
			boost::posix_time::time_duration et = (end_time - start_time);
			try
			{
				storm::Monitoring::getInstance()->getMonitor(
						storm::SRM_PING_MONITOR_NAME)->registerFailure(et.total_milliseconds());
			}catch(storm::MonitorNotEnabledException *exc)
			{
				srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
			}
			return (SOAP_OK);
		}
		else
		{
			srmlogit(STORM_LOG_ERROR, func, "Error in response allocation\n");
			boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();
			boost::posix_time::time_duration et = (end_time - start_time);
			try
			{
				storm::Monitoring::getInstance()->getMonitor(storm::SRM_PING_MONITOR_NAME)->registerError(et.total_milliseconds());
			}catch(storm::MonitorNotEnabledException *exc)
			{
				srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
			}
			return SOAP_EOM;
		}
	}
	else
	{
		srmlogit(STORM_LOG_DEBUG, func, "The user is not blacklisted\n");
	}
	/* commented out. not available in first Argus integration prototype
	bool black = false;
	storm::Credentials cred(soap);
	try
	{
		storm::Authorization auth((storm::Credentials*)&cred);
		black = auth.isBlacklisted();
	} catch (storm::AuthorizationException& e)
	{
		srmlogit(STORM_LOG_WARNING, func, "Received AuthorizationException during authorization authorization: %s\n" , e.what());
	}
    if(black)
    {
    	srmlogit(STORM_LOG_INFO, func, "The user is blacklisted\n");
    	struct ns1__srmPingResponse *repp;
    	int ret_val = get_ns1__srmPingResponse(soap, repp);
		repp->otherInfo = NULL;
		rep->srmPingResponse = repp;
		repp->versionInfo= "User not authorized to ping the service";
		return(SOAP_OK);
    }
    else
    {
    	srmlogit(STORM_LOG_DEBUG, func, "The user is not blacklisted\n");
    }
	 */

	/*
	cred.getCertChain();
	fprintf(stdout,"getCertChain chiamata\n");
	 */


	/*
	storm::Credentials cred(soap);
	 char* proxy = cred.getProxy();
	 */
	int ret_val = ns1__srmPing_impl(soap, req, rep);

	boost::posix_time::ptime end_time = boost::posix_time::microsec_clock::local_time();

	boost::posix_time::time_duration et = (end_time - start_time);

	if (ret_val != SOAP_OK)
	{
		try
		{
			storm::Monitoring::getInstance()->getMonitor(storm::SRM_PING_MONITOR_NAME)->registerFailure(et.total_milliseconds());
		}catch(storm::MonitorNotEnabledException *exc)
		{
			srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
		}
	}
	else
	{
		try
		{
			((storm::InstrumentedMonitor*) storm::Monitoring::getInstance()->getMonitor(
					storm::SRM_PING_MONITOR_NAME))->registerCompleted(
					et.total_milliseconds(),
					SRM_USCORESUCCESS);
		}catch(storm::MonitorNotEnabledException *exc)
		{
			srmlogit(STORM_LOG_ERROR, func, "Error monitor notification. MonitorNotEnabledException: %s\n" , exc->what());
		}
	}

	return ret_val;
}
