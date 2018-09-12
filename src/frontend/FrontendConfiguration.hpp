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

#include <iostream>
#include <fstream>
#include <iterator>
#include <string>

extern const std::string DEFAULT_CONFIGURATION_FILE;
extern const std::string DEFAULT_GRIDMAPFILE;
extern const std::string DEFAULT_HOST_CERT_FILE;
extern const std::string DEFAULT_HOST_KEY_FILE;
extern const std::string DEFAULT_LOG_FILE_NAME;
extern const std::string DEFAULT_MONITORING_FILE_NAME;
extern const std::string DEFAULT_WSDL_FILE;
extern const std::string DEFAULT_XMLRPC_HOST;
extern const std::string DEFAULT_XMLRPC_PORT;
extern const std::string DEFAULT_XMLRPC_PATH;
extern const std::string DEFAULT_XMLRPC_TOKEN;
extern const std::string DEFAULT_DEBUG_LEVEL;
extern const unsigned int DEFAULT_SLEEP_THREADPOOL_MAX_PENDING;
extern const int DEFAULT_THREADS_NUMBER;
extern const int DEFAULT_THREADPOOL_MAX_PENDING;

extern const int DEFAULT_GSOAP_MAX_PENDING;
extern const int DEFAULT_GSOAP_SEND_TIMEOUT; // seconds
extern const int DEFAULT_GSOAP_RECV_TIMEOUT; // seconds

extern const int DEFAULT_PORT;
extern const bool DEFAULT_MONITORING_ENABLED;
extern const int DEFAULT_MONITORING_TIME_INTERVAL;
extern const bool DEFAULT_MONITORING_DETAILED;
extern const bool DEFAULT_XMLRPC_CHECK_ASCII;
extern const bool DEFAULT_USER_CHECK_BLACKLIST;
extern const std::string DEFAULT_CA_CERTIFICATES_FOLDER;
extern const std::string DEFAULT_ARGUS_RESOURCE_ID;

extern const char* EMPTY_DESCRIPTION;

extern const char* ENVVAR_GRIDMAP;
extern const char* ENVVAR_X509_USER_CERT;
extern const char* ENVVAR_X509_USER_KEY;
extern const char* ENVVAR_X509_CERT_DIR;

extern const std::string OPT_HELP;
extern const std::string OPTL_HELP;
extern const char* OPT_HELP_DESCRIPTION;

extern const std::string OPT_VERSION;
extern const std::string OPTL_VERSION;
extern const char* OPT_VERSION_DESCRIPTION;

extern const std::string OPT_CONFIG_FILE;
extern const std::string OPTL_CONFIG_FILE;
extern const char* OPT_CONFIG_FILE_DESCRIPTION;

extern const std::string OPT_DEBUG;
extern const std::string OPTL_DEBUG;
extern const char* OPT_DEBUG_DESCRIPTION;

// Renamed disable with enable and changed checks accordingly
extern const std::string OPTL_ENABLE_MAPPING;
extern const char* OPT_ENABLE_MAPPING_DESCRIPTION;

extern const std::string OPTL_ENABLE_VOMSCHECK;
extern const char* OPT_ENABLE_VOMSCHECK_DESCRIPTION;

extern const std::string OPTL_MONITORING_ENABLED;

extern const std::string OPTL_MONITORING_FILE_NAME;
extern const char* OPT_MONITORING_FILE_NAME_DESCRIPTION;

extern const std::string OPTL_MONITORING_DETAILED;
extern const char* OPT_MONITORING_DETAILED_DESCRIPTION;

extern const std::string OPTL_MONITORING_TIME_INTERVAL;
extern const char* OPT_MONITORING_TIME_INTERVAL_DESCRIPTION;

extern const std::string OPTL_LOG_FILE_NAME;
extern const char* OPT_LOG_FILE_NAME_DESCRIPTION;

extern const std::string OPTL_DEBUG_LEVEL;
extern const char* OPT_DEBUG_LEVEL_DESCRIPTION;

extern const std::string OPTL_PORT;
extern const char* OPT_PORT_DESCRIPTION;

extern const std::string OPTL_NUM_THREADS;

extern const std::string OPTL_MAX_THREADPOOL_PENDING;

extern const std::string OPTL_SLEEP_THREADPOOL_MAX_PENDING;

extern const std::string OPTL_MAX_GSOAP_PENDING;

extern const std::string OPTL_GSOAP_SEND_TIMEOUT;
extern const std::string OPTL_GSOAP_RECV_TIMEOUT;

extern const std::string OPTL_PROXY_DIR;
extern const char* OPT_PROXY_DIR_DESCRIPTION;

extern const std::string OPTL_PROXY_USER;
extern const char* OPT_PROXY_USER_DESCRIPTION;

extern const std::string OPTL_XMLRPC_HOST;
extern const char* OPT_XMLRPC_HOST_DESCRIPTION;

extern const std::string OPTL_RECALLTABLE_PORT;
extern const std::string OPTL_XMLRPC_PORT;
extern const char* OPT_XMLRPC_PORT_DESCRIPTION;

extern const std::string OPTL_XMLRPC_PATH;
extern const char* OPT_XMLRPC_PATH_DESCRIPTION;

extern const std::string OPTL_XMLRPC_TOKEN;
extern const char* OPT_XMLRPC_TOKEN_DESCRIPTION;

extern const std::string OPTL_WSDL_FILE;
extern const char* OPT_WSDL_FILE_DESCRIPTION;

extern const std::string OPTL_DB_HOST;
extern const char* OPT_DB_HOST_DESCRIPTION;

extern const std::string OPTL_DB_USER;
extern const char* OPT_DB_USER_DESCRIPTION;

extern const std::string OPTL_DB_USER_PASSWORD;
extern const char* OPT_DB_USER_PASSWORD_DESCRIPTION;

extern const std::string OPTL_XMLRPC_CHECK_ASCII;
extern const char* OPT_XMLRPC_CHECK_ASCII_DESCRIPTION;

extern const std::string OPTL_USER_CHECK_BLACKLIST;
extern const char* OPT_USER_CHECK_BLACKLIST_DESCRIPTION;

extern const std::string OPTL_ARGUS_PEPD_ENDPOINT;
extern const char* OPT_ARGUS_PEPD_ENDPOINT_DESCRIPTION;

extern const std::string OPTL_ARGUS_RESOURCE_ID;
extern const char* OPT_ARGUS_RESOURCE_ID_DESCRIPTION;



// A helper function to simplify printing options stuff
template<class T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& v)
{
    std::copy(v.begin(), v.end(), std::ostream_iterator<T>(os, " "));
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
    void checkFileReadPerm(std::string fileAbsolutePath);

    bool requestedHelp();
    bool requestedVersion();
    bool requestedDebug();
    bool foundConfigurationFile();

    bool mappingEnabled();
    bool getMonitoringEnabled();
    bool getMonitoringDetailed();

    bool vomsCheckEnabled();
    int getDebugLevel();
    int getNumThreads();
    int getThreadpoolMaxPending();
    int getGsoapMaxPending();
    int getGsoapSendTimeout();
    int getGsoapRecvTimeout();

    int getPort();
    int getRecalltablePort();
    int getMonitoringTimeInterval();
    unsigned int getThreadpoolMaxpendingSleepTime();
    std::string getDebugLevelString();
    std::string getProxyDir();
    std::string getProxyUser();
    std::string getXMLRPCEndpoint();

    const std::string& getXMLRPCToken();

    std::string getXmlRpcHost();
    std::string getUser();
    std::string getWSDLFilePath();
    std::string getMonitoringFile();
    std::string getLogFile();
    std::string getDBHost();
    std::string getDBUser();
    std::string getDBUserPassword();
    std::string getConfigurationFile();
    std::string getGridmapfile();
    std::string getHostCertFile();
    std::string getHostKeyFile();
    bool getXMLRPCCheckAscii();
    bool getUserCheckBlacklist();
    std::string getArgusPepdEndpoint();
    std::string getArgusResourceId();
    std::string getCaCertificatesFolder();

private:
    FrontendConfiguration();
    virtual ~FrontendConfiguration();
    boost::program_options::options_description defineConfigFileOptions();
    boost::program_options::options_description defineCommandLineOptions();
    void setCommandLineOptions(boost::program_options::variables_map& vm);
    void setConfigurationOptions(boost::program_options::variables_map& vm);
    int decodeDebugLevelOption(std::string& debugLevel);
    void checkCreateDir(std::string dirAbsolutePath);
    std::string getFromEnvironment(const char* envVar, const std::string& defaultValue);
    std::string getFilename(std::string path);
    std::string getParentPath(std::string path);

    static FrontendConfiguration* instance;

    boost::program_options::options_description configurationFileOptions;
    boost::program_options::options_description commandLineOptions;

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
    int gsoap_send_timeout;
    int gsoap_recv_timeout;
    int debugLevel;
    int port;
    int recalltablePort;
    int monitoring_time_interval;
    std::string monitoring_file;
    std::string log_file;
    std::string proxy_dir;
    std::string proxy_user;
    std::string user;
    std::string wsdl_file;
    std::string debugLevelString;
    std::string xmlrpc_host;
    std::string xmlrpc_port;
    std::string xmlrpc_path;
    std::string xmlrpc_token;
    std::string dbHost;
    std::string dbUser;
    std::string dbUserPassword;
    std::string configuration_file;
    std::string gridmapfile;
    std::string hostcertfile;
    std::string hostkeyfile;
    bool xmlrpc_check_ascii;
    bool user_check_blacklist;
    std::string argus_pepd_endpoint;
    std::string argus_resource_id;
    std::string ca_certificates_folder;

};

int getInstanceXMLRPCCheckAscii();

#endif /* FRONTENDOPTIONS_HPP_ */
