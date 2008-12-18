/*
 * FrontendConfiguration.h
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
#include <string>
using namespace std;

// Costants
static const string DEFAULT_CONFIGURATION_FILE = "storm-frontend.conf";
static const string DEFAULT_GRIDMAPFILE = "/etc/grid-security/grid-mapfile";
static const string DEFAULT_HOST_CERT_FILE = "/etc/grid-security/hostcert.pem";
static const string DEFAULT_HOST_KEY_FILE = "/etc/grid-security/hostkey.pem";
static const string DEFAULT_LOG_FILE_NAME = "storm-frontend.log";
static const string DEFAULT_WSDL_FILE = "";
static const string DEFAULT_XMLRPC_HOST = "localhost";
static const string DEFAULT_XMLRPC_PORT = "8080";
static const string DEFAULT_XMLRPC_PATH = "/RPC2";
static const string DEFAULT_DEBUG_LEVEL = "INFO";
static const int DEFAULT_THREADS_NUMBER = 20;
static const int DEFAULT_PORT = 8444;

static const char* ENVVAR_GRIDMAP = "GRIDMAP";
static const char* ENVVAR_X509_USER_CERT = "X509_USER_CERT";
static const char* ENVVAR_X509_USER_KEY = "X509_USER_KEY";

static const string OPT_HELP = "h";
static const string OPTL_HELP = "help";
static const char* OPT_HELP_DESCRIPTION = "Print this message";

static const string OPT_VERSION = "v";
static const string OPTL_VERSION = "version";
static const char* OPT_VERSION_DESCRIPTION = "Print version";

static const string OPT_CONFIG_FILE = "c";
static const string OPTL_CONFIG_FILE = "config-file";
static const char* OPT_CONFIG_FILE_DESCRIPTION = "Configuration file";

static const string OPTL_DISABLE_MAPPING = "security.disable.mapping";
static const char* OPT_DISABLE_MAPPING_DESCRIPTION = "Enable/Disable mapping via gridmafile.";

static const string OPTL_DISABLE_VOMSCHECK = "security.disable.vomscheck";
static const char* OPT_DISABLE_VOMSCHECK_DESCRIPTION = "Enable/Disable VOMS credentials check.";

static const string OPTL_GRIDMAFILE = "security.gridmapfile";

static const string OPTL_HOST_CERT = "security.hostcert";

static const string OPTL_HOST_KEY = "security.hostkey";

static const string OPT_LOG_FILE_NAME = "l";
static const string OPTL_LOG_FILE_NAME = "log.filename";
static const char* OPT_LOG_FILE_NAME_DESCRIPTION = "Use <arg> as log file";

static const string OPT_DEBUG_LEVEL = "v";
static const string OPTL_DEBUG_LEVEL = "log.debuglevel";
static const char* OPT_DEBUG_LEVEL_DESCRIPTION = "Debug level. <arg> can be: ERROR, WARN, INFO, DEBUG, DEBUG2, DEBUG3";

static const string OPT_NUM_THREADS = "t";
static const string OPTL_NUM_THREADS = "fe.threads.number";
static const char* OPT_NUM_THREADS_DESCRIPTION = "Use <arg> threads";

static const string OPT_PROXY_DIR = "P";
static const string OPTL_PROXY_DIR = "proxy.dir";
static const char* OPT_PROXY_DIR_DESCRIPTION = "Directory used to store proxies delegated by the client";

static const string OPT_PROXY_USER = "U";
static const string OPTL_PROXY_USER = "proxy.user";
static const char* OPT_PROXY_USER_DESCRIPTION = "Save the proxy certificate using <arg>'s uid and gid";

static const string OPT_PORT = "p";
static const string OPTL_PORT = "fe.port";
static const char* OPT_PORT_DESCRIPTION = "Listen to port <arg>";

static const string OPTL_XMLRPC_HOST = "be.xmlrpc.host";
static const char* OPT_XMLRPC_HOST_DESCRIPTION = "StoRM XMLRPC server (the same as the StoRM backend server)";

static const string OPTL_XMLRPC_PORT = "be.xmlrpc.port";
static const char* OPT_XMLRPC_PORT_DESCRIPTION = "Port used by the StoRM XMLRPC server";

static const string OPTL_XMLRPC_PATH = "be.xmlrpc.path";
static const char* OPT_XMLRPC_PATH_DESCRIPTION = "Path of the StoRM XMLRPC server service";

static const string OPT_WSDL_FILE = "w";
static const string OPTL_WSDL_FILE = "wsdl.file";
static const char* OPT_WSDL_FILE_DESCRIPTION = "Path to the WSDL to publish in case of GET request";

static const string OPT_DEBUG = "d";
static const string OPTL_DEBUG = "debug-mode";
static const char* OPT_DEBUG_DESCRIPTION = "Start in debug-mode: do not exec fork() and stay in foreground";

static const string OPTL_DB_HOST = "db.host";
static const char* OPT_DB_HOST_DESCRIPTION = "Machine hosting the DB";

static const string OPTL_DB_USER = "db.user";
static const char* OPT_DB_USER_DESCRIPTION = "Database user";

static const string OPTL_DB_USER_PASSWORD = "db.passwd";
static const char* OPT_DB_USER_PASSWORD_DESCRIPTION = "Database user password";

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
    bool mappingDisabled();
    bool vomsCheckDisabled();
    int getDebugLevel();
    int getNumThreads();
    int getPort();
    string getDebugLevelString();
    string getProxyDir();
    string getProxyUser();
    string getXMLRPCEndpoint();
    string getUser();
    string getWSDLFilePath();
    string getLogFile();
    string getDBHost();
    string getDBUser();
    string getDBUserPassword();
    string getConfigurationFile();
    string getGridmapfile();
    string getHostCertFile();
    string getHostKeyFile();

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
    bool disableMapping;
    bool disableVOMSCheck;
    int numberOfThreads;
    int debugLevel;
    int port;
    string log_file;
    string log_file_dir;
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

};

#endif /* FRONTENDOPTIONS_HPP_ */
