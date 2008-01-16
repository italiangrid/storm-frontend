// $Id$

#include "srmv2H.h"
#include "srmSoapBinding.nsmap"
#include "storm_util.h"
#define DEFPOLLINT 10

#define SRM_EP_PATH "/v2_1_1/srm"
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

main(int argc,char** argv)
{
//	int flags;
//	int i;
//	struct ns1__ArrayOfTCopyRequestFileStatus *repfs;
//	struct ns1__TReturnStatus *reqstatp;
//	struct soap soap;
//	struct ns1__srmStatusOfCopyRequestResponse_ srep;
//	struct ns1__srmStatusOfCopyRequestRequest sreq;
//	char *srm_endpoint;
//	char *sfn;
//
//	//Print Header
//	printLine();
//	printLine();
//	printChar();
//	printf("SrmStatusOfCopyRequest \n");
//	printLine();
//
//
//	if (argc < 3) {
//		printChar();
//		fprintf (stderr, "Usage: %s srm_endpoint request_token\n", argv[0]);
//		printLine();
//		exit (1);
//	}
//
////	if (parsesrm (argv[1], &srm_endpoint) < 0) {
//	if (parseSURL_getEndpoint(argv[1],&srm_endpoint,&sfn)<0) {		
//
//		//perror ("parsesrm");
//		printLine();	
//		exit (1);
//	}
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
//	sreq.requestToken->value = argv[2];
//
//	if (soap_call_ns1__srmStatusOfCopyRequest (&soap, srm_endpoint,
//	    "StatusOfCopyRequest", &sreq, &srep)) {
//		soap_print_fault (&soap, stderr);
//		soap_end (&soap);
//		exit (1);
//	}
//	reqstatp = srep.srmStatusOfCopyRequestResponse->returnStatus;
//	repfs = srep.srmStatusOfCopyRequestResponse->arrayOfFileStatuses;
//
//	printChar();
//	printf ("Global Request Status [%d] = [%s]\n", reqstatp->statusCode,reconvertStatusCode(reqstatp->statusCode));
//	if (reqstatp->statusCode != SRM_USCORESUCCESS &&
//	    reqstatp->statusCode != SRM_USCOREDONE) {
//		if (reqstatp->explanation) {
//			printChar();
//			printf ("Explanation String: [%s]\n", reqstatp->explanation);
//		}
//		soap_end (&soap);
//		printLine();
//		exit (1);
//	}
//	if (! repfs) {
//		printChar();
//		printf ("WARNING: arrayOfFileStatuses is NULL\n");
//		soap_end (&soap);
//		printLine();
//		exit (1);
//	}
//
//	printLine();
//	for (i = 0; i < repfs->__sizecopyStatusArray; i++) {
//		printChar();
//		printf("Request [%d]\n",i);
//		
//		if ((repfs->copyStatusArray[i]->fromSURL)&&(repfs->copyStatusArray[i]->toSURL)) {
//			printChar();
//			printf ("Status[%d] = [%s]\n= fromSURL = [%s]\n= toSURL = [%s] \n",
//			    (repfs->copyStatusArray[i])->status->statusCode,
//			    reconvertStatusCode((repfs->copyStatusArray[i])->status->statusCode), 
//			    (repfs->copyStatusArray[i])->fromSURL->value,
//			    (repfs->copyStatusArray[i])->toSURL->value);
//			if ((repfs->copyStatusArray[i]->status->explanation)) {
//				printChar();
//				printf("Explanation String: [%s]\n",(repfs->copyStatusArray[i])->status->explanation);
//			}
//
//		} else if ((repfs->copyStatusArray[i])->status->explanation) {
//			printChar();
//			printf ("Status[%d] = [%s], Explanation = [%s]\n", 
//			    (repfs->copyStatusArray[i])->status->statusCode,
//			reconvertStatusCode((repfs->copyStatusArray[i])->status->statusCode),
//			    (repfs->copyStatusArray[i])->status->explanation);
//	 	} else {
//			printChar();
//			printf ("Status[%d] = [%s]\n", i,
//			    (repfs->copyStatusArray[i])->status->statusCode,
//		reconvertStatusCode((repfs->copyStatusArray[i])->status->statusCode));
//		}
//	}
//	soap_end (&soap);
//	printLine();
//	exit (0);
}
