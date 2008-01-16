// $Id$

#include "srmv2H.h"
#include "srmSoapBinding.nsmap"
#include "storm_util.h"
#define DEFPOLLINT 10

#define SRM_EP_PATH "/v2_1_1/srm"
#ifdef GSI_PLUGINS
#include "cgsi_plugin.h"
#endif

static char *protocols[] = 
{
#if GFAL_ENABLE_RFIO
    "rfio",
#endif
#if GFAL_ENABLE_DCAP
    "gsidcap",
#endif
    "file",
    "" 
};

main(int argc,char** argv)
{
    struct ns1__srmRmResponse_ rep;
    struct ns1__ArrayOfTSURLReturnStatus *repfs;
    struct ns1__TSURLReturnStatus **statusArray;
    struct ns1__srmRmRequest req;
    struct ns1__TReturnStatus *reqstatp;
    struct soap soap;
    int i, j, flags, nbfiles, nbproto;
	char *sfn, **reqfilep, *r_token, *srm_endpoint;

	// Print Help Menu
	printLine();
	printLine();
	printChar();
	printf("SrmRm Request \n");
	printLine();

	if (argc < 2) {
		fprintf (stderr, "Usage: %s SURL1 SURL2 SURL3 ...\n", argv[0]);
		printLine();
		exit (1);
	}
	nbfiles = argc - 1;

	//Check syntax for each SURL specified
	for (j=0; j<nbfiles; j++) {
		//From the FIRST SURL specified get the service EndPoint
		if ( (j==0) && (parseSURL_getEndpoint(argv[1+j], &srm_endpoint, &sfn)<0) ) {
			//perror ("Error parsing SURL");
			printLine();
			exit (1);
		} else if (parseSURL(argv[1+j]) < 0) {
			//perror ("Error parsing SURL");
			printLine();
			exit (1);
		}
	}

    nbproto = 0;
	while (*protocols[nbproto]) nbproto++;

	soap_init(&soap);
#ifdef GSI_PLUGINS
	flags = CGSI_OPT_DISABLE_NAME_CHECK | CGSI_OPT_DELEG_FLAG;
	soap_register_plugin_arg (&soap, client_cgsi_plugin, &flags);
#endif

	memset(&req, 0, sizeof(req));
    
    req.arrayOfSURLs = soap_malloc(&soap, sizeof(struct ns1__ArrayOfAnyURI));
	if (req.arrayOfSURLs == NULL) {
        perror("malloc");
        soap_end(&soap);
        exit (1);
    }
    
    req.arrayOfSURLs->urlArray = soap_malloc(&soap, nbfiles * sizeof(char *));
     if (req.arrayOfSURLs->urlArray == NULL) {
        perror ("malloc");
        soap_end (&soap);
        exit (1);
	}

	req.arrayOfSURLs->__sizeurlArray = nbfiles;

	for (i=0; i<nbfiles; i++) req.arrayOfSURLs->urlArray[i] = argv[i+1];

	if (soap_call_ns1__srmRm(&soap, srm_endpoint, "SrmRm", &req, &rep)) {
		soap_print_fault(&soap, stderr);
		soap_print_fault_location(&soap, stderr);
		soap_end(&soap);
		exit(1);
	}
    
	reqstatp = rep.srmRmResponse->returnStatus;

	printChar();
	printf ("Global Request Status [%d] : [%s]\n", reqstatp->statusCode,reconvertStatusCode(reqstatp->statusCode));
    if (reqstatp->explanation) {
        printChar();
        printf ("Explanation: [%s]\n", reqstatp->explanation);
        printLine();
    }
    
	if (rep.srmRmResponse->arrayOfFileStatuses == NULL) {
		printChar();
		printf ("ArrayOfFileStatuses is NULL\n");
		printLine();
		soap_end (&soap);
		exit (0);
	}
    
    nbfiles = rep.srmRmResponse->arrayOfFileStatuses->__sizestatusArray;
    statusArray = rep.srmRmResponse->arrayOfFileStatuses->statusArray;
    
	for (i=0; i<nbfiles; i++) {
		printChar();
		printf("Request [%d]\n",i);
        
        printChar();
        printf("SURL: %s\n", statusArray[i]->surl);

        printChar();
        printf("Status [%d] : [%s]\n", statusArray[i]->status->statusCode, reconvertStatusCode(statusArray[i]->status->statusCode));
        
		if (statusArray[i]->status->explanation != NULL) printf("= Explanation = [%s]\n", statusArray[i]->status->explanation);
		printLine();
	}
	soap_end (&soap);
	exit (0);
}
