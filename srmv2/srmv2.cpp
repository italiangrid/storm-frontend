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

#include <getopt.h>
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

#include "FrontendOptions.hpp"

#define NAME "StoRM SRM v2.2"

char db_pwd[33];
char db_srvr[33];
char db_user[33];
struct srm_srv_thread_info srm_srv_thread_info[SRMV2_NBTHREADS_MAX];
int jid;
char localdomain[ST_MAXHOSTNAMELEN+1];
char localhost[ST_MAXHOSTNAMELEN+1];
char logfile[ST_MAXPATHLEN+1];
char *wsdl_file = NULL;

char *SRMV2_PROXY_DIR;
uid_t proxy_uid = 0;
gid_t proxy_gid = 0;
char *xmlrpc_endpoint = NULL;

int nb_supported_protocols;
char **supported_protocols;

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
    if (NULL == gridmap || NULL == gridmap)
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

static int srm_main(struct main_args *main_args) {
    char *func = "srm_main";
    void *process_request(void *);

    // Set pid in global variable
    jid = getpid();

    // ------------------------------------------------------------------------
    //------------------------- Set configuration -----------------------------
    // ------------------------------------------------------------------------

    FrontendOptions configuration;

    configuration.parseOptions(main_args->argc, main_args->argv);

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

    // Setting global variables... TODO: do not use global variables
    // DB stuff
    strcpy(db_srvr, dbHost.c_str());
    strcpy(db_user, dbUser.c_str());
    strcpy(db_pwd, dbUserPasswd.c_str());

    // Logfile
    strcpy(logfile, log_file.c_str());

    // Proxy directory
    SRMV2_PROXY_DIR = strdup(proxy_dir.c_str());

    // User
    struct passwd *pwd;
    pwd = getpwnam(user.c_str());
    if (NULL == pwd) { // error
        fprintf(stderr, "Invalid user: %s\n", user.c_str());
        return CONFERR;
    }
    gid_t drop_gid = pwd->pw_gid;
    uid_t drop_uid = pwd->pw_uid;

    // Proxy User
    pwd = getpwnam(optarg);
    if (NULL == pwd) { // error
        fprintf(stderr, "Invalid user for proxy: %s\n", proxy_user.c_str());
        return CONFERR;
    }
    proxy_uid = pwd->pw_uid;
    proxy_gid = pwd->pw_gid;

    // WSDL file
    wsdl_file = strdup(wsdl_file_path.c_str());

    // XMLRPC
    xmlrpc_endpoint = strdup(xmlrpc_ep.c_str());

    // ------------------------------------------------------------------------
    //------------------- Done: set configuration -----------------------------
    // ------------------------------------------------------------------------

    srmlogit_set_debuglevel(debuglevel);

    srmlogit(STORM_LOG_INFO, func, "Starting StoRM frontend as user: %s", user.c_str());
    srmlogit(STORM_LOG_INFO, func, "Configuration information: ----------------------------");
    srmlogit(STORM_LOG_INFO, func, "%s=%d\n", OPTL_NUM_THREADS.c_str(), nThreads);
    srmlogit(STORM_LOG_INFO, func, "%s=%d\n", OPTL_PORT.c_str(), port);
    srmlogit(STORM_LOG_INFO, func, "%s=%s\n", OPTL_LOG_FILE.c_str(), logfile);
    srmlogit(STORM_LOG_INFO, func, "xmlrpc-endpoint=%s\n", xmlrpc_endpoint);
    srmlogit(STORM_LOG_INFO, func, "%s=%s\n", OPTL_DEBUG_LEVEL.c_str(), debugLevelString.c_str());
    srmlogit(STORM_LOG_INFO, func, "%s=%s (to store proxies for srmCopy)\n", SRMV2_PROXY_DIR);
    srmlogit(STORM_LOG_INFO, func, "%s=%s\n", OPTL_PROXY_USER.c_str(), proxy_user.c_str());
    srmlogit(STORM_LOG_INFO, func, "%s=%s\n", OPTL_DB_HOST.c_str(), db_srvr);
    srmlogit(STORM_LOG_INFO, func, "%s=%s\n", OPTL_DB_USER.c_str(), db_user);
    srmlogit(STORM_LOG_INFO, func, "%s=%s\n", OPTL_DB_USER_PASSWORD.c_str(), db_pwd);
    srmlogit(STORM_LOG_INFO, func, "%s=%s\n", OPTL_WSDL_FILE.c_str(), wsdl_file);
    srmlogit(STORM_LOG_INFO, func, "-------------------------------------------------------");


    gethostname(localhost, ST_MAXHOSTNAMELEN+1);
    if (strchr(localhost, '.') == NULL) {
        strcat(localhost, ".");
        strcat(localhost, localdomain);
    }

    /* Get list of supported protocols */
    if ((nb_supported_protocols = get_supported_protocols(&supported_protocols)) < 0) {
        srmlogit(STORM_LOG_ERROR, func, SRM02, "get_supported_protocols", strerror(ENOMEM));
        return (SYERR);
    }

    if (0 != check_environment()) {
        srmlogit(STORM_LOG_ERROR, func, "Environment not safe! Exiting\n");
        exit(CONFERR);
    }

    srmlogit(STORM_LOG_INFO, func, "StoRM frontend started\n");

    if (debugMode) {
        if (Cinitdaemon("srmv2", NULL) < 0)
            exit(SYERR);
        srmlogit_init();
    }

    /* Create a pool of threads */
    int ipool;
    if ((ipool = Cpool_create (nThreads, NULL)) < 0) {
        srmlogit(STORM_LOG_DEBUG, func, SRM02, "Cpool_create", sstrerror(serrno));
        return (SYERR);
    }

    int i;
    for (i = 0; i < nThreads; i++) {
        srm_srv_thread_info[i].s = -1;
        srm_srv_thread_info[i].dbfd.idx = i;
    }
    srm_srv_thread_info[nThreads].s = -1;
    srm_srv_thread_info[nThreads].dbfd.idx = -1;

#if ! defined(_WIN32)
    signal(SIGPIPE, SIG_IGN);
    signal(SIGXFSZ, SIG_IGN);
#endif

    struct soap *soap_data;
    soap_data = (struct soap *) calloc(1, sizeof(struct soap));
    soap_init2(soap_data, SOAP_IO_KEEPALIVE, SOAP_IO_KEEPALIVE);
    soap_data->max_keep_alive = SOAP_MAX_KEEPALIVE;
    soap_data->accept_timeout = 0;
    int flags;
#if defined(GSI_PLUGINS)
    flags = CGSI_OPT_DELEG_FLAG;
    soap_register_plugin_arg(soap_data, server_cgsi_plugin, &flags);
#endif
    soap_data->bind_flags |= SO_REUSEADDR;

    if (soap_bind(soap_data, NULL, port, BACKLOG) < 0) {
        srmlogit(STORM_LOG_ERROR, func, SRM02, "soap_bind", strerror(soap_data->errnum));
        soap_done(soap_data);
        free(soap_data);
        return (SYERR);
    }
    srmlogit(STORM_LOG_DEBUG, func, "Port Number %d\n", port);

    /* supporting HTTP GET in order to reply the wsdl */
    soap_data->fget = http_get;

    /* Start up our XML-RPC client library. */
    xmlrpc_env env;
    xmlrpc_env_init(&env);
    xmlrpc_client_init2(&env, XMLRPC_CLIENT_NO_FLAGS, NAME, VERSION, NULL, 0);
    //die_if_fault_occurred(&env);
    xmlrpc_env_clean(&env);

    /* main loop */
    if (drop_uid != 0 || drop_gid != 0) {

        if (setgid(drop_gid) || setuid(drop_uid)) {
            srmlogit(STORM_LOG_ERROR, func, "Unable to drop privileges to %d,%d. Exiting.\n",
                    drop_uid, drop_gid);
            return 1;
        }
        srmlogit(STORM_LOG_INFO, func, "StoRM FE started as (uid,gid) = (%d,%d)\n", drop_uid,
                drop_gid);
    } else {
        srmlogit(STORM_LOG_INFO, func, "StoRM FE started.\n");
    }

    struct soap *tsoap;
    int thread_index;

    while (1) {
        if (soap_accept(soap_data) < 0) {
            srmlogit(STORM_LOG_ERROR, func, SRM02, "soap_accept", strerror(soap_data->errnum));
            soap_done(soap_data);
            return (1);
        }
        if ((tsoap = soap_copy(soap_data)) == NULL) {
            srmlogit(STORM_LOG_ERROR, func, SRM02, "soap_copy", strerror(ENOMEM));
            soap_done(soap_data);
            free(soap_data);
            return (1);
        }
        if ((thread_index = Cpool_next_index (ipool)) < 0) {
            srmlogit(STORM_LOG_ERROR, func, SRM02, "Cpool_next_index", sstrerror(serrno));
            return (SYERR);
        }
        tsoap->user = &srm_srv_thread_info[thread_index];
        if (Cpool_assign (ipool, &process_request, tsoap, 1) < 0) {
            free(tsoap);
            srm_srv_thread_info[thread_index].s = -1;
            srmlogit(STORM_LOG_ERROR, func, SRM02, "Cpool_assign", sstrerror(serrno));
            return (SYERR);
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

int serrno2statuscode(int err) {
    switch (err) {
    case ENOENT:
    case ENOTDIR:
    case ENAMETOOLONG:
        return (SRM_USCOREINVALID_USCOREPATH);
    case EACCES:
        return (SRM_USCOREAUTHORIZATION_USCOREFAILURE);
    case EEXIST:
        return (SRM_USCOREDUPLICATION_USCOREERROR);
    case EINVAL:
        return (SRM_USCOREINVALID_USCOREREQUEST);
    case ENOSPC:
        return (SRM_USCORENO_USCOREFREE_USCORESPACE);
    default:
        return (SRM_USCOREFAILURE);
    }
}
