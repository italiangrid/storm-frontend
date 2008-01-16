// $Id$

/* TODO: let the user choose if recursive flag should be set or not */
#include "srmv2H.h"
#include "srmSoapBinding.nsmap"
#include "storm_util.h"

#define DEFPOLLINT 10

#define SRM_EP_PATH "/v2_1_1/srm"
#ifdef GSI_PLUGINS
#include "cgsi_plugin.h"
#endif
/*
parsesurl (const char *surl, char **endpoint, char **sfn)
{
	int len;
	int lenp;
	char *p;
	static char srm_ep[256];

	if (strncmp (surl, "srm://", 6)) {
		errno = EINVAL;
		return (-1);
	}
	if (p = strstr (surl + 6, "?SFN=")) {
		*sfn = p + 5;
	} else if (p = strchr (surl + 6, '/')) {
		*sfn = p;
	} else {
		errno = EINVAL;
		return (-1);
	}
#ifdef GSI_PLUGINS
	strcpy (srm_ep, "https://");
	lenp = 8;
#else
	strcpy (srm_ep, "http://");
	lenp = 7;
#endif
	len = p - surl - 6;
	if (lenp + len >= sizeof(srm_ep)) {
		errno = EINVAL;
		return (-1);
	}
	strncpy (srm_ep + lenp, surl + 6, len);
	*(srm_ep + lenp + len) = '\0';
	if (strchr (srm_ep + lenp, '/') == NULL) {
		if (strlen (SRM_EP_PATH) + lenp + len >= sizeof(srm_ep)) {
			errno = EINVAL;
			return (-1);
		}
		strcat (srm_ep, SRM_EP_PATH);
	}
	*endpoint = srm_ep;
	return (0);
}
*/

main(int argc,char** argv)
{
	int flags;
	int i;
	int nbfiles;
	int nbproto = 0;
	static char *protocols[] = {
#if GFAL_ENABLE_RFIO
		"rfio",
#endif
#if GFAL_ENABLE_DCAP
		"gsidcap",
#endif
		"file",
		""
	};
	int r = 0;
	char *r_token;
	struct ns1__srmRmdirResponse_ rep;
	struct ns1__srmRmdirRequest req;
	struct ns1__TReturnStatus *reqstatp;
	int recursive;	
	char *dummy;
	char *p;
	char *sfn;
	struct soap soap;
	char *srm_endpoint;
	
	// Print Help Menu
	printLine();
	printLine();
	printChar();
	printf("SrmRmDir Request \n");
	printLine();


	
	if (argc < 2) {
                fprintf(stderr, "Usage: %s SURL [options]\n", argv[0]);
                fprintf(stderr, "   options:\n");
                fprintf(stderr, "       recursive={0|1} \n");
		printLine();
		exit(1);
	}
/*	
	if (argc < 2) {
		fprintf (stderr, "usage: %s SURL\n", argv[0]);
		exit (1);
	}
*/
	if (parseSURL_getEndpoint (argv[1], &srm_endpoint, &sfn) < 0) {
		//perror ("parsesurl");
		exit (1);
	}

	while (*protocols[nbproto]) nbproto++;

	soap_init (&soap);
#ifdef GSI_PLUGINS
	flags = CGSI_OPT_DISABLE_NAME_CHECK | CGSI_OPT_DELEG_FLAG;
	soap_register_plugin_arg (&soap, client_cgsi_plugin, &flags);
#endif

    memset (&req, 0, sizeof(req));
    
    //Get Option
    recursive = 0;
    for (i=0; i<(argc-2); i++) {
        if ((p = strstr(argv[i+2], "recursive=")) != NULL) {
            dummy = (char*) malloc(10);
            strcpy(dummy, p+10);
            recursive = atoi(dummy);
            if ( (recursive<0) || (recursive>2)) {
                printChar();
                fprintf(stderr, "Error: recursive can only be 0 or 1 \n");
                printLine();
                exit(-1);
            }
            printChar();
            printf("Recursive: %i \n", recursive);
            free(dummy);
        }
    }  
    
    req.authorizationID = NULL;
    req.directoryPath = argv[1];
    req.storageSystemInfo = NULL;
    req.recursive = &recursive;
    
    printf("Parameter storageSystemInfo is NULL\n");
	//Assign Recursive

    if (soap_call_ns1__srmRmdir (&soap, srm_endpoint, "Rmdir", &req, &rep)) {
        soap_print_fault (&soap, stderr);
        soap_print_fault_location (&soap, stderr);
        soap_end(&soap);
        exit (1);
    }
    
	reqstatp = rep.srmRmdirResponse->returnStatus;
	printChar();
	printf ("Request Status [%d] : [%s]\n", reqstatp->statusCode,reconvertStatusCode(reqstatp->statusCode));
	if (reqstatp->explanation) {
            printChar();
            printf ("Explanation: [%s]\n", reqstatp->explanation);
        }
	if (reqstatp->statusCode != SRM_USCORESUCCESS &&
	    reqstatp->statusCode != SRM_USCOREDONE) {
		soap_end (&soap);
		exit (1);
		prinLine();
		
	}

	printLine();

	soap_end (&soap);
	exit (0);
}
