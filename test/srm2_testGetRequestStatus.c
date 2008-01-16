// $Id$

#include "srmv2H.h"
#include "srmSoapBinding.nsmap"
#define DEFPOLLINT 10

#define SRM_EP_PATH "/v2_1_1/srm"
#define TOKEN_MAXSIZE 256
#ifdef GSI_PLUGINS
#include "cgsi_plugin.h"
#endif
/*
parsesrm (const char *srm, char **endpoint)
{
	int len;
	int lenp;
	char *p;
	static char srm_ep[256];

	if (strncmp (srm, "srm://", 6)) {
		errno = EINVAL;
		return (-1);
	}
        if (p = strchr (srm + 6, '/')) {
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
	len = p - srm - 6;
	if (lenp + len >= sizeof(srm_ep)) {
		errno = EINVAL;
		return (-1);
	}
	strncpy (srm_ep + lenp, srm + 6, len);
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

main(int argc, char **argv)
{
//	int flags;
//	int i;
//	int r = 0;
//	struct ns1__ArrayOfTGetRequestFileStatus *repfs;
//	struct ns1__TReturnStatus *reqstatp;
//	struct soap soap;
//	struct ns1__srmStatusOfGetRequestResponse_ srep;
//	struct ns1__srmStatusOfGetRequestRequest sreq;
//	char *srm_endpoint;
//	char *sfn;
//	//
//	char requestToken[256];
//
//	//Print Header
//	printLine();
//	printLine();
//	printChar();
//	printf("SrmStatusOfGetRequest \n");
//	printLine();
//
//	if (argc < 3) {
//		printChar();
//		fprintf (stderr, "Usage: %s srm_endpoint request_token\n", argv[0]);
//		printLine();
//		exit (-1);
//	}
////	if (parsesrm (argv[1], &srm_endpoint) < 0) {
//	if (parseSURL_getEndpoint(argv[1],&srm_endpoint,&sfn)<0) {		
//	
//		//perror ("parsesrm");
//		printLine();
//		exit (1);
//	}
//
//
//	soap_init (&soap);
//#ifdef GSI_PLUGINS
//	flags = CGSI_OPT_DISABLE_NAME_CHECK;
//	soap_register_plugin_arg (&soap, client_cgsi_plugin, &flags);
//#endif
//
//	memset (&sreq, 0, sizeof(sreq));
//        if (((sreq.requestToken =
//                soap_malloc (&soap, sizeof(struct ns1__TRequestToken))) == NULL) || 
//            ((sreq.userID = 
//                soap_malloc (&soap, sizeof(struct ns1__TUserID))) == NULL))        { 
//                perror ("malloc");
//                soap_end (&soap);
//                exit (1);
//        }
//
//
//	//	
//	//Parsing here! TOO LONG request crash FE!
//	if(strlen(argv[2])>TOKEN_MAXSIZE) {
//		printChar();
//		fprintf (stderr, "Usage: %s srm_endpoint request_token\n", argv[0]);
//		printLine();
//		exit (-1);
//	
//	}
//
//	//Token is correct
//	sreq.requestToken->value = argv[2];
//	
//	/* wait for file "ready" */
//
//        if (soap_call_ns1__srmStatusOfGetRequest (&soap, srm_endpoint,
//	    "StatusOfGetRequest", &sreq, &srep)) {
//		soap_print_fault (&soap, stderr);
//		soap_end (&soap);
//		exit (1);
//	}
//	reqstatp = srep.srmStatusOfGetRequestResponse->returnStatus;
//	repfs = srep.srmStatusOfGetRequestResponse->arrayOfFileStatuses;
//	
//	printChar();
//	printf ("Request Status [%d] = [%s] \n", reqstatp->statusCode,
//	reconvertStatusCode(reqstatp->statusCode));
//
//	if (reqstatp->statusCode != SRM_USCORESUCCESS &&
//	    reqstatp->statusCode != SRM_USCOREDONE) {
//		if (reqstatp->explanation) {
//			printChar();
//			printf ("Explanation String: [%s]\n", reqstatp->explanation);
//		
//		}
//		soap_end (&soap);
//		printLine();
//		exit (1);
//	
//	}
//
//	if (! repfs) {
//		printChar();
//		printf ("WARNING:arrayOfFileStatuses is NULL\n");
//		soap_end (&soap);
//		printLine();
//		exit (1);
//	}
//
//	for (i = 0; i < repfs->__sizegetStatusArray; i++) {
//		printChar();
//		printf("Request [%d]\n",i);
//		printLine();
//		if (repfs->getStatusArray[i]->transferURL) {
//			printChar();
//			printf ("Status[%d] = [%s], TURL = [%s]\n", 
//			(repfs->getStatusArray[i])->status->statusCode,
//			reconvertStatusCode((repfs->getStatusArray[i])->status->statusCode),
//			    (repfs->getStatusArray[i])->transferURL->value);
//		} else if ((repfs->getStatusArray[i])->status->explanation) {
//			printChar();
//			printf ("Status[%d] = [%s], explanation = %s\n", 
//			(repfs->getStatusArray[i])->status->statusCode,
//			reconvertStatusCode((repfs->getStatusArray[i])->status->statusCode), 
//			    (repfs->getStatusArray[i])->status->explanation);
//		} else {
//			printChar();
//			printf ("Status[%d] = [%s]\n", 			    (repfs->getStatusArray[i])->status->statusCode,
//		reconvertStatusCode((repfs->getStatusArray[i])->status->statusCode));
//		
//		}
//	}
//	soap_end (&soap);
//	printLine();
//	exit (0);
}
