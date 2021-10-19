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

#include <boost/program_options.hpp>

#include <iostream>
#include <fstream>
#include <iterator>
#include <string>

extern const int DEFAULT_THREADS_NUMBER;
extern const int DEFAULT_THREADPOOL_MAX_PENDING;
// Renamed disable with enable and changed checks accordingly
extern const std::string OPTL_ENABLE_MAPPING;
extern const std::string OPTL_ENABLE_VOMSCHECK;
extern const std::string OPTL_MONITORING_ENABLED;
extern const std::string OPTL_MONITORING_FILE_NAME;
extern const std::string OPTL_MONITORING_DETAILED;
extern const std::string OPTL_MONITORING_TIME_INTERVAL;
extern const std::string OPTL_LOG_FILE_NAME;
extern const std::string OPTL_DEBUG_LEVEL;
extern const std::string OPTL_PORT;
extern const std::string OPTL_NUM_THREADS;
extern const std::string OPTL_MAX_THREADPOOL_PENDING;
extern const std::string OPTL_SLEEP_THREADPOOL_MAX_PENDING;
extern const std::string OPTL_MAX_GSOAP_PENDING;
extern const std::string OPTL_GSOAP_SEND_TIMEOUT;
extern const std::string OPTL_GSOAP_RECV_TIMEOUT;
extern const std::string OPTL_RECALLTABLE_PORT;
extern const std::string OPTL_WSDL_FILE;
extern const std::string OPTL_DB_HOST;
extern const std::string OPTL_DB_USER;
extern const std::string OPTL_ARGUS_PEPD_ENDPOINT;
extern const std::string OPTL_ARGUS_RESOURCE_ID;

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

#endif /* FRONTENDOPTIONS_HPP_ */
