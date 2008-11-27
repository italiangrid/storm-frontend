/*
 * $Id: srmv2.c 2777 2006-08-29 13:20:17Z amessina $
 */

/**
 * \file srmv2.c This file contains the main functions to parse
 * command line, configuration file and start the frontend.
 */

#include "Cinit.h"
#include "Cpool_api.h"
#include "serrno.h"

#include "srmSoapBinding.nsmap"
#include "cgsi_plugin.h"
#include "srm_server.h"
#include "srmv2H.h"

#include "storm_functions.h"
#include "storm_limits.h"

extern "C" {
#include "storm_util.h"
}

#include "srmlogit.h"

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <pwd.h>

#include <xmlrpc-c/base.h>
#include <xmlrpc-c/client.h>

#include <exception>

#include "FrontendOptions.hpp"

#define NAME "StoRM SRM v2.2"

struct srm_srv_thread_info srm_srv_thread_info[SRMV2_NBTHREADS_MAX];
char *db_pwd;
char *db_srvr;
char *db_user;

char* SRMV2_PROXY_DIR;
char* xmlrpc_endpoint;
char* wsdl_file;
char** supported_protocols;

int nb_supported_protocols;
int jid;

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

static int srm_main(struct main_args *main_args) {
    char *func = "srm_main";
    void *process_request(void *);

    // Set pid in global variable
    jid = getpid();

    // ------------------------------------------------------------------------
    //------------------------- Set configuration -----------------------------
    // ------------------------------------------------------------------------

    FrontendOptions configuration;

    try {
        configuration.parseOptions(main_args->argc, main_args->argv);
    } catch (exception& e) {
        cout << e.what() << endl << endl;
        return 1;
    }

    if (configuration.requestedHelp()) {
        configuration.printHelpMessage();
        return 0;
    }

    if (configuration.requestedVersion()) {
        printf("Frontend version: ...");
        return 0;
    }

    int nThreads = configuration.getNumThreads();
    int port = configuration.getPort();
    string log_file = configuration.getLogFile();
    string wsdl_file_path = configuration.getWSDLFilePath();
    string proxy_dir = configuration.getProxyDir();
    string proxy_user = configuration.getProxyUser();
    string xmlrpc_ep = configuration.getXMLRPCEndpoint();
    string user = configuration.getUser();
    string dbHost = configuration.getDBHost();
    string dbUser = configuration.getDBUser();
    string dbUserPasswd = configuration.getDBUserPassword();
    bool debugMode = configuration.requestedDebug();
    int debuglevel = configuration.getDebugLevel();
    string debugLevelString = configuration.getDebugLevelString();
    bool checkGridmapfile = configuration.gridmapfileCheckEnabled();

    // Run using "user" privileges
    if (runAsUser(user) != 0) {
        fprintf(stderr, "Error: cannot run as user \"%s\".\n", user.c_str());
        return 1;
    }

    // Proxy directory
    SRMV2_PROXY_DIR = strdup(proxy_dir.c_str());

    // Proxy User
    if (setProxyUserGlobalVariables(proxy_user) != 0) {
        fprintf(stderr, "Error: request invalid user \"%s\" for proxy dir.\n", proxy_user.c_str());
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
    srmlogit(STORM_LOG_NONE, func, "%s=%s\n", OPTL_LOG_FILE.c_str(), log_file.c_str());
    srmlogit(STORM_LOG_NONE, func, "xmlrpc endpoint=%s\n", xmlrpc_endpoint);
    srmlogit(STORM_LOG_NONE, func, "%s=%s\n", OPTL_DEBUG_LEVEL.c_str(), debugLevelString.c_str());
    srmlogit(STORM_LOG_NONE, func, "%s=%s\n", OPTL_PROXY_DIR.c_str(), SRMV2_PROXY_DIR);
    srmlogit(STORM_LOG_NONE, func, "%s=%s\n", OPTL_PROXY_USER.c_str(), proxy_user.c_str());
    srmlogit(STORM_LOG_NONE, func, "%s=%s\n", OPTL_DB_HOST.c_str(), db_srvr);
    srmlogit(STORM_LOG_NONE, func, "%s=%s\n", OPTL_DB_USER.c_str(), db_user);
    srmlogit(STORM_LOG_NONE, func, "%s=%s\n", OPTL_DB_USER_PASSWORD.c_str(), db_pwd);
    srmlogit(STORM_LOG_NONE, func, "%s=%s\n", OPTL_WSDL_FILE.c_str(), wsdl_file);
    if (checkGridmapfile)
        srmlogit(STORM_LOG_NONE, func, "%s=true\n", OPTL_CHECK_GRIDMAPFILE.c_str());
    else
        srmlogit(STORM_LOG_NONE, func, "%s=false\n", OPTL_CHECK_GRIDMAPFILE.c_str());
    srmlogit(STORM_LOG_NONE, func, "-------------------------------------------------------\n");

    /* Get list of supported protocols */
    if ((nb_supported_protocols = get_supported_protocols(&supported_protocols)) < 0) {
        srmlogit(STORM_LOG_ERROR, func, SRM02, "get_supported_protocols", strerror(ENOMEM));
        return (SYERR);
    }

    if (0 != check_environment()) {
        srmlogit(STORM_LOG_ERROR, func, "Environment not safe! Exiting\n");
        exit(CONFERR);
    }

    if (! debugMode) { // fork and leave the daemon in background
        if (Cinitdaemon("srmv2", NULL) < 0) {
            exit(SYERR);
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
    if (!chcheckGridmapfile) {
        flags &&= CGSI_OPT_DISABLE_NAME_CHECK;
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

    /* Create a pool of threads */
    int ipool;
    if ((ipool = Cpool_create(nThreads, NULL)) < 0) {
        srmlogit(STORM_LOG_DEBUG, func, SRM02, "Cpool_create", sstrerror(serrno));
        exit(SYERR);
    }

    // Initialize structure srm_srv_thread_info
    int i;
    for (i = 0; i < nThreads; i++) {
        srm_srv_thread_info[i].s = -1;
        srm_srv_thread_info[i].dbfd.idx = i;
    }
    srm_srv_thread_info[nThreads].s = -1;
    srm_srv_thread_info[nThreads].dbfd.idx = -1;

    srmlogit(STORM_LOG_NONE, func, "StoRM frontend successfully started...\n");

    /******************************* main loop ******************************/
    struct soap *tsoap;
    int thread_index;

    while (1) {
        if (soap_accept(soap_data) < 0) {
            srmlogit(STORM_LOG_ERROR, func, SRM02, "soap_accept", strerror(soap_data->errnum));
            soap_done(soap_data);
            return 1;
        }
        if ((tsoap = soap_copy(soap_data)) == NULL) {
            srmlogit(STORM_LOG_ERROR, func, SRM02, "soap_copy", strerror(ENOMEM));
            soap_done(soap_data);
            free(soap_data);
            return 1;
        }
        if ((thread_index = Cpool_next_index (ipool)) < 0) {
            srmlogit(STORM_LOG_ERROR, func, SRM02, "Cpool_next_index", sstrerror(serrno));
            return SYERR;
        }
        tsoap->user = &srm_srv_thread_info[thread_index];
        if (Cpool_assign (ipool, &process_request, tsoap, 1) < 0) {
            free(tsoap);
            srm_srv_thread_info[thread_index].s = -1;
            srmlogit(STORM_LOG_ERROR, func, SRM02, "Cpool_assign", sstrerror(serrno));
            return SYERR;
        }
    }
    soap_done(soap_data);
    free(soap_data);
    return (0);
}

int main(int argc, char **argv) {
#if ! defined(_WIN32)
    struct main_args main_args;
    int ret;
    main_args.argc = argc;
    main_args.argv = argv;
    exit(srm_main(&main_args));

#else
    if (Cinitservice ("srmv2", &srm_main))
    exit (SYERR);
#endif
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
