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

/**
 * \file srmv2.c This file contains the main functions to parse
 * command line, configuration file and start the frontend.
 */

#include "srmSoapBinding.nsmap"

extern "C"{
#include "cgsi_plugin.h"
}

#include "srm_server.h"
#include "srmv2H.h"
#include "storm_functions.h"
#include "storm_limits.h"
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
#include "FrontendConfiguration.hpp"
#include "ThreadPool.hpp"
#include "boost/bind.hpp"
#include "DBConnectionPool.hpp"
#include <signal.h>
#include "frontend_version.h"
#include <curl/curl.h>
#include "ProtocolChecker.hpp"

#include "Monitoring.hpp"
#include "InstrumentedMonitorBuilder.hpp"

#define NAME "StoRM SRM v2.2"

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

// SIGSTOP handler. Used to stop the daemon.
void sigint_handler(int sig) {
    srmlogit(STORM_LOG_NONE, "SIGINT_handler", "Caught SIGINT: stopping frontend...\n");
    stay_running = false;
}

static int http_get(struct soap *soap) {

    int fd = open(wsdl_file, O_RDONLY | O_NONBLOCK);
    char buf[10240]; /* 10k */
    int nr = 0;

    srmlogit(STORM_LOG_DEBUG, "http_get", "Receives GET request\n");
    soap_response(soap, SOAP_HTML); // HTTP response header with text/html
    if (-1 == fd) {
        srmlogit(STORM_LOG_DEBUG2, "http_get", "Error opening file %s. (%s)\n", wsdl_file, strerror(errno));
        srmlogit(STORM_LOG_ERROR, "http_get", "Error opening file %s. (%s)\n", wsdl_file, strerror(errno));
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

int setProxyUserGlobalVariables(string proxy_user) {

    struct passwd *pwd;

    string proxy_user_name;
    if (proxy_user.empty()) {

        // Get current user name
        pwd = getpwuid(getuid());
        proxy_user.assign(pwd->pw_name);

    } else {

        // Get information on the requested user
        pwd = getpwnam(proxy_user.c_str());
        if (NULL == pwd) { // error
            return CONFERR;
        }

        proxy_uid = pwd->pw_uid;
        proxy_gid = pwd->pw_gid;
    }

    return 0;
}

/*************************** Function: process_request() *********************/
void *
process_request(struct soap* tsoap) {

	srmlogit(STORM_LOG_DEBUG, "process_request", "-- START process_request\n");
    srm_srv_thread_info * thread_info = mysql_connection_pool->getConnection(boost::this_thread::get_id());
    tsoap->user = thread_info;

    tsoap->recv_timeout = SOAP_RECV_TIMEOUT;
    tsoap->send_timeout = SOAP_SEND_TIMEOUT;

	srmlogit(STORM_LOG_DEBUG2, "process_request", "-- Start soap_serve\n");

    if (soap_serve((struct soap*)tsoap) && (tsoap->error != SOAP_EOF || (tsoap->errnum != 0 && !(tsoap->omode & SOAP_IO_KEEPALIVE))))
    {
//      srmlogit(STORM_LOG_ERROR, "process_request", "Thread %d completed with failure %d\n", (int)tsoap->user, tsoap->error);
      soap_print_fault(tsoap, stderr);
    }
    srmlogit(STORM_LOG_DEBUG2, "process_request", "End soap_serve\n");

	srmlogit(STORM_LOG_DEBUG2, "process_request", "Start soap_destroy\n");
    soap_destroy((struct soap*)tsoap); // cleanup class instances (C++)
	srmlogit(STORM_LOG_DEBUG2, "process_request", "End soap_destroy\n");

	srmlogit(STORM_LOG_DEBUG2, "process_request", "Start soap_end\n");
    soap_end((struct soap*)tsoap); // dealloc data and clean up
	srmlogit(STORM_LOG_DEBUG2, "process_request", "End soap_end\n");

	srmlogit(STORM_LOG_DEBUG2, "process_request", "Start soap_free\n");
	soap_free((struct soap*)tsoap); // detach and free thread's copy of soap environment
	srmlogit(STORM_LOG_DEBUG2, "process_request", "End soap_free\n");

	srmlogit(STORM_LOG_DEBUG, "process_request", "-- END process_request\n");
    return NULL;
}

/*****************************************************************************/
/************************************ Main ***********************************/
/*****************************************************************************/

int loadConfiguration(int argc, char** argv)
{
	char *func = "loadConfiguration()";

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

	if (setProxyUserGlobalVariables(configuration->getProxyUser()) != 0) {
		fprintf(stderr, "Error: request invalid user \"%s\" for proxy dir.\n", configuration->getProxyUser().c_str());
		return -1;
	}

	try {
		configuration->checkConfigurationData();
	} catch (exception& e) {
		cout << e.what() << endl << endl;
		return -1;
	}

	return 0;
}

void fillGlobalVars()
{
	FrontendConfiguration* configuration = FrontendConfiguration::getInstance();
	db_user = strdup(configuration->getDBUser().c_str());
	db_pwd = strdup(configuration->getDBUserPassword().c_str());
	db_srvr = strdup(configuration->getDBHost().c_str());
    wsdl_file = strdup(configuration->getWSDLFilePath().c_str());
    xmlrpc_endpoint = strdup(configuration->getXMLRPCEndpoint().c_str());
}

void initLogging()
{
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

void logConfiguration()
{
	char *func = "printConfiguration()";
	FrontendConfiguration* configuration = FrontendConfiguration::getInstance();

    srmlogit(STORM_LOG_NONE, func, "Starting StoRM frontend as user: %s\n", configuration->getUser().c_str());
    srmlogit(STORM_LOG_NONE, func, "---------------------- Configuration ------------------\n");
    srmlogit(STORM_LOG_NONE, func, "%s=%d\n", OPTL_PORT.c_str(), configuration->getPort());
    srmlogit(STORM_LOG_NONE, func, "%s=%d\n", OPTL_NUM_THREADS.c_str(), configuration->getNumThreads());
    srmlogit(STORM_LOG_NONE, func, "%s=%d\n", OPTL_MAX_THREADPOOL_PENDING.c_str(), configuration->getThreadpoolMaxPending());
    srmlogit(STORM_LOG_NONE, func, "%s=%u\n", OPTL_SLEEP_THREADPOOL_MAX_PENDING.c_str(), configuration->getThreadpoolMaxpendingSleepTime());
    srmlogit(STORM_LOG_NONE, func, "%s=%d\n", OPTL_MAX_GSOAP_PENDING.c_str(), configuration->getGsoapMaxPending());
    srmlogit(STORM_LOG_NONE, func, "%s=%s\n", OPTL_LOG_FILE_NAME.c_str(), configuration->getLogFile().c_str());
    srmlogit(STORM_LOG_NONE, func, "%s=%s\n", OPTL_MONITORING_ENABLED.c_str(), (configuration->getMonitoringEnabled() ? "true" : "false"));
    srmlogit(STORM_LOG_NONE, func, "%s=%s\n", OPTL_MONITORING_FILE_NAME.c_str(), configuration->getMonitoringFile().c_str());
    srmlogit(STORM_LOG_NONE, func, "%s=%s\n", OPTL_MONITORING_DETAILED.c_str(), (configuration->getMonitoringDetailed() ? "true" : "false"));
    srmlogit(STORM_LOG_NONE, func, "%s=%u\n", OPTL_MONITORING_TIME_INTERVAL.c_str(), configuration->getMonitoringTimeInterval());
    srmlogit(STORM_LOG_NONE, func, "%s=%s\n", OPTL_ARGUS_PEP_AUTH_PROTOCOL.c_str(), configuration->getArgusPepProtocol().c_str());
    srmlogit(STORM_LOG_NONE, func, "%s=%s\n", OPTL_ARGUS_PEP_HOSTNAME.c_str(), configuration->getArgusPepHostname().c_str());
    srmlogit(STORM_LOG_NONE, func, "%s=%s\n", OPTL_ARGUS_PEP_AUTH_PORT.c_str(), configuration->getArgusPepAuthzPort().c_str());
    srmlogit(STORM_LOG_NONE, func, "%s=%s\n", OPTL_ARGUS_PEP_AUTH_SERVICE.c_str(), configuration->getArgusPepAuthzService().c_str());
    srmlogit(STORM_LOG_NONE, func, "xmlrpc endpoint=%s\n", configuration->getXMLRPCEndpoint().c_str());
    srmlogit(STORM_LOG_NONE, func, "%s=%d\n", OPTL_RECALLTABLE_PORT.c_str(), configuration->getRecalltablePort());
    srmlogit(STORM_LOG_NONE, func, "%s=%s\n", OPTL_DEBUG_LEVEL.c_str(), configuration->getDebugLevelString().c_str());
    srmlogit(STORM_LOG_NONE, func, "%s=%s\n", OPTL_PROXY_DIR.c_str(), configuration->getProxyDir().c_str());
    srmlogit(STORM_LOG_NONE, func, "%s=%s\n", OPTL_PROXY_USER.c_str(), configuration->getProxyUser().c_str());
    srmlogit(STORM_LOG_NONE, func, "%s=%s\n", OPTL_DB_HOST.c_str(), configuration->getDBHost().c_str());
    srmlogit(STORM_LOG_NONE, func, "%s=%s\n", OPTL_DB_USER.c_str(), configuration->getDBUser().c_str());
    srmlogit(STORM_LOG_NONE, func, "%s=%s\n", OPTL_WSDL_FILE.c_str(), configuration->getWSDLFilePath().c_str());
    srmlogit(STORM_LOG_NONE, func, "%s=%s\n", OPTL_ENABLE_MAPPING.c_str(), (configuration->mappingEnabled() ? "true" : "false"));
    srmlogit(STORM_LOG_NONE, func, "%s=%s\n", OPTL_ENABLE_VOMSCHECK.c_str(), (configuration->vomsCheckEnabled() ? "true" : "false"));
    srmlogit(STORM_LOG_NONE, func, "-------------------------------------------------------\n");
}

int performSanityChecks()
{
	char *func = "performSanityChecks()";
	FrontendConfiguration* configuration = FrontendConfiguration::getInstance();
    try {
        configuration->checkFileReadPerm(configuration->getGridmapfile());
    } catch (exception& e) {
        srmlogit(STORM_LOG_WARNING, func, "%s\n", e.what());
    }

    try {
        configuration->checkFileReadPerm(configuration->getHostCertFile());
    } catch (exception& e) {
    	srmlogit(STORM_LOG_WARNING, func, "%s\n", e.what());
        return 1;
    }

    try {
        configuration->checkFileReadPerm(configuration->getHostKeyFile());
    } catch (exception& e) {
    	srmlogit(STORM_LOG_WARNING, func, "%s\n", e.what());
        return 1;
    }

    /**** Get list of supported protocols ****/
    if ((nb_supported_protocols = get_supported_protocols(&supported_protocols)) < 0) {
    	srmlogit(STORM_LOG_ERROR, func, "Error in get_supported_protocols(): unable to retrieve "
            "supported protocols from the DB.");
        return 1;
    }
    return 0;
}

soap* initSoap()
{
	char *func = "initSoap()";
	FrontendConfiguration* configuration = FrontendConfiguration::getInstance();
	/**** gSOAP and CGSI_gSOAP plugin initializaion ****/
	struct soap *soap_data = soap_new2(SOAP_IO_KEEPALIVE, SOAP_IO_KEEPALIVE);

	soap_data->max_keep_alive = SOAP_MAX_KEEPALIVE;
	// non-blocking soap_accept()... exit from soap_accept() every 5 secs if no requests arrive
	soap_data->accept_timeout = 5;
	// supporting HTTP GET in order to reply the wsdl
	soap_data->fget = http_get;
	//    soap_data->bind_flags |= SO_REUSEADDR;

	int flags = CGSI_OPT_DELEG_FLAG;
	// Renamed disable with enable and changed checks accordingly

	if (!configuration->mappingEnabled()) {
		flags |= CGSI_OPT_DISABLE_MAPPING;
		srmlogit(STORM_LOG_NONE, func, "Mapping disabled\n");
	}

	if (!configuration->vomsCheckEnabled()) {
		flags |= CGSI_OPT_DISABLE_VOMS_CHECK;
		srmlogit(STORM_LOG_NONE, func, "VOMS check disabled\n");
	}

	soap_register_plugin_arg(soap_data, server_cgsi_plugin, &flags);

	int m;
	m = soap_bind(soap_data, NULL, configuration->getPort(), configuration->getGsoapMaxPending());

	if (!soap_valid_socket(m)) {
		soap_print_fault(soap_data, stderr);
		soap_done(soap_data);
		free(soap_data);
		return NULL;
	}
	return soap_data;
}

void setupXmlrpc()
{
    /**** Start up XML-RPC client library. ****/
    xmlrpc_env env;
    xmlrpc_env_init(&env);
    xmlrpc_client_init2(&env, XMLRPC_CLIENT_NO_FLAGS, NAME, VERSION, NULL, 0);
    xmlrpc_env_clean(&env);
}

storm::Monitoring* initMonitoring()
{
	FrontendConfiguration* configuration = FrontendConfiguration::getInstance();
    storm::Monitoring* monitoring = storm::Monitoring::getInstance();
    if(configuration->getMonitoringEnabled())
    {
        // Init monitoring

        monitoring->setTimeInterval(configuration->getMonitoringTimeInterval());
        monitoring->addMonitor(storm::InstrumentedMonitorBuilder::buildAbortFiles());
        monitoring->addMonitor(storm::InstrumentedMonitorBuilder::buildAbortRequest());
        monitoring->addMonitor(storm::InstrumentedMonitorBuilder::buildBringOnline());
        monitoring->addMonitor(storm::InstrumentedMonitorBuilder::buildChangeSpaceForFiles());
        monitoring->addMonitor(storm::InstrumentedMonitorBuilder::buildCheckPermission());
        monitoring->addMonitor(storm::InstrumentedMonitorBuilder::buildCopy());
        monitoring->addMonitor(storm::InstrumentedMonitorBuilder::buildExtendFileLifeTimeInSpace());
        monitoring->addMonitor(storm::InstrumentedMonitorBuilder::buildExtendFileLifeTime());
        monitoring->addMonitor(storm::InstrumentedMonitorBuilder::buildGetPermission());
        monitoring->addMonitor(storm::InstrumentedMonitorBuilder::buildGetRequestSummary());
		monitoring->addMonitor(storm::InstrumentedMonitorBuilder::buildGetRequestTokens());
		monitoring->addMonitor(storm::InstrumentedMonitorBuilder::buildGetSpaceMetaData());
		monitoring->addMonitor(storm::InstrumentedMonitorBuilder::buildGetSpaceTokens());
		monitoring->addMonitor(storm::InstrumentedMonitorBuilder::buildGetTransferProtocols());
		monitoring->addMonitor(storm::InstrumentedMonitorBuilder::buildLs());
		monitoring->addMonitor(storm::InstrumentedMonitorBuilder::buildMkdir());
		monitoring->addMonitor(storm::InstrumentedMonitorBuilder::buildMv());
		monitoring->addMonitor(storm::InstrumentedMonitorBuilder::buildPing());
		monitoring->addMonitor(storm::InstrumentedMonitorBuilder::buildPrepareToGet());
		monitoring->addMonitor(storm::InstrumentedMonitorBuilder::buildPrepareToPut());
		monitoring->addMonitor(storm::InstrumentedMonitorBuilder::buildPurgeFromSpace());
		monitoring->addMonitor(storm::InstrumentedMonitorBuilder::buildPutDone());
		monitoring->addMonitor(storm::InstrumentedMonitorBuilder::buildReleaseFiles());
		monitoring->addMonitor(storm::InstrumentedMonitorBuilder::buildReleaseSpace());
		monitoring->addMonitor(storm::InstrumentedMonitorBuilder::buildReserveSpace());
		monitoring->addMonitor(storm::InstrumentedMonitorBuilder::buildResumeRequest());
		monitoring->addMonitor(storm::InstrumentedMonitorBuilder::buildRmdir());
		monitoring->addMonitor(storm::InstrumentedMonitorBuilder::buildRm());
		monitoring->addMonitor(storm::InstrumentedMonitorBuilder::buildSetPermission());
		monitoring->addMonitor(storm::InstrumentedMonitorBuilder::buildStatusOfBringOnlineRequest());
		monitoring->addMonitor(storm::InstrumentedMonitorBuilder::buildStatusOfChangeSpaceForFilesRequest());
		monitoring->addMonitor(storm::InstrumentedMonitorBuilder::buildStatusOfCopyRequest());
		monitoring->addMonitor(storm::InstrumentedMonitorBuilder::buildStatusOfGetRequest());
		monitoring->addMonitor(storm::InstrumentedMonitorBuilder::buildStatusOfLsRequest());
		monitoring->addMonitor(storm::InstrumentedMonitorBuilder::buildStatusOfPutRequest());
		monitoring->addMonitor(storm::InstrumentedMonitorBuilder::buildStatusOfReserveSpaceRequest());
		monitoring->addMonitor(storm::InstrumentedMonitorBuilder::buildStatusOfUpdateSpaceRequest());
		monitoring->addMonitor(storm::InstrumentedMonitorBuilder::buildSuspendRequest());
		monitoring->addMonitor(storm::InstrumentedMonitorBuilder::buildUpdateSpace());
		monitoring->setDetailed(configuration->getMonitoringDetailed());
		monitoring->start();
    }
    return monitoring;
}

int acceptRequest(soap* soap_data)
{
	char *func = "acceptRequest()";
	int fd = soap_accept(soap_data);
	while (!soap_valid_socket(fd)) {

		 if (!stay_running) {
			 // received a SIGINT
			 break;
		 }

		 if (soap_data->errnum) {
			 srmlogit(STORM_LOG_INFO, func, "Error in soap_socket. Error %ld\n", soap_data->errnum);
			 soap_print_fault(soap_data, stderr);
		 }

		 fd = soap_accept(soap_data);

	}
	return fd;
}

int run(soap* soap_data)
{
	char *func = "run()";
	/******************************* main loop ******************************/
	struct soap *tsoap;
	int exit_code = 0;
	SOAP_SOCKET sock;

	int fd;
	while (stay_running) {
		fd = acceptRequest(soap_data);
		if (!stay_running) {
			// received a SIGINT
			break;
		}

		while ((tsoap = soap_copy(soap_data)) == NULL) {
			srmlogit(STORM_LOG_ERROR, func,
					"Error in soap_copy(), probably system busy retry in 3 seconds. Error message: %s\n",
					strerror(ENOMEM));
			srmlogit(STORM_LOG_DEBUG, func, "AUDIT - CP - Active tasks: %ld\n", storm::ThreadPool::getInstance()->get_active());
			srmlogit(STORM_LOG_DEBUG, func, "AUDIT - CP - Pending tasks: %ld\n", storm::ThreadPool::getInstance()->get_pending());

			// A memory allocation error here is probably due to system busy so... going to sleep for a while.
			sleep(3);
		}

		try {
			srmlogit(STORM_LOG_DEBUG2, func, "Going to bind to a thread\n");
			storm::ThreadPool::getInstance()->schedule(boost::bind(process_request, tsoap));
			srmlogit(STORM_LOG_DEBUG2, func, "Bound request to a thread\n");

		} catch (exception& e) {
			srmlogit(STORM_LOG_ERROR, func, "Cannot schedule task, request is lost: %s\n", e.what());

			soap_destroy(tsoap);
			soap_end(tsoap);
			soap_free(tsoap);
		}

		srmlogit(STORM_LOG_DEBUG, func, "AUDIT - Active tasks: %ld\n", storm::ThreadPool::getInstance()->get_active());
		srmlogit(STORM_LOG_DEBUG, func, "AUDIT - Pending tasks: %ld\n", storm::ThreadPool::getInstance()->get_pending());
	}
	return exit_code;
}

int main(int argc, char** argv) {
    char *func = "main";

    int ret = loadConfiguration(argc, argv);
	if(ret != 0)
	{
		if(ret > 0)
		{
			return 0;
		}
		else
		{
			return 1;
		}
	}
	FrontendConfiguration* configuration = FrontendConfiguration::getInstance();

	fillGlobalVars();
	initLogging();
	logConfiguration();

	if(performSanityChecks() != 0)
	{
		return 1;
	}
    srmlogit(STORM_LOG_DEBUG, func, "Initializing the ProtocolChecker instance\n");
    ProtocolChecker::getInstance()->ProtocolChecker::init(&supported_protocols, nb_supported_protocols);
    srmlogit(STORM_LOG_DEBUG, func, "ProtocolChecker initialization completed\n");
    ProtocolChecker::getInstance()->ProtocolChecker::printProtocols();

    if (!configuration->requestedDebug()) { // fork and leave the daemon in background
        int pid = fork();
        if (pid > 0) {
            return 0;
        }
    }
    try {
		storm::ThreadPool::buildInstance(configuration->getNumThreads());
	} catch (boost::thread_resource_error e) {
		cout << "Cannot create all the requested threads, not enough resources.\n";
		return SYERR;
	}

    soap* soap_data = initSoap();
    if(soap_data == NULL)
    {
    	return SYERR;
    }

    curl_global_init(CURL_GLOBAL_NOTHING);

    setupXmlrpc();
    // the size of mysql_connection pool and thread pool MUST be the same
    mysql_connection_pool = new DBConnectionPool(configuration->getNumThreads());
	storm::Monitoring* monitoring = initMonitoring();
    // SIGINT (kill -2) to stop the frontend
    signal(SIGINT, sigint_handler);

    srmlogit(STORM_LOG_NONE, func, "StoRM frontend successfully started...\n");

    int exit_code = run(soap_data);

    srmlogit(STORM_LOG_NONE, func, "Active tasks: %ld\n", storm::ThreadPool::getInstance()->get_active());
    srmlogit(STORM_LOG_NONE, func, "Pending tasks: %ld\n", storm::ThreadPool::getInstance()->get_pending());
    srmlogit(STORM_LOG_NONE, func, "Waiting for active and pending tasks to finish...\n");

    soap_destroy(soap_data);
    soap_end(soap_data);
    soap_done(soap_data);
    free(soap_data);

    delete mysql_connection_pool;
    delete monitoring;
    delete storm::ThreadPool::getInstance();

    curl_global_cleanup();

    srmlogit(STORM_LOG_NONE, func, "StoRM Frontend shutdown complete.\n");

    return (exit_code);
}

