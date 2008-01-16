// $Id$

#include "srmv2H.h"
#include "srmSoapBinding.nsmap"
#include "storm_util.h"

#define DEFPOLLINT 10

#define SRM_EP_PATH "/v2_1_1/srm"
#ifdef GSI_PLUGINS
#include "cgsi_plugin.h"
#endif

parseGetRequest (const char *sfn, struct ns1__TDirOption *dirOption, 
          enum ns1__TFileStorageType *fileStorageType,
          LONG64 *lifeTime, char *spaceToken, char *surl)
{
        size_t len;
        int lenp;
        char *p, *f;
        char  c[256];
/*
        if (strncmp (sfn, "srm://", 6)) {
                errno = EINVAL;
                return (-1);
        }
        if (p = strstr (sfn + 6, "?SFN=")) {
                p = p + 5;
        } else if (p = strchr (sfn + 6, '/')) {
                p = p;
        } else {
                errno = EINVAL;
                return (-1);
        }
*/  
    //Check SURL syntax (DOPPIO, RIMUOVERE?)
        if(parseSURL(sfn)){
        errno = EINVAL;
                return (-1);
        }
    
    *lifeTime = -1;


      if ( (f = strchr(sfn,'(')) != NULL) {
           len = (size_t)(f - sfn);
        } else {
           len = strlen(sfn);
        };
       
     strncpy(surl, sfn, len);
        *(surl + len) = '\0';

        if ((f = strchr(sfn,'(')) != NULL) {
        //Parsing "dirOptionParamter"
       p=strstr(sfn,"dirOption={");
           if (p != NULL) {
              f=strchr(p,',');
              if (f == NULL) {
        //printChar();
        //printf("Error in parsing: %s ",sfn);
                errno = EINVAL;
                return (-1);
              } else {
                 strncpy(c,p+11,(size_t)(f-p-11));
                 dirOption->allLevelRecursive = malloc(sizeof(int));
                 *(dirOption->allLevelRecursive) = atoi(c);

        if(*(dirOption->allLevelRecursive)>1) {
            printChar();
            printf("Error: \"dirOption->allLevelRecursive\"  parameter must be : 0|1.\n= Value specified:  %s \n",c);
            errno = EINVAL;
            return (-1);

        }
              } 
              
          p=strchr(f+1,',');
              
          if (f==NULL) {
                //printChar();
        //printf("Error in parsing: %s ",sfn);
                 errno = EINVAL;
                 return (-1);
              } else {
                 strncpy(c,f+1,(size_t)(p-f-1));
                 dirOption->isSourceADirectory = atoi(c);
                if(dirOption->isSourceADirectory>1) {
            printChar();
            printf("Error: \"dirOption->isSourceADirectory\" parameter must be : 0|1.\n= Value specified:  %s \n",c);
            errno = EINVAL;
            return (-1);
        }     
          
          }
              f=strchr(p+1,'}');
              if (f==NULL) {
                //printChar();
        //printf("Error in parsing: %s ",sfn);
                 errno = EINVAL;
                 return (-1);
              } else {
                strncpy(c,p+1,(size_t)(f-p-1));
        //char* numLev;
        //*numLev = strtol(c,NULL,10);
        dirOption->numOfLevels = atoi(c);
              }
           };
           p=strstr(sfn,"fileStorageType=");
           if (p != NULL) {
              f=strchr(p,',');
              if (f == NULL) {
                 f=strchr(p,')');
                 if (f==NULL) {
                    //printChar();
            //printf("Error in parsing: %s ",sfn);
                errno = EINVAL;
                    return (-1);
                 }
               }
               strncpy(c,p+16,(size_t)(f-p-16));
               *(c+(f-p-16)) = '\0';
               *fileStorageType = atoi(c);
         //Check the int value for  fileStorageTyoe 0|1|2
        if(fileStorageType>2) {
            printChar();
            printf("Error: FileStorageType parameter must be : 0|1|2.\n= Value specified:  %s \n",c);
            errno = EINVAL;
            return (-1);
                 }
        
           };

    //Parsing Lifetime Paramter
           p=strstr(sfn,"lifeTime=");
           if (p != NULL) {
              f=strchr(p,',');
              if (f==NULL) {
                 f=strchr(p,')');
                 if (f==NULL) {
                    errno = EINVAL;
                    return (-1);
                 }
               }
               strncpy(c,p+9,(size_t)(f-p-9));
               *(c+(f-p-9)) = '\0';
              // *lifeTime = atoll(c);
        char** errorString = NULL;
        *lifeTime =  strtol(c, NULL, 10);

        //TODO Occhio che in questo modo una stringa del tipo
        // 100aaa viene interpretata come 100
        if(*lifeTime==0) {
            printChar();
            printf("Error: LifeTime parameter must be numeric value.\n= Value specified:  %s \n",c);
            errno = EINVAL;
            return (-1);
        }
           };

    //Parsing SpaceToken Parameter
           p=strstr(sfn,"spaceToken=");
           if (p != NULL) {
              f=strchr(p,',');
              if (f==NULL) {
                 f=strchr(p,')');
                 if (f==NULL) {
                    errno = EINVAL;
                    return (-1);
                 }
               }
               strncpy(spaceToken,p+11,(size_t)(f-p-11));
               *(spaceToken+(f-p-11)) = '\0';
           };
        };

        return (0);
}


main(int argc, char **argv)
{
//    int flags;
//    int i;
//    int nbfiles;
//    int nbproto = 0;
//    char** lprotos;
//    static char *protocols[10];
///* = {
//#if GFAL_ENABLE_RFIO
//        "rfio    ",
//#endif
//#if GFAL_ENABLE_DCAP
//        "gsidcap ",
//#endif
//        ""
//    };
//*/
//    int r = 0;
//    char *r_token;
//    struct ns1__srmPrepareToGetResponse_ rep;
//    struct ns1__ArrayOfTGetRequestFileStatus *repfs;
//    struct ns1__srmPrepareToGetRequest req;
//    struct ns1__TGetFileRequest *reqfilep;
//    struct ns1__TReturnStatus *reqstatp;
//    char *sfn;
//    struct soap soap;
//    struct ns1__srmStatusOfGetRequestResponse_ srep;
//    struct ns1__srmStatusOfGetRequestRequest sreq;
//    char *srm_endpoint;
//
//        enum ns1__TFileStorageType FileStorageType = 1;
//        struct ns1__TDirOption DirOption;
//    enum xsd__boolean allLevels;
//        LONG64 LifeTime = 0;
//        char SpaceToken[256] = "";
//        char u_token[256] = "";
//        char *p;
//        int max_arg;
//        int num_arg;
//        int pollflg;
//    int j;
//    
//    // Print Help Menu
//    printLine();
//    printChar();
//    printf("SrmPrepareTGet Request \n");
//    printLine();
//
//        if (argc < 3) {
//        fprintf (stderr, "Usage: %s poll|nopoll [options] SURL[([surl_options])] [SURLs(..)]\n\n", argv[0]);
//        fprintf (stderr,  "  options:\n");
//                fprintf (stderr, "       prot=<protocol>         (<protocol> must be file or gsiftp.)\n");
//                fprintf (stderr, "       u_token=<usertoken>     (<usertoken> is the userRequestDescription string.)\n\n");
//                //fprintf (stderr, "       SURLs(");
//        fprintf (stderr, "  surl_options:\n");
//        fprintf (stderr, "       dirOption=<0|1>,<0|1>,<numofLevels>  (isSourceADirectory Flag, allLevelRecursive Flag ,\n");
//        fprintf (stderr, "                                             <numOfLevels> is the number of level to explore.) \n");
//            fprintf (stderr, "       fileStorageType=<0|1|2> (0:Volatile, 1:Durable, 2:Permanent) \n");
//                fprintf (stderr, "       lifeTime=<lifetime>     (<lifetime> is the lifetime in seconds.)\n");
//                fprintf (stderr, "       spaceToken=<token>      (<spaceToken> is a valid spaceToken string.)\n\n");
//
//        exit (1);
//
//       }
//       
//    DirOption.allLevelRecursive=&allLevels;
//        if (strncmp(argv[1],"poll",4) == 0) {
//            pollflg = 1;
//        } else if (strncmp(argv[1],"nopoll",6) == 0) {
//            pollflg = 0;
//        } else {
//            pollflg = -1;
//        }
//        if (pollflg < 0) {
//    
//        fprintf (stderr, "Usage: %s poll|nopoll [options] SURL[([surl_options])] [SURLs(..)]\n\n", argv[0]);
//        fprintf (stderr, "  options:\n");
//                fprintf (stderr, "       prot=<protocol>         (<protocol> must be file or gsiftp.)\n");
//                fprintf (stderr, "       u_token=<usertoken>     (<usertoken> is the userRequestDescription string.)\n\n");
//                //fprintf (stderr, "       SURLs(");
//        fprintf (stderr, "  surl_options:\n");
//        fprintf (stderr, "       dirOption=<0|1>,<0|1>,<numofLevels>  (isSourceADirectory Flag, allLevelRecursive Flag ,\n");
//        fprintf (stderr, "                                             <numOfLevels> is the number of level to explore.) \n");
//            fprintf (stderr, "       fileStorageType=<0|1|2> (0:Volatile, 1:Durable, 2:Permanent) \n");
//                fprintf (stderr, "       lifeTime=<lifetime>     (<lifetime> is the lifetime in seconds.)\n");
//                fprintf (stderr, "       spaceToken=<token>      (<spaceToken> is a valid spaceToken string.)\n\n");
//
//        exit (1);
//
//
//
//
//        /*
//                fprintf (stderr, "usage: %s poll|nopoll\n", argv[0]);
//                fprintf (stderr, "       [prot=protocol]\n");
//                fprintf (stderr, "       [u_token=u_token]\n");
//                fprintf (stderr, "       SURLs([dirOption={0|1,0|1,<numofLevels>}[,]]\n");
//                fprintf (stderr, "       [fileStorageType=0|1|2[,]][lifeTime=life[,]]\n");
//                fprintf (stderr, "       [spaceToken=token[,]])[...]\n");
//                exit (1);
//        */
//        }
//
//        if (argc < 5) {
//           max_arg = argc -1;
//        } else {
//           max_arg = 5;
//        };
//        
//    num_arg = 0;
//        protocols[0]=(char *)malloc((size_t)(5));
//        strcpy(protocols[0],"file");
//        for ( i=2; i<max_arg; i++) {
//           if ( (p = strstr(argv[i],"prot=")) != NULL ) {
//            free(protocols[0]);
//            protocols[0]=(char *)malloc((size_t)(strlen((const char*)(p+5))+1));
//            strcpy(protocols[0],p+5);
//            num_arg += 1;
//            p=NULL;
//           }
//           if ( (p = strstr(argv[i],"u_token=")) != NULL ) {
//            strcpy(u_token,p+8);
//            num_arg += 1;
//            p=NULL;
//           }
//        }
//
//        nbfiles = argc - num_arg - 2;
//    
//    //Check Syntax for each SURL specified
//    for(j=0;j<nbfiles;j++) {
//        if((j==0)&&(parseSURL_getEndpoint (argv[num_arg+2], &srm_endpoint, &sfn) < 0)) {
//            //perror ("parsesurl");
//            printLine();
//            exit(1);
//        } else if(parseSURL(argv[num_arg+2+j])<0) {
//            //perror ("parsesurl");
//            printLine();
//            exit(1);
//        }
//    }
//    
//
//    //while (*protocols[nbproto]) nbproto++;
//
//    nbproto = 1;
///*
//    lprotos = calloc (nbproto, sizeof(char*));
//    lprotos[0] = strdup(argv[1]);
//*/
//    soap_init (&soap);
//#ifdef GSI_PLUGINS
///* Delegate proxy ---- TO BE CHANGED */
///*     flags = CGSI_OPT_DISABLE_NAME_CHECK|CGSI_OPT_DELEG_FLAG; */
//
//    flags = CGSI_OPT_DISABLE_NAME_CHECK;
//    soap_register_plugin_arg (&soap, client_cgsi_plugin, &flags);
//#endif
//
//    memset (&req, 0, sizeof(req));
//    if ((req.arrayOfFileRequests =
//        soap_malloc (&soap, sizeof(struct ns1__ArrayOfTGetFileRequest))) == NULL ||
//        (req.arrayOfFileRequests->getRequestArray =
//        soap_malloc (&soap, nbfiles * sizeof(struct ns1__TGetFileRequest *))) == NULL ||
//        (req.arrayOfTransferProtocols =
//        soap_malloc (&soap, sizeof(struct ns1__ArrayOf_USCORExsd_USCOREstring))) == NULL) {
//        perror ("malloc");
//        soap_end (&soap);
//        exit (1);
//    }
//
//    for (i = 0; i < nbfiles; i++) {
//        if ((req.arrayOfFileRequests->getRequestArray[i] =
//            soap_malloc (&soap, sizeof(struct ns1__TGetFileRequest))) == NULL) {
//            perror ("malloc");
//            soap_end (&soap);
//            exit (1);
//        }
//    }
//    req.arrayOfFileRequests->__sizegetRequestArray = nbfiles;
//    req.arrayOfTransferProtocols->stringArray = protocols;
//    req.arrayOfTransferProtocols->__sizestringArray = nbproto;
//    if ( strcmp(u_token, "") )
//      req.userRequestDescription = u_token;
//
//    for (i = 0; i < nbfiles; i++) {
//                //Check syntax for each SURL and optional attribute specifiedi
//        char *SURL=calloc(256,sizeof(char));
//
//        if (parseGetRequest(argv[i+num_arg+2],&DirOption,&FileStorageType,
//                              &LifeTime,SpaceToken,SURL) < 0) {
//                        printChar();
//            printf("Error in parsing SURL: %s\n",argv[i+num_arg+2]);
//            //perror ("par");
//                        exit (1);
//                }
//
//        reqfilep = req.arrayOfFileRequests->getRequestArray[i];
//                memset (reqfilep, 0, sizeof(*reqfilep));
//        if ((reqfilep->fromSURLInfo =
//            soap_malloc (&soap, sizeof(struct ns1__TSURLInfo))) == NULL ||
//            (reqfilep->fromSURLInfo->SURLOrStFN =
//            soap_malloc (&soap, sizeof(struct ns1__TSURL))) == NULL) {
//            perror ("malloc");
//            soap_end (&soap);
//            exit (1);
//        }
//        reqfilep->fromSURLInfo->SURLOrStFN->value = SURL;
//                if ((reqfilep->dirOption =
//                    soap_malloc (&soap, sizeof(struct ns1__TDirOption))) == NULL) {
//                        perror ("malloc");
//                        soap_end (&soap);
//                        exit (1);
//                }
//                reqfilep->dirOption->isSourceADirectory = DirOption.isSourceADirectory;
//                reqfilep->dirOption->numOfLevels = DirOption.numOfLevels;
//                if ((reqfilep->dirOption->allLevelRecursive =
//                     soap_malloc (&soap, sizeof(int ))) == NULL) {
//                    perror ("malloc");
//                    soap_end (&soap);
//                    exit (1);
//                }
//                *(reqfilep->dirOption->allLevelRecursive) = *(DirOption.allLevelRecursive);
//
//                if ((reqfilep->fileStorageType =
//                    soap_malloc (&soap, sizeof(enum ns1__TFileStorageType))) == NULL) {
//                        perror ("malloc");
//                        soap_end (&soap);
//                        exit (1);
//                }
//                *reqfilep->fileStorageType = FileStorageType;
//                if(LifeTime == -1)
//                    reqfilep->lifetime = NULL;
//                else{
//                    if ((reqfilep->lifetime =
//                         soap_malloc (&soap, sizeof(struct ns1__TLifeTimeInSeconds))) == NULL) {
//                        perror ("malloc");
//                        soap_end (&soap);
//                        exit (1);
//                    }else
//                        reqfilep->lifetime->value = LifeTime;
//                }
//                if ((reqfilep->spaceToken =
//                    soap_malloc (&soap, sizeof(struct ns1__TSpaceToken))) == NULL) {
//                        perror ("malloc");
//                        soap_end (&soap);
//                        exit (1);
//                }
//                reqfilep->spaceToken->value = SpaceToken;
//    }
//
//    if (soap_call_ns1__srmPrepareToGet (&soap, srm_endpoint, "PrepareToGet",
//        &req, &rep)) {
//        soap_print_fault (&soap, stderr);
//        soap_print_fault_location (&soap, stderr);
//        soap_end (&soap);
//        exit (1);
//    }
//    reqstatp = rep.srmPrepareToGetResponse->returnStatus;
//    repfs = rep.srmPrepareToGetResponse->arrayOfFileStatuses;
//    if (rep.srmPrepareToGetResponse->requestToken) {
//        r_token = rep.srmPrepareToGetResponse->requestToken->value;
//        printChar();
//        printf ("Request Token :[%s]\n", r_token);
//    }
//
//    printLine();
//
//    memset (&sreq, 0, sizeof(sreq));
//    sreq.requestToken = rep.srmPrepareToGetResponse->requestToken;
//
//        if (pollflg) {
//    /* wait for file "ready" */
//       printChar();
//       printf("StatusOfGetRequest: \n");    
//
//       while (reqstatp->statusCode == SRM_USCOREREQUEST_USCOREQUEUED ||
//        reqstatp->statusCode == SRM_USCOREREQUEST_USCOREINPROGRESS ||
//        reqstatp->statusCode == SRM_USCOREREQUEST_USCORESUSPENDED) {
//        printChar();
//        printf("Request Status [%d] : [%s]\n", 
//        reqstatp->statusCode,
//        reconvertStatusCode(reqstatp->statusCode) );
//
//        sleep ((r++ == 0) ? 1 : DEFPOLLINT);
//        if (soap_call_ns1__srmStatusOfGetRequest (&soap, srm_endpoint,
//            "StatusOfGetRequest", &sreq, &srep)) {
//            soap_print_fault (&soap, stderr);
//            soap_end (&soap);
//            exit (1);
//        }
//        reqstatp = srep.srmStatusOfGetRequestResponse->returnStatus;
//        repfs = srep.srmStatusOfGetRequestResponse->arrayOfFileStatuses;
//       }
//       
//       printChar();
//       printf ("Request Status [%d] : [%s]\n", reqstatp->statusCode,reconvertStatusCode(reqstatp->statusCode) );
//       
//       //printf ("request state %d\n", reqstatp->statusCode);
//       if (reqstatp->statusCode != SRM_USCORESUCCESS &&
//          reqstatp->statusCode != SRM_USCOREDONE) {
//        if (reqstatp->explanation) {
//            printChar();
//            printf ("Explanation String: [%s]\n", reqstatp->explanation);
//        
//        }
//        soap_end (&soap);
//        exit (1);
//       }  
//       if (! repfs) {
//        printf ("arrayOfFileStatuses is NULL\n");
//        soap_end (&soap);
//        exit (1);
//       }
//        
//       printLine();
//        
//       for (i = 0; i < repfs->__sizegetStatusArray; i++) {
//        printChar();
//        printf("Request [%d]\n",i);
//        
//        if (repfs->getStatusArray[i]->transferURL) {
//            printChar();
//            printf ("Status[%d] : [%s]\n= Explanation : [%s] \n",
//                (repfs->getStatusArray[i])->status->statusCode,
//                reconvertStatusCode((repfs->getStatusArray[i])->status->statusCode),
//             (repfs->getStatusArray[i])->status->explanation );
//            printChar();
//            printf ("TURL = [%s]\n", (repfs->getStatusArray[i])->transferURL->value);
//            
//
//    } else if ((repfs->getStatusArray[i])->status->explanation) {
//            printChar();
//            printf ("Status[%d] : [%s]\n= Explanation = [%s]\n",
//            (repfs->getStatusArray[i])->status->statusCode,
//            reconvertStatusCode((repfs->getStatusArray[i])->status->statusCode),
//            (repfs->getStatusArray[i])->status->explanation);
//         } else {
//            printChar();
//            printf ("Status[%d] : [%s]\n", 
//                (repfs->getStatusArray[i])->status->statusCode,
//                reconvertStatusCode((repfs->getStatusArray[i])->status->statusCode) );
//        
//        }
//       }
//            printLine();
//    }
//
//    soap_end (&soap);
//    exit (0);
}
