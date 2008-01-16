// $Id$

#include "srmv2H.h"
#include "srmSoapBinding.nsmap"
#include "storm_util.h"
#ifdef GSI_PLUGINS
#include "cgsi_plugin.h"
#endif

#define DEFPOLLINT 10
#define SRM_EP_PATH "/v2_1_1/srm"

int parsesurl (const char *surl, char **endpoint, char **sfn)
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
    struct ns1__srmPutDoneRequest req;
    struct ns1__srmPutDoneResponse_ rep;
    struct ns1__ArrayOfTSURLReturnStatus *arrayOfFileStatuses;
    struct ns1__TReturnStatus *requestStatus;
    char *r_token, *sfn, *srm_endpoint;
	int flags, i, nbfiles;
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

	if (argc < 3) {
		fprintf (stderr, "usage: %s <requestToken> [authorizationID] <SURL> [[SURL] [SURL] ...]\n", argv[0]);
		exit (1);
	}
	nbfiles = argc - 2;

	if (parsesurl (argv[2], &srm_endpoint, &sfn) < 0) {
		perror ("parsesurl");
		exit (1);
	}

	while (*protocols[nbproto]) nbproto++;

	soap_init (&soap);
#ifdef GSI_PLUGINS
	flags = CGSI_OPT_DISABLE_NAME_CHECK | CGSI_OPT_DELEG_FLAG;
	soap_register_plugin_arg (&soap, client_cgsi_plugin, &flags);
#endif

	memset (&req, 0, sizeof(req));
    req.arrayOfSURLs = soap_malloc(&soap, sizeof(struct ns1__ArrayOfAnyURI));
	if (req.arrayOfSURLs == NULL) {
        perror ("malloc");
        soap_end (&soap);
        exit (1);
    }
    req.arrayOfSURLs->urlArray = soap_malloc(&soap, nbfiles * sizeof(char *));
	if (req.arrayOfSURLs->urlArray == NULL) {
		perror ("malloc");
		soap_end (&soap);
		exit (1);
	}

	req.requestToken = argv[1];
	req.arrayOfSURLs->__sizeurlArray = nbfiles;

	for (i = 0; i < nbfiles; i++) {
		req.arrayOfSURLs->urlArray[i] = argv[i+2];
	}

	if (soap_call_ns1__srmPutDone (&soap, srm_endpoint, "PutDone", &req, &rep)) {
		soap_print_fault (&soap, stderr);
		soap_print_fault_location (&soap, stderr);
		soap_end (&soap);
		exit (1);
	}
    
	requestStatus = rep.srmPutDoneResponse->returnStatus;
    arrayOfFileStatuses = rep.srmPutDoneResponse->arrayOfFileStatuses;

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
