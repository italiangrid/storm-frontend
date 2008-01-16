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

main(int argc,char** argv)
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
//		"file",
//		""
//	};
//	int r = 0;
//	char *r_token;
//	struct ns1__srmMvResponse_ rep;
//	struct ArrayOfTGetRequestFileStatus *repfs;
//	struct ns1__srmMvRequest req;
//	struct ns1__TReturnStatus *reqstatp;
//	char *sfn;
//	struct soap soap;
//	char *srm_endpoint;
//
//	if (argc != 3) {
//		fprintf (stderr, "usage: %s src_SURL dst_SURL\n", argv[0]);
//		exit (1);
//	}
//
//	if (parsesurl (argv[1], &srm_endpoint, &sfn) < 0) {
//		perror ("parsesurl");
//		exit (1);
//	}
//
//	while (*protocols[nbproto]) nbproto++;
//
//	soap_init (&soap);
//#ifdef GSI_PLUGINS
//	flags = CGSI_OPT_DISABLE_NAME_CHECK;
//	soap_register_plugin_arg (&soap, client_cgsi_plugin, &flags);
//#endif
//
//	memset (&req, 0, sizeof(req));
//	if ((req.fromPath =
//		soap_malloc (&soap, sizeof(struct ns1__TSURLInfo))) == NULL ||
//	    (req.fromPath->SURLOrStFN =
//	        soap_malloc (&soap, sizeof(struct ns1__TSURL))) == NULL /*||
//	    (req.toPath =
//		soap_malloc (&soap, sizeof(struct ns1__TSURLInfo))) == NULL ||
//	    (req.toPath->SURLOrStFN = 
//	    soap_malloc (&soap, sizeof(struct ns1__TSURL)))*/) {
//		perror ("malloc1");
//		soap_end (&soap);
//		exit (1);
//	}
//	if (/*(req.fromPath =
//		soap_malloc (&soap, sizeof(struct ns1__TSURLInfo))) == NULL ||
//	    (req.fromPath->SURLOrStFN =
//	    soap_malloc (&soap, sizeof(struct ns1__TSURL))) == NULL ||*/
//	    (req.toPath =
//		soap_malloc (&soap, sizeof(struct ns1__TSURLInfo))) == NULL /*||
//	    (req.toPath->SURLOrStFN = 
//	    soap_malloc (&soap, sizeof(struct ns1__TSURL)))*/) {
//		perror ("malloc2");
//		soap_end (&soap);
//		exit (1);
//	}
//	if (/*(req.fromPath =
//		soap_malloc (&soap, sizeof(struct ns1__TSURLInfo))) == NULL ||
//	    (req.fromPath->SURLOrStFN =
//	    soap_malloc (&soap, sizeof(struct ns1__TSURL))) == NULL ||
//	    (req.toPath =
//		soap_malloc (&soap, sizeof(struct ns1__TSURLInfo))) == NULL ||*/
//	    (req.toPath->SURLOrStFN = 
//	        soap_malloc (&soap, sizeof(struct ns1__TSURL))) == NULL) {
//		perror ("malloc3");
//		soap_end (&soap);
//		exit (1);
//	}
//	/*
//	soap_end (&soap);
//	exit(0);
//	*/
//
//	req.fromPath->SURLOrStFN->value = argv[1];
//	req.toPath->SURLOrStFN->value = argv[2];
//	
//	if (soap_call_ns1__srmMv (&soap, srm_endpoint, "Mv",
//	    &req, &rep)) {
//		soap_print_fault (&soap, stderr);
//		soap_print_fault_location (&soap, stderr);
//		soap_end (&soap);
//		exit (1);
//	}
//	reqstatp = rep.srmMvResponse->returnStatus;
//
//	printf ("request state %d\n", reqstatp->statusCode);
//	if (reqstatp->statusCode != SRM_USCORESUCCESS &&
//	    reqstatp->statusCode != SRM_USCOREDONE) {
//		if (reqstatp->explanation)
//			printf ("explanation: %s\n", reqstatp->explanation);
//		soap_end (&soap);
//		exit (1);
//	}
//	soap_end (&soap);
//	exit (0);
}
