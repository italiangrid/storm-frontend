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
//		"file"
//	};
//	int r = 0;
//	char *r_token;
//	struct ns1__srmExtendFileLifeTimeResponse_ rep;
//	struct ns1__srmExtendFileLifeTimeRequest req;
//	struct ns1__TReturnStatus *reqstatp;
//	char *sfn;
//	struct soap soap;
//	struct ns1__srmStatusOfGetRequestResponse_ srep;
//	struct ns1__srmStatusOfGetRequestRequest sreq;
//	char *srm_endpoint;
//
//	if (argc < 4) {
//		fprintf (stderr, "usage: %s reqid SURL seconds\n", argv[0]);
//		exit (1);
//	}
//	nbfiles = argc - 3;
//
//	if (parsesurl (argv[2], &srm_endpoint, &sfn) < 0) {
//		perror ("parsesurl");
//		exit (1);
//	}
//
//	//while (*protocols[nbproto]) nbproto++;
//
//	soap_init (&soap);
//#ifdef GSI_PLUGINS
//	flags = CGSI_OPT_DISABLE_NAME_CHECK;
//	soap_register_plugin_arg (&soap, client_cgsi_plugin, &flags);
//#endif
//
//	memset (&req, 0, sizeof(req));
//	if ((req.requestToken =
//		soap_malloc (&soap, sizeof(struct ns1__TRequestToken))) == NULL) {
//		perror ("malloc");
//		soap_end (&soap);
//		exit (1);
//	}
//
//	req.requestToken->value = argv[1];
//
//	if ((req.siteURL =
//		soap_malloc (&soap, sizeof(struct ns1__TSURL))) == NULL) {
//		perror ("malloc");
//		soap_end (&soap);
//		exit (1);
//	}
//	req.siteURL->value = argv[2];
//
//	if ((req.newLifeTime =
//		soap_malloc (&soap, sizeof(struct ns1__TLifeTimeInSeconds))) == NULL) {
//		perror ("malloc");
//		soap_end (&soap);
//		exit (1);
//	}
//	req.newLifeTime->value = atoi(argv[3]);
//	
//	if (soap_call_ns1__srmExtendFileLifeTime (&soap, srm_endpoint, "ExtendFileLifeTime",
//	    &req, &rep)) {
//		soap_print_fault (&soap, stderr);
//		soap_print_fault_location (&soap, stderr);
//		soap_end (&soap);
//		exit (1);
//	}
//
//	reqstatp = rep.srmExtendFileLifeTimeResponse->returnStatus;
//
//	printf ("request state %d\n", reqstatp->statusCode);
//	if (reqstatp->statusCode != SRM_USCORESUCCESS &&
//	    reqstatp->statusCode != SRM_USCOREDONE) {
//		if (reqstatp->explanation)
//			printf ("explanation: %s\n", reqstatp->explanation);
//		soap_end (&soap);
//		exit (1);
//	}
//
//	if (rep.srmExtendFileLifeTimeResponse->newTimeExtended) 
//		printf("time extended to: %d\n", rep.srmExtendFileLifeTimeResponse->newTimeExtended->value);
//
//	soap_end (&soap);
//	exit (0);
}

