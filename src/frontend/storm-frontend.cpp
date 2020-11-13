/* Copyright (c) Istituto Nazionale di Fisica Nucleare (INFN). 2006-2013.
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

#include "srmSoapBinding.nsmap"

#include "cgsi_plugin.h"
#include "config.h"

#include "srm_server.h"
#include "srmv2H.h"
#include "storm_util.h"
#include "srmlogit.h"

#include <signal.h>
#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <pwd.h>
#include <unistd.h>
#include <xmlrpc-c/base.h>
#include <xmlrpc-c/client.h>
#include <exception>
#include <boost/date_time/posix_time/posix_time_types.hpp>
namespace dt = boost::posix_time;
#include "FrontendConfiguration.hpp"
#include "ThreadPool.hpp"
#include <boost/bind.hpp>
#include "DBConnectionPool.hpp"
#include <signal.h>
#include "frontend_version.h"
#include <curl/curl.h>
#include "ProtocolChecker.hpp"

#include "Monitoring.hpp"
#include "InstrumentedMonitorBuilder.hpp"

#include <globus_thread.h>
#include "storm_exception.hpp"
#include "request_id.hpp"
#include "get_socket_info.hpp"

#define NAME "StoRM SRM v2.2"

using namespace std;

static bool stay_running = true;

char *db_pwd;
char *db_srvr;
char *db_user;

char* xmlrpc_endpoint;
char* wsdl_file;
char** supported_protocols;

int nb_supported_protocols;

DBConnectionPool* mysql_connection_pool;

uid_t proxy_uid = 0;
gid_t proxy_gid = 0;

static const char* STORM_GLOBUS_THREADING_MODEL = "pthread";

static int gsoap_send_timeout = 10;
static int gsoap_recv_timeout = 10;

void sigint_handler(int /* sig */) {
	srmlogit(STORM_LOG_INFO, __func__,
			"Signal SIGINT received: shutting down...\n");
	stay_running = false;
}

static int http_get(struct soap *soap) {

	int fd = open(wsdl_file, O_RDONLY | O_NONBLOCK);
	char buf[10240]; /* 10k */
	int nr = 0;

	srmlogit(STORM_LOG_DEBUG, "http_get", "Receives GET request\n");
	soap_response(soap, SOAP_HTML); // HTTP response header with text/html
	if (-1 == fd) {
		srmlogit(STORM_LOG_DEBUG2, "http_get", "Error opening file %s. (%s)\n",
				wsdl_file, strerror(errno));
		srmlogit(STORM_LOG_ERROR, "http_get", "Error opening file %s. (%s)\n",
				wsdl_file, strerror(errno));
		soap_send(soap, "<html><body>Error in GET method</body><html>\n");
	} else {
		memset(buf, 0, 10240); /* uhmm... R&K: this should be already zero-ed... */
		while ((nr = read(fd, buf, 10239)) > 0) {
			if (nr < 10239)
				buf[nr] = '\0';
			soap_send(soap, buf);
		}
		close(fd);
	}
	soap_end_send(soap);
	return SOAP_OK;
}

void *
process_request(struct soap* tsoap) {

	dt::ptime t0 = dt::microsec_clock::local_time();
	storm::set_request_id();

	srmlogit(STORM_LOG_DEBUG, "process_request", "-- START process_request\n");
	srm_srv_thread_info * thread_info = mysql_connection_pool->getConnection();

	// explicitly manage the request id here since threadinfo is not
	// destroyed for each request but kept in the database connection pool
	thread_info->request_id = storm::get_request_id();

	tsoap->user = thread_info;

	tsoap->recv_timeout = gsoap_recv_timeout;
	tsoap->send_timeout = gsoap_send_timeout;

	std::string peer_ip = get_ip(tsoap);

	srmlogit(STORM_LOG_INFO, __func__, "Connection from %s\n", peer_ip.c_str());
	srmlogit(STORM_LOG_DEBUG2, "process_request", "-- Start soap_serve\n");

	if (soap_serve(tsoap)
			&& (tsoap->error != SOAP_EOF
					|| (tsoap->errnum != 0
							&& !(tsoap->omode & SOAP_IO_KEEPALIVE)))) {
		soap_print_fault(tsoap, stderr);
	}

	srmlogit(STORM_LOG_DEBUG2, "process_request", "End soap_serve\n");

	srmlogit(STORM_LOG_DEBUG2, "process_request", "Start soap_destroy\n");
	soap_destroy(tsoap); // cleanup class instances (C++)
	srmlogit(STORM_LOG_DEBUG2, "process_request", "End soap_destroy\n");

	srmlogit(STORM_LOG_DEBUG2, "process_request", "Start soap_end\n");
	soap_end(tsoap); // dealloc data and clean up
	srmlogit(STORM_LOG_DEBUG2, "process_request", "End soap_end\n");

	srmlogit(STORM_LOG_DEBUG2, "process_request", "Start soap_free\n");
	soap_free(tsoap); // detach and free thread's copy of soap environment
	srmlogit(STORM_LOG_DEBUG2, "process_request", "End soap_free\n");

	dt::ptime t1 = dt::microsec_clock::local_time();

	srmlogit(STORM_LOG_DEBUG, "process_request", "-- END process_request [took %d us]\n", (t1 - t0).total_microseconds());
	
	thread_info->request_id = NULL;
	storm::clear_request_id();

	return NULL;
}

int loadConfiguration(int argc, char** argv) {
	FrontendConfiguration* configuration = FrontendConfiguration::getInstance();
	try {
		configuration->parseOptions(argc, argv);
	} catch (exception& e) {
		cout << e.what() << endl << endl;
		return -1;
	}

	if (configuration->requestedHelp()) {
		configuration->printHelpMessage();
		return 1;
	}

	if (configuration->requestedVersion()) {
		printf("Frontend version: %s\n", frontend_version);
		return 1;
	}

	return 0;
}

void fillGlobalVars() {
	FrontendConfiguration* configuration = FrontendConfiguration::getInstance();
	db_user = strdup(configuration->getDBUser().c_str());
	db_pwd = strdup(configuration->getDBUserPassword().c_str());
	db_srvr = strdup(configuration->getDBHost().c_str());
	wsdl_file = strdup(configuration->getWSDLFilePath().c_str());
	xmlrpc_endpoint = strdup(configuration->getXMLRPCEndpoint().c_str());
	gsoap_send_timeout = configuration->getGsoapSendTimeout();
	gsoap_recv_timeout = configuration->getGsoapRecvTimeout();
}

void initLogging() {
	FrontendConfiguration* configuration = FrontendConfiguration::getInstance();
	if (configuration->requestedDebug()) {
		srmlogit_init(NULL, NULL, configuration->getMonitoringEnabled()); // i.e. log to stderr
		configuration->getLogFile().assign("stderr"); // Just because it's printed in the logs, see below.
		configuration->getMonitoringFile().assign("stderr"); // Just because it's printed in the logs, see below.
	} else {
		srmlogit_init(configuration->getLogFile().c_str(),
				configuration->getMonitoringFile().c_str(),
				configuration->getMonitoringEnabled());
	}
	srmlogit_set_debuglevel(configuration->getDebugLevel());
}

void logConfiguration() {
	FrontendConfiguration* configuration = FrontendConfiguration::getInstance();

	srmlogit(STORM_LOG_NONE, __func__, "Starting StoRM frontend as user: %s\n",
			configuration->getUser().c_str());
	srmlogit(STORM_LOG_NONE, __func__,
			"---------------------- Configuration ------------------\n");
	srmlogit(STORM_LOG_NONE, __func__, "%s=%d\n", OPTL_PORT.c_str(),
			configuration->getPort());
	srmlogit(STORM_LOG_NONE, __func__, "%s=%d\n", OPTL_NUM_THREADS.c_str(),
			configuration->getNumThreads());
	srmlogit(STORM_LOG_NONE, __func__, "%s=%d\n",
			OPTL_MAX_THREADPOOL_PENDING.c_str(),
			configuration->getThreadpoolMaxPending());
	srmlogit(STORM_LOG_NONE, __func__, "%s=%u\n",
			OPTL_SLEEP_THREADPOOL_MAX_PENDING.c_str(),
			configuration->getThreadpoolMaxpendingSleepTime());
	srmlogit(STORM_LOG_NONE, __func__, "%s=%d\n",
			OPTL_MAX_GSOAP_PENDING.c_str(),
			configuration->getGsoapMaxPending());

	srmlogit(STORM_LOG_NONE, __func__, "%s=%d\n",
			OPTL_GSOAP_SEND_TIMEOUT.c_str(),
			configuration->getGsoapSendTimeout());

	srmlogit(STORM_LOG_NONE, __func__, "%s=%d\n",
			OPTL_GSOAP_RECV_TIMEOUT.c_str(),
			configuration->getGsoapRecvTimeout());

	srmlogit(STORM_LOG_NONE, __func__, "%s=%s\n", OPTL_LOG_FILE_NAME.c_str(),
			configuration->getLogFile().c_str());
	srmlogit(STORM_LOG_NONE, __func__, "%s=%s\n",
			OPTL_MONITORING_ENABLED.c_str(),
			(configuration->getMonitoringEnabled() ? "true" : "false"));
	srmlogit(STORM_LOG_NONE, __func__, "%s=%s\n",
			OPTL_MONITORING_FILE_NAME.c_str(),
			configuration->getMonitoringFile().c_str());
	srmlogit(STORM_LOG_NONE, __func__, "%s=%s\n",
			OPTL_MONITORING_DETAILED.c_str(),
			(configuration->getMonitoringDetailed() ? "true" : "false"));
	srmlogit(STORM_LOG_NONE, __func__, "%s=%u\n",
			OPTL_MONITORING_TIME_INTERVAL.c_str(),
			configuration->getMonitoringTimeInterval());
	srmlogit(STORM_LOG_NONE, __func__, "%s=%s\n",
			OPTL_ARGUS_PEPD_ENDPOINT.c_str(),
			configuration->getArgusPepdEndpoint().c_str());
	srmlogit(STORM_LOG_NONE, __func__, "%s=%s\n",
			OPTL_ARGUS_RESOURCE_ID.c_str(),
			configuration->getArgusResourceId().c_str());
	srmlogit(STORM_LOG_NONE, __func__, "xmlrpc endpoint=%s\n",
			configuration->getXMLRPCEndpoint().c_str());
	srmlogit(STORM_LOG_NONE, __func__, "%s=%d\n", OPTL_RECALLTABLE_PORT.c_str(),
			configuration->getRecalltablePort());
	srmlogit(STORM_LOG_NONE, __func__, "%s=%s\n", OPTL_DEBUG_LEVEL.c_str(),
			configuration->getDebugLevelString().c_str());
	srmlogit(STORM_LOG_NONE, __func__, "%s=%s\n", OPTL_DB_HOST.c_str(),
			configuration->getDBHost().c_str());
	srmlogit(STORM_LOG_NONE, __func__, "%s=%s\n", OPTL_DB_USER.c_str(),
			configuration->getDBUser().c_str());
	srmlogit(STORM_LOG_NONE, __func__, "%s=%s\n", OPTL_WSDL_FILE.c_str(),
			configuration->getWSDLFilePath().c_str());
	srmlogit(STORM_LOG_NONE, __func__, "%s=%s\n", OPTL_ENABLE_MAPPING.c_str(),
			(configuration->mappingEnabled() ? "true" : "false"));
	srmlogit(STORM_LOG_NONE, __func__, "%s=%s\n", OPTL_ENABLE_VOMSCHECK.c_str(),
			(configuration->vomsCheckEnabled() ? "true" : "false"));
	srmlogit(STORM_LOG_NONE, __func__,
			"-------------------------------------------------------\n");
}

int performSanityChecks() {
	FrontendConfiguration* configuration = FrontendConfiguration::getInstance();
	try {
		configuration->checkFileReadPerm(configuration->getGridmapfile());
	} catch (exception& e) {
		srmlogit(STORM_LOG_WARNING, __func__, "%s\n", e.what());
	}

	try {
		configuration->checkFileReadPerm(configuration->getHostCertFile());
	} catch (exception& e) {
		srmlogit(STORM_LOG_WARNING, __func__, "%s\n", e.what());
		return 1;
	}

	try {
		configuration->checkFileReadPerm(configuration->getHostKeyFile());
	} catch (exception& e) {
		srmlogit(STORM_LOG_WARNING, __func__, "%s\n", e.what());
		return 1;
	}

	/**** Get list of supported protocols ****/
	if (ProtocolChecker::getInstance()->getProtocols().empty()) {
		srmlogit(STORM_LOG_ERROR, __func__, "No supported protocols\n");
		return 1;
	}

	if (!mysql_thread_safe()) {
		srmlogit(STORM_LOG_ERROR, __func__, "MySQL is not thread-safe\n");
		return 1;
	}

	return 0;
}

soap* initSoap() {
	FrontendConfiguration* configuration = FrontendConfiguration::getInstance();
	/**** gSOAP and CGSI_gSOAP plugin initializaion ****/
	struct soap *soap_data = soap_new2(SOAP_IO_KEEPALIVE, SOAP_IO_KEEPALIVE);

	int const SOAP_MAX_KEEPALIVE = 100;
	soap_data->max_keep_alive = SOAP_MAX_KEEPALIVE;
	// non-blocking soap_accept()... exit from soap_accept() every 5 secs if no requests arrive
	soap_data->accept_timeout = 5;
	// supporting HTTP GET in order to reply the wsdl
	soap_data->fget = http_get;
	soap_data->bind_flags = SO_REUSEADDR;

	int flags = CGSI_OPT_DELEG_FLAG;
	// Renamed disable with enable and changed checks accordingly

	if (!configuration->mappingEnabled()) {
		flags |= CGSI_OPT_DISABLE_MAPPING;
		srmlogit(STORM_LOG_NONE, __func__, "Mapping disabled\n");
	}

	if (!configuration->vomsCheckEnabled()) {
		flags |= CGSI_OPT_DISABLE_VOMS_CHECK;
		srmlogit(STORM_LOG_NONE, __func__, "VOMS check disabled\n");
	}

	soap_register_plugin_arg(soap_data, server_cgsi_plugin, &flags);

	int m;
	m = soap_bind(soap_data, NULL, configuration->getPort(),
			configuration->getGsoapMaxPending());

	if (!soap_valid_socket(m)) {
		soap_print_fault(soap_data, stderr);
		soap_done(soap_data);
		free(soap_data);
		return NULL;
	}
	return soap_data;
}

storm::Monitoring* initMonitoring() {
	FrontendConfiguration* configuration = FrontendConfiguration::getInstance();
	storm::Monitoring* monitoring = storm::Monitoring::getInstance();
	if (configuration->getMonitoringEnabled()) {
		// Init monitoring

		monitoring->setTimeInterval(configuration->getMonitoringTimeInterval());
		monitoring->addMonitor(
				storm::InstrumentedMonitorBuilder::buildAbortFiles());
		monitoring->addMonitor(
				storm::InstrumentedMonitorBuilder::buildAbortRequest());
		monitoring->addMonitor(
				storm::InstrumentedMonitorBuilder::buildBringOnline());
		monitoring->addMonitor(
				storm::InstrumentedMonitorBuilder::buildChangeSpaceForFiles());
		monitoring->addMonitor(
				storm::InstrumentedMonitorBuilder::buildCheckPermission());
		monitoring->addMonitor(storm::InstrumentedMonitorBuilder::buildCopy());
		monitoring->addMonitor(
				storm::InstrumentedMonitorBuilder::buildExtendFileLifeTimeInSpace());
		monitoring->addMonitor(
				storm::InstrumentedMonitorBuilder::buildExtendFileLifeTime());
		monitoring->addMonitor(
				storm::InstrumentedMonitorBuilder::buildGetPermission());
		monitoring->addMonitor(
				storm::InstrumentedMonitorBuilder::buildGetRequestSummary());
		monitoring->addMonitor(
				storm::InstrumentedMonitorBuilder::buildGetRequestTokens());
		monitoring->addMonitor(
				storm::InstrumentedMonitorBuilder::buildGetSpaceMetaData());
		monitoring->addMonitor(
				storm::InstrumentedMonitorBuilder::buildGetSpaceTokens());
		monitoring->addMonitor(
				storm::InstrumentedMonitorBuilder::buildGetTransferProtocols());
		monitoring->addMonitor(storm::InstrumentedMonitorBuilder::buildLs());
		monitoring->addMonitor(storm::InstrumentedMonitorBuilder::buildMkdir());
		monitoring->addMonitor(storm::InstrumentedMonitorBuilder::buildMv());
		monitoring->addMonitor(storm::InstrumentedMonitorBuilder::buildPing());
		monitoring->addMonitor(
				storm::InstrumentedMonitorBuilder::buildPrepareToGet());
		monitoring->addMonitor(
				storm::InstrumentedMonitorBuilder::buildPrepareToPut());
		monitoring->addMonitor(
				storm::InstrumentedMonitorBuilder::buildPurgeFromSpace());
		monitoring->addMonitor(
				storm::InstrumentedMonitorBuilder::buildPutDone());
		monitoring->addMonitor(
				storm::InstrumentedMonitorBuilder::buildReleaseFiles());
		monitoring->addMonitor(
				storm::InstrumentedMonitorBuilder::buildReleaseSpace());
		monitoring->addMonitor(
				storm::InstrumentedMonitorBuilder::buildReserveSpace());
		monitoring->addMonitor(
				storm::InstrumentedMonitorBuilder::buildResumeRequest());
		monitoring->addMonitor(storm::InstrumentedMonitorBuilder::buildRmdir());
		monitoring->addMonitor(storm::InstrumentedMonitorBuilder::buildRm());
		monitoring->addMonitor(
				storm::InstrumentedMonitorBuilder::buildSetPermission());
		monitoring->addMonitor(
				storm::InstrumentedMonitorBuilder::buildStatusOfBringOnlineRequest());
		monitoring->addMonitor(
				storm::InstrumentedMonitorBuilder::buildStatusOfChangeSpaceForFilesRequest());
		monitoring->addMonitor(
				storm::InstrumentedMonitorBuilder::buildStatusOfCopyRequest());
		monitoring->addMonitor(
				storm::InstrumentedMonitorBuilder::buildStatusOfGetRequest());
		monitoring->addMonitor(
				storm::InstrumentedMonitorBuilder::buildStatusOfLsRequest());
		monitoring->addMonitor(
				storm::InstrumentedMonitorBuilder::buildStatusOfPutRequest());
		monitoring->addMonitor(
				storm::InstrumentedMonitorBuilder::buildStatusOfReserveSpaceRequest());
		monitoring->addMonitor(
				storm::InstrumentedMonitorBuilder::buildStatusOfUpdateSpaceRequest());
		monitoring->addMonitor(
				storm::InstrumentedMonitorBuilder::buildSuspendRequest());
		monitoring->addMonitor(
				storm::InstrumentedMonitorBuilder::buildUpdateSpace());
		monitoring->setDetailed(configuration->getMonitoringDetailed());
		monitoring->start();
	}
	return monitoring;
}

int acceptRequest(soap* soap_data) {
	int fd = soap_accept(soap_data);
	while (!soap_valid_socket(fd)) {

		if (!stay_running) {
			// received a SIGINT
			break;
		}

		if (soap_data->errnum) {
			srmlogit(STORM_LOG_INFO, __func__,
					"Error in soap_socket. Error %ld\n", soap_data->errnum);
			soap_print_fault(soap_data, stderr);
		}

		fd = soap_accept(soap_data);

	}
	return fd;
}

int run(soap* soap_data) {
	/******************************* main loop ******************************/
	struct soap *tsoap;
	int exit_code = 0;

	while (stay_running) {
		acceptRequest(soap_data);
		if (!stay_running) {
			// received a SIGINT
			break;
		}

		while ((tsoap = soap_copy(soap_data)) == NULL) {
			srmlogit(STORM_LOG_ERROR, __func__,
					"Error in soap_copy(), probably system busy retry in 3 seconds. Error message: %s\n",
					strerror(ENOMEM));
			srmlogit(STORM_LOG_DEBUG, __func__,
					"AUDIT - CP - Active tasks: %ld\n",
					storm::ThreadPool::getInstance()->get_active());
			srmlogit(STORM_LOG_DEBUG, __func__,
					"AUDIT - CP - Pending tasks: %ld\n",
					storm::ThreadPool::getInstance()->get_pending());

			// A memory allocation error here is probably due to system busy so... going to sleep for a while.
			sleep(3);
		}

		try {
			srmlogit(STORM_LOG_DEBUG2, __func__, "Going to bind to a thread\n");
			storm::ThreadPool::getInstance()->schedule(
					boost::bind(process_request, tsoap));
			srmlogit(STORM_LOG_DEBUG2, __func__, "Bound request to a thread\n");

		} catch (exception& e) {
			srmlogit(STORM_LOG_ERROR, __func__,
					"Cannot schedule task, request is lost: %s\n", e.what());

			soap_destroy(tsoap);
			soap_end(tsoap);
			soap_free(tsoap);
		}

		srmlogit(STORM_LOG_DEBUG, __func__, "AUDIT - Active tasks: %ld\n",
				storm::ThreadPool::getInstance()->get_active());
		srmlogit(STORM_LOG_DEBUG, __func__, "AUDIT - Pending tasks: %ld\n",
				storm::ThreadPool::getInstance()->get_pending());
	}
	return exit_code;
}

void init_globus_threading() {
	if (globus_thread_set_model(STORM_GLOBUS_THREADING_MODEL) != GLOBUS_SUCCESS) {
		throw storm::storm_error("Error initalizing Globus threading model!");
	}
}

int main(int argc, char** argv) {

	mysql_library_init(argc, argv, NULL);

	int ret = loadConfiguration(argc, argv);
	if (ret != 0) {
		if (ret > 0) {
			return 0;
		} else {
			return 1;
		}
	}
	FrontendConfiguration* configuration = FrontendConfiguration::getInstance();

	fillGlobalVars();
	initLogging();
	logConfiguration();

	srmlogit(STORM_LOG_DEBUG, __func__,
			"Initializing the ProtocolChecker instance\n");
	std::vector<std::string> supported_protocols = get_supported_protocols(
		configuration->getDBHost(),
		configuration->getDBUser(),
		configuration->getDBUserPassword()
	);
	ProtocolChecker::getInstance()->init(supported_protocols);
	srmlogit(STORM_LOG_DEBUG, __func__,
			"ProtocolChecker initialization completed\n");
	ProtocolChecker::getInstance()->printProtocols();

	if (performSanityChecks() != 0) {
		return 1;
	}

	if (!configuration->requestedDebug()) { // fork and leave the daemon in background
		int pid = fork();
		if (pid > 0) {
			return 0;
		}
	}

	curl_global_init(CURL_GLOBAL_ALL);

	xmlrpc_env env;
	xmlrpc_env_init(&env);
    xmlrpc_client_setup_global_const(&env);
	if (env.fault_occurred) {
		srmlogit(STORM_LOG_DEBUG, __func__, env.fault_string);
		xmlrpc_env_clean(&env);
		return SYERR;
	}
	xmlrpc_env_clean(&env);


	try {
		init_globus_threading();
	} catch (storm::storm_error& e) {
		std::cerr << e.what() << std::endl;
		exit(1);
	}

	soap* soap_data = initSoap();
	if (soap_data == NULL) {
		return SYERR;
	}

	try {
		storm::ThreadPool::buildInstance(configuration->getNumThreads(), configuration->getThreadpoolMaxPending());
	} catch (boost::thread_resource_error& e) {
		cout
				<< "Cannot create all the requested threads, not enough resources.\n";
		return SYERR;
	}

	// the size of mysql_connection pool and thread pool MUST be the same
	mysql_connection_pool = new DBConnectionPool(
		configuration->getDBHost(),
		configuration->getDBUser(),
		configuration->getDBUserPassword()
	);
	storm::Monitoring* monitoring = initMonitoring();
	// SIGINT (kill -2) to stop the frontend
	signal(SIGINT, sigint_handler);

	srmlogit(STORM_LOG_NONE, __func__,
			"StoRM frontend successfully started...\n");

	int exit_code = run(soap_data);

	srmlogit(STORM_LOG_NONE, __func__, "Active tasks: %ld\n",
			storm::ThreadPool::getInstance()->get_active());
	srmlogit(STORM_LOG_NONE, __func__, "Pending tasks: %ld\n",
			storm::ThreadPool::getInstance()->get_pending());
	srmlogit(STORM_LOG_NONE, __func__,
			"Waiting for active and pending tasks to finish...\n");

	soap_destroy(soap_data);
	soap_end(soap_data);
	soap_done(soap_data);
	free(soap_data);

	delete mysql_connection_pool;
	delete monitoring;
	delete storm::ThreadPool::getInstance();

	curl_global_cleanup();

	mysql_library_end();

	srmlogit(STORM_LOG_NONE, __func__, "StoRM Frontend shutdown complete.\n");

	return exit_code;
}

