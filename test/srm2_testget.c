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
//	struct ns1__srmPrepareToGetResponse_ rep;
//	struct ns1__ArrayOfTGetRequestFileStatus *repfs;
//	struct ns1__srmPrepareToGetRequest req;
//	struct ns1__TGetFileRequest *reqfilep;
//	struct ns1__TReturnStatus *reqstatp;
//	char *sfn;
//	struct soap soap;
//	struct ns1__srmStatusOfGetRequestResponse_ srep;
//	struct ns1__srmStatusOfGetRequestRequest sreq;
//	char *srm_endpoint;
//
//	if (argc < 3) {
//		fprintf (stderr, "usage: %s [poll|nopoll] SURLs\n", argv[0]);
//		exit (1);
//	}
//	nbfiles = argc - 2;
//
//	if (parseSURL_getEndpoint  (argv[2], &srm_endpoint, &sfn) < 0) {
//		perror ("parsesurl");
//		exit (1);
//	}
//
//	/*while (*protocols[nbproto]) nbproto++; */
//	nbproto = 1;
//
//	soap_init (&soap);
//#ifdef GSI_PLUGINS
//	flags = CGSI_OPT_DISABLE_NAME_CHECK;
//	soap_register_plugin_arg (&soap, client_cgsi_plugin, &flags);
//#endif
//
//	memset (&req, 0, sizeof(req));
//	if ((req.arrayOfFileRequests =
//		soap_malloc (&soap, sizeof(struct ns1__ArrayOfTGetFileRequest))) == NULL ||
//	    (req.arrayOfFileRequests->getRequestArray =
//		soap_malloc (&soap, nbfiles * sizeof(struct ns1__TGetFileRequest *))) == NULL ||
//	    (req.arrayOfTransferProtocols =
//		soap_malloc (&soap, sizeof(struct ns1__ArrayOf_USCORExsd_USCOREstring))) == NULL) {
//		perror ("malloc");
//		soap_end (&soap);
//		exit (1);
//	}
//
//	for (i = 0; i < nbfiles; i++) {
//		if ((req.arrayOfFileRequests->getRequestArray[i] =
//		    soap_malloc (&soap, sizeof(struct ns1__TGetFileRequest))) == NULL) {
//			perror ("malloc");
//			soap_end (&soap);
//			exit (1);
//		}
//	}
//	req.arrayOfFileRequests->__sizegetRequestArray = nbfiles;
//	req.arrayOfTransferProtocols->stringArray = protocols;
//	req.arrayOfTransferProtocols->__sizestringArray = nbproto;
//
//	for (i = 0; i < nbfiles; i++) {
//		reqfilep = req.arrayOfFileRequests->getRequestArray[i];
//		if ((reqfilep->fromSURLInfo =
//		    soap_malloc (&soap, sizeof(struct ns1__TSURLInfo))) == NULL ||
//		    (reqfilep->fromSURLInfo->SURLOrStFN =
//		    soap_malloc (&soap, sizeof(struct ns1__TSURL))) == NULL) {
//			perror ("malloc");
//			soap_end (&soap);
//			exit (1);
//		}
//		reqfilep->fromSURLInfo->SURLOrStFN->value = argv[i+2];
//	}
//
//	if (soap_call_ns1__srmPrepareToGet (&soap, srm_endpoint, "PrepareToGet",
//	    &req, &rep)) {
//		soap_print_fault (&soap, stderr);
//		soap_print_fault_location (&soap, stderr);
//		soap_end (&soap);
//		exit (1);
//	}
//	reqstatp = rep.srmPrepareToGetResponse->returnStatus;
//	repfs = rep.srmPrepareToGetResponse->arrayOfFileStatuses;
//	
//	printLine();
//	printChar(); 
//	printf("SrmPrepareToGet \n");	
//
//	if (rep.srmPrepareToGetResponse->requestToken) {
//		r_token = rep.srmPrepareToGetResponse->requestToken->value;
//		printChar();
//		printf ("Request Token: [%s]\n", r_token);
//	}
//
//	printLine();
//
//	memset (&sreq, 0, sizeof(sreq));
//	sreq.requestToken = rep.srmPrepareToGetResponse->requestToken;
//
//
//        if (! strncmp(argv[1],"poll",4)) { 
//	   /* wait for file "ready" */
//	   
//   	   printChar();
//	   printf("StatusOfGetRequest: \n");	
//
//	   while (reqstatp->statusCode == SRM_USCOREREQUEST_USCOREQUEUED ||
//		reqstatp->statusCode == SRM_USCOREREQUEST_USCOREINPROGRESS ||
//		reqstatp->statusCode == SRM_USCOREREQUEST_USCORESUSPENDED) {
//		printChar();
//		printf ("Request State [%d] : [%s]\n", reqstatp->statusCode,reconvertStatusCode(reqstatp->statusCode));
//		//printf("request state %d\n", reqstatp->statusCode);
//		sleep ((r++ == 0) ? 1 : DEFPOLLINT);
//		if (soap_call_ns1__srmStatusOfGetRequest (&soap, srm_endpoint,
//		    "StatusOfGetRequest", &sreq, &srep)) {
//			soap_print_fault (&soap, stderr);
//			soap_end (&soap);
//			exit (1);
//		}
//		reqstatp = srep.srmStatusOfGetRequestResponse->returnStatus;
//		repfs = srep.srmStatusOfGetRequestResponse->arrayOfFileStatuses;
//	   }
//	   printChar();
//	   printf ("Request State [%d] : [%s]\n", reqstatp->statusCode, reconvertStatusCode(reqstatp->statusCode));
//	   //printf ("request state %d\n", reqstatp->statusCode);
//	   
//	  if (reqstatp->statusCode != SRM_USCORESUCCESS &&
//	       reqstatp->statusCode != SRM_USCOREDONE) {
//	   	   if (reqstatp->explanation) {
//			printChar();
//			printf ("Explanation: [%s]\n", reqstatp->explanation);
//		   }
//		   soap_end (&soap);
//		   exit (1);
//	   }
//	   if (! repfs) {
//		printf ("= WARN: arrayOfFileStatuses is NULL\n");
//		soap_end (&soap);
//		exit (1);
//	   }
//
//	   printLine();
//
//	   for (i = 0; i < repfs->__sizegetStatusArray; i++) {
//
//		printChar();
//		printf("Request [%d]\n",i);
//
//		if (repfs->getStatusArray[i]->transferURL) {
//			printChar();
//			printf ("Status [%d] : [%s]\n= Explanation : [%s]\n",
//				(repfs->getStatusArray[i])->status->statusCode,
//				reconvertStatusCode((repfs->getStatusArray[i])->status->statusCode),
//	 			(repfs->getStatusArray[i])->status->explanation );
//
//			printChar();
//			printf ("TURL : [%s]\n",(repfs->getStatusArray[i])->transferURL->value);
//		
//
//		} else if ((repfs->getStatusArray[i])->status->explanation) {
//			printChar();
//			printf ("Request Status[%d] : [%s]\n= Explanation = [%s]\n",			
//			    (repfs->getStatusArray[i])->status->statusCode,
//			    reconvertStatusCode((repfs->getStatusArray[i])->status->statusCode),
//			    (repfs->getStatusArray[i])->status->explanation);
//	
//
//		} else {
//			printChar();
//			printf ("Request Status[%d] : [%s]\n",
//				(repfs->getStatusArray[i])->status->statusCode,
//			    reconvertStatusCode((repfs->getStatusArray[i])->status->statusCode)	);
//   	
//		}
//
//	   printLine();
//	   }
//
//        }
//	soap_end (&soap);
//	exit (0);
}
