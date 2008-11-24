/*
 * FrontendOptions.cpp
 *
 *  Created on: Nov 21, 2008
 *      Author: alb
 */

#include "FrontendOptions.hpp"

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

    if (vm.count(OPTL_VERBOSE))
        verboseLevelString = vm[OPTL_VERBOSE].as<string> ();

    if (vm.count(OPTL_PROXY_DIR))
        proxy_dir = vm[OPTL_PROXY_DIR].as<string> ();

    if (vm.count(OPTL_PORT))
        port = vm[OPTL_PORT].as<int> ();

    if (vm.count(OPTL_XMLRPC_ENPOINT))
        xmlrpc_endpoint = vm[OPTL_XMLRPC_ENPOINT].as<string> ();

    if (vm.count(OPTL_PROXY_USER))
        proxy_user = vm[OPTL_PROXY_USER].as<string> ();

    if (vm.count(OPTL_USER))
        user = vm[OPTL_USER].as<string> ();

    if (vm.count(OPTL_WSDL_FILE))
        wsdl_file = vm[OPTL_WSDL_FILE].as<string> ();

}

bool FrontendOptions::requestedHelp() {
    return helpRequested;
}

bool FrontendOptions::requestedVersion() {
    return versionRequested;
}

int FrontendOptions::getNumThreads() {
    return numberOfThreads;
}

string FrontendOptions::getLogFile() {
    return log_file;
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
        (string(OPTL_XMLRPC_ENPOINT + "," + OPT_XMLRPC_ENPOINT).c_str(), po::value<string>(), OPT_XMLRPC_ENPOINT_DESCRIPTION)
        (string(OPTL_PROXY_USER + "," + OPT_PROXY_USER).c_str(), po::value<string>(), OPT_PROXY_USER_DESCRIPTION)
        (string(OPTL_USER + "," + OPT_USER).c_str(), po::value<string>(), OPT_USER_DESCRIPTION)
        (string(OPTL_WSDL_FILE + "," + OPT_WSDL_FILE).c_str(), po::value<string>(), OPT_WSDL_FILE_DESCRIPTION)
        (string(OPTL_VERBOSE + "," + OPT_VERBOSE).c_str(), po::value<string>(), OPT_VERBOSE_DESCRIPTION);

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
