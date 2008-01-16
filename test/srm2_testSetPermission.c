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
//	int nbgroups = 0;
//	int nbusers  = 0;
//	//static enum ns1__TRequestType rtypes[] = {PrepareToGet, PrepareToPut, Copy};
//	//static enum ns1__TPermissionType rtypes[] = {ADD, REMOVE, CHANGE};
//#if 0
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
//#endif
//	int r = 0;
//	char *r_token;
//	struct ns1__srmSetPermissionResponse_ rep;
//	struct ns1__srmSetPermissionRequest req;
//	struct ns1__TGroupPermission *reqfilep;
//	struct ns1__TUserPermission *requserp;
//	struct ns1__TReturnStatus *reqstatp;
//	char *sfn;
//	struct soap soap;
//	char *srm_endpoint;
//
//	if (argc < 5) {
//		fprintf (stderr, "usage: %s PermT OwnP OthP SURL nbGr nbUs [ [ [group1 perm1] [group2 perm2] [...] ] | [ [user1 perm1] [user2 perm2] [...] ] ]\n", argv[0]);
//		exit (1);
//	}
//
//	if (parsesurl (argv[4], &srm_endpoint, &sfn) < 0) {
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
//
//	/*
//	if ((req.permissionType =
//		soap_malloc (&soap, sizeof(struct ns1__TPermissionType))) == NULL) {
//		perror ("malloc");
//		soap_end (&soap);
//		exit (1);
//	}
//	*/
//	req.permissionType = atoi(argv[1]);
//	
//	if ( strcmp(argv[2], "-") ) {
//	if ((req.ownerPermission =
//		soap_malloc (&soap, sizeof(struct ns1__TOwnerPermission))) == NULL) {
//		perror ("malloc");
//		soap_end (&soap);
//		exit (1);
//	}
//	req.ownerPermission->mode = atoi(argv[2]);
//	}
//
//	if ( strcmp(argv[3], "-") ) {
//	if ((req.otherPermission =
//		soap_malloc (&soap, sizeof(struct ns1__TOtherPermission))) == NULL) {
//		perror ("malloc");
//		soap_end (&soap);
//		exit (1);
//	}
//	req.otherPermission->mode = atoi(argv[3]);
//	}
//
//	/* To set the Group Permissions ... */
//
//	if ( strcmp(argv[5], "-") ) {
//	  nbgroups = atoi(argv[5]);
//
//	if ((req.groupPermission =
//		soap_malloc (&soap, sizeof(struct ArrayOfTGroupPermission))) == NULL ||
//	    (req.groupPermission->__ptr =
//	        soap_malloc (&soap, nbgroups * sizeof(struct ns1__TGroupPermission *))) == NULL) {
//		perror ("malloc");
//		soap_end (&soap);
//		exit (1);
//	}
//
//	for (i = 0; i < nbgroups; i++) {
//		if ((req.groupPermission->__ptr[i] =
//		    soap_malloc (&soap, sizeof(struct ns1__TGroupPermission))) == NULL) {
//			perror ("malloc");
//			soap_end (&soap);
//			exit (1);
//		}
//	}
//	req.groupPermission->__size = nbgroups;
//
//	for (i = 0; i < nbgroups; i++) {
//		reqfilep = req.groupPermission->__ptr[i];
//		/*
//		if ((reqfilep->mode =
//		    soap_malloc (&soap, sizeof(struct ns1__TPermissionMode))) == NULL) {
//			perror ("malloc");
//			soap_end (&soap);
//			exit (1);
//		}
//		*/
//		reqfilep->mode = atoi(argv[2*i+8]);
//
//		//reqfilep->groupID =   argv[2*i+7];
//		if ((reqfilep->groupID =
//		    soap_malloc (&soap, sizeof(struct ns1__TGroupID))) == NULL) {
//			perror ("malloc");
//			soap_end (&soap);
//			exit (1);
//		}
//		/*
//		if ((reqfilep->groupID->value =
//			    soap_strdup (soap, argv[2*i+7])) == NULL)
//				repfilep->owner = NULL;
//		*/
//		reqfilep->groupID->value = argv[2*i+7];
//	}
//	}
//
//	/* To set the User Permissions ... */
//
//	if ( strcmp(argv[6], "-") ) {
//	  nbusers  = atoi(argv[6]);
// 
//	  if ((req.userPermission =
//	       soap_malloc (&soap, sizeof(struct ArrayOfTUserPermission))) == NULL ||
//	      (req.userPermission->__ptr =
//	       soap_malloc (&soap, nbusers * sizeof(struct ns1__TUserPermission *))) == NULL) {
//	    perror ("malloc");
//	    soap_end (&soap);
//	    exit (1);
//	  }
//
//	  for (i = 0; i < nbusers; i++) {
//	    if ((req.userPermission->__ptr[i] =
//		 soap_malloc (&soap, sizeof(struct ns1__TUserPermission))) == NULL) {
//	      perror ("malloc");
//	      soap_end (&soap);
//	      exit (1);
//	    }
//	  }
//	  req.userPermission->__size = nbusers;
//
//	  for (i = 0; i < nbusers; i++) {
//	    requserp = req.userPermission->__ptr[i];
//	    requserp->mode = atoi(argv[2*i+8]);
//	    if ((requserp->userID =
//		 soap_malloc (&soap, sizeof(struct ns1__TUserID))) == NULL) {
//	      perror ("malloc");
//	      soap_end (&soap);
//	      exit (1);
//	    }
//	    requserp->userID->value = argv[2*i+7];
//	  }
//
//	}
//
//	/* To set the SURL path ... */
//
//	if ((req.path =
//	    soap_malloc (&soap, sizeof(struct ns1__TSURLInfo))) == NULL ||
//	    (req.path->SURLOrStFN =
//	    soap_malloc (&soap, sizeof(struct ns1__TSURL))) == NULL) {
//		perror ("malloc");
//		soap_end (&soap);
//		exit (1);
//	}
//	req.path->SURLOrStFN->value = argv[4];
//
//	/* To send the request ... */
//
//	if (soap_call_ns1__srmSetPermission (&soap, srm_endpoint, "SetPermission",
//	    &req, &rep)) {
//		soap_print_fault (&soap, stderr);
//		soap_print_fault_location (&soap, stderr);
//		soap_end (&soap);
//		exit (1);
//	}
//	reqstatp = rep.srmSetPermissionResponse->returnStatus;
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
//	soap_end (&soap);
//	exit (0);
}
