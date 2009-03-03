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

FrontendConfiguration* FrontendConfiguration::instance= NULL;

FrontendConfiguration* FrontendConfiguration::getInstance() {
    if (instance == NULL)
        instance = new FrontendConfiguration();

    return instance;
}

FrontendConfiguration::FrontendConfiguration() {
    commandLineOptions.add(defineCommandLineOptions());
    configurationFileOptions.add(defineConfigFileOptions());

    struct passwd* pwd = getpwuid(getuid());
    user.assign(pwd->pw_name);

}

FrontendConfiguration::~FrontendConfiguration() {
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

    string configuration_file_path(cmdline_vm[OPTL_CONFIG_FILE].as<string>());

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

void FrontendConfiguration::checkConfigurationData() {

    string dir;
    try {
        dir = proxy_dir;
        checkCreateDir(dir);
        dir = getParentPath(log_file);
        checkCreateDir(dir);
    } catch (exception& e) {
        throw runtime_error("Error while attempting to create \"" + dir + "\".\n" + string(e.what()));
    }

}

void FrontendConfiguration::checkFileReadPerm(string fileAbsolutePath) {

    struct stat statInfo;

    int ret = stat(fileAbsolutePath.c_str(), &statInfo);

    if (ret !=0 ) {
        throw runtime_error("File doesn't exists: " + fileAbsolutePath);
    }

    if (S_ISDIR(statInfo.st_mode)) {
        throw runtime_error("Error looking for a file but a directory were found: "
                + fileAbsolutePath);
    }

    if (access(fileAbsolutePath.c_str(), R_OK) != 0) {
        throw runtime_error("Read permission needed for file: " + fileAbsolutePath);
    }

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

bool FrontendConfiguration::mappingDisabled() {
    return disableMapping;
}

bool FrontendConfiguration::vomsCheckDisabled() {
    return disableVOMSCheck;
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
    return atoi(threadpool_max_pending_sleeptime);
}

int FrontendConfiguration::getGsoapMaxPending() {
    return gsoap_max_pending;
}

int FrontendConfiguration::getPort() {
    return port;
}

string FrontendConfiguration::getProxyDir() {
    return proxy_dir;
}

string FrontendConfiguration::getProxyUser() {
    return proxy_user;
}

string FrontendConfiguration::getXMLRPCEndpoint() {
    return xmlrpc_host + ":" + xmlrpc_port + xmlrpc_path;
}

string FrontendConfiguration::getUser() {
    return user;
}

string FrontendConfiguration::getWSDLFilePath() {
    return wsdl_file;
}

string FrontendConfiguration::getLogFile() {
    return log_file_dir + "/" + log_file;
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

/******************************** Private methods ****************************/

po::options_description FrontendConfiguration::defineConfigFileOptions() {

    po::options_description configurationFileOptions("Configuration file options");

    configurationFileOptions.add_options()
        (OPTL_NUM_THREADS.c_str(), po::value<int>()->default_value(DEFAULT_THREADS_NUMBER), EMPTY_DESCRIPTION)
        (OPTL_MAX_THREADPOOL_PENDING.c_str(), po::value<int>()->default_value(DEFAULT_THREADPOOL_MAX_PENDING), EMPTY_DESCRIPTION)
        (OPTL_SLEEP_THREADPOOL_MAX_PENDING.c_str(), po::value<unsigned int>()->default_value(DEFAULT_SLEEP_THREADPOOL_MAX_PENDING), EMPTY_DESCRIPTION)
        (OPTL_MAX_GSOAP_PENDING.c_str(), po::value<int>()->default_value(DEFAULT_GSOAP_MAX_PENDING), EMPTY_DESCRIPTION)
        (OPTL_LOG_FILE_NAME.c_str(), po::value<string>()->default_value(DEFAULT_LOG_FILE_NAME), EMPTY_DESCRIPTION)
        (OPTL_PROXY_DIR.c_str(), po::value<string>(), OPT_PROXY_DIR_DESCRIPTION)
        (OPTL_PORT.c_str(), po::value<int>()->default_value(DEFAULT_PORT), OPT_PORT_DESCRIPTION)
        (OPTL_XMLRPC_HOST.c_str(), po::value<string>()->default_value(DEFAULT_XMLRPC_HOST), OPT_XMLRPC_HOST_DESCRIPTION)
        (OPTL_XMLRPC_PORT.c_str(), po::value<string>()->default_value(DEFAULT_XMLRPC_PORT), OPT_XMLRPC_PORT_DESCRIPTION)
        (OPTL_XMLRPC_PATH.c_str(), po::value<string>()->default_value(DEFAULT_XMLRPC_PATH), OPT_XMLRPC_PATH_DESCRIPTION)
        (OPTL_PROXY_USER.c_str(), po::value<string>(), OPT_PROXY_USER_DESCRIPTION)
        (OPTL_WSDL_FILE.c_str(), po::value<string>()->default_value(DEFAULT_WSDL_FILE), OPT_WSDL_FILE_DESCRIPTION)
        (OPTL_DEBUG_LEVEL.c_str(), po::value<string>()->default_value(DEFAULT_DEBUG_LEVEL), OPT_DEBUG_LEVEL_DESCRIPTION)
        (OPTL_DB_HOST.c_str(), po::value<string>(), OPT_DB_HOST_DESCRIPTION)
        (OPTL_DB_USER.c_str(), po::value<string>(), OPT_DB_USER_DESCRIPTION)
        (OPTL_DB_USER_PASSWORD.c_str(), po::value<string>(), OPT_DB_USER_PASSWORD_DESCRIPTION)
        (OPTL_DISABLE_MAPPING.c_str(), po::value<bool>()->default_value(false), OPT_DISABLE_MAPPING_DESCRIPTION)
        (OPTL_DISABLE_VOMSCHECK.c_str(), po::value<bool>()->default_value(false), OPT_DISABLE_VOMSCHECK_DESCRIPTION);

    return configurationFileOptions;
}

po::options_description FrontendConfiguration::defineCommandLineOptions() {
    po::options_description commandLineOptions("Options");
    commandLineOptions.add_options()
        (string(OPTL_HELP + "," + OPT_HELP).c_str(), OPT_HELP_DESCRIPTION)
        (string(OPTL_VERSION + "," + OPT_VERSION).c_str(), OPT_VERSION_DESCRIPTION)
        (string(OPTL_CONFIG_FILE + "," + OPT_CONFIG_FILE).c_str(), po::value<string>()->default_value(DEFAULT_CONFIGURATION_FILE), OPT_CONFIG_FILE_DESCRIPTION)
        (string(OPTL_DEBUG + "," + OPT_DEBUG).c_str(), OPT_DEBUG_DESCRIPTION);

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
        configuration_file = vm[OPTL_CONFIG_FILE].as<string>();
    }

}

void FrontendConfiguration::setConfigurationOptions(po::variables_map& vm) {

    if (vm.count(OPTL_LOG_FILE_NAME))
        log_file = vm[OPTL_LOG_FILE_NAME].as<string> ();

    numberOfThreads = vm[OPTL_NUM_THREADS].as<int> ();
    threadpool_max_pending = vm[OPTL_MAX_THREADPOOL_PENDING].as<int> ();
    threadpool_max_pending_sleeptime = vm[OPTL_SLEEP_THREADPOOL_MAX_PENDING].as<unsigned int> ();
    gsoap_max_pending = vm[OPTL_MAX_GSOAP_PENDING].as<int> ();

    debugLevelString = vm[OPTL_DEBUG_LEVEL].as<string> ();
    debugLevel = decodeDebugLevelOption(debugLevelString);

    if (vm.count(OPTL_PROXY_DIR))
        proxy_dir = vm[OPTL_PROXY_DIR].as<string> ();

    if (vm.count(OPTL_PORT))
        port = vm[OPTL_PORT].as<int> ();

    if (vm.count(OPTL_XMLRPC_HOST))
        xmlrpc_host = vm[OPTL_XMLRPC_HOST].as<string> ();

    if (vm.count(OPTL_XMLRPC_PORT))
        xmlrpc_port = vm[OPTL_XMLRPC_PORT].as<string> ();

    if (vm.count(OPTL_XMLRPC_PATH))
        xmlrpc_path = vm[OPTL_XMLRPC_PATH].as<string> ();

    if (vm.count(OPTL_PROXY_USER))
        proxy_user = vm[OPTL_PROXY_USER].as<string> ();

    if (vm.count(OPTL_WSDL_FILE))
        wsdl_file = vm[OPTL_WSDL_FILE].as<string> ();

    if (vm.count(OPTL_DB_HOST))
        dbHost = vm[OPTL_DB_HOST].as<string> ();

    if (vm.count(OPTL_DB_USER))
        dbUser = vm[OPTL_DB_USER].as<string> ();

    if (vm.count(OPTL_DB_USER_PASSWORD))
        dbUserPassword = vm[OPTL_DB_USER_PASSWORD].as<string> ();

    disableMapping = vm[OPTL_DISABLE_MAPPING].as<bool> ();
    disableVOMSCheck = vm[OPTL_DISABLE_VOMSCHECK].as<bool> ();

    gridmapfile = getFromEnvironment(ENVVAR_GRIDMAP, DEFAULT_GRIDMAPFILE);
    hostcertfile = getFromEnvironment(ENVVAR_X509_USER_CERT, DEFAULT_HOST_CERT_FILE);
    hostkeyfile = getFromEnvironment(ENVVAR_X509_USER_KEY, DEFAULT_HOST_KEY_FILE);
}

int FrontendConfiguration::decodeDebugLevelOption(string& debugLevelString) {

    int debugLevel = STORM_LOG_ERROR;

    if (debugLevelString == "NONE")
            debugLevel = STORM_LOG_NONE;
    else if (debugLevelString == "ERROR")
        debugLevel = STORM_LOG_ERROR;
    else if (debugLevelString == "WARN")
        debugLevel = STORM_LOG_WARNING;
    else if (debugLevelString == "INFO")
            debugLevel = STORM_LOG_INFO;
    else if (debugLevelString == "DEBUG")
        debugLevel = STORM_LOG_DEBUG;
    else if (debugLevelString == "DEBUG2")
        debugLevel = STORM_LOG_DEBUG2;
    else
        throw runtime_error("Error: unknown debug level value " + debugLevelString
                            + ". Allowed values are: ERROR, WARN, INFO, DEBUG, DEBUG2.");

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

    if (access(dirAbsolutePath.c_str(), W_OK| X_OK) != 0) {
        if (! ((statInfo.st_mode & S_IWOTH) && (statInfo.st_mode & S_IXOTH)) ) {
            throw runtime_error("Permission denied (\"wx\" is needed) for directory \""
                    + dirAbsolutePath + "\". "
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
