/*
 * FrontendOptions.cpp
 *
 *  Created on: Nov 21, 2008
 *      Author: alb
 */

#include "FrontendOptions.hpp"
#include "srmlogit.h"

FrontendOptions::FrontendOptions() {
    commandLineOptions.add(defineCommandLineOptions());
    configurationFileOptions.add(defineConfigFileOptions());
}

FrontendOptions::~FrontendOptions() {
}

void FrontendOptions::parseOptions(int argc, char* argv[]) {

    po::variables_map cmdline_vm;

    store(po::parse_command_line(argc, argv, commandLineOptions), cmdline_vm);
    notify(cmdline_vm);

    setCommandLineOptions(cmdline_vm);

    if (requestedHelp() || requestedVersion()) {
        return;
    }

    string configuration_file_path(cmdline_vm[OPTL_CONFIG_FILE].as<string>());

    cout << "Reading configuration file: " << configuration_file_path << "...\n";

    po::variables_map config_vm;
    ifstream conf_file(configuration_file_path.c_str());

    store(parse_config_file(conf_file, configurationFileOptions), config_vm);
    notify(config_vm);

    setConfigurationOptions(config_vm);

}

void FrontendOptions::setCommandLineOptions(po::variables_map& vm) {

    helpRequested = false;
    versionRequested = false;
    debugMode = false;

    if (vm.count(OPTL_HELP))
        helpRequested = true;

    if (vm.count(OPTL_VERSION))
        versionRequested = true;

    if (vm.count(OPTL_DEBUG))
        debugMode = true;

}

void FrontendOptions::setConfigurationOptions(po::variables_map& vm) {

    if (vm.count(OPTL_LOG_FILE))
        log_file = vm[OPTL_LOG_FILE].as<string> ();

    if (vm.count(OPTL_NUM_THREADS))
        numberOfThreads = vm[OPTL_NUM_THREADS].as<int> ();

    if (vm.count(OPTL_DEBUG_LEVEL))
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

    if (vm.count(OPTL_USER))
        user = vm[OPTL_USER].as<string> ();

    if (vm.count(OPTL_WSDL_FILE))
        wsdl_file = vm[OPTL_WSDL_FILE].as<string> ();

    if (vm.count(OPTL_DB_HOST))
        dbHost = vm[OPTL_DB_HOST].as<string> ();

    if (vm.count(OPTL_DB_USER))
        dbUser = vm[OPTL_DB_USER].as<string> ();

    if (vm.count(OPTL_DB_USER_PASSWORD))
        dbUserPassword = vm[OPTL_DB_USER_PASSWORD].as<string> ();

}

bool FrontendOptions::requestedHelp() {
    return helpRequested;
}

bool FrontendOptions::requestedVersion() {
    return versionRequested;
}

bool FrontendOptions::requestedDebug() {
    return debugMode;
}

int FrontendOptions::getDebugLevel() {
    return debugLevel;
}

string FrontendOptions::getDebugLevelString() {
    return debugLevelString;
}

int FrontendOptions::getNumThreads() {
    return numberOfThreads;
}

int FrontendOptions::getPort() {
    return port;
}

string FrontendOptions::getProxyDir() {
    return proxy_dir;
}

string FrontendOptions::getProxyUser() {
    return proxy_user;
}

string FrontendOptions::getXMLRPCEndpoint() {
    return xmlrpc_host + "," + xmlrpc_port + xmlrpc_path;
}

string FrontendOptions::getUser() {
    return user;
}

string FrontendOptions::getWSDLFilePath() {
    return wsdl_file;
}

string FrontendOptions::getLogFile() {
    return log_file;
}

string FrontendOptions::getDBHost() {
    return dbHost;
}

string FrontendOptions::getDBUser() {
    return dbUser;
}

string FrontendOptions::getDBUserPassword() {
    return dbUserPassword;
}

void FrontendOptions::printHelpMessage() {
    cout << commandLineOptions << "\n\n\n";
    cout << configurationFileOptions << "\n";
}

po::options_description FrontendOptions::defineConfigFileOptions() {

    po::options_description configurationFileOptions("Configuration file options");

    configurationFileOptions.add_options()
        (string(OPTL_NUM_THREADS + "," + OPT_NUM_THREADS).c_str(), po::value<int>()->default_value(DEFAULT_THREADS_NUMBER), OPT_NUM_THREADS_DESCRIPTION)
        (string(OPTL_LOG_FILE + "," + OPT_LOG_FILE).c_str(), po::value<string>()->default_value(DEFAULT_LOG_FILE), OPT_LOG_FILE_DESCRIPTION)
        (string(OPTL_PROXY_DIR + "," + OPT_PROXY_DIR).c_str(), po::value<string>(), OPT_PROXY_DIR_DESCRIPTION)
        (string(OPTL_PORT + "," + OPT_PORT).c_str(), po::value<int>()->default_value(DEFAULT_PORT), OPT_PORT_DESCRIPTION)
        (string(OPTL_XMLRPC_HOST).c_str(), po::value<string>()->default_value(DEFAULT_XMLRPC_HOST), OPT_XMLRPC_HOST_DESCRIPTION)
        (string(OPTL_XMLRPC_PORT).c_str(), po::value<string>()->default_value(DEFAULT_XMLRPC_PORT), OPT_XMLRPC_PORT_DESCRIPTION)
        (string(OPTL_XMLRPC_PATH).c_str(), po::value<string>()->default_value(DEFAULT_XMLRPC_PATH), OPT_XMLRPC_PATH_DESCRIPTION)
        (string(OPTL_PROXY_USER + "," + OPT_PROXY_USER).c_str(), po::value<string>(), OPT_PROXY_USER_DESCRIPTION)
        (string(OPTL_USER + "," + OPT_USER).c_str(), po::value<string>(), OPT_USER_DESCRIPTION)
        (string(OPTL_WSDL_FILE + "," + OPT_WSDL_FILE).c_str(), po::value<string>()->default_value(DEFAULT_WSDL_FILE), OPT_WSDL_FILE_DESCRIPTION)
        (string(OPTL_DEBUG_LEVEL + "," + OPT_DEBUG_LEVEL).c_str(), po::value<string>(), OPT_DEBUG_LEVEL_DESCRIPTION);

    return configurationFileOptions;
}

po::options_description FrontendOptions::defineCommandLineOptions() {
    po::options_description commandLineOptions("Options");
    commandLineOptions.add_options()
        (string(OPTL_HELP + "," + OPT_HELP).c_str(), OPT_HELP_DESCRIPTION)
        (string(OPTL_VERSION + "," + OPT_VERSION).c_str(), OPT_VERSION_DESCRIPTION)
        (string(OPTL_CONFIG_FILE + "," + OPT_CONFIG_FILE).c_str(), po::value<string>()->default_value(DEFAULT_CONFIGURATION_FILE), OPT_CONFIG_FILE_DESCRIPTION)
        (string(OPTL_DEBUG + "," + OPT_DEBUG).c_str(), OPT_DEBUG_DESCRIPTION);

    return commandLineOptions;
}

int FrontendOptions::decodeDebugLevelOption(string& debugLevelString) {

    int debugLevel = STORM_LOG_ERROR;

    if (debugLevelString == "NONE")
            debugLevel = STORM_LOG_NONE;
    else if (debugLevelString == "ERROR")
        debugLevel = STORM_LOG_ERROR;
    else if (debugLevelString == "WARN")
        debugLevel = STORM_LOG_WARNING;
    else if (debugLevelString == "DEBUG")
        debugLevel = STORM_LOG_DEBUG;
    else if (debugLevelString == "DEBUG2")
        debugLevel = STORM_LOG_DEBUG2;
    else if (debugLevelString == "DEBUG3")
        debugLevel = STORM_LOG_DEBUG3;
    else if (debugLevelString == "DEBUG4")
        debugLevel = STORM_LOG_DEBUG4;

    return debugLevel;
}
