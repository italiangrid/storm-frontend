// $Id$

#include "srmv2H.h"
#include "srmSoapBinding.nsmap"
#define DEFPOLLINT 10

#define SRM_EP_PATH "/v2_1_1/srm"
#ifdef GSI_PLUGINS
#include "cgsi_plugin.h"
#endif
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

main(int argc, char **argv)
{
//	int flags;
//	int i;
//	int nbfiles;
//	int nbproto = 0;
//	static char *protocols[] = {
//#if GFAL_ENABLE_RFIO
//		"rfio",
//#endif
//#if GFAL_ENABLE_DCAP
//		"gsidcap",
//#endif
//		"file"
//	};
//	int r = 0;
//	char *r_token;
//	struct ns1__srmGetRequestSummaryResponse_ rep;
//	struct ns1__ArrayOfTRequestSummary *repfs;
//	struct ns1__srmGetRequestSummaryRequest req;
//	struct ns1__TRequestToken *reqfilep;
//	struct ns1__TReturnStatus *reqstatp;
//	char *sfn;
//	struct soap soap;
//	struct ns1__srmStatusOfPutRequestResponse_ srep;
//	struct ns1__srmStatusOfPutRequestRequest sreq;
//	char *srm_endpoint;
//	char *rtype;
//
//	if (argc < 3) {
//		fprintf (stderr, "usage: %s endPoint reqids\n", argv[0]);
//		exit (1);
//	}
//	nbfiles = argc - 2;
//
//#if 0
//	if (parsesurl (argv[1], &srm_endpoint, &sfn) < 0) {
//		perror ("parsesurl");
//		exit (1);
//	}
//#endif
//
//	/* while (*protocols[nbproto]) nbproto++; */
//        nbproto = 1;
//
//	soap_init (&soap);
//#ifdef GSI_PLUGINS
//	flags = CGSI_OPT_DISABLE_NAME_CHECK;
//	soap_register_plugin_arg (&soap, client_cgsi_plugin, &flags);
//#endif
//
//	memset (&req, 0, sizeof(req));
//	//CHECK THIS!
//	if ((req.arrayOfRequestTokens =
//		soap_malloc (&soap, sizeof(struct ns1__ArrayOfTRequestToken))) == NULL ||
//	    (req.arrayOfRequestTokens->requestTokenArray=
//		soap_malloc (&soap, nbfiles * sizeof(struct ns1__TRequestToken *))) == NULL) {
//		perror ("malloc");
//		soap_end (&soap);
//		exit (1);
//	}
//
//	for (i = 0; i < nbfiles; i++) {
//		if ((req.arrayOfRequestTokens->requestTokenArray[i] =
//		    soap_malloc (&soap, sizeof(struct ns1__TRequestToken))) == NULL) {
//			perror ("malloc");
//			soap_end (&soap);
//			exit (1);
//		}
//	}
//
//	req.arrayOfRequestTokens->__sizerequestTokenArray = nbfiles;
//
//	for (i = 0; i < nbfiles; i++) {
//		reqfilep = req.arrayOfRequestTokens->requestTokenArray[i];
//		reqfilep->value = argv[i+2];
//	}
//
//#if 0
//	if (soap_call_ns1__srmGetRequestSummary (&soap, srm_endpoint, "GetRequestSummary",
//#else
//	if (soap_call_ns1__srmGetRequestSummary (&soap, argv[1], "GetRequestSummary",
//#endif
//	    &req, &rep)) {
//		soap_print_fault (&soap, stderr);
//		soap_print_fault_location (&soap, stderr);
//		soap_end (&soap);
//		exit (1);
//	}
//
//	reqstatp = rep.srmGetRequestSummaryResponse->returnStatus;
//	repfs = rep.srmGetRequestSummaryResponse->arrayOfRequestSummaries;
//
//	/* wait for file "ready" */
//
//	printf ("request state %d\n", reqstatp->statusCode);
//	printf ("request state %d %d\n", reqstatp->statusCode, repfs->__sizesummaryArray);  //  GG
//	if (reqstatp->statusCode != SRM_USCORESUCCESS &&
//	    reqstatp->statusCode != SRM_USCOREDONE) {
//		if (reqstatp->explanation)
//			printf ("explanation: %s\n", reqstatp->explanation);
//		soap_end (&soap);
//		exit (1);
//	}
//	if (! repfs) {
//		printf ("arrayOfRequestSummaries is NULL\n");
//		soap_end (&soap);
//		exit (1);
//	}
//
//	for (i = 0; i < repfs->__sizesummaryArray; i++) {
//		printf("======= BEGIN SUMMARY ========\n");
//		printf("Finished   files: %d\n", repfs->summaryArray[i]->numOfFinishedRequests);
//		printf("Processing files: %d\n", repfs->summaryArray[i]->numOfProgressingRequests);
//		printf("Number of  files: %d\n", repfs->summaryArray[i]->totalFilesInThisRequest);
//        if ((*repfs->summaryArray[i]->requestType) == PrepareToGet)
//                rtype = "PrepareToGet";
//        else if ((*repfs->summaryArray[i]->requestType) == PrepareToPut)
//	        rtype = "PrepareToPut";
//        else
//                rtype = "Copy";
//                printf("Type of      req: %s\n", rtype);
//		//printf("Type of      req: %d\n", (*repfs->__ptr[i]->requestType));
//		printf("Request    token: %s\n", repfs->summaryArray[i]->requestToken->value);
//		/* FIXME */
//		printf("...\n");
//
//	}
//	soap_end (&soap);
//	exit (0);
}
