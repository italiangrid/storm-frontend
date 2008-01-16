
#include "srmv2H.h"
#include "srmSoapBinding.nsmap"
#include "storm_util.h"
#define DEFPOLLINT 10

#define SRM_EP_PATH "/v2_1_1/srm"
#ifdef GSI_PLUGINS
#include "cgsi_plugin.h"
#endif
/*
parsesrm(const char *srm, char **endpoint)
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
    struct ns1__ArrayOfTPutRequestFileStatus *repfs;
    struct ns1__TReturnStatus *reqstatp;
    struct soap soap;
    struct ns1__srmStatusOfPutRequestResponse_ srep;
    struct ns1__srmStatusOfPutRequestRequest sreq;
	int flags, i;
	char *srm_endpoint;
	char *sfn; //ForParsing surl

	//Print Header
	printLine();
	printLine();
	printChar();
	printf("SrmStatusOfPutRequest \n");
	printLine();

	if (argc < 3) {
		printChar();
		fprintf (stderr, "Usage: %s srm_endpoint request_token\n", argv[0]);
		printLine();
		exit (1);
	}

//	if (parsesrm (argv[1], &srm_endpoint) < 0) {
	if (parseSURL_getEndpoint(argv[1],&srm_endpoint,&sfn)<0) {		
		//perror ("parsesrm");
		printLine();
		exit (1);
	}

	soap_init (&soap);
#ifdef GSI_PLUGINS
	flags = CGSI_OPT_DISABLE_NAME_CHECK | CGSI_OPT_DELEG_FLAG;
	soap_register_plugin_arg (&soap, client_cgsi_plugin, &flags);
#endif

    memset (&sreq, 0, sizeof(sreq));
    
	sreq.requestToken = argv[2];

	if (soap_call_ns1__srmStatusOfPutRequest(&soap, srm_endpoint, "StatusOfPutRequest", &sreq, &srep)) {
		soap_print_fault(&soap, stderr);
		soap_end(&soap);
		exit(1);
	}
    
	reqstatp = srep.srmStatusOfPutRequestResponse->returnStatus;
	repfs = srep.srmStatusOfPutRequestResponse->arrayOfFileStatuses;

	printChar();
	printf("Request Status [%d] = [%s]", reqstatp->statusCode, reconvertStatusCode(reqstatp->statusCode));
    
    if (reqstatp->explanation) { 
        printChar();
        printf(" Returned explanation: [%s]\n", reqstatp->explanation);
    } else printf("\n");

	if (!repfs) {
		printChar();
		printf ("Warning: arrayOfFileStatuses is NULL\n");
		soap_end (&soap);
		printLine();
		exit (1);
	}

    for (i=0; i<repfs->__sizestatusArray; i++) {
        printLine();
    	printChar();
    	printf("Request [%d]\n",i);

        if (repfs->statusArray[i]->status->explanation) {
            printChar();
            printf ("Status[%d] = [%s], Explanation = [%s]\n", repfs->statusArray[i]->status->statusCode,
                    reconvertStatusCode(repfs->statusArray[i]->status->statusCode),    
                    repfs->statusArray[i]->status->explanation);
        } else {
            printChar();
            printf ("Status[%d] = [%s]\n", repfs->statusArray[i]->status->statusCode, 
                    reconvertStatusCode(repfs->statusArray[i]->status->statusCode));
        }
        
        printChar();
    	if (repfs->statusArray[i]->transferURL) {
    		printf("TURL = %s\n", repfs->statusArray[i]->transferURL);
    	} else printf("TURL = NULL\n");
         
        printChar();
        if (repfs->statusArray[i]->SURL) {
            printf("SURL = %s\n", repfs->statusArray[i]->SURL);
        } else printf("SURL = NULL\n");
        
        printChar();
        if (repfs->statusArray[i]->fileSize) {
            printf("fileSize = %lld\n", *(repfs->statusArray[i]->fileSize));
        } else printf("fileSize = NULL\n");
         
        printChar();
        if (repfs->statusArray[i]->remainingPinLifetime) {
            printf("remainingPinLifetime = %lld\n", *(repfs->statusArray[i]->remainingPinLifetime));
        } else printf("remainingPinLifetime = NULL\n");
        
        printChar();
        if (repfs->statusArray[i]->remainingFileLifetime) {
            printf("remainingFileLifetime = %lld\n", *(repfs->statusArray[i]->remainingFileLifetime));
        } else printf("remainingFileLifetime = NULL\n");
        
        printChar();
        if (repfs->statusArray[i]->estimatedWaitTime) {
            printf("estimatedWaitTime = %lld\n", *(repfs->statusArray[i]->estimatedWaitTime));
        } else printf("estimatedWaitTime = NULL\n");
        
        printChar();
        if (repfs->statusArray[i]->transferProtocolInfo) {
            printf("transferProtocolInfo->__sizeextraInfoArray = %d\n", repfs->statusArray[i]->transferProtocolInfo->__sizeextraInfoArray);
        } else printf("transferProtocolInfo = NULL\n");
        
        printChar();
        if (srep.srmStatusOfPutRequestResponse->remainingTotalRequestTime) {
            printf("remainingTotalRequestTime = %d\n", *(srep.srmStatusOfPutRequestResponse->remainingTotalRequestTime));
        } else printf("remainingTotalRequestTime = NUL\n");
        
        printLine();
    }

	printLine();

	soap_end (&soap);
	exit (0);
}
