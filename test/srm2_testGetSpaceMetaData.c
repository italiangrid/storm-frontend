// $Id$

#include "srmv2H.h"
#include "srmSoapBinding.nsmap"
#define DEFPOLLINT 10

#define SRM_EP_PATH "/v2_1_1/srm"
#ifdef GSI_PLUGINS
#include "cgsi_plugin.h"
#endif

main(int argc, char **argv)
{
	int flags;
	int i;
	int nbfiles;
#if 0
	int nbproto = 0;
	static char *protocols[] = {
#if GFAL_ENABLE_RFIO
		"rfio",
#endif
#if GFAL_ENABLE_DCAP
		"gsidcap",
#endif
		"file"
	};
#endif
	int r = 0;
	char *r_token;
	struct ns1__srmGetSpaceMetaDataResponse_ rep;
	struct ns1__srmGetSpaceMetaDataRequest req;
	struct ns1__TSpaceToken *reqfilep;
	struct ns1__TReturnStatus *reqstatp;
    struct ns1__ArrayOfTMetaDataSpace *mdp;    

	char *sfn;
	struct soap soap;
	char *srm_endpoint;

	if (argc < 3) {
		fprintf (stderr, "usage: %s srm_endpoint SpaceTokenS\n", argv[0]);
		exit (1);
	}

	nbfiles = argc - 2;

	soap_init (&soap);
#ifdef GSI_PLUGINS
	flags = CGSI_OPT_DISABLE_NAME_CHECK | CGSI_OPT_DELEG_FLAG;
	soap_register_plugin_arg (&soap, client_cgsi_plugin, &flags);
#endif

	memset (&req, 0, sizeof(req));
    
    req.arrayOfSpaceTokens = soap_malloc(&soap, sizeof(struct ns1__ArrayOfString));
    if (req.arrayOfSpaceTokens == NULL) {
        perror ("malloc");
        soap_end (&soap);
        exit (1);
    }
    req.arrayOfSpaceTokens->stringArray = soap_malloc(&soap, nbfiles * sizeof(char *));
    if (req.arrayOfSpaceTokens->stringArray == NULL) {
        perror ("malloc");
        soap_end (&soap);
        exit (1);
    }
        
    req.arrayOfSpaceTokens->__sizestringArray = nbfiles;

	for (i=0; i<nbfiles; i++) req.arrayOfSpaceTokens->stringArray[i] = argv[i+2];


	/* To send the request ... */

	if (soap_call_ns1__srmGetSpaceMetaData (&soap, argv[1], "SrmGetSpaceMetaData",
	    &req, &rep)) {
		soap_print_fault (&soap, stderr);
		soap_print_fault_location (&soap, stderr);
		soap_end (&soap);
		exit (1);
	}
	reqstatp = rep.srmGetSpaceMetaDataResponse->returnStatus;

	if (reqstatp->statusCode) {
        printf ("\nRequest status code: [%d] ", reqstatp->statusCode);
    }
    if (reqstatp->explanation) printf ("Explanation: %s\n", reqstatp->explanation);
    else fprintf (stderr, "Error request did not return information\n");

	if ((reqstatp->statusCode != SRM_USCORESUCCESS) && (reqstatp->statusCode != SRM_USCOREDONE)) {
		soap_end (&soap);
		exit (1);
	}

    if (rep.srmGetSpaceMetaDataResponse->arrayOfSpaceDetails) {
        mdp = rep.srmGetSpaceMetaDataResponse->arrayOfSpaceDetails;
        if (mdp->__sizespaceDataArray == 0) printf("arrayOfSpaceDetails size is zero!\n");
        for (i=0; i<mdp->__sizespaceDataArray; i++) {
            printf("\nDetails for space token [%d]:\n", i);

            if (mdp->spaceDataArray[i]->spaceToken != NULL)
                printf("  spaceToken         : %s\n", mdp->spaceDataArray[i]->spaceToken);
            else printf("STRANGE ERROR: spaceToke=NULL should not happen, it is a mandatory parameter!\n");
            
            if (mdp->spaceDataArray[i]->retentionPolicyInfo != NULL)
                printf("  retentionPolicyInfo: TODO\n");
            else printf("  retentionPolicyInfo: NULL\n");

            if (mdp->spaceDataArray[i]->status != NULL)
                printf("  status             : Code[%d] Explanation[%s]\n", mdp->spaceDataArray[i]->status->statusCode, mdp->spaceDataArray[i]->status->explanation);
            else printf("  status             : NULL\n");

            if (mdp->spaceDataArray[i]->owner != NULL)
                printf("  Owner              : %s\n", mdp->spaceDataArray[i]->owner);
            else printf("  Owner              : NULL\n");

            if (mdp->spaceDataArray[i]->totalSize != NULL)
                printf("  totalSize          : %lld\n", *(mdp->spaceDataArray[i]->totalSize));
            else printf("  totalSize          : NULL\n");

            if (mdp->spaceDataArray[i]->guaranteedSize != NULL) 
                printf("  guaranteedSize     : %lld\n", *(mdp->spaceDataArray[i]->guaranteedSize));
            else printf("  guaranteedSize     : NULL\n");

            if (mdp->spaceDataArray[i]->unusedSize != NULL)
                printf("  unusedSize         : %ld\n", *(mdp->spaceDataArray[i]->unusedSize));
            else  printf("  unusedSize         : NULL\n");

            if (mdp->spaceDataArray[i]->lifetimeAssigned != NULL)
                printf("  lifetimeAssigned   : %d\n", *(mdp->spaceDataArray[i]->lifetimeAssigned));
            else printf("  lifetimeAssigned   : NULL\n");

            if (mdp->spaceDataArray[i]->lifetimeLeft != NULL)
                printf("  lifetimeLeft       : %ld\n", *(mdp->spaceDataArray[i]->lifetimeLeft));
            else printf("  lifetimeLeft       : NULL\n");
        }
    } else {
        printf("No details assigned\n");
    }



#if 0
	/* code not ready FIXME */
	if (! repfs) {
		printf ("arrayOfPermissions is NULL\n");
		soap_end (&soap);
		exit (0);
	}

	for (i = 0; i < repfs->__size; i++) {
		if ((repfs->__ptr[i])->status->explanation)
			printf ("state[%d] = %d, explanation = %s\n", i,
			    (repfs->__ptr[i])->status->statusCode,
			    (repfs->__ptr[i])->status->explanation);
		else
			printf ("state[%d] = %d, Perm = %d, SURL = %s\n", i,
			    (repfs->__ptr[i])->status->statusCode,
				(repfs->__ptr[i])->userPermission,
				(repfs->__ptr[i])->surl->value);
	}

#endif
	soap_end (&soap);
	exit (0);
}
