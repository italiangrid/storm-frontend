// $Id$

#include "srmv2H.h"
#include "srmSoapBinding.nsmap"
#include "storm_util.h"

#define DEFPOLLINT 10

#define SRM_EP_PATH "/v2_1_1/srm"
#ifdef GSI_PLUGINS
#include "cgsi_plugin.h"
#endif

main(int argc,char** argv)
{
	int flags;
	int i;
	int nbfiles;
	int nbproto = 0;
	static enum xsd__boolean trueoption = true_;  //  JPB
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
	int r = 0;
	char *r_token;
	struct ns1__srmLsResponse_ rep;
	struct ns1__ArrayOfTMetaDataPathDetail *repfs;
	struct ns1__srmLsRequest req;
	struct ns1__TReturnStatus *reqstatp;
	char *sfn;
	struct soap soap;
	struct ns1__srmStatusOfGetRequestResponse_ srep;
	struct ns1__srmStatusOfGetRequestRequest sreq;
	char *srm_endpoint;
    char *p;
    char *dummy; // used for reading values of optional arguments
    int fileStorageType = -1;
    int fullDetailedList = 0;
    int allLevelRecursive = 0;
    int numOfLevels = 0;
    int offset = 0; 
    int count = 0;
    int numArgs = 0; // number of optional arguments
	int j;
	
	// Print Help Menu
	printLine();
	printLine();
	printChar();
	printf("SrmLs Request \n");
	printLine();



	if (argc < 2) {
		fprintf(stderr, "usage: %s [options] SURLs\n", argv[0]);
        fprintf(stderr, "   options:\n");    
        fprintf(stderr, "       fileStorageType={0|1|2}  (Valatile|Durable|Permanent) \n");
        fprintf(stderr, "       fullDetailedList={0|1}   (false|true)\n");
        fprintf(stderr, "       allLevelRecursive={0|1}  (false|true)\n");
        fprintf(stderr, "       numOfLevels=<value>      (value is an integer value)\n");
        fprintf(stderr, "       count=<value>            (value is an integer value)\n");
		exit (1);
	}

	for (i=0; i<(argc-1); i++) {
           
        if ((p = strstr(argv[i+1],"fileStorageType=")) != NULL) {
            numArgs++;
            dummy = (char*) malloc (10);
            strcpy(dummy,p+16);
            fileStorageType = atoi(dummy);
            if (fileStorageType<0 || fileStorageType>2) {
                printChar();
                fprintf(stderr, "Error: fileStorageType can only be 0, 1 or 2 (representing Volatile, Durable and Permanent)\n");
                printLine();
                exit(-1);
            }
            printChar();
            printf("fileStorageType: [%i] \n", fileStorageType);
            p = NULL;
            free(dummy);
            continue; 
        }
          
        if ( (p = strstr(argv[i+1],"fullDetailedList=")) != NULL) {
            numArgs++;
            dummy = (char*) malloc (1);
            strcpy(dummy,p+17);
            fullDetailedList = atoi(dummy);
            if (fullDetailedList !=0 && fullDetailedList !=1) {
                printChar();
                fprintf(stderr, "Error: fullDetailedList has to be either 0 or 1 (representing false or true)\n");
                printLine();
                exit(-1); 
            }
            printChar();
            printf("fullDetailedList: [%i] \n", fullDetailedList);
            p = NULL;
            free(dummy);
            continue;
        }

        if ( (p = strstr(argv[i+1],"allLevelRecursive=")) != NULL) {
            numArgs++; 
            dummy = (char*) malloc (1);
            strcpy(dummy,p+18);
            allLevelRecursive = atoi(dummy);
            printChar();
            if (allLevelRecursive !=0 && allLevelRecursive !=1) {
                fprintf(stderr, "Error: allLevelRecursive has to be either 0 or 1 (representing false or true)\n");
                printLine();
                exit(-1);
            }
            printf("allLevelRecursive: [%i] \n", allLevelRecursive);
            p = NULL;
            free(dummy);
            continue;
        }

        if ((p = strstr(argv[i+1],"numOfLevels=")) != NULL) {
            numArgs++;
            dummy = (char*) malloc (10);
            strcpy(dummy,p+12); 
            printChar();
            numOfLevels =  strtol(dummy, NULL, 10);
            if (numOfLevels==0) {
                printf("Error: numOfLevels has to be a numeric value different than 0");
                printLine();
                exit(-1);	
            }

            printf("numOfLevels: [%i] \n", numOfLevels); 
            p = NULL;
            free(dummy);
            continue;
        }  
       
        if ((p = strstr(argv[i+1],"offset=")) != NULL) {
            numArgs++;
            dummy = (char*) malloc (10);
            strcpy(dummy,p+7);
       		offset =  strtol(dummy, NULL, 10);
            if (offset==0) {
                printf("Error: offset has to be a numeric value different than 0");
                printLine();
                exit(-1);	
            }

            printf("offset: [%i] \n", offset);
            p = NULL;
            free(dummy);
            continue;
        } 
          
        if ((p = strstr(argv[i+1],"count=")) != NULL) {
            numArgs++; 
            dummy = (char*) malloc (10);
            strcpy(dummy,p+6);
            printChar();
          	count =  strtol(dummy, NULL, 10);
            if(count==0) {
                printf("Error: count has to be a numeric value different than 0");
                printLine();
                exit(-1);	
            }

            printf("count: [%i] \n", count);
            p = NULL;
            free(dummy);
            continue;
        }
	} // for (i=0; i<(agrc-2); i++)      
        
    nbfiles = argc - 1 - numArgs;

	for(j=0;j<nbfiles;j++) {
        if ((j==0) && parseSURL_getEndpoint(argv[1+numArgs+j], &srm_endpoint, &sfn)<0) {
            printLine();
			exit(1);
		} else if(parseSURL(argv[1+numArgs+j])<0) {
			printLine();
			exit(1);
		}
	}	

	while (*protocols[nbproto]) nbproto++;

	soap_init (&soap);
#ifdef GSI_PLUGINS
	flags = CGSI_OPT_DISABLE_NAME_CHECK | CGSI_OPT_DELEG_FLAG;
	soap_register_plugin_arg (&soap, client_cgsi_plugin, &flags);
#endif

	memset(&req, 0, sizeof(req));
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

	req.arrayOfSURLs->__sizeurlArray = nbfiles;

	for (i=0; i<nbfiles; i++) req.arrayOfSURLs->urlArray[i] = argv[i+numArgs+1];

    // Assign all optional arguments
    req.authorizationID   = NULL;
    req.storageSystemInfo = NULL; 
    req.fileStorageType   = (enum ns1__TFileStorageType *) &fileStorageType;
    req.fullDetailedList  = (enum xsd__boolean *) &fullDetailedList;
    req.allLevelRecursive = (enum xsd__boolean *) &allLevelRecursive;
    req.numOfLevels       = &numOfLevels;      
    req.offset            = &offset;
    req.count             = &count;   

	if (soap_call_ns1__srmLs (&soap, srm_endpoint, "Ls", &req, &rep)) {
		soap_print_fault (&soap, stderr);
		soap_print_fault_location (&soap, stderr);
		soap_end (&soap);
		exit (1);
	}
        
    // Check if server sends a correct response
    if (rep.srmLsResponse == NULL) {
		printChar();
		printf("Error:StoRM Server cannot process the request! \n");
		printLine();
        soap_end(&soap);
        exit(-1);
    }
    
	reqstatp = rep.srmLsResponse->returnStatus;
    repfs = rep.srmLsResponse->details;
	printChar();
	printf ("Global Request Status [%d] : [%s],\n= Explanation [%s] \n", reqstatp->statusCode,
                                                                         reconvertStatusCode(reqstatp->statusCode),
                                                                         reqstatp->explanation);
      
    if (repfs) {
        printChar();
        printf ("Number of pathDetail result: %d\n", repfs->__sizepathDetailArray);
    }
    
	if (! repfs) {
		printf ("arrayOfTMetaDataPathDetail is NULL\n");
		soap_end (&soap);
		exit (1);
	}
	printChar();
	printf("Details on the returned entries: \n================================ \n"); // HS

	for (i=0; i<repfs->__sizepathDetailArray; i++) {
		printChar();
		if (repfs->pathDetailArray[i]->surl != NULL)
        	printf("Path: [%s]\n", repfs->pathDetailArray[i]->surl);
		else printf("Path: NULL\n");
        
        printChar();
        if (repfs->pathDetailArray[i]->status != NULL) {
        	printf("Status Code: [%d] ", repfs->pathDetailArray[i]->status->statusCode);
        	if (repfs->pathDetailArray[i]->status->explanation != NULL)
        		printf("[%s]\n", repfs->pathDetailArray[i]->status->explanation);
			else printf("[NULL]\n");
        }
        else printf("Status Code: NULL\n");
		
		printChar();
        if (repfs->pathDetailArray[i]->size != NULL)
			printf("Size: [%lld]\n", *(repfs->pathDetailArray[i]->size));
		else printf("Size: NULL\n");
		
		printChar();
		if (repfs->pathDetailArray[i]->type != NULL)
            printf("Type: [%d]\n", *(repfs->pathDetailArray[i]->type));
		else printf("Type: NULL\n");
		
		printChar();
		if (repfs->pathDetailArray[i]->fileStorageType) {
            printf("FileStorageType:[%d]\n", *(repfs->pathDetailArray[i]->fileStorageType));
		}
		else printf("FileStorageType: NULL\n");
        
        printChar();
		if ( repfs->pathDetailArray[i]->checkSumType ) {
            printf("CheckSumTYpe: %s\n", repfs->pathDetailArray[i]->checkSumType);
		}
		else printf("CheckSumTYpe: NULL\n");
		
		if ( repfs->pathDetailArray[i]->createdAtTime ) {
            printf("= CreateadAtTime: [%s]\n", repfs->pathDetailArray[i]->createdAtTime);
		} else printf("= CreateadAtTime: NULL\n");
            
		if ( repfs->pathDetailArray[i]->lastModificationTime )
            printf("= LastModificationTime: [%s]\n", repfs->pathDetailArray[i]->lastModificationTime);
		else printf("= LastModificationTime: NULL\n");
            
		if ( repfs->pathDetailArray[i]->ownerPermission )
            printf("= OwnerPermission: [%d]\n", repfs->pathDetailArray[i]->ownerPermission->mode);
		else printf("= OwnerPermission: NULL\n");
            
		if ( repfs->pathDetailArray[i]->otherPermission )
            printf("= OtherPermission: [%d]\n", *(repfs->pathDetailArray[i]->otherPermission));
        else printf("= OtherPermission: NULL\n");
                      
		if ( repfs->pathDetailArray[i]->groupPermission ) 
            printf("= GroupPermission: [%d]\n", repfs->pathDetailArray[i]->groupPermission->mode);
        else printf("= GroupPermission: NULL\n");
        
		if ( repfs->pathDetailArray[i]->lifetimeAssigned )
            printf("= LifetimeAssigned: [%d]\n", *(repfs->pathDetailArray[i]->lifetimeAssigned));
        else printf("= LifetimeAssigned: NULL\n");
            
		if ( repfs->pathDetailArray[i]->lifetimeLeft )
            printf("= LifetimeLeft: [%d]\n", *(repfs->pathDetailArray[i]->lifetimeLeft));
        else printf("= LifetimeLeft: NULL\n");
		
		printLine();
	}
	soap_end (&soap); 
	exit (0);
}

