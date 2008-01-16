// $Id$

#include "srmv2H.h"
#include "srmSoapBinding.nsmap"
#include "storm_util.h"
#ifdef GSI_PLUGINS
#include "cgsi_plugin.h"
#endif

#define DEFPOLLINT 10
#define SRM_EP_PATH "/v2_1_1/srm"


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

int main(int argc, char **argv)
{
    struct soap soap;
    struct ns1__srmReleaseFilesResponse_ rep;
    struct ns1__srmReleaseFilesRequest req;
    struct ns1__ArrayOfTSURLReturnStatus *arrayOfFileStatuses;
    struct ns1__TReturnStatus *requestStatus;
    char *r_token, *sfn, *srm_endpoint, *p;
	int flags;
	int i, numArgs;
	int nbfiles;
	int nbproto = 0;
    int r = 0;
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
    
	if (argc < 2) {
		fprintf (stderr, "usage: %s [requestToken=<token>] [doRemove=<0|1>] <SURL> [SURL [SURL] ...]\n", argv[0]);
		exit (1);
	}

	while (*protocols[nbproto]) nbproto++;

	soap_init (&soap);
#ifdef GSI_PLUGINS
	flags = CGSI_OPT_DISABLE_NAME_CHECK;
	soap_register_plugin_arg (&soap, client_cgsi_plugin, &flags);
#endif

	memset (&req, 0, sizeof(req));
    
	if ((req.arrayOfSURLs = soap_malloc(&soap, sizeof(struct ns1__ArrayOfAnyURI))) == NULL ||
	    (req.doRemove = soap_malloc(&soap, sizeof(enum xsd__boolean))) == NULL) {
		perror ("malloc");
		soap_end (&soap);
		exit (1);
	}
    
    printLine();
    printLine();
    printChar();
    printf("SrmReleasFiles Request \n");
    printLine();
    
    for (numArgs=0, i=1; i<argc; i++) {
        if ((p = strstr(argv[i],"requestToken=")) != NULL) {
            numArgs++;
            req.requestToken = p + 13;
            p = NULL;
            continue; 
        }
        
        if ((p = strstr(argv[i],"doRemove=")) != NULL) {
            numArgs++;
            p += 9;
            if ((*p!='0') && (*p!='1')) {
                printChar();
                fprintf(stderr, "Error: doRemove can only be 0 or 1 (representing false and true)\n");
                printLine();
                exit(-1);
            }
            p[1] = 0;
            *(req.doRemove) = atoi(p);
            p = NULL;
            continue; 
        }
    }
    
    if (parsesurl (argv[1+numArgs], &srm_endpoint, &sfn) < 0) {
        perror ("parsesurl");
        exit (1);
    }
    
    nbfiles = argc - 1 - numArgs;
    
    if ((req.arrayOfSURLs->urlArray = soap_malloc(&soap, nbfiles * sizeof(char *))) == NULL) {
        perror ("malloc");
        soap_end (&soap);
        exit (1);
    }

	req.arrayOfSURLs->__sizeurlArray = nbfiles;

	for (i=0; i<nbfiles; i++) {
		req.arrayOfSURLs->urlArray[i] = argv[1+numArgs+i];
	}

	if (soap_call_ns1__srmReleaseFiles(&soap, srm_endpoint, "ReleaseFiles", &req, &rep)) {
		soap_print_fault(&soap, stderr);
		soap_print_fault_location(&soap, stderr);
		soap_end (&soap);
		exit (1);
	}
    
    requestStatus = rep.srmReleaseFilesResponse->returnStatus;
    arrayOfFileStatuses = rep.srmReleaseFilesResponse->arrayOfFileStatuses;
    
    printLine();
    printChar();
    printf ("Global request status: code=[%s]", reconvertStatusCode(requestStatus->statusCode));
    if (requestStatus->explanation) printf (" explanation=[%s]\n", requestStatus->explanation);
    else printf("\n");
    printLine();
    if (!arrayOfFileStatuses) {
        printChar();
        printf("arrayOfFileStatuses is NULL\n");
        printLine();
        soap_end (&soap);
        exit (1);
    }
    for (i=0; i<arrayOfFileStatuses->__sizestatusArray; i++) {
        printChar();
        if (arrayOfFileStatuses->statusArray[i]->surl != NULL)
            printf("SURL[%d]: %s\n", i, arrayOfFileStatuses->statusArray[i]->surl);
        else
            printf("SURL[%d]: NULL\n", i);
        printChar();
        printf("Status: code=[%s]", reconvertStatusCode(arrayOfFileStatuses->statusArray[i]->status->statusCode));
        if (arrayOfFileStatuses->statusArray[i]->status->explanation != NULL)
            printf(" explanation=[%s]\n", arrayOfFileStatuses->statusArray[i]->status->explanation);
        else
            printf("\n");
        printLine();
    }
    soap_end (&soap);
    exit (0);
}
