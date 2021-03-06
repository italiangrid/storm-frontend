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
 * FrontendConfiguration.cpp
 *
 *  Created on: Nov 21, 2008
 *      Author: alb
 */

#include <stdexcept>
#include <sys/stat.h>
#include <string>
#include <pwd.h>
#include "FrontendConfiguration.hpp"
#include "srmlogit.h"
#include "xmlrpc_encode.hpp"

using namespace std;
namespace po = boost::program_options;

const string DEFAULT_CONFIGURATION_FILE = string("storm-frontend.conf");
const string DEFAULT_GRIDMAPFILE = string("/etc/grid-security/grid-mapfile");
const string DEFAULT_HOST_CERT_FILE = string("/etc/grid-security/hostcert.pem");
const string DEFAULT_HOST_KEY_FILE = string("/etc/grid-security/hostkey.pem");
const string DEFAULT_LOG_FILE_NAME = string("storm-frontend.log");
const string DEFAULT_MONITORING_FILE_NAME = string("monitoring.log");
const string DEFAULT_WSDL_FILE = "";
const string DEFAULT_XMLRPC_HOST = string("localhost");
const string DEFAULT_XMLRPC_PORT = string("8080");
const string DEFAULT_XMLRPC_PATH = string("/RPC2");
const string DEFAULT_XMLRPC_TOKEN = "unsecure_token";
const string DEFAULT_DEBUG_LEVEL = string("INFO");
const unsigned int DEFAULT_SLEEP_THREADPOOL_MAX_PENDING = 3;
const int DEFAULT_THREADS_NUMBER = 20;
const int DEFAULT_THREADPOOL_MAX_PENDING = 200;

const int DEFAULT_GSOAP_MAX_PENDING = 2000;
const int DEFAULT_GSOAP_SEND_TIMEOUT = 10; // seconds
const int DEFAULT_GSOAP_RECV_TIMEOUT = 10; // seconds

const int DEFAULT_PORT = 8444;
const bool DEFAULT_MONITORING_ENABLED = true;
const int DEFAULT_MONITORING_TIME_INTERVAL = 60;
const bool DEFAULT_MONITORING_DETAILED = false;
const bool DEFAULT_XMLRPC_CHECK_ASCII = true;
const bool DEFAULT_USER_CHECK_BLACKLIST = false;
const string DEFAULT_CA_CERTIFICATES_FOLDER = string("/etc/grid-security/certificates");
const string DEFAULT_ARGUS_RESOURCE_ID = "storm";

const char* EMPTY_DESCRIPTION = "";

const char* ENVVAR_GRIDMAP = "GRIDMAP";
const char* ENVVAR_X509_USER_CERT = "X509_USER_CERT";
const char* ENVVAR_X509_USER_KEY = "X509_USER_KEY";
const char* ENVVAR_X509_CERT_DIR = "X509_CERT_DIR";

const string OPT_HELP = string("h");
const string OPTL_HELP = string("help");
const char* OPT_HELP_DESCRIPTION = "Print this message";

const string OPT_VERSION = string("v");
const string OPTL_VERSION = string("version");
const char* OPT_VERSION_DESCRIPTION = "Print version";

const string OPT_CONFIG_FILE = string("c");
const string OPTL_CONFIG_FILE = string("config-file");
const char* OPT_CONFIG_FILE_DESCRIPTION = "Configuration file";

const string OPT_DEBUG = string("d");
const string OPTL_DEBUG = string("debug-mode");
const char* OPT_DEBUG_DESCRIPTION = "Start in debug-mode: do not exec fork() and stay in foreground";

// Renamed disable with enable and changed checks accordingly
const string OPTL_ENABLE_MAPPING = string("security.enable.mapping");
const char* OPT_ENABLE_MAPPING_DESCRIPTION = "Enable/Disable mapping via gridmafile.";

const string OPTL_ENABLE_VOMSCHECK = string("security.enable.vomscheck");
const char* OPT_ENABLE_VOMSCHECK_DESCRIPTION = "Enable/Disable VOMS credentials check.";

const string OPTL_MONITORING_ENABLED = string("monitoring.enabled");

const string OPTL_MONITORING_FILE_NAME = string("monitoring.filename");
const char* OPT_MONITORING_FILE_NAME_DESCRIPTION = "Use <arg> as monitoring file";

const string OPTL_MONITORING_DETAILED = string("monitoring.detailed");
const char* OPT_MONITORING_DETAILED_DESCRIPTION = "Enable detailed monitoring for each operation";

const string OPTL_MONITORING_TIME_INTERVAL = string("monitoring.timeInterval");
const char* OPT_MONITORING_TIME_INTERVAL_DESCRIPTION = "Number of seconds to print monitoring information.";

const string OPTL_LOG_FILE_NAME = string("log.filename");
const char* OPT_LOG_FILE_NAME_DESCRIPTION = "Use <arg> as log file";

const string OPTL_DEBUG_LEVEL = string("log.debuglevel");
const char* OPT_DEBUG_LEVEL_DESCRIPTION = "Debug level. <arg> can be: ERROR, WARN, INFO, DEBUG, DEBUG2";

const string OPTL_PORT = string("fe.port");
const char* OPT_PORT_DESCRIPTION = "Listen to port <arg>";

const string OPTL_NUM_THREADS = string("fe.threadpool.threads.number");

const string OPTL_MAX_THREADPOOL_PENDING = string("fe.threadpool.maxpending");

const string OPTL_SLEEP_THREADPOOL_MAX_PENDING = string("fe.threadpool.maxpending.sleep");

const string OPTL_MAX_GSOAP_PENDING = string("fe.gsoap.maxpending");

const string OPTL_GSOAP_SEND_TIMEOUT = string("fe.gsoap.send_timeout");
const string OPTL_GSOAP_RECV_TIMEOUT = string("fe.gsoap.recv_timeout");

const string OPTL_XMLRPC_HOST = string("be.xmlrpc.host");
const char* OPT_XMLRPC_HOST_DESCRIPTION = "StoRM XMLRPC server (the same as the StoRM backend server)";

const string OPTL_RECALLTABLE_PORT = string("be.recalltable.port");
const string OPTL_XMLRPC_PORT = string("be.xmlrpc.port");
const char* OPT_XMLRPC_PORT_DESCRIPTION = "Port used by the StoRM XMLRPC server";

const string OPTL_XMLRPC_PATH = string("be.xmlrpc.path");
const char* OPT_XMLRPC_PATH_DESCRIPTION = "Path of the StoRM XMLRPC server service";

const string OPTL_XMLRPC_TOKEN = "be.xmlrpc.token";
const char* OPT_XMLRPC_TOKEN_DESCRIPTION = "The XMLRPC authentication token";

const string OPTL_WSDL_FILE = string("wsdl.file");
const char* OPT_WSDL_FILE_DESCRIPTION = "Path to the WSDL to publish in case of GET request";

const string OPTL_DB_HOST = string("db.host");
const char* OPT_DB_HOST_DESCRIPTION = "Machine hosting the DB";

const string OPTL_DB_USER = string("db.user");
const char* OPT_DB_USER_DESCRIPTION = "Database user";

const string OPTL_DB_USER_PASSWORD = string("db.passwd");
const char* OPT_DB_USER_PASSWORD_DESCRIPTION = "Database user password";

const string OPTL_XMLRPC_CHECK_ASCII = string("be.xmlrpc.check.ascii");
const char* OPT_XMLRPC_CHECK_ASCII_DESCRIPTION = "Flag to check or not strings to be sent via xmlrpc to the BE";

const string OPTL_USER_CHECK_BLACKLIST = string("check.user.blacklisting");
const char* OPT_USER_CHECK_BLACKLIST_DESCRIPTION = "Flag to check or not strings if a user is blacklisted in Argus";

const string OPTL_ARGUS_PEPD_ENDPOINT = string("argus-pepd-endpoint");
const char* OPT_ARGUS_PEPD_ENDPOINT_DESCRIPTION = "Full SERVICE ENDPOINT of the Argus PEP Daemon";

const string OPTL_ARGUS_RESOURCE_ID = string("argus.resource-id");
const char* OPT_ARGUS_RESOURCE_ID_DESCRIPTION = "the resource identifier for StoRM service in Argus policies";


FrontendConfiguration* FrontendConfiguration::instance = NULL;

FrontendConfiguration* FrontendConfiguration::getInstance() {
    if (instance == NULL) {
        instance = new FrontendConfiguration();
    }

    return instance;
}

FrontendConfiguration::FrontendConfiguration() {
    commandLineOptions.add(defineCommandLineOptions());
    configurationFileOptions.add(defineConfigFileOptions());

    struct passwd* pwd = getpwuid(getuid());
    user.assign(pwd->pw_name);

}

void FrontendConfiguration::parseOptions(int argc, char* argv[]) {

    po::variables_map cmdline_vm;

    try {
        store(po::parse_command_line(argc, argv, commandLineOptions), cmdline_vm);
    } catch (exception& e) {
        throw runtime_error("Error: " + string(e.what()));
    }

    notify(cmdline_vm);

    setCommandLineOptions(cmdline_vm);

    if (requestedHelp() || requestedVersion()) {
        return;
    }

    string configuration_file_path(cmdline_vm[OPTL_CONFIG_FILE].as<string> ());

    po::variables_map config_vm;
    ifstream conf_file(configuration_file_path.c_str());

    if (conf_file.fail()) {
        string msg = "Configuration file not found \"" + configuration_file_path + "\"";
        throw runtime_error(msg);
    }

    try {
        store(parse_config_file(conf_file, configurationFileOptions), config_vm);
    } catch (exception& e) {
        throw runtime_error("Error reading configuration file: " + string(e.what()));
    }

    notify(config_vm);

    setConfigurationOptions(config_vm);

}

void FrontendConfiguration::checkFileReadPerm(string fileAbsolutePath) {

    struct stat statInfo;

    int ret = stat(fileAbsolutePath.c_str(), &statInfo);

    if (ret != 0) {
        throw runtime_error("File doesn't exists: " + fileAbsolutePath);
    }

    if (S_ISDIR(statInfo.st_mode)) {
        throw runtime_error("Error looking for a file but a directory were found: " + fileAbsolutePath);
    }

    if (access(fileAbsolutePath.c_str(), R_OK) != 0) {
        throw runtime_error("Read permission needed for file: " + fileAbsolutePath);
    }

}

bool FrontendConfiguration::getMonitoringEnabled() {
    return monitoringEnabled;
}

bool FrontendConfiguration::getMonitoringDetailed(){
	return monitoringDetailed;
}

bool FrontendConfiguration::requestedHelp() {
    return helpRequested;
}

bool FrontendConfiguration::requestedVersion() {
    return versionRequested;
}

bool FrontendConfiguration::requestedDebug() {
    return debugMode;
}

bool FrontendConfiguration::foundConfigurationFile() {
    return configurationFileFound;
}

// Renamed disable with enable and changed checks accordingly
//bool FrontendConfiguration::mappingDisabled() {
//    return disableMapping;
//}

bool FrontendConfiguration::mappingEnabled() {
    return enableMapping;
}

//bool FrontendConfiguration::vomsCheckDisabled() {
//    return disableVOMSCheck;
//}

bool FrontendConfiguration::vomsCheckEnabled() {
    return enableVOMSCheck;
}

int FrontendConfiguration::getDebugLevel() {
    return debugLevel;
}

string FrontendConfiguration::getDebugLevelString() {
    return debugLevelString;
}

int FrontendConfiguration::getNumThreads() {
    return numberOfThreads;
}

int FrontendConfiguration::getThreadpoolMaxPending() {
    return threadpool_max_pending;
}

unsigned int FrontendConfiguration::getThreadpoolMaxpendingSleepTime() {
    return threadpool_max_pending_sleeptime;
}

int FrontendConfiguration::getGsoapMaxPending() {
    return gsoap_max_pending;
}

int FrontendConfiguration::getGsoapSendTimeout() {
    return gsoap_send_timeout;
}

int FrontendConfiguration::getGsoapRecvTimeout() {
    return gsoap_recv_timeout;
}

int FrontendConfiguration::getPort() {
    return port;
}

int FrontendConfiguration::getRecalltablePort() {
    return recalltablePort;
}

int FrontendConfiguration::getMonitoringTimeInterval() {
    return monitoring_time_interval;
}

string FrontendConfiguration::getXMLRPCEndpoint() {
    return xmlrpc_host + ":" + xmlrpc_port + xmlrpc_path;
}

string FrontendConfiguration::getXmlRpcHost() {
    return xmlrpc_host;
}

string FrontendConfiguration::getUser() {
    return user;
}

string FrontendConfiguration::getWSDLFilePath() {
    return wsdl_file;
}

string FrontendConfiguration::getLogFile() {
    return log_file;
}

string FrontendConfiguration::getMonitoringFile() {
    return monitoring_file;
}

string FrontendConfiguration::getDBHost() {
    return dbHost;
}

string FrontendConfiguration::getDBUser() {
    return dbUser;
}

string FrontendConfiguration::getDBUserPassword() {
    return dbUserPassword;
}

void FrontendConfiguration::printHelpMessage() {
    cout << commandLineOptions << endl;
}

string FrontendConfiguration::getConfigurationFile() {
    return configuration_file;
}

string FrontendConfiguration::getGridmapfile() {
    return gridmapfile;
}

string FrontendConfiguration::getHostCertFile() {
    return hostcertfile;
}

string FrontendConfiguration::getHostKeyFile() {
    return hostkeyfile;
}

bool FrontendConfiguration::getXMLRPCCheckAscii() {
    return xmlrpc_check_ascii;
}

bool FrontendConfiguration::getUserCheckBlacklist() {
    return user_check_blacklist;
}

string FrontendConfiguration::getArgusPepdEndpoint() {
    return argus_pepd_endpoint;
}

string FrontendConfiguration::getArgusResourceId() {
    return argus_resource_id;
}

string FrontendConfiguration::getCaCertificatesFolder() {
    return ca_certificates_folder;
}

const string& FrontendConfiguration::getXMLRPCToken(){
	return xmlrpc_token;
}

/******************************** Private methods ****************************/

po::options_description FrontendConfiguration::defineConfigFileOptions() {

    po::options_description configurationFileOptions("Configuration file options");

    configurationFileOptions.add_options()
            (OPTL_NUM_THREADS.c_str(), po::value<int>()->default_value(DEFAULT_THREADS_NUMBER), EMPTY_DESCRIPTION)
            (OPTL_MAX_THREADPOOL_PENDING.c_str(), po::value<int>()->default_value(DEFAULT_THREADPOOL_MAX_PENDING), EMPTY_DESCRIPTION)
            (OPTL_SLEEP_THREADPOOL_MAX_PENDING.c_str(), po::value<unsigned int>()->default_value(DEFAULT_SLEEP_THREADPOOL_MAX_PENDING), EMPTY_DESCRIPTION)
            (OPTL_MAX_GSOAP_PENDING.c_str(), po::value<int>()->default_value(DEFAULT_GSOAP_MAX_PENDING), EMPTY_DESCRIPTION)
            (OPTL_GSOAP_SEND_TIMEOUT.c_str(), po::value<int>()->default_value(DEFAULT_GSOAP_SEND_TIMEOUT), EMPTY_DESCRIPTION)
            (OPTL_GSOAP_RECV_TIMEOUT.c_str(), po::value<int>()->default_value(DEFAULT_GSOAP_RECV_TIMEOUT), EMPTY_DESCRIPTION)
            (OPTL_LOG_FILE_NAME.c_str(), po::value<string>()->default_value(DEFAULT_LOG_FILE_NAME), EMPTY_DESCRIPTION)
            (OPTL_MONITORING_ENABLED.c_str(), po::value<bool>()->default_value(DEFAULT_MONITORING_ENABLED), EMPTY_DESCRIPTION)
            (OPTL_MONITORING_DETAILED.c_str(), po::value<bool>()->default_value(DEFAULT_MONITORING_DETAILED), OPT_MONITORING_DETAILED_DESCRIPTION)
            (OPTL_MONITORING_FILE_NAME.c_str(), po::value<string>()->default_value(DEFAULT_MONITORING_FILE_NAME), EMPTY_DESCRIPTION)
            (OPTL_MONITORING_TIME_INTERVAL.c_str(), po::value<int>()->default_value(DEFAULT_MONITORING_TIME_INTERVAL), EMPTY_DESCRIPTION)
            (OPTL_PORT.c_str(), po::value<int>()->default_value(DEFAULT_PORT), OPT_PORT_DESCRIPTION)
            (OPTL_XMLRPC_HOST.c_str(), po::value<string>()->default_value(DEFAULT_XMLRPC_HOST), OPT_XMLRPC_HOST_DESCRIPTION)
            (OPTL_XMLRPC_PORT.c_str(), po::value<string>()->default_value(DEFAULT_XMLRPC_PORT), OPT_XMLRPC_PORT_DESCRIPTION)
            (OPTL_XMLRPC_PATH.c_str(), po::value<string>()->default_value(DEFAULT_XMLRPC_PATH), OPT_XMLRPC_PATH_DESCRIPTION)
            (OPTL_XMLRPC_TOKEN.c_str(), po::value<string>()->default_value(DEFAULT_XMLRPC_TOKEN), OPT_XMLRPC_TOKEN_DESCRIPTION)
            (OPTL_XMLRPC_CHECK_ASCII.c_str(), po::value<bool>()->default_value(DEFAULT_XMLRPC_CHECK_ASCII), OPT_XMLRPC_CHECK_ASCII_DESCRIPTION)
            (OPTL_RECALLTABLE_PORT.c_str(), po::value<int>()->default_value(9998), EMPTY_DESCRIPTION)
            (OPTL_WSDL_FILE.c_str(), po::value<string>()->default_value(DEFAULT_WSDL_FILE), OPT_WSDL_FILE_DESCRIPTION)
            (OPTL_DEBUG_LEVEL.c_str(), po::value<string>()->default_value(DEFAULT_DEBUG_LEVEL), OPT_DEBUG_LEVEL_DESCRIPTION)
            (OPTL_DB_HOST.c_str(), po::value<string>(), OPT_DB_HOST_DESCRIPTION)
            (OPTL_DB_USER.c_str(), po::value<string>(), OPT_DB_USER_DESCRIPTION)
            (OPTL_DB_USER_PASSWORD.c_str(), po::value<string>(), OPT_DB_USER_PASSWORD_DESCRIPTION)
            (OPTL_ENABLE_MAPPING.c_str(), po::value<bool>()->default_value(false), OPT_ENABLE_MAPPING_DESCRIPTION)
            (OPTL_ENABLE_VOMSCHECK.c_str(), po::value<bool>()->default_value(true), OPT_ENABLE_VOMSCHECK_DESCRIPTION)
            (OPTL_USER_CHECK_BLACKLIST.c_str(), po::value<bool>()->default_value(DEFAULT_USER_CHECK_BLACKLIST), OPT_USER_CHECK_BLACKLIST_DESCRIPTION)
            (OPTL_ARGUS_PEPD_ENDPOINT.c_str(), po::value<string>(), OPT_ARGUS_PEPD_ENDPOINT_DESCRIPTION)
            (OPTL_ARGUS_RESOURCE_ID.c_str(), po::value<string>()->default_value(DEFAULT_ARGUS_RESOURCE_ID), OPT_ARGUS_RESOURCE_ID_DESCRIPTION);

    return configurationFileOptions;
}

po::options_description FrontendConfiguration::defineCommandLineOptions() {
    po::options_description commandLineOptions("Options");
    commandLineOptions.add_options()(string(OPTL_HELP + "," + OPT_HELP).c_str(), OPT_HELP_DESCRIPTION)(
            string(OPTL_VERSION + "," + OPT_VERSION).c_str(), OPT_VERSION_DESCRIPTION)(string(
            OPTL_CONFIG_FILE + "," + OPT_CONFIG_FILE).c_str(), po::value<string>()->default_value(
            DEFAULT_CONFIGURATION_FILE), OPT_CONFIG_FILE_DESCRIPTION)(
            string(OPTL_DEBUG + "," + OPT_DEBUG).c_str(), OPT_DEBUG_DESCRIPTION);

    return commandLineOptions;
}

void FrontendConfiguration::setCommandLineOptions(po::variables_map& vm) {

    helpRequested = false;
    versionRequested = false;
    debugMode = false;
    configurationFileFound = false;

    if (vm.count(OPTL_HELP))
        helpRequested = true;

    if (vm.count(OPTL_VERSION))
        versionRequested = true;

    if (vm.count(OPTL_DEBUG))
        debugMode = true;

    if (vm.count(OPTL_CONFIG_FILE)) {
        configurationFileFound = true;
        configuration_file = vm[OPTL_CONFIG_FILE].as<string> ();
    }

}

void FrontendConfiguration::setConfigurationOptions(po::variables_map& vm) {

    numberOfThreads = vm[OPTL_NUM_THREADS].as<int> ();
    threadpool_max_pending = vm[OPTL_MAX_THREADPOOL_PENDING].as<int> ();
    threadpool_max_pending_sleeptime = vm[OPTL_SLEEP_THREADPOOL_MAX_PENDING].as<unsigned int> ();

    gsoap_max_pending = vm[OPTL_MAX_GSOAP_PENDING].as<int> ();

    gsoap_send_timeout = vm[OPTL_GSOAP_SEND_TIMEOUT].as<int> ();
    gsoap_recv_timeout = vm[OPTL_GSOAP_RECV_TIMEOUT].as<int> ();

    debugLevelString = vm[OPTL_DEBUG_LEVEL].as<string> ();
    debugLevel = decodeDebugLevelOption(debugLevelString);

    if (vm.count(OPTL_PORT))
        port = vm[OPTL_PORT].as<int> ();

    if (vm.count(OPTL_XMLRPC_HOST))
        xmlrpc_host = vm[OPTL_XMLRPC_HOST].as<string> ();

    if (vm.count(OPTL_XMLRPC_PORT))
        xmlrpc_port = vm[OPTL_XMLRPC_PORT].as<string> ();

    if (vm.count(OPTL_XMLRPC_PATH))
        xmlrpc_path = vm[OPTL_XMLRPC_PATH].as<string> ();

    if (vm.count(OPTL_XMLRPC_TOKEN))
    	xmlrpc_token = vm[OPTL_XMLRPC_TOKEN].as<string> ();

    if (vm.count(OPTL_XMLRPC_CHECK_ASCII))
            xmlrpc_check_ascii = vm[OPTL_XMLRPC_CHECK_ASCII].as<bool> ();

    if (vm.count(OPTL_WSDL_FILE))
        wsdl_file = vm[OPTL_WSDL_FILE].as<string> ();

    if (vm.count(OPTL_DB_HOST))
        dbHost = vm[OPTL_DB_HOST].as<string> ();

    if (vm.count(OPTL_DB_USER))
        dbUser = vm[OPTL_DB_USER].as<string> ();

    if (vm.count(OPTL_DB_USER_PASSWORD))
        dbUserPassword = vm[OPTL_DB_USER_PASSWORD].as<string> ();

    if (vm.count(OPTL_USER_CHECK_BLACKLIST))
    	user_check_blacklist = vm[OPTL_USER_CHECK_BLACKLIST].as<bool> ();

    if (vm.count(OPTL_ARGUS_PEPD_ENDPOINT))
    	argus_pepd_endpoint = vm[OPTL_ARGUS_PEPD_ENDPOINT].as<string> ();

    if (vm.count(OPTL_ARGUS_RESOURCE_ID))
    	argus_resource_id = vm[OPTL_ARGUS_RESOURCE_ID].as<string> ();

    log_file = vm[OPTL_LOG_FILE_NAME].as<string> ();
    monitoring_file = vm[OPTL_MONITORING_FILE_NAME].as<string> ();
    monitoringEnabled = vm[OPTL_MONITORING_ENABLED].as<bool> ();
    monitoringDetailed = vm[OPTL_MONITORING_DETAILED].as<bool> ();

    // Renamed disable with enable and changed checks accordingly
    //    disableMapping = vm[OPTL_DISABLE_MAPPING].as<bool> ();
    //    disableVOMSCheck = vm[OPTL_DISABLE_VOMSCHECK].as<bool> ();
    enableMapping = vm[OPTL_ENABLE_MAPPING].as<bool> ();
    enableVOMSCheck = vm[OPTL_ENABLE_VOMSCHECK].as<bool> ();
    monitoring_time_interval = vm[OPTL_MONITORING_TIME_INTERVAL].as<int> ();
    recalltablePort = vm[OPTL_RECALLTABLE_PORT].as<int> ();

    gridmapfile = getFromEnvironment(ENVVAR_GRIDMAP, DEFAULT_GRIDMAPFILE);
    hostcertfile = getFromEnvironment(ENVVAR_X509_USER_CERT, DEFAULT_HOST_CERT_FILE);
    hostkeyfile = getFromEnvironment(ENVVAR_X509_USER_KEY, DEFAULT_HOST_KEY_FILE);
    ca_certificates_folder = getFromEnvironment(ENVVAR_X509_CERT_DIR, DEFAULT_CA_CERTIFICATES_FOLDER);
}

int FrontendConfiguration::decodeDebugLevelOption(string& debugLevelString) {

    int debugLevel = STORM_LOG_ERROR;

    if (debugLevelString == "NONE") {
        debugLevel = STORM_LOG_NONE;
    } else if (debugLevelString == "ERROR") {
        debugLevel = STORM_LOG_ERROR;
    } else if (debugLevelString == "WARN") {
        debugLevel = STORM_LOG_WARNING;
    } else if (debugLevelString == "INFO") {
        debugLevel = STORM_LOG_INFO;
    } else if (debugLevelString == "DEBUG") {
        debugLevel = STORM_LOG_DEBUG;
    } else if (debugLevelString == "DEBUG2") {
        debugLevel = STORM_LOG_DEBUG2;
    } else {
        throw runtime_error("Error: unknown debug level value " + debugLevelString
                + ". Allowed values are: ERROR, WARN, INFO, DEBUG, DEBUG2.");
    }

    return debugLevel;
}

void FrontendConfiguration::checkCreateDir(string dirAbsolutePath) {

    if (dirAbsolutePath.empty())
        return;

    struct stat statInfo;

    int ret = stat(dirAbsolutePath.c_str(), &statInfo);

    if (ret != 0) {
        checkCreateDir(getParentPath(dirAbsolutePath));

        if (mkdir(dirAbsolutePath.c_str(), S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH) != 0) {
            throw runtime_error("Cannot create directory \"" + dirAbsolutePath + "\". "
                "Currently running as user \"" + user + "\".");
        }
        stat(dirAbsolutePath.c_str(), &statInfo);
    }

    if (!S_ISDIR(statInfo.st_mode)) {
        throw runtime_error("Not a directory: " + dirAbsolutePath);
    }

    if (access(dirAbsolutePath.c_str(), W_OK | X_OK) != 0) {
        if (!((statInfo.st_mode & S_IWOTH) && (statInfo.st_mode & S_IXOTH))) {
            throw runtime_error("Permission denied (\"wx\" is needed) for directory \"" + dirAbsolutePath
                    + "\". "
                        "Currently running as user \"" + user + "\".");
        }
    }

}

string FrontendConfiguration::getFromEnvironment(const char* envVar, const string& defaultValue) {
    char* envVal = getenv(envVar);

    if (envVal == NULL) {
        return defaultValue;
    }
    return string(envVal);
}

string FrontendConfiguration::getFilename(string path) {

    size_t pos = path.find_last_of('/');

    if (pos == string::npos)
        return path;

    return path.substr(pos + 1);

}

string FrontendConfiguration::getParentPath(string path) {

    string parentPath("");

    size_t pos = path.find_last_of('/');

    if (pos == string::npos)
        return parentPath;

    parentPath = path.substr(0, pos);

    return parentPath;

}
