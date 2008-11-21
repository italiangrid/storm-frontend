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

//extern "C" {
    #include "stdsoap2.h"
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

#define NAME "StoRM SRM v2.2"

char db_pwd[33];
char db_srvr[33];
char db_user[33];
struct srm_srv_thread_info srm_srv_thread_info[SRMV2_NBTHREADS_MAX];
char func[16];
int jid;
char localdomain[ST_MAXHOSTNAMELEN+1];
char localhost[ST_MAXHOSTNAMELEN+1];
char logfile[ST_MAXPATHLEN+1];
char *wsdl_file=NULL;

char *SRMV2_PROXY_DIR=NULL;
uid_t proxy_uid=0;
gid_t proxy_gid=0;
char *xmlrpc_endpoint=NULL;


int nb_supported_protocols;
char **supported_protocols;
//char protocols[1][5];
//char *myProtocol = "file"; /*HS*/


/**
 * check environment variables contents.
 *
 * @return 0 if environment is safe
 * 
 * @return -1 otherwise
 */
int check_environment()
{
    char *gridmap;
    char *gridmapdir;
    struct stat stbuf;
    int ret;

#ifdef GSI_PLUGINS
    /* environment is set? */
    gridmap=getenv("GRIDMAP");
    gridmapdir=getenv("GRIDMAPDIR");
    if(NULL == gridmap || NULL == gridmap)
        return -1;

    /* grid-mapfile is a file? */
    ret=stat(gridmap, &stbuf);
    if(0 != ret || ! S_ISREG(stbuf.st_mode))
        return -1;

    /* grid-mapfile file is readable? */
    if( 0 != access(gridmap, R_OK) )
        return -1;

    /* gridmapdir is a directory? */
    ret = stat(gridmapdir,&stbuf);
    if(0 != ret || !S_ISDIR(stbuf.st_mode))
        return -1;

    /* gridmapdir is readable and executable? */
    if( 0 != access(gridmapdir, R_OK & X_OK) )
        return -1;
#endif            
    return 0;
}

/**
 *  print usage for storm-frontend
 *  
 *  @param  fd       The filedescriptor to use
 *
 *  @param progname  The program name (<code>argv[0]</code>, for
 *                   exemple)
 *  
 *  @return -1 in case of error
 *
 *  @return 0 otherwise.
 */
int print_usage(FILE *fd, const char *progname)
{

    int rc=fprintf(fd,"Usage: %s [options]\n"
            "Valid options are:\n\n"
            "  --num-threads, -t N   start N threads\n"
            "  --conffile, -c FILE   use 'file' as configuration file\n"
            "  --logfile, -l  FILE   use 'file' as log file\n"
            "  --proxy-dir, -P DIR   Directory in which the frontend will store\n"
            "                        proxyes delegated by the client\n"
            "  --port, -p N          listen to port N\n"
            "  --xmlrpc-endpoint, -x  ENDPOINT\n"
            "                        Specify the XMLRPC endpoint of the StoRM Backend.\n"
            "                        ENDPOINT is in one of the following form:\n" 
            "                           [HOST][:PORT][/PATH]\n"
            "                        where the defaults are:\n"
            "                            HOST = %s\n"
            "                            PORT = %s\n"
            "                            PATH = %s\n"
            "  --proxy-user, -U USER save the proxy certificate using USER's uid and gid\n"
            "  --user, -u USER       drop privileges to user USER\n"
/*             "  --proxy-uid, -u N     save the proxy certificate using uid N " */
/*             "  --proxy-gid, -g N     save the proxy certificate using gid N " */
            "  --wsdl-file, -w FILE  path to the WSDL to publish in case of GET request\n"
            "  --verbose, -v         Verbose mode. Cause %s to print debugging messages.  \n"
            "                        Multiple -v options increse the verbosity.\n"
            "  --debug, -d           start in debug-mode: do not exec fork() and stay in foreground\n"
            "                        In debug-mode the logfile is not opened, and all the log are\n"
            "                        printed to standard error.\n"
            "  --help, -h            Print this unuseful help\n",progname,DEFAULT_XMLRPC_HOST,DEFAULT_XMLRPC_PORT,DEFAULT_XMLRPC_PATH,progname);
    if(rc<0)
        return -1;

    return 0;
}

static int http_get(struct soap *soap){
    
    int fd = open(wsdl_file,O_RDONLY|O_NONBLOCK);
    char buf[10240]; /* 10k */
    int nr=0;

    srmlogit(STORM_LOG_DEBUG,"http_get","Ricevo richiesta GET\n");
    soap_response(soap, SOAP_HTML); // HTTP response header with text/html 
    if(-1 == fd){
        srmlogit(STORM_LOG_ERROR,"http_get","Error opening file %s. (%s)\n",wsdl_file,strerror(errno));
        soap_send(soap, "<html><body>Error in GET method</body><html>\n");
    }else{
        memset(buf,0,10240); /* uhmm... R&K: this should be already zero-ed... */
        while((nr = read(fd,buf,10239)) >0){
            if(nr < 10239)
                buf[nr]='\0';
            soap_send(soap, buf);
        }
        close(fd);
    }
    soap_end_send(soap);
    return SOAP_OK;
}

static int srm_main(struct main_args *main_args)
{
    int c;
    FILE *cf;
    char cfbuf[80];
    void *process_request(void *);
    char configfile[ST_MAXPATHLEN+1];
    char num_threads[20];
    int flags;
    int threads;
    char *getconfent();
    int i;
    int ipool;
    char *p;
    char *p_p, *p_s, *p_u;
    int port=-1;
    struct soap *soap_data;
    int thread_index;
    struct soap *tsoap;
    xmlrpc_env env;
    int debug=0;
    int debuglevel=STORM_LOG_ERROR;
    uid_t drop_gid=0;
    gid_t drop_uid=0;
    static struct option long_options[] = 
        {
            {"help",no_argument,0,'h'},
            {"debug",no_argument,0,'d'},
            {"verbose",no_argument,0,'v'},
            {"port",required_argument,0,'p'},
            {"proxy-dir",required_argument,0,'P'},
            {"proxy-uid",required_argument,0,'u'},
            {"proxy-gid",required_argument,0,'g'},
            {"proxy-user",required_argument,0,'U'},
            {"num-threads",required_argument,0,'t'},
            {"conffile",required_argument,0,'c'},
            {"logfile",required_argument,0,'l'},
            {"wsdl-file",required_argument,0,'w'},
            {"xmlrpc-endpoint",required_argument,0,'x'},
            {0, 0, 0, 0}
        };
    int option_index = 0;

    jid = getpid();
    strcpy (func, "srmv2");
    configfile[0] = '\0';
    strcpy (logfile, LOGFILE);
    num_threads[0] = '\0';
        threads = SRMV2_NBTHREADS;  

    /* process command line options if any */
    SRMV2_PROXY_DIR=NULL;
    while ((c = getopt_long (main_args->argc, main_args->argv, "P:p:c:l:t:x:w:u:U:dhv",
                             long_options, &option_index)) != EOF) {
        switch (c) {
        case 'c':
            strncpy (configfile, optarg, sizeof(configfile));
            configfile[sizeof(configfile) - 1] = '\0';
            break;
        case 'l':
            strncpy (logfile, optarg, sizeof(logfile));
            logfile[sizeof(logfile) - 1] = '\0';
            break;
        case 't':
            strncpy (num_threads, optarg, sizeof(num_threads));
                        num_threads[sizeof(num_threads) - 1] = '\0';
            threads = atoi(num_threads);
                        break;
        case 'p':
            port=atoi(optarg);
            break;
        case 'P':
            SRMV2_PROXY_DIR=strdup(optarg);
            break;
        case 'u':
        {
            struct passwd *pwd=getpwnam(optarg);
            if(NULL == pwd){
                /* error */
                fprintf(stderr, "Invalid user\n");
                print_usage(stderr,main_args->argv[0]);
                return CONFERR;
            }
            drop_gid=pwd->pw_gid;
            drop_uid=pwd->pw_uid;
        }
            break;
        case 'U':
        {
            struct passwd *pwd=getpwnam(optarg);
            if(NULL == pwd){
                /* error */
                fprintf(stderr, "Invalid user\n");
                print_usage(stderr,main_args->argv[0]);
                return CONFERR;
            }
            proxy_uid = pwd->pw_uid;
            proxy_gid = pwd->pw_gid;
        }
            break;
        case 'd':
            debug=1;
            break;
        case 'v':
            debuglevel++;
            break;
        case 'w':
            wsdl_file=strdup(optarg);
            break;
        case 'x':
        {
            int len=8; /* len of "http://" string  and the '\0' char */
            int hlen=0;
            int polen=0;
            int palen=0;
            int optlen;
            int ret;
            char *h=NULL,*po=NULL,*pa=NULL;

            po = strchr(optarg,':');
            pa = strchr(optarg,'/');
            optlen=strlen(optarg);

            if(':' == *optarg || '/' == *optarg){
                hlen = strlen(DEFAULT_XMLRPC_HOST);
                h = DEFAULT_XMLRPC_HOST;
            }else{
                h=optarg;
                if(NULL == po)
                    if(NULL == pa)
                        hlen=optlen;
                    else
                        hlen = pa-h;
                else
                    hlen = po-h;
            }
            len += hlen;

            if(NULL == po){
                po=DEFAULT_XMLRPC_PORT;
                polen=strlen(DEFAULT_XMLRPC_PORT);
            }else{
                po++;
                if(NULL == pa)
                    if(h == optarg)
                        polen=optlen-hlen;
                    else
                        polen=optlen;
                else
                    polen=pa-po;
            }
            len+=polen;
            len+=1;

            if(NULL == pa){
                pa=DEFAULT_XMLRPC_PATH;
                palen=strlen(DEFAULT_XMLRPC_PATH);
            }else
                palen=strlen(pa);
            
            len+=palen;
            len+=1;
                
            xmlrpc_endpoint = (char *) calloc(len, sizeof(char));

            strncat(xmlrpc_endpoint,"http://",7);
            strncat(xmlrpc_endpoint,h, hlen);
            strncat(xmlrpc_endpoint,":",1);
            strncat(xmlrpc_endpoint,po,polen);
            strncat(xmlrpc_endpoint,pa,palen);
        }
            break;
        case 'h':
        case '?':
            print_usage(stderr,main_args->argv[0]);
            return CONFERR;
        }
    }

    srmlogit(0,"srm_main","verbose level = %d\n", debuglevel);
    srmlogit_set_debuglevel(debuglevel);


    if(NULL == xmlrpc_endpoint)
        xmlrpc_endpoint = strdup(DEFAULT_XMLRPC_ENDPOINT);
    
    if(NULL == SRMV2_PROXY_DIR)
        SRMV2_PROXY_DIR=strdup(SRMV2_PROXY_DEFAULT_DIR);

    if(-1 == port)
        port = SRMV2_PORT;
    
    srmlogit(STORM_LOG_INFO, func, "started\n");
    srmlogit(STORM_LOG_DEBUG, func, "Using '%s' as XMLRPC endpoint\n", xmlrpc_endpoint);
    srmlogit(STORM_LOG_DEBUG, func, "Proxy directory for srmCopy: %s\n", SRMV2_PROXY_DIR);

    gethostname (localhost, ST_MAXHOSTNAMELEN+1);
/*     if (Cdomainname (localdomain, sizeof(localdomain)) < 0) { */
/*         srmlogit(STORM_LOG_ERROR, func, "Unable to get local domain name\n"); */
/*         return (2); */
/*     } */
    if (strchr (localhost, '.') == NULL) {
        strcat (localhost, ".");
        strcat (localhost, localdomain);
    }

    /* Get list of supported protocols */
/*
    if ((nb_supported_protocols = get_supported_protocols (&supported_protocols)) < 0) {
        srmlogit (func, SRM02, "get_supported_protocols", strerror (ENOMEM));
        return (SYERR);
    }
*/
/* FLAVIA
    nb_supported_protocols = 1;
    strcpy(protocols[0],"file"); 
    supported_protocols = (char **)protocols;

*/
/* HS we need the number of supported protocols and the protocol here.
      TODO: dynamically add more protocols
        nb_supported_protocols = 1;
        supported_protocols = (char**) malloc(5);
        supported_protocols[0] = myProtocol;
        srmlogit(func, "DEBUG file protocol is used by default\n");

end HS */
/******* FLAVIA TO DO *******/
    /* get DB login info from the StoRM server config file */

    if (! *configfile) {
        if (strncmp (STORM_NSCONFIG, "%SystemRoot%\\", 13) == 0 &&
            (p = getenv ("SystemRoot")))
               sprintf (configfile, "%s%s", p, strchr (STORM_NSCONFIG, '\\'));
        else
               strcpy (configfile, STORM_NSCONFIG);
    }
    if ((cf = fopen (configfile, "r")) == NULL) {
        srmlogit(STORM_LOG_ERROR, func, SRM23, configfile);
        return (CONFERR);
    }
    if (fgets (cfbuf, sizeof(cfbuf), cf) &&
        strlen (cfbuf) >= 5 && (p_u = strtok (cfbuf, "/\n")) &&
        (p_p = strtok (NULL, "@\n")) && (p_s = strtok (NULL, "\n"))) {
        strcpy (db_user, p_u);
        strcpy (db_pwd, p_p);
        strcpy (db_srvr, p_s);
    } else {
        srmlogit (STORM_LOG_ERROR, func, SRM09, configfile, "incorrect");
        return (CONFERR);
    }
    (void) fclose (cf);

    /* Get list of supported protocols */

    if ((nb_supported_protocols = get_supported_protocols (&supported_protocols)) < 0) {
        srmlogit (STORM_LOG_ERROR, func, SRM02, "get_supported_protocols", strerror (ENOMEM));
        return (SYERR);
    }

    if(0 != check_environment()){
        srmlogit(STORM_LOG_ERROR, func,"Environment not safe! Exiting\n");
        exit (CONFERR);
    }
        
    if(0 == debug) {
        if (Cinitdaemon ("srmv2", NULL) < 0)
            exit (SYERR);
        srmlogit_init();
    }
//    (void) storm_init_dbpkg ();

    /* Create a pool of threads */

        srmlogit (STORM_LOG_DEBUG, func, "Number of threads %d\n", threads);
    if ((ipool = Cpool_create (threads, NULL)) < 0) {
        srmlogit (STORM_LOG_DEBUG, func, SRM02, "Cpool_create", sstrerror (serrno));
        return (SYERR);
    }
    for (i = 0; i < threads; i++) {
        srm_srv_thread_info[i].s = -1;
        srm_srv_thread_info[i].dbfd.idx = i;
    }
        srm_srv_thread_info[threads].s = -1;
    srm_srv_thread_info[threads].dbfd.idx = -1;

#if ! defined(_WIN32)
    signal (SIGPIPE, SIG_IGN);
    signal (SIGXFSZ, SIG_IGN);
#endif
    soap_data = (struct soap *) calloc(1, sizeof( struct soap ));
    soap_init2(soap_data,SOAP_IO_KEEPALIVE, SOAP_IO_KEEPALIVE);
    soap_data->max_keep_alive = SOAP_MAX_KEEPALIVE; 
    soap_data->accept_timeout = 0;
#if defined(GSI_PLUGINS)
    flags = CGSI_OPT_DELEG_FLAG;
    soap_register_plugin_arg(soap_data, server_cgsi_plugin, &flags);
#endif
// not supported in gsoap 2.6
//    soap_data->bind_flags |= MSG_NOSIGNAL;
    soap_data->bind_flags |= SO_REUSEADDR;

    if (soap_bind(soap_data, NULL, port, BACKLOG) < 0) {
        srmlogit (STORM_LOG_ERROR, func, SRM02, "soap_bind", strerror (soap_data->errnum));
        soap_done(soap_data);
        free(soap_data);
        return (SYERR);
    }
    srmlogit (STORM_LOG_DEBUG, func, "Port Number %d\n", port);

    /* supporting HTTP GET in order to reply the wsdl */
    soap_data->fget = http_get; 

        /* Start up our XML-RPC client library. */
        xmlrpc_env_init(&env);
        xmlrpc_client_init2(&env, XMLRPC_CLIENT_NO_FLAGS, NAME, VERSION, NULL, 0);
        //die_if_fault_occurred(&env);
        xmlrpc_env_clean(&env);


    /* main loop */
        if(drop_uid!=0 || drop_gid != 0){
            if(setgid(drop_gid)
               || setuid(drop_uid)){
                srmlogit(STORM_LOG_ERROR, func, "Unable to drop privileges to %d,%d. Exiting.\n",drop_uid,drop_gid);
                return 1;
            }
            srmlogit(STORM_LOG_INFO, func, "StoRM FE started as (uid,gid) = (%d,%d)\n",drop_uid,drop_gid);
        }else
            srmlogit(STORM_LOG_INFO, func, "StoRM FE started.\n");
    while (1) {
        if (soap_accept (soap_data) < 0) {
            srmlogit (STORM_LOG_ERROR, func, SRM02, "soap_accept", strerror (soap_data->errnum));
            soap_done (soap_data);
            return (1);
        }
        if ((tsoap = soap_copy (soap_data)) == NULL) {
            srmlogit (STORM_LOG_ERROR, func, SRM02, "soap_copy", strerror (ENOMEM));
            soap_done (soap_data);
            free(soap_data);
            return (1);
        }
        if ((thread_index = Cpool_next_index (ipool)) < 0) {
            srmlogit (STORM_LOG_ERROR, func, SRM02, "Cpool_next_index",
                sstrerror (serrno));
            return (SYERR);
        }
        tsoap->user = &srm_srv_thread_info[thread_index];
        if (Cpool_assign (ipool, &process_request, tsoap, 1) < 0) {
            free (tsoap);
            srm_srv_thread_info[thread_index].s = -1;
            srmlogit (STORM_LOG_ERROR, func, SRM02, "Cpool_assign", sstrerror (serrno));
            return (SYERR);
        }
    }
    soap_done (soap_data);
    free(soap_data);
    return (0);
}

main(int argc, char **argv)
{
#if ! defined(_WIN32)
    struct main_args main_args;
    int ret;
    main_args.argc = argc;
    main_args.argv = argv;
    exit(srm_main (&main_args));
    
#else
    if (Cinitservice ("srmv2", &srm_main))
        exit (SYERR);
#endif
}

void *
process_request(void *soap_vp)
{
    struct soap *soap = (struct soap *)soap_vp;
    struct srm_srv_thread_info *thip = (struct srm_srv_thread_info *) soap->user;
    soap->recv_timeout = SOAP_RECV_TIMEOUT;
    soap->send_timeout = SOAP_SEND_TIMEOUT;
    soap_serve (soap);

    thip->s = -1;
    
    soap_end (soap);
    soap_done (soap);
    free (soap);
    return (NULL);
}

int
serrno2statuscode (int err)
{
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
