// $Id$

#include "srmv2H.h"
#include "srmSoapBinding.nsmap"
#include "storm_util.h"

#define DEFPOLLINT 10

#define SRM_EP_PATH "/v2_1_1/srm"

#ifdef GSI_PLUGINS
#include "cgsi_plugin.h"
#endif

#define DEB 0

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
	struct ns1__srmReserveSpaceResponse_ rep;
    struct ns1__srmReserveSpaceResponse *repp;
	struct ns1__srmReserveSpaceRequest req;
	struct ns1__TReturnStatus *reqstatp;
	char *sfn;
	struct soap soap;
	
	char *srm_endpoint;
	char u_token[256];
	char sysinfo[256];
	
	LONG64* totalSizeInBytes;
	LONG64* guaranteedSizeInBytes;
	long *lifetime;
	int spacetype;
	
	int max_arg;
	int num_arg=0;
	int old_num_arg=0;
	char* p;


	// Print Help Menu
	printLine();
	printLine();
	printChar();
	printf("SrmReserveSpace Request \n");
	printLine();



	if (argc < 2) {
		fprintf (stderr, "\nUsage: %s endPoint  [options]\n\n", argv[0]);
		fprintf (stderr, "       endPoint                (is the StoRM server endpoint : srm://hostname:port.)\n");
        fprintf (stderr, "       u_token=<token>         (<token> is the userTokenDescription string.)\n");
        fprintf (stderr, "       sysinfo=<sysinfo>       (<sysinfo> is the storageSystemInfo string.)\n");
        fprintf (stderr, "       totalSize=<t_size>      (<t_size> is the Total size of space desired in bytes)\n");
		fprintf (stderr, "       guaranteedSize=<g_size> (<g_size> is the Size of Guaranteed space desired in bytes)\n");
		fprintf (stderr, "       lifeTime=<lifetime>     (<lifetime> is the space lifetime in seconds.)\n");
       	fprintf (stderr, "       typeOfSpace=<0|1|2>     (0:Volatile, 1:Durable, 2:Permanent) \n\n");
		exit (-1);
	}
#if 0
	if (parsesurl (argv[3], &srm_endpoint, &sfn) < 0) {
		perror ("parsesurl");
		exit (1);
	}
#endif

	soap_init (&soap);
#ifdef GSI_PLUGINS
	flags = CGSI_OPT_DISABLE_NAME_CHECK | CGSI_OPT_DELEG_FLAG;
	soap_register_plugin_arg (&soap, client_cgsi_plugin, &flags);
#endif
	memset (&req, 0, sizeof(req));

	if (argc <= 7) max_arg = argc - 1; 
	else max_arg = 7;
       
	//TODO PARSE SURL DOVE?
	if(parseSURL_noSFN_getEndpoint(argv[1], &srm_endpoint)<0 ) {
    	printLine();
    	exit(-1);
	}
		
	if (DEB) printf("Nubmer of max_arg=%i\n",max_arg);
	
	//Check for specified parameters 
	for(i=2;i<max_arg+1; i++) {
		old_num_arg = num_arg;
		if(DEB) printf(" Checking Parameter:%i\n",i);	
		
		//Check User Token Description
        if ((p = strstr(argv[i],"u_token="))!=NULL ) {
            strcpy((char*) u_token, p+8);
            req.userSpaceTokenDescription = u_token;
            num_arg++;
        } else
        
		//Check SystemInfo
		if ((p = strstr(argv[i], "sysinfo=")) != NULL ) {
			strcpy((char*) sysinfo, p+8);
            printf("Argument sysinfo IGNORED\n");
			num_arg++;
        } else
        
		//CHECH TOTAL SIZE
	  	if ((p=strstr(argv[i],"totalSize=")) != NULL) {

			if ((req.desiredSizeOfTotalSpace = soap_malloc(&soap, sizeof(ULONG64))) == NULL) {
				perror ("malloc");
				soap_end (&soap);
				exit (-1);
			}
			
			if ((totalSizeInBytes =(ULONG64*) malloc(sizeof(ULONG64))) == NULL) {
				perror("malloc");
				exit(-1);
			}	

			*totalSizeInBytes =  strtol(p+10, NULL, 10);

			//	printf("SIZE=%d",*sizeInBytes);
			//TODO Occhio che in questo modo una stringa del tipo
			// 100aaa viene interpretata come 100
			if (*totalSizeInBytes<=0) {
				printChar();
				printf("Error: TotalSizeInBytes parameter must be numeric value.\n= Value specified:  %ld \n",*totalSizeInBytes);
				errno = EINVAL;
				return (-1);
			} 
          
			*(req.desiredSizeOfTotalSpace) = *totalSizeInBytes;
            free(totalSizeInBytes);
			num_arg++;
		} else
        
		//Check Guaranteed Size
	  	if ((p=strstr(argv[i],"guaranteedSize=")) != NULL) {
			if(DEB) printf("GuaranteedSize found.%s\n",p+15);

			if ((guaranteedSizeInBytes =(ULONG64*) malloc(sizeof(ULONG64))) == NULL) {
				perror("malloc");
				exit(-1);
			}	
				
			*guaranteedSizeInBytes = strtol(p+15, NULL, 10);
			//	printf("SIZE=%d",*sizeInBytes);
			//TODO Occhio che in questo modo una stringa del tipo
			// 100aaa viene interpretata come 100
			if (*guaranteedSizeInBytes<=0) {
				printChar();
				printf("Error: GuaranteedSizeInBytes parameter must be numeric value.\n= Value specified:  %ld \n",*guaranteedSizeInBytes);
				errno = EINVAL;
				return (-1);
			} 
			          
			req.desiredSizeOfGuaranteedSpace = *guaranteedSizeInBytes;
            num_arg+=1;
		}else
        
		//Check Lifetime
	  	if ((p=strstr(argv[i],"lifeTime=")) != NULL) {

			if ((req.desiredLifetimeOfReservedSpace = soap_malloc (&soap, sizeof(int))) == NULL) {
				perror ("malloc");
				soap_end (&soap);
				exit (1);
			}
			lifetime = (long *) malloc(sizeof(long));
	
			*lifetime = (int) strtol(p+9, NULL, 10);
			
			//	printf("SIZE=%d",*sizeInBytes);
			//TODO Occhio che in questo modo una stringa del tipo
			// 100aaa viene interpretata come 100
			if (*lifetime<=0) {
				printChar();
				printf("Error: lifetime parameter must be numeric value.\n= Value specified:  %ld \n",*lifetime);
				errno = EINVAL;
				return (-1);
			} 
          
			*(req.desiredLifetimeOfReservedSpace) = *lifetime;
			num_arg++;
		}
    } //for
    
//    req.retentionPolicyInfo = soap_malloc(&soap, sizeof(struct ns1__TRetentionPolicyInfo));
//    if (req.retentionPolicyInfo == NULL) {
//        perror ("Error allocationg memory for retentionPolicyInfo\n");
//        soap_end(&soap);
//        exit(1);
//    }
//    req.retentionPolicyInfo->accessLatency = soap_malloc(&soap, sizeof(int));
//    if (req.retentionPolicyInfo->accessLatency == NULL) {
//        perror ("Error allocationg memory for retentionPolicyInfo->accessLatency\n");
//        soap_end(&soap);
//        exit(1);
//    }
//    req.retentionPolicyInfo->retentionPolicy = 0;
//    *(req.retentionPolicyInfo->accessLatency) = 0;
    req.retentionPolicyInfo = NULL;
    req.authorizationID = NULL;
    req.arrayOfExpectedFileSizes = NULL;
    req.storageSystemInfo = NULL;
    req.transferParameters = NULL;

     	/* To send the request ... */
	if (soap_call_ns1__srmReserveSpace (&soap, srm_endpoint, "SrmReserveSpace", &req, &rep)) {
		soap_print_fault (&soap, stderr);
		soap_print_fault_location (&soap, stderr);
		soap_end (&soap);
		exit (1);
	}
	reqstatp = rep.srmReserveSpaceResponse->returnStatus;
	printChar();
	printf ("Request Status [%d] : [%s]\n", reqstatp->statusCode, reconvertStatusCode(reqstatp->statusCode));
    if (reqstatp->explanation) {
        printChar();
        printf ("Explanation:[%s]\n", reqstatp->explanation);
    }

    repp = rep.srmReserveSpaceResponse;
	printChar();
    if (repp->spaceToken != NULL)
        printf("SpaceToken             : [%s]\n", repp->spaceToken);
    else printf("SpaceToken             : NULL\n");
    printChar();
    if (repp->sizeOfTotalReservedSpace != NULL)
        printf("totalReservedSpace     : %lld\n", *(repp->sizeOfTotalReservedSpace));
    else printf("totalReservedSpace     : NULL\n");
    printChar();
    if (repp->sizeOfGuaranteedReservedSpace != NULL)
        printf("guaranteedReservedSpace: %lld\n", *(repp->sizeOfGuaranteedReservedSpace));
    else printf("guaranteedReservedSpace: NULL\n");
    printChar();
    if (repp->lifetimeOfReservedSpace != NULL)
        printf("lifetimeOfReservedSpace: %d\n", *(repp->lifetimeOfReservedSpace));
    else printf("lifetimeOfReservedSpace: NULL\n");
    printChar();
    if (repp->retentionPolicyInfo != NULL)
        printf("retentionPolicyInfo    : NOT NULL\n");
    else printf("retentionPolicyInfo    : NULL\n");

	soap_end (&soap);
	printLine();
	exit (1);

} 
