/*
 * FrontendOptions.h
 *
 *  Created on: Nov 21, 2008
 *      Author: alb
 */

#ifndef FRONTENDOPTIONS_HPP_
#define FRONTENDOPTIONS_HPP_

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include <iostream>
#include <fstream>
#include <iterator>
using namespace std;

// Costants
static const string DEFAULT_CONFIGURATION_FILE = "storm-frontend.conf";
static const string DEFAULT_LOG_FILE = "storm-frontend.log";
static const int DEFAULT_THREADS_NUMBER = 20;
static const int DEFAULT_PORT = 8444;

static const string OPT_HELP = "h";
static const string OPTL_HELP = "help";
static const char* OPT_HELP_DESCRIPTION = "Print this message";

static const string OPT_VERSION = "v";
static const string OPTL_VERSION = "version";
static const char* OPT_VERSION_DESCRIPTION = "Print version";

static const string OPT_CONFIG_FILE = "c";
static const string OPTL_CONFIG_FILE = "config-file";
static const char* OPT_CONFIG_FILE_DESCRIPTION = "Configuration file";

static const string OPT_LOG_FILE = "l";
static const string OPTL_LOG_FILE = "logfile";
static const char* OPT_LOG_FILE_DESCRIPTION = "Use <arg> as log file";

static const string OPT_NUM_THREADS = "t";
static const string OPTL_NUM_THREADS = "num-threads";
static const char* OPT_NUM_THREADS_DESCRIPTION = "Use <arg> threads";

static const string OPT_PROXY_DIR = "P";
static const string OPTL_PROXY_DIR = "proxy-dir";
static const char* OPT_PROXY_DIR_DESCRIPTION = "Directory used to store proxies delegated by the client";

static const string OPT_PORT = "p";
static const string OPTL_PORT = "port";
static const char* OPT_PORT_DESCRIPTION = "Listen to port <arg>";

static const string OPT_XMLRPC_ENPOINT = "x";
static const string OPTL_XMLRPC_ENPOINT = "xmlrpc-endpoint";
static const char* OPT_XMLRPC_ENPOINT_DESCRIPTION = "Specify the XMLRPC endpoint of the StoRM Backend. <arg> is of the form:\n"
    "[HOST][:PORT][/PATH]\nDefault is: localhost:8080/RPC2";

static const string OPT_PROXY_USER = "U";
static const string OPTL_PROXY_USER = "proxy-user";
static const char* OPT_PROXY_USER_DESCRIPTION = "Save the proxy certificate using <arg>'s uid and gid";

static const string OPT_USER = "u";
static const string OPTL_USER = "user";
static const char* OPT_USER_DESCRIPTION = "Run the frontend as user <arg>";

static const string OPT_WSDL_FILE = "w";
static const string OPTL_WSDL_FILE = "wsdl-file";
static const char* OPT_WSDL_FILE_DESCRIPTION = "Path to the WSDL to publish in case of GET request";

static const string OPT_VERBOSE = "v";
static const string OPTL_VERBOSE = "debug-level";
static const char* OPT_VERBOSE_DESCRIPTION = "Debug level. <arg> can be: ERROR, WARN, INFO, DEBUG, DEBUG2, DEBUG3";

static const string OPT_DEBUG = "d";
static const string OPTL_DEBUG = "debug-mode";
static const char* OPT_DEBUG_DESCRIPTION = "Start in debug-mode: do not exec fork() and stay in foreground";


// A helper function to simplify printing options stuff
template<class T>
ostream& operator<<(ostream& os, const vector<T>& v)
{
    copy(v.begin(), v.end(), ostream_iterator<T>(cout, " "));
    return os;
}


class FrontendOptions {
public:
    FrontendOptions();
    virtual ~FrontendOptions();

    void parseOptions(int argc, char* argv[]);
    void printHelpMessage();

    bool requestedHelp();
    bool requestedVersion();
    bool requestedDebug();
    int getVerboseLevel();
    int getNumThreads();
    string getProxyDir();
    string getPort();
    string getXMLRPCEndpoint();
    string getProxyUser();
    string getUser();
    string getWSDLFilePath();
    string getLogFile();

private:
    po::options_description defineConfigFileOptions();
    po::options_description defineCommandLineOptions();
    void setCommandLineOptions(po::variables_map& vm);
    void setConfigurationOptions(po::variables_map& vm);

    po::options_description configurationFileOptions;
    po::options_description commandLineOptions;

    bool helpRequested;
    bool versionRequested;
    bool debugMode;
    int numberOfThreads;
    int verboseLevel;
    int port;
    string log_file;
    string proxy_dir;
    string proxy_user;
    string user;
    string wsdl_file;
    string verboseLevelString;
    string xmlrpc_endpoint;


};

#endif /* FRONTENDOPTIONS_HPP_ */
