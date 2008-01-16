// $Id$

#include "srmv2H.h"
#include "srmSoapBinding.nsmap"
#include "storm_util.h"

#define DEFPOLLINT 10

#define SRM_EP_PATH "/v2_1_1/srm"
#ifdef GSI_PLUGINS
#include "cgsi_plugin.h"
#endif

main(int argc, char** argv)
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
	struct ns1__srmMkdirResponse_ rep;
	struct ns1__srmMkdirRequest req;
	struct ns1__TReturnStatus *reqstatp;
	char *sfn;
	struct soap soap;
	char *srm_endpoint;
	
	// Print Help Menu
	printLine();
	printLine();
	printChar();
	printf("SrmMkdir Request \n");
	printLine();


	if (argc < 2) {
		fprintf (stderr, "Usage: %s SURL\n", argv[0]);
		printLine();
		exit (1);
	}

	if (parseSURL_getEndpoint(argv[1], &srm_endpoint, &sfn) < 0) {
		//perror ("parsesurl");
		exit (1);
	}

	while (*protocols[nbproto]) nbproto++;

	soap_init (&soap);
#ifdef GSI_PLUGINS
	flags = CGSI_OPT_DISABLE_NAME_CHECK | CGSI_OPT_DELEG_FLAG;
	soap_register_plugin_arg(&soap, client_cgsi_plugin, &flags);
#endif

	memset (&req, 0, sizeof(req));
    
    req.authorizationID = NULL;
	req.directoryPath = argv[1];
	req.storageSystemInfo = NULL;

	if (soap_call_ns1__srmMkdir(&soap, srm_endpoint, "Mkdir", &req, &rep)) {
		soap_print_fault (&soap, stderr);
		soap_print_fault_location (&soap, stderr);
		soap_end (&soap);
		exit (1);
	}
    
	reqstatp = rep.srmMkdirResponse->returnStatus;
	
	printChar();
	printf ("Request Status  [%d] : [%s]\n", reqstatp->statusCode, reconvertStatusCode(reqstatp->statusCode));
    if (reqstatp->explanation) {
        printChar();
        printf ("Explanation: [%s]\n", reqstatp->explanation);
    }
    
	if (reqstatp->statusCode != SRM_USCORESUCCESS && reqstatp->statusCode != SRM_USCOREDONE) {
		soap_end (&soap);
		exit(1);
	}

	soap_end (&soap);
	exit(0);
}
