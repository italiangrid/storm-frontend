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

/*
 * FrontendConfiguration.h
 *
 *  Created on: Nov 21, 2008
 *      Author: alb
 */

#ifndef FRONTENDOPTIONS_HPP_
#define FRONTENDOPTIONS_HPP_

#include "boost/program_options.hpp"
namespace po = boost::program_options;

#include <iostream>
#include <fstream>
#include <iterator>
#include <string>

//#include "xmlrpc_encode.h"

using namespace std;

// Costants
static const string DEFAULT_CONFIGURATION_FILE = string("storm-frontend.conf");
static const string DEFAULT_GRIDMAPFILE = string("/etc/grid-security/grid-mapfile");
static const string DEFAULT_HOST_CERT_FILE = string("/etc/grid-security/hostcert.pem");
static const string DEFAULT_HOST_KEY_FILE = string("/etc/grid-security/hostkey.pem");
static const string DEFAULT_LOG_FILE_NAME = string("storm-frontend.log");
static const string DEFAULT_MONITORING_FILE_NAME = string("monitoring.log");
static const string DEFAULT_WSDL_FILE = "";
static const string DEFAULT_XMLRPC_HOST = string("localhost");
static const string DEFAULT_XMLRPC_PORT = string("8080");
static const string DEFAULT_XMLRPC_PATH = string("/RPC2");
static const string DEFAULT_DEBUG_LEVEL = string("INFO");
static const unsigned int DEFAULT_SLEEP_THREADPOOL_MAX_PENDING = 3;
static const int DEFAULT_THREADS_NUMBER = 20;
static const int DEFAULT_THREADPOOL_MAX_PENDING = 200;
static const int DEFAULT_GSOAP_MAX_PENDING = 2000;
static const int DEFAULT_PORT = 8444;
static const bool DEFAULT_MONITORING_ENABLED = true;
static const int DEFAULT_MONITORING_TIME_INTERVAL = 60;
static const bool DEFAULT_MONITORING_DETAILED = false;
static const bool DEFAULT_XMLRPC_CHECK_ASCII = true;
static const bool DEFAULT_USER_CHECK_BLACKLIST = false;
static const string DEFAULT_CA_CERTIFICATES_FOLDER = string("/etc/grid-security/certificates");
static const string DEFAULT_ARGUS_RESOURCE_ID = "storm";

static const char* EMPTY_DESCRIPTION = "";

static const char* ENVVAR_GRIDMAP = "GRIDMAP";
static const char* ENVVAR_X509_USER_CERT = "X509_USER_CERT";
static const char* ENVVAR_X509_USER_KEY = "X509_USER_KEY";
static const char* ENVVAR_X509_CERT_DIR = "X509_CERT_DIR";

static const string OPT_HELP = string("h");
static const string OPTL_HELP = string("help");
static const char* OPT_HELP_DESCRIPTION = "Print this message";

static const string OPT_VERSION = string("v");
static const string OPTL_VERSION = string("version");
static const char* OPT_VERSION_DESCRIPTION = "Print version";

static const string OPT_CONFIG_FILE = string("c");
static const string OPTL_CONFIG_FILE = string("config-file");
static const char* OPT_CONFIG_FILE_DESCRIPTION = "Configuration file";

static const string OPT_DEBUG = string("d");
static const string OPTL_DEBUG = string("debug-mode");
static const char* OPT_DEBUG_DESCRIPTION = "Start in debug-mode: do not exec fork() and stay in foreground";

// Renamed disable with enable and changed checks accordingly
static const string OPTL_ENABLE_MAPPING = string("security.enable.mapping");
static const char* OPT_ENABLE_MAPPING_DESCRIPTION = "Enable/Disable mapping via gridmafile.";

static const string OPTL_ENABLE_VOMSCHECK = string("security.enable.vomscheck");
static const char* OPT_ENABLE_VOMSCHECK_DESCRIPTION = "Enable/Disable VOMS credentials check.";

static const string OPTL_MONITORING_ENABLED = string("monitoring.enabled");

static const string OPTL_MONITORING_FILE_NAME = string("monitoring.filename");
static const char* OPT_MONITORING_FILE_NAME_DESCRIPTION = "Use <arg> as monitoring file";

static const string OPTL_MONITORING_DETAILED = string("monitoring.detailed");
static const char* OPT_MONITORING_DETAILED_DESCRIPTION = "Enable detailed monitoring for each operation";

static const string OPTL_MONITORING_TIME_INTERVAL = string("monitoring.timeInterval");
static const char* OPT_MONITORING_TIME_INTERVAL_DESCRIPTION = "Number of seconds to print monitoring information.";

static const string OPTL_LOG_FILE_NAME = string("log.filename");
static const char* OPT_LOG_FILE_NAME_DESCRIPTION = "Use <arg> as log file";

static const string OPTL_DEBUG_LEVEL = string("log.debuglevel");
static const char* OPT_DEBUG_LEVEL_DESCRIPTION = "Debug level. <arg> can be: ERROR, WARN, INFO, DEBUG, DEBUG2";

static const string OPTL_PORT = string("fe.port");
static const char* OPT_PORT_DESCRIPTION = "Listen to port <arg>";

static const string OPTL_NUM_THREADS = string("fe.threadpool.threads.number");

static const string OPTL_MAX_THREADPOOL_PENDING = string("fe.threadpool.maxpending");

static const string OPTL_SLEEP_THREADPOOL_MAX_PENDING = string("fe.threadpool.maxpending.sleep");

static const string OPTL_MAX_GSOAP_PENDING = string("fe.gsoap.maxpending");

static const string OPTL_PROXY_DIR = string("proxy.dir");
static const char* OPT_PROXY_DIR_DESCRIPTION = "Directory used to store proxies delegated by the client";

static const string OPTL_PROXY_USER = string("proxy.user");
static const char* OPT_PROXY_USER_DESCRIPTION = "Save the proxy certificate using <arg>'s uid and gid";

static const string OPTL_XMLRPC_HOST = string("be.xmlrpc.host");
static const char* OPT_XMLRPC_HOST_DESCRIPTION = "StoRM XMLRPC server (the same as the StoRM backend server)";

static const string OPTL_RECALLTABLE_PORT = string("be.recalltable.port");
static const string OPTL_XMLRPC_PORT = string("be.xmlrpc.port");
static const char* OPT_XMLRPC_PORT_DESCRIPTION = "Port used by the StoRM XMLRPC server";

static const string OPTL_XMLRPC_PATH = string("be.xmlrpc.path");
static const char* OPT_XMLRPC_PATH_DESCRIPTION = "Path of the StoRM XMLRPC server service";

static const string OPTL_WSDL_FILE = string("wsdl.file");
static const char* OPT_WSDL_FILE_DESCRIPTION = "Path to the WSDL to publish in case of GET request";

static const string OPTL_DB_HOST = string("db.host");
static const char* OPT_DB_HOST_DESCRIPTION = "Machine hosting the DB";

static const string OPTL_DB_USER = string("db.user");
static const char* OPT_DB_USER_DESCRIPTION = "Database user";

static const string OPTL_DB_USER_PASSWORD = string("db.passwd");
static const char* OPT_DB_USER_PASSWORD_DESCRIPTION = "Database user password";

static const string OPTL_XMLRPC_CHECK_ASCII = string("be.xmlrpc.check.ascii");
static const char* OPT_XMLRPC_CHECK_ASCII_DESCRIPTION = "Flag to check or not strings to be sent via xmlrpc to the BE";

static const string OPTL_USER_CHECK_BLACKLIST = string("check.user.blacklisting");
static const char* OPT_USER_CHECK_BLACKLIST_DESCRIPTION = "Flag to check or not strings if a user is blacklisted in Argus";

static const string OPTL_ARGUS_PEPD_ENDPOINT = string("argus-pepd-endpoint");
static const char* OPT_ARGUS_PEPD_ENDPOINT_DESCRIPTION = "Full SERVICE ENDPOINT of the Argus PEP Daemon";

static const string OPTL_ARGUS_RESOURCE_ID = string("argus.resource-id");
static const char* OPT_ARGUS_RESOURCE_ID_DESCRIPTION = "the resource identifier for StoRM service in Argus policies";

// A helper function to simplify printing options stuff
template<class T>
ostream& operator<<(ostream& os, const vector<T>& v)
{
    copy(v.begin(), v.end(), ostream_iterator<T>(cout, " "));
    return os;
}


// This class is a singleton. The creation is not thread-safe because it is
// instantiated from main().
class FrontendConfiguration {
public:
    static FrontendConfiguration* getInstance();

    void parseOptions(int argc, char* argv[]);
    void printHelpMessage();
    void checkConfigurationData();
    void checkFileReadPerm(string fileAbsolutePath);

    bool requestedHelp();
    bool requestedVersion();
    bool requestedDebug();
    bool foundConfigurationFile();
    //bool mappingDisabled();
    bool mappingEnabled();
    bool getMonitoringEnabled();
    bool getMonitoringDetailed();
    //bool vomsCheckDisabled();
    bool vomsCheckEnabled();
    int getDebugLevel();
    int getNumThreads();
    int getThreadpoolMaxPending();
    int getGsoapMaxPending();
    int getPort();
    int getRecalltablePort();
    int getMonitoringTimeInterval();
    unsigned int getThreadpoolMaxpendingSleepTime();
    string getDebugLevelString();
    string getProxyDir();
    string getProxyUser();
    string getXMLRPCEndpoint();
    string getXmlRpcHost();
    string getUser();
    string getWSDLFilePath();
    string getMonitoringFile();
    string getLogFile();
    string getDBHost();
    string getDBUser();
    string getDBUserPassword();
    string getConfigurationFile();
    string getGridmapfile();
    string getHostCertFile();
    string getHostKeyFile();
    bool getXMLRPCCheckAscii();
    bool getUserCheckBlacklist();
    string getArgusPepdEndpoint();
    string getArgusResourceId();
    string getCaCertificatesFolder();

private:
    FrontendConfiguration();
    virtual ~FrontendConfiguration();
    po::options_description defineConfigFileOptions();
    po::options_description defineCommandLineOptions();
    void setCommandLineOptions(po::variables_map& vm);
    void setConfigurationOptions(po::variables_map& vm);
    int decodeDebugLevelOption(string& debugLevel);
    void checkCreateDir(string dirAbsolutePath);
    string getFromEnvironment(const char* envVar, const string& defaultValue);
    string getFilename(string path);
    string getParentPath(string path);

    static FrontendConfiguration* instance;

    po::options_description configurationFileOptions;
    po::options_description commandLineOptions;

    bool helpRequested;
    bool versionRequested;
    bool debugMode;
    bool configurationFileFound;
    //bool disableMapping;
    //bool disableVOMSCheck;
    bool enableMapping;
    bool enableVOMSCheck;
    bool monitoringEnabled;
    bool monitoringDetailed;
    int numberOfThreads;
    int threadpool_max_pending;
    unsigned int threadpool_max_pending_sleeptime;
    int gsoap_max_pending;
    int debugLevel;
    int port;
    int recalltablePort;
    int monitoring_time_interval;
    string monitoring_file;
    string log_file;
    string proxy_dir;
    string proxy_user;
    string user;
    string wsdl_file;
    string debugLevelString;
    string xmlrpc_host;
    string xmlrpc_port;
    string xmlrpc_path;
    string dbHost;
    string dbUser;
    string dbUserPassword;
    string configuration_file;
    string gridmapfile;
    string hostcertfile;
    string hostkeyfile;
    bool xmlrpc_check_ascii;
    bool user_check_blacklist;
    string argus_pepd_endpoint;
    string argus_resource_id;
    string ca_certificates_folder;

};

int getInstanceXMLRPCCheckAscii();

#endif /* FRONTENDOPTIONS_HPP_ */
