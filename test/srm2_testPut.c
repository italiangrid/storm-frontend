// $Id$

#include "srmv2H.h"
#include "srmSoapBinding.nsmap"
#include "storm_util.h"
#define DEFPOLLINT 10

#define SRM_EP_PATH "/v2_1_1/srm"

#ifdef GSI_PLUGINS
#include "cgsi_plugin.h"
#endif

#include <stdlib.h>
#include <string.h>
#include <limits.h>


parsePutRequest (const char *sfn, enum ns1__TFileStorageType *fileStorageType, 
          LONG64 *sizeInBytes, LONG64 *lifeTime, char *spaceToken, char *surl)
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
    
    f = strchr(sfn,'(');
    len = strlen(sfn);
    if ( f != NULL ) {
        len = (size_t)(f - sfn);
    };
    strncpy(surl, sfn, len);
    *(surl + len) = '\0';

    if ((f = strchr(sfn,'(')) != NULL) {

        //Parsing "fileStorageType" parameter 
        p=strstr(sfn,"fileStorageType=");
        if (p != NULL) {
            f=strchr(p,',');
            if (f == NULL) {
                f=strchr(p,')');
                if (f==NULL) {
                    printChar();
                    printf("Error in parsing: %s ",sfn);
                    errno = EINVAL;
                    return (-1);
                }
            }
            strncpy(c,p+16,(size_t)(f-p-16));
            *(c+(f-p-16)) = '\0';
            *fileStorageType = atoi(c);
    
            //Check the int value ofr fileStorageTyoe 0|1|2
            if(*fileStorageType>2) {
                printChar();
                printf("Error: fileStorageType parameter must be : 0|1|2.\n= Value specified:  %s \n",c);
                errno = EINVAL;
                return (-1);
            }
        
        }
    
        //Parsing "sizeInBytes" paramter
        //FARE Check usando strtol()
        p=strstr(sfn,"sizeInBytes=");

        if (p != NULL) {
            f=strchr(p,',');
            if (f == NULL) {
                f=strchr(p,')');
                if (f == NULL) {
                    //  printChar();
                    //  printf("Error in parsing: %s ",sfn);
                    errno = EINVAL;
                    return (-1);
                }
            }
            strncpy(c,p+12,(size_t)(f-p-12));
            *(c+(f-p-12)) = '\0';
            // *sizeInBytes = atoll(c);
        
            char** errorString = NULL;
            char *strtoll_err;
            *sizeInBytes =  strtoll(c, &strtoll_err, 10);
            /* some gcc version does not define LLONG_MIN and LLONG_MAX. See limits.h...*/
/*             if(*sizeInBytes == LLONG_MIN || *sizeInBytes == LLONG_MAX){ */
/*                 printChar(); */
/*                 printf("Error: SizeInBytes parameter overflow or underflow.\n"); */
/*                 errno = EINVAL; */
/*                 return (-1); */
/*             } */
            if(*strtoll_err != '\0'){ 
                printChar();
                printf("Error: SizeInBytes contains invalid char.\n");
                errno = EINVAL;
                return (-1);
            }
            //  printf("SIZE=%d",*sizeInBytes);
            //TODO Occhio che in questo modo una stringa del tipo
            // 100aaa viene interpretata come 100
            if(*sizeInBytes==0) {
                printChar();
                printf("Error: SizeInBytes parameter must be numeric value.\n= Value specified:  %s \n",c);
                errno = EINVAL;
                return (-1);
            }
        };


        //Parsing "lifetime" paramter
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
    
        }

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
//    static char *protocols[10]; 
///* = {
//#if GFAL_ENABLE_RFIO
//        "rfio",
//#endif
//#if GFAL_ENABLE_DCAP
//        "gsidcap",
//#endif
//        "file"
//    };
//*/
//    int r = 0;
//    char *r_token;
//    struct ns1__srmPrepareToPutResponse_ rep;
//    struct ns1__ArrayOfTPutRequestFileStatus *repfs;
//    struct ns1__srmPrepareToPutRequest req;
//    struct ns1__TPutFileRequest *reqfilep;
//    struct ns1__TReturnStatus *reqstatp;
//    char *sfn;
//    struct soap soap;
//    struct soap soapr;
//    struct ns1__srmStatusOfPutRequestResponse_ srep;
//    struct ns1__srmStatusOfPutRequestRequest sreq;
//    char *srm_endpoint;
//
//    enum ns1__TFileStorageType FileStorageType = 1;
//        enum ns1__TOverwriteMode overwrite = 0;
//        LONG64 SizeInBytes = 0;
//        LONG64 LifeTime = 0;
//        char SpaceToken[256] = "";
//        char u_token[256] = "";
//        char overwr[10] = "";
//        char *p;
//        int max_arg;
//        int num_arg;
//        int pollflg;
//        
//    int j;
//    //userRequestDescription
//    //char *u_token;
//
//    // Print Header
//    printLine();
//    printLine();
//    printChar();
//    printf("SrmPrepareToPut Request \n");
//    printLine();
//
//    if (argc < 3) {
//        fprintf (stderr, "\nUsage: %s poll|nopoll [options] SURL[([surl_option])] [SURLs(..)]\n\n", argv[0]);
//        fprintf (stderr,  "  option:\n");
//                fprintf (stderr, "       prot=<protocol>     (<protocol> must be file or gsiftp)\n");
//                fprintf (stderr, "       u_token=<usertoken>     (<usertoken> is the userRequestDescription string)\n");
//                fprintf (stderr, "       overwrite=<0|1|2>   (0:Never, 1:Always,  2:WhenFilesAreDifferent)\n\n");
//                //fprintf (stderr, "       SURLs(");
//        fprintf (stderr, "  surl_option:\n");
//        fprintf (stderr, "       fileStorageType=<0|1|2> (0:Volatile, 1:Durable, 2:Permanent) \n");
//                fprintf (stderr, "       sizeInBytes=<size>  (<size> is the size in bytes)\n");
//        fprintf (stderr, "       lifeTime=<lifetime>     (<lifetime> is the lifetime in seconds)\n");
//                fprintf (stderr, "       spaceToken=<token>  (<spaceToken> is a valid spaceToken string)\n\n");
//        exit (1);
//    }
//
//        if (strncmp(argv[1],"poll",4) == 0) {
//            pollflg = 1;
//        } else if (strncmp(argv[1],"nopoll",6) == 0) {
//            pollflg = 0;
//        } else {
//            pollflg = -1;
//        }
//        if (pollflg < 0) {
//        
//        fprintf (stderr, "\nUsage: %s poll|nopoll [options] SURL[([surl_option])] [SURLs(..)]\n\n", argv[0]);
//        fprintf (stderr,  "  option:\n");
//                fprintf (stderr, "       prot=<protocol>     (<protocol> must be file or gsiftp)\n");
//                fprintf (stderr, "       u_token=<usertoken>     (<usertoken> is the userRequestDescription string)\n");
//                fprintf (stderr, "       overwrite=<0|1|2>   (0:Never, 1:Always,  2:WhenFilesAreDifferent)\n\n");
//                //fprintf (stderr, "       SURLs(");
//        fprintf (stderr, "  surl_option:\n");
//        fprintf (stderr, "       fileStorageType=<0|1|2> (0:Volatile, 1:Durable, 2:Permanent) \n");
//                fprintf (stderr, "       sizeInBytes=<size>  (<size> is the size in bytes)\n");
//        fprintf (stderr, "       lifeTime=<lifetime>     (<lifetime> is the lifetime in seconds)\n");
//                fprintf (stderr, "       spaceToken=<token>  (<spaceToken> is a valid spaceToken string)\n\n");
//        exit (1);
//
//
//
//
//
//        /*
//            fprintf (stderr, "Usage: %s poll|nopoll\n", argv[0]);
//                fprintf (stderr, "       [prot=protocol]\n");
//                fprintf (stderr, "       [u_token=u_token]\n");
//                fprintf (stderr, "       [overwrite=0|1|2]\n");
//                fprintf (stderr, "       SURLs([fileStorageType=0|1|2[,]]\n");
//                fprintf (stderr, "       [sizeInBytes=size[,][lifeTime=life[,]]\n");
//                fprintf (stderr, "       [spaceToken=token[,]])\n");
//                exit (1);
//        */
//        }
//
//        if (argc < 6) {
//           max_arg = argc -1;
//        } else {
//           max_arg = 5;
//        };
//        num_arg = 0;
//        protocols[0]=(char *)malloc((size_t)(5));
//    
//    //File Protocol DEFAULT
//        strcpy(protocols[0],"file");
//
//        for ( i=2; i<max_arg; i++) {
//           if ( (p = strstr(argv[i],"prot=")) != NULL ) {
//            free(protocols[0]);
//            protocols[0]=(char *)malloc((size_t)(strlen((const char*)(p+5))+1));
//            strcpy(protocols[0],p+5);
//            num_arg += 1;
//            p=NULL;
//           }
//           if ( (p = strstr(argv[i],"u_token=")) != NULL ) {
//        strcpy(u_token,p+8);
//            num_arg += 1;
//            p=NULL;
//           }
//           if ( (p = strstr(argv[i],"overwrite=")) != NULL ) {
//            strcpy(overwr,p+10);
//            overwrite = atoi(overwr);
//            num_arg += 1;
//            p = NULL;
//           }
//        }
//    //Number of SURL in the request
//    nbfiles = argc - num_arg - 2;
//
//    //Check syntax for each SURL specified
//    for (j=0;j<nbfiles;j++) {
//        //From the FIRST SURL specified get the service EndPoint
//        if((j==0)&&(parseSURL_getEndpoint(argv[num_arg+2+j], &srm_endpoint, &sfn)<0)) {
//            //perror ("Error parsing SURL");
//            printLine();
//            exit (1);
//        } else if (parseSURL(argv[num_arg+2+j]) < 0) {
//            //perror ("Error parsing SURL");
//            printLine();
//            exit (1);
//        }
//    }
//
//    /* while (*protocols[nbproto]) nbproto++; */
//        nbproto = 1;
//
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
//        soap_malloc (&soap, sizeof(struct ns1__ArrayOfTPutFileRequest))) == NULL ||
//        (req.arrayOfFileRequests->putRequestArray =
//        soap_malloc (&soap, nbfiles * sizeof(struct ns1__TPutFileRequest *))) == NULL ||
//        (req.arrayOfTransferProtocols =
//        soap_malloc (&soap, sizeof(struct ns1__ArrayOf_USCORExsd_USCOREstring))) == NULL) {
//        perror ("malloc");
//        soap_end (&soap);
//        exit (1);
//    }
//
//    for (i = 0; i < nbfiles; i++) {
//        if ((req.arrayOfFileRequests->putRequestArray[i] =
//            soap_malloc (&soap, sizeof(struct ns1__TPutFileRequest))) == NULL) {
//            perror ("malloc");
//            soap_end (&soap);
//            exit (1);
//        }
//    }
//    req.arrayOfFileRequests->__sizeputRequestArray = nbfiles;
//    req.arrayOfTransferProtocols->stringArray = protocols;
//    req.arrayOfTransferProtocols->__sizestringArray = nbproto;
//    if ( strcmp(u_token, "") )
//      req.userRequestDescription = u_token;
//        if ((req.overwriteOption =
//                soap_malloc (&soap, sizeof(enum ns1__TOverwriteMode))) == NULL) {
//                perror ("malloc");
//                soap_end (&soap);
//                exit (1);
//        }; 
//        *req.overwriteOption = overwrite;
//
//    for (i = 0; i < nbfiles; i++) {
//        char *SURL=calloc(256,sizeof(char));
//        //Verify Syntax for each SURL and optional attribute specified
//            if (parsePutRequest (argv[i+num_arg+2],&FileStorageType,&SizeInBytes,&LifeTime,
//                                 SpaceToken,SURL) < 0) {
//                        printChar();
//            printf("Error in parsing SURL: %s\n",argv[i+num_arg+2]);
//            //perror ("");
//                        exit (1);
//                }
//        reqfilep = req.arrayOfFileRequests->putRequestArray[i];
//        memset (reqfilep, 0, sizeof(*reqfilep));
//        if ((reqfilep->toSURLInfo =
//            soap_malloc (&soap, sizeof(struct ns1__TSURLInfo))) == NULL ||
//            (reqfilep->toSURLInfo->SURLOrStFN =
//                soap_malloc (&soap, sizeof(struct ns1__TSURL))) == NULL) {
//            perror ("malloc");
//            soap_end (&soap);
//            exit (1);
//            }
//        reqfilep->toSURLInfo->SURLOrStFN->value = SURL;
//                if ((reqfilep->fileStorageType =
//                    soap_malloc (&soap, sizeof(enum ns1__TFileStorageType))) == NULL) {
//                        perror ("malloc");
//                        soap_end (&soap);
//                        exit (1);
//                }
//                *reqfilep->fileStorageType = FileStorageType;
//                if ((reqfilep->knownSizeOfThisFile =
//                    soap_malloc (&soap, sizeof(struct ns1__TSizeInBytes))) == NULL) {
//                        perror ("malloc");
//                        soap_end (&soap);
//                        exit (1);
//                }
//                reqfilep->knownSizeOfThisFile->value = SizeInBytes;
//                if ((reqfilep->lifetime =
//                    soap_malloc (&soap, sizeof(struct ns1__TLifeTimeInSeconds))) == NULL) {
//                        perror ("malloc");
//                        soap_end (&soap);
//                        exit (1);
//                }
//                if(LifeTime == -1)
//                    reqfilep->lifetime = NULL;
//                else
//                    reqfilep->lifetime->value = LifeTime;
//                if ((reqfilep->spaceToken =
//                    soap_malloc (&soap, sizeof(struct ns1__TSpaceToken))) == NULL) {
//                        perror ("malloc");
//                        soap_end (&soap);
//                        exit (1);
//                }
//                reqfilep->spaceToken->value = SpaceToken;
//    }
//
//    if (soap_call_ns1__srmPrepareToPut (&soap, srm_endpoint, "PrepareToPut",
//        &req, &rep)) {
//        soap_print_fault (&soap, stderr);
//        soap_print_fault_location (&soap, stderr);
//        soap_end (&soap);
//        exit (1);
//    }
//    reqstatp = rep.srmPrepareToPutResponse->returnStatus;
//    repfs = rep.srmPrepareToPutResponse->arrayOfFileStatuses;
//    if (rep.srmPrepareToPutResponse->requestToken) {
//        r_token = rep.srmPrepareToPutResponse->requestToken->value;
//        printChar();
//        printf ("Request Token [%s]\n", r_token);
//    }
//    
//    printLine();
//
//    memset (&sreq, 0, sizeof(sreq));
//    sreq.requestToken = rep.srmPrepareToPutResponse->requestToken;
//    
//        
//    if (pollflg) {
//       /* wait for file "ready" */
//       printChar();
//       printf("StatusOfPutRequest: \n");    
//
//
//       int soap_bool = 0;
//       while (reqstatp->statusCode == SRM_USCOREREQUEST_USCOREQUEUED ||
//        reqstatp->statusCode == SRM_USCOREREQUEST_USCOREINPROGRESS ||
//        reqstatp->statusCode == SRM_USCOREREQUEST_USCORESUSPENDED) {
//        //printLine();
//        printChar();
//        printf("Request Status [%d] : [%s]\n", reqstatp->statusCode,reconvertStatusCode(reqstatp->statusCode) );
//
//        sleep ((r++ == 0) ? 1 : DEFPOLLINT);
//        if ( soap_bool ) soap_end (&soapr);
//        soap_bool = 1;
//        soap_init (&soapr);
//#ifdef GSI_PLUGINS
//                flags = CGSI_OPT_DISABLE_NAME_CHECK;
//                soap_register_plugin_arg (&soapr, client_cgsi_plugin, &flags);
//#endif
//        if (soap_call_ns1__srmStatusOfPutRequest (&soapr, srm_endpoint,
//            "StatusOfPutRequest", &sreq, &srep)) {
//                        soap_print_fault (&soapr, stderr);
//                        soap_end (&soapr);
//                        exit (1);
//                }
//                reqstatp = srep.srmStatusOfPutRequestResponse->returnStatus;
//        repfs = srep.srmStatusOfPutRequestResponse->arrayOfFileStatuses;
//       }
//     
//       //  printLine();
//       printChar();
//       printf ("Request Status [%d] : [%s]\n", reqstatp->statusCode,reconvertStatusCode(reqstatp->statusCode) );
//       
//       if (reqstatp->statusCode != SRM_USCORESUCCESS &&
//        reqstatp->statusCode != SRM_USCOREDONE) {
//        if (reqstatp->explanation) {
//                printChar();
//                printf ("Explanation String: [%s]\n", reqstatp->explanation);
//                
//        }
//        soap_end (&soap);
//        exit (1);
//       }
//       
//           if (! repfs) {
//        printf ("arrayOfFileStatuses is NULL\n");
//        soap_end (&soap);
//        exit (1);
//       }
//    
//       printLine();
//       
//       for (i = 0; i < repfs->__sizeputStatusArray; i++) {
//        
//        printChar();
//        printf("Request [%d]\n",i);
//
//        if (repfs->putStatusArray[i]->transferURL) {
//        
//            printChar();
//            printf ("Status [%d] : [%s]\n= Explanation : [%s]\n",
//                (repfs->putStatusArray[i])->status->statusCode,
//                reconvertStatusCode((repfs->putStatusArray[i])->status->statusCode),    
//               (repfs->putStatusArray[i])->status->explanation);
//        
//            printChar();
//            printf ("TURL = [%s]\n",(repfs->putStatusArray[i])->transferURL->value);
//        
//        } else if ((repfs->putStatusArray[i])->status->explanation) {
//            printChar();
//            printf ("Request Status[%d] : [%s]\n= Explanation = [%s]\n",            
//                (repfs->putStatusArray[i])->status->statusCode,
//                reconvertStatusCode((repfs->putStatusArray[i])->status->statusCode),
//                (repfs->putStatusArray[i])->status->explanation);
//        } else {
//            printChar();
//            printf ("Request Status[%d] : [%s]\n",
//                (repfs->putStatusArray[i])->status->statusCode,
//                reconvertStatusCode((repfs->putStatusArray[i])->status->statusCode)
//                );
//        }
//        
//        printLine();
//       }
//    
//       soap_end (&soapr);   
//    }
//
//    soap_end (&soap);
////  soap_end (&soapr);
//    exit (0);
}
