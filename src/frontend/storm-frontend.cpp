/*
 * $Id: srmv2.c 2777 2006-08-29 13:20:17Z amessina $
 */

/**
 * \file srmv2.c This file contains the main functions to parse
 * command line, configuration file and start the frontend.
 */

#include "srmSoapBinding.nsmap"
#include "cgsi_plugin.h"
#include "srm_server.h"
#include "srmv2H.h"
#include "storm_functions.h"
#include "storm_limits.h"
#include "storm_util.h"
#include "srmlogit.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pwd.h>
#include <xmlrpc-c/base.h>
#include <xmlrpc-c/client.h>
#include <exception>
#include "FrontendConfiguration.hpp"
#include <boost/threadpool.hpp>
#include <boost/bind.hpp>

#define NAME "StoRM SRM v2.2"

char *db_pwd;
char *db_srvr;
char *db_user;

char* xmlrpc_endpoint;
char* wsdl_file;
char** supported_protocols;

int nb_supported_protocols;
//int jid;

uid_t proxy_uid = 0;
gid_t proxy_gid = 0;

/**
 * check environment variables contents.
 *
 * @return 0 if environment is safe
 *
 * @return -1 otherwise
 */
int check_environment() {
    char *gridmap;
    char *gridmapdir;
    struct stat stbuf;
    int ret;

#ifdef GSI_PLUGINS
    /* environment is set? */
    gridmap = getenv("GRIDMAP");
    gridmapdir = getenv("GRIDMAPDIR");
    if (NULL == gridmap || NULL == gridmapdir)
        return -1;

    /* grid-mapfile is a file? */
    ret = stat(gridmap, &stbuf);
    if (0 != ret || ! S_ISREG(stbuf.st_mode))
        return -1;

    /* grid-mapfile file is readable? */
    if ( 0 != access(gridmap, R_OK) )
        return -1;

    /* gridmapdir is a directory? */
    ret = stat(gridmapdir,&stbuf);
    if (0 != ret || !S_ISDIR(stbuf.st_mode))
        return -1;

    /* gridmapdir is readable and executable? */
    if ( 0 != access(gridmapdir, R_OK & X_OK) )
        return -1;
#endif
    return 0;
}

static int http_get(struct soap *soap) {

    int fd = open(wsdl_file, O_RDONLY | O_NONBLOCK);
    char buf[10240]; /* 10k */
    int nr = 0;

    srmlogit(STORM_LOG_DEBUG,"http_get", "Ricevo richiesta GET\n");
    soap_response(soap, SOAP_HTML); // HTTP response header with text/html
    if (-1 == fd) {
        srmlogit(STORM_LOG_ERROR,"http_get", "Error opening file %s. (%s)\n", wsdl_file, strerror(
                errno));
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

int runAsUser(string& user) {

    struct passwd *pwd;

    if (user.empty()) {

        // Get current user name
        pwd = getpwuid(getuid());
        user.assign(pwd->pw_name);

    } else {

        // Get information on the requested user
        pwd = getpwnam(user.c_str());
        if (NULL == pwd) { // error
            return CONFERR;
        }

        if (setgid(pwd->pw_uid) || setuid(pwd->pw_gid)) {
            return CONFERR;
        }

    }

    return 0;
}

int setProxyUserGlobalVariables(string& proxy_user) {

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

int main(int argc, char** argv)
{
    char *func = "srm_main";
    void *process_request(void *);

    // Set pid in global variable
    //jid = getpid();

    // ------------------------------------------------------------------------
    //------------------------- Set configuration -----------------------------
    // ------------------------------------------------------------------------

    FrontendConfiguration* configuration = FrontendConfiguration::getInstance();

    try {
        configuration->parseOptions(argc, argv);
    } catch (exception& e) {
        cout << e.what() << endl << endl;
        return 1;
    }

    if (configuration->requestedHelp()) {
        configuration->printHelpMessage();
        return 0;
    }

    if (configuration->requestedVersion()) {
        printf("Frontend version: ...");
        return 0;
    }

    int nThreads = configuration->getNumThreads();
    int port = configuration->getPort();
    string log_file = configuration->getLogFile();
    string wsdl_file_path = configuration->getWSDLFilePath();
    string proxy_dir = configuration->getProxyDir();
    string proxy_user = configuration->getProxyUser();
    string xmlrpc_ep = configuration->getXMLRPCEndpoint();
    string user = configuration->getUser();
    string dbHost = configuration->getDBHost();
    string dbUser = configuration->getDBUser();
    string dbUserPasswd = configuration->getDBUserPassword();
    bool debugMode = configuration->requestedDebug();
    int debuglevel = configuration->getDebugLevel();
    string debugLevelString = configuration->getDebugLevelString();
    bool disableMapping = configuration->mappingDisabled();
    bool disableVOMSCheck = configuration->vomsCheckDisabled();

    // Run using "user" privileges
    if (runAsUser(user) != 0) {
        fprintf(stderr, "Error: cannot run as user \"%s\".\n", user.c_str());
        return 1;
    }

    // Proxy User
    if (setProxyUserGlobalVariables(proxy_user) != 0) {
        fprintf(stderr, "Error: request invalid user \"%s\" for proxy dir.\n", proxy_user.c_str());
        return 1;
    }

    try {
        configuration->checkConfigurationData();
    } catch (exception& e) {
        cout << e.what() << endl << endl;
        return 1;
    }

    // Setting global variables... TODO: do not use global variables
    // DB stuff
    db_user = strdup(dbUser.c_str());
    db_pwd = strdup(dbUserPasswd.c_str());
    db_srvr = strdup(dbHost.c_str());

    // Initialize the loging system
    if (debugMode) {
        srmlogit_init(NULL); // i.e. log to stderr
        log_file.assign("stderr"); // Just because it's printed in the logs, see below.
    } else {
        srmlogit_init(log_file.c_str());
    }

    // WSDL file
    wsdl_file = strdup(wsdl_file_path.c_str());

    // XMLRPC
    xmlrpc_endpoint = strdup(xmlrpc_ep.c_str());

    // ------------------------------------------------------------------------
    //------------------- Done: set configuration -----------------------------
    // ------------------------------------------------------------------------

    srmlogit_set_debuglevel(debuglevel);

    srmlogit(STORM_LOG_NONE, func, "Starting StoRM frontend as user: %s\n", user.c_str());
    srmlogit(STORM_LOG_NONE, func, "---------------------- Configuration ------------------\n");
    srmlogit(STORM_LOG_NONE, func, "%s=%d\n", OPTL_NUM_THREADS.c_str(), nThreads);
    srmlogit(STORM_LOG_NONE, func, "%s=%d\n", OPTL_PORT.c_str(), port);
    srmlogit(STORM_LOG_NONE, func, "logfile=%s\n", log_file.c_str());
    srmlogit(STORM_LOG_NONE, func, "xmlrpc endpoint=%s\n", xmlrpc_endpoint);
    srmlogit(STORM_LOG_NONE, func, "%s=%s\n", OPTL_DEBUG_LEVEL.c_str(), debugLevelString.c_str());
    srmlogit(STORM_LOG_NONE, func, "%s=%s\n", OPTL_PROXY_DIR.c_str(), configuration->getProxyDir().c_str());
    srmlogit(STORM_LOG_NONE, func, "%s=%s\n", OPTL_PROXY_USER.c_str(), proxy_user.c_str());
    srmlogit(STORM_LOG_NONE, func, "%s=%s\n", OPTL_DB_HOST.c_str(), db_srvr);
    srmlogit(STORM_LOG_NONE, func, "%s=%s\n", OPTL_DB_USER.c_str(), db_user);
    srmlogit(STORM_LOG_NONE, func, "%s=%s\n", OPTL_DB_USER_PASSWORD.c_str(), db_pwd);
    srmlogit(STORM_LOG_NONE, func, "%s=%s\n", OPTL_WSDL_FILE.c_str(), wsdl_file);
    if (disableMapping) {
        srmlogit(STORM_LOG_NONE, func, "%s=true\n", OPTL_DISABLE_MAPPING.c_str());
    } else {
        srmlogit(STORM_LOG_NONE, func, "%s=false\n", OPTL_DISABLE_MAPPING.c_str());
    }
    if (disableVOMSCheck) {
        srmlogit(STORM_LOG_NONE, func, "%s=true\n", OPTL_DISABLE_VOMSCHECK.c_str());
    } else {
        srmlogit(STORM_LOG_NONE, func, "%s=false\n", OPTL_DISABLE_VOMSCHECK.c_str());
    }
    srmlogit(STORM_LOG_NONE, func, "-------------------------------------------------------\n");

    /* Get list of supported protocols */
    if ((nb_supported_protocols = get_supported_protocols(&supported_protocols)) < 0) {
        srmlogit(STORM_LOG_ERROR, func, "Error in get_supported_protocols(): unable to retrieve "
                 "supported protocols from the DB.");
        exit(1);
    }

    if (0 != check_environment()) {
        srmlogit(STORM_LOG_ERROR, func, "Environment not safe! Exiting\n");
        exit(CONFERR);
    }

    if (! debugMode) { // fork and leave the daemon in background
        int pid = fork();
        if (pid > 0) {
            exit(0);
        }
    }

#if ! defined(_WIN32)
    signal(SIGPIPE, SIG_IGN);
    signal(SIGXFSZ, SIG_IGN);
#endif

    struct soap *soap_data = soap_new2(SOAP_IO_KEEPALIVE, SOAP_IO_KEEPALIVE);
    soap_data->max_keep_alive = SOAP_MAX_KEEPALIVE;
    soap_data->accept_timeout = 0;
    int flags;
#if defined(GSI_PLUGINS)
    flags = CGSI_OPT_DELEG_FLAG;
    if (disableMapping) {
        flags |= CGSI_OPT_DISABLE_MAPPING;
        srmlogit(STORM_LOG_NONE, func, "Mapping disabled\n");
    }
    if (disableVOMSCheck) {
            flags |= CGSI_OPT_DISABLE_VOMS_CHECK;
            srmlogit(STORM_LOG_NONE, func, "VOMS check disabled\n");
        }
    soap_register_plugin_arg(soap_data, server_cgsi_plugin, &flags);
#endif
    soap_data->bind_flags |= SO_REUSEADDR;

    if (!soap_valid_socket(soap_bind(soap_data, NULL, port, BACKLOG))) {
        soap_print_fault(soap_data, stderr);
        soap_done(soap_data);
        free(soap_data);
        exit(SYERR);
    }

    // supporting HTTP GET in order to reply the wsdl
    soap_data->fget = http_get;

    /* Start up XML-RPC client library. */
    xmlrpc_env env;
    xmlrpc_env_init(&env);
    xmlrpc_client_init2(&env, XMLRPC_CLIENT_NO_FLAGS, NAME, VERSION, NULL, 0);
    xmlrpc_env_clean(&env);

    boost::threadpool::fifo_pool tp(nThreads);

    srmlogit(STORM_LOG_NONE, func, "StoRM frontend successfully started...\n");

    /******************************* main loop ******************************/
    struct soap *tsoap;
    int exit_code = 0;

    while (1) {

        if (soap_accept(soap_data) < 0) {
            srmlogit(STORM_LOG_ERROR, func, "Error in soap_accept(): %s\n", strerror(
                    soap_data->errnum));
            exit_code = 1;
            break;
        }

        if ((tsoap = soap_copy(soap_data)) == NULL) {
            srmlogit(STORM_LOG_ERROR, func, "Error in soap_copy(): %s\n", strerror(ENOMEM));
            exit_code = 1;
            break;
        }

        struct srm_srv_thread_info
                *srm_srv_thread_info = (struct srm_srv_thread_info*) soap_malloc(tsoap,
                        sizeof(struct srm_srv_thread_info));

        tsoap->user = srm_srv_thread_info;

        tp.schedule(boost::bind(process_request, tsoap));

    }

    tp.wait();
    soap_end(soap_data);
    soap_done(soap_data);
    free(soap_data);

    return (exit_code);
}

void *
process_request(void *soap_vp) {
    struct soap *soap = (struct soap *) soap_vp;
    struct srm_srv_thread_info *thip = (struct srm_srv_thread_info *) soap->user;
    soap->recv_timeout = SOAP_RECV_TIMEOUT;
    soap->send_timeout = SOAP_SEND_TIMEOUT;
    soap_serve(soap);

    thip->s = -1;

    soap_end(soap);
    soap_done(soap);
    free(soap);
    return (NULL);
}
