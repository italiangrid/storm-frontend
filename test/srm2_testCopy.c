// $Id$

#include "srmv2H.h"
#include "srmSoapBinding.nsmap"
#include "storm_util.h"
#define DEFPOLLINT 10

#define SRM_EP_PATH "/v2_1_1/srm"
#ifdef GSI_PLUGINS
#include "cgsi_plugin.h"
#endif

/*
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
*/

parseCopyRequest(const char* sfn, LONG64 *lifeTime, enum ns1__TFileStorageType* fileStorageType,
                 char* spaceToken, enum ns1__TOverwriteMode* overwrite, struct ns1__TDirOption dirOption, char* surl) {

    size_t len;
    int lenp;
    char *p, *f;
    char  c[256];

    //Check SURL 
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

        // Parse FileStorageType
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
            if(*fileStorageType>2) {
                printChar();
                printf("Error: FileStorageType parameter must be : 0|1|2.\n= Value specified:  %s \n",c);
                errno = EINVAL;
                return (-1);
            }
        
        };


        //Parse SpaceToken
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

        //Parsing Overwrite Paramter
        p=strstr(sfn,"overwrite=");
        if (p != NULL) {
            f=strchr(p,',');
            if (f==NULL) {
                f=strchr(p,')');
                if (f==NULL) {
                    errno = EINVAL;
                    return (-1);
                }
            }
            strncpy(c,p+10,(size_t)(f-p-9));
            *(c+(f-p-10)) = '\0';
            // *lifeTime = atoll(c);
            char** errorString = NULL;
            *overwrite =  strtol(c, NULL, 10);

            //TODO Occhio che in questo modo una stringa del tipo
            // 100aaa viene interpretata come 100
            if(*overwrite>2) {
                printChar();
                printf("Error: \"Overwrite\" parameter must be numeric value.\n= Value specified:  %s \n",c);
                errno = EINVAL;
                return (-1);
            }
        };

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
                *dirOption.allLevelRecursive = atoi(c);

                if(*dirOption.allLevelRecursive>1) {
                    printChar();
                    printf("Error: \"dirOption.allLevelRecursive\"  parameter must be : 0|1.\n= Value specified:  %s \n",c);
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
                dirOption.isSourceADirectory = atoi(c);
                if(dirOption.isSourceADirectory>1) {
                    printChar();
                    printf("Error: \"dirOption.isSourceADirectory\" parameter must be : 0|1.\n= Value specified:  %s \n",c);
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
                dirOption.numOfLevels = atoi(c);
            }
        };
         
    }

    return(0);       
}

main(int argc,char** argv)
{
//    int flags;
//    int pollflg;
//    int i;
//    int nbfiles;
//    int r = 0;
//    char *r_token;
//    struct ns1__srmCopyResponse_ rep;
//    struct ns1__ArrayOfTCopyRequestFileStatus *repfs;
//    struct ns1__srmCopyRequest req;
//    struct ns1__TCopyFileRequest *reqfilep;
//    struct ns1__TReturnStatus *reqstatp;
//    char *sfn;
//    struct soap soap;
//    struct ns1__srmStatusOfCopyRequestResponse_ srep;
//    struct ns1__srmStatusOfCopyRequestRequest sreq;
//    char *srm_endpoint;
//
//    enum xsd__boolean allLevels=0;
//    enum xsd__boolean removeSource=0;
//
//    //Request Paramter for each transfer request
//    char fromSURL[256];
//    char toSURL[256];
//    LONG64 LifeTime = 0;
//    //Default FileStorageType = Permanent
//    enum ns1__TFileStorageType FileStorageType = 1;
//    char SpaceToken[256] = "";
//    enum    ns1__TOverwriteMode overwrite = 0;
//    struct  ns1__TDirOption DirOption;
//
//    char u_token[256] = "";
//    char c[256];
//    char* p;
//    int max_arg = 0;
//    int num_arg = 0;
//    int j;
//    // Print Help Menu
//    printLine();
//    printLine();
//    printChar();
//    printf("SrmCopy Request \n");
//
//
//    if (argc < 3) {
//
//        fprintf (stderr, "\nUsage: %s poll|nopoll [options] fromSURL toSURL[([surl_options])] [fromSURL toSURL(..)]\n\n", argv[0]);
//        fprintf (stderr,  "  options:\n");
//        fprintf (stderr, "       u_token=<usertoken>     (<usertoken> is the userRequestDescription string.)\n");
//        fprintf (stderr, "       overwrite=<0|1>         (Overwrite Flag 0:False 1:True.)\n");
//        fprintf (stderr, "       removeSourceFiles=<0|1> (RemoveSourceFiles Flag 0:False 1:True.)\n\n");
//                
//        //fprintf (stderr, "       SURLs(");
//        fprintf (stderr, "  surl_options:\n");
//        fprintf (stderr, "       lifeTime=<lifetime>     (<lifetime> is the lifetime in seconds.)\n");
//        fprintf (stderr, "       fileStorageType=<0|1|2> (0:Volatile, 1:Durable, 2:Permanent) \n");
//        fprintf (stderr, "       spaceToken=<token>      (<spaceToken> is a valid spaceToken string.)\n");
//        fprintf (stderr, "       overwrite=<0|1>         (Overwrite Flag 0:False 1:True.)\n");
//        fprintf (stderr, "       dirOption=<0|1>,<0|1>,<numofLevels>  (isSourceADirectory Flag, allLevelRecursive Flag ,\n");
//        fprintf (stderr, "                                             <numOfLevels> is the number of level to explore.) \n\n");
//        
//        exit (1);
//
//
//
//        /*
//          fprintf (stderr, "Usage: %s poll|nopoll\n", argv[0]);
//
//          fprintf (stderr, "       [u_token=u_token]\n");
//          fprintf (stderr, "       [overwrite=0|1]\n");
//          fprintf (stderr, "       [removeSourceFiles=0|1]\n");
//          fprintf (stderr, "       fromSURL toSURL([lifetime=lifetime][,]\n");
//          fprintf (stderr, "       [fileStorageType=0|1|2][,]\n");
//          fprintf (stderr, "       [spaceToken=token][,]\n");
//          fprintf (stderr, "   [overwrite=0|1][,]\n");
//          fprintf (stderr, "       [dirOption={0|1,0|1,<numofLevels>][,] )\n");
//                            
//                
//          exit (1);
//
//        */
//
//    }
//
//    
//    //Check parameter.
//    DirOption.allLevelRecursive=&allLevels;
//       
//    //CHeck Poll Option     
//    if (strncmp(argv[1],"poll",4) == 0) {
//        pollflg = 1;
//    } else if (strncmp(argv[1],"nopoll",6) == 0) {
//        pollflg = 0;
//        //printChar();
//        //printf("Error: in this version only \"poll\" option supported!");     
//    } else {
//        pollflg = -1;
//    }
//
////  nbfiles = (argc - num_arg - 2)/2;
////  if ( (nbfiles*2 + 2) < argc ) 
//
//    if (pollflg < 0) {
//      
//
//        fprintf (stderr, "\nUsage: %s poll|nopoll [options] fromSURL toSURL[([surl_options])] [fromSURL toSURL(..)]\n\n", argv[0]);
//        fprintf (stderr,  "  options:\n");
//        fprintf (stderr, "       u_token=<usertoken>     (<usertoken> is the userRequestDescription string.)\n");
//        fprintf (stderr, "       overwrite=<0|1>         (Overwrite Flag 0:False 1:True.)\n");
//        fprintf (stderr, "       removeSourceFiles=<0|1> (RemoveSourceFiles Flag 0:False 1:True.)\n\n");
//                
//        //fprintf (stderr, "       SURLs(");
//        fprintf (stderr, "  surl_options:\n");
//        fprintf (stderr, "       lifeTime=<lifetime>     (<lifetime> is the lifetime in seconds.)\n");
//        fprintf (stderr, "       fileStorageType=<0|1|2> (0:Volatile, 1:Durable, 2:Permanent) \n");
//        fprintf (stderr, "       spaceToken=<token>      (<spaceToken> is a valid spaceToken string.)\n");
//        fprintf (stderr, "       overwrite=<0|1>         (Overwrite Flag 0:False 1:True.)\n");
//        fprintf (stderr, "       dirOption=<0|1>,<0|1>,<numofLevels>  (isSourceADirectory Flag, allLevelRecursive Flag ,\n");
//        fprintf (stderr, "                                             <numOfLevels> is the number of level to explore.) \n\n");
//        
//        exit (1);
//
//
//        /*
//          fprintf (stderr, "Usage: %s poll|nopoll\n", argv[0]);
//
//          fprintf (stderr, "       [u_token=u_token]\n");
//          fprintf (stderr, "       [overwrite=0|1]\n");
//          fprintf (stderr, "       [removeSourceFiles=0|1]\n");
//          fprintf (stderr, "       fromSURL toSURL([lifetime=lifetime][,]\n");
//          fprintf (stderr, "       [fileStorageType=0|1|2][,]\n");
//          fprintf (stderr, "       [spaceToken=token][,]\n");
//          fprintf (stderr, "   [overwrite=0|1][,]\n");
//          fprintf (stderr, "       [dirOption={0|1,0|1,<numofLevels>][,] )\n");
//                
//        
//          //  fprintf (stderr, "usage: %s u_token fromSURLs toSURLs\n", argv[0]);
//          exit (1);
//        */
//
//    }
//
//    //Get Global Paramter Specified
//
//    if (argc < 5) {
//        max_arg = argc -1;
//    } else {
//        max_arg = 5;//(3+2)...NO COMMENT...
//    };
//        
//    num_arg = 0;
//    
//    //Check syntax for Global Parameter       
//    //DEBUG
//    //printf("===Parameter CHeck");
//    
//    for ( i=2; i<max_arg; i++) {
//        //CHECK if argument is not a SURL
//        if((p=strstr(argv[i],"srm://")) == NULL) {
//    
//            if ( (p = strstr(argv[i],"u_token=")) != NULL ) {
//                strcpy(u_token,p+8);
//                num_arg += 1;
//                p=NULL;
//            }
//            if ( (p = strstr(argv[i],"overwrite=")) != NULL ) {
//                strncpy(c,p+10,(size_t)(strlen(argv[i])-10));
//                
//                //*(c+10) = '\0';
//                overwrite = atoi(c);
//                //Check the int value for  fileStorageTyoe 0|1|2    
//                if(overwrite>1) {
//                    printChar();
//                    printf("Error: \"Overwrite\" parameter must be : 0|1.\n= Value specified:  %s \n",c);
//                    errno = EINVAL;
//                    return (-1);
//                }   
//            
//                num_arg += 1;
//    
//            }
//                
//            if ( (p = strstr(argv[i],"removeSourceFiles=")) != NULL ) {
//                strncpy(c,p+18,(size_t)(strlen(argv[i])-18));
//                
//                //*(c+10) = '\0';
//                removeSource = atoi(c);
//                //Check the int value for  fileStorageTyoe 0|1|2    
//                if(removeSource>1) {
//                    printChar();
//                    printf("Error: \"removeSourceFiles\" parameter must be : 0|1.\n= Value specified:  %s \n",c);
//                    errno = EINVAL;
//                    return (-1);
//                }
//                
//                num_arg += 1;
//    
//            }
//        }
//    }
//    
//    //DEBUG
//    //printf("===Parameter CHecki Done, NumArg: %d \n",num_arg);
//        
//    //nbfiles = argc - num_arg - 2;
//
//    nbfiles = (argc - num_arg - 2)/2;
//
//    if((nbfiles*2 +2 +num_arg)<argc) {  
//        
//        fprintf (stderr, "\nUsage: %s poll|nopoll [options] fromSURL toSURL[([surl_options])] [fromSURL toSURL(..)]\n\n", argv[0]);
//        fprintf (stderr,  "  options:\n");
//        fprintf (stderr, "       u_token=<usertoken>     (<usertoken> is the userRequestDescription string.)\n");
//        fprintf (stderr, "       overwrite=<0|1>         (Overwrite Flag 0:False 1:True.)\n");
//        fprintf (stderr, "       removeSourceFiles=<0|1> (RemoveSourceFiles Flag 0:False 1:True.)\n\n");
//                
//        //fprintf (stderr, "       SURLs(");
//        fprintf (stderr, "  surl_options:\n");
//        fprintf (stderr, "       lifeTime=<lifetime>     (<lifetime> is the lifetime in seconds.)\n");
//        fprintf (stderr, "       fileStorageType=<0|1|2> (0:Volatile, 1:Durable, 2:Permanent) \n");
//        fprintf (stderr, "       spaceToken=<token>      (<spaceToken> is a valid spaceToken string.)\n");
//        fprintf (stderr, "       overwrite=<0|1>         (Overwrite Flag 0:False 1:True.)\n");
//        fprintf (stderr, "       dirOption=<0|1>,<0|1>,<numofLevels>  (isSourceADirectory Flag, allLevelRecursive Flag ,\n");
//        fprintf (stderr, "                                             <numOfLevels> is the number of level to explore.) \n\n");
//        
//        fprintf (stderr, "usage: %s u_token fromSURLs toSURLs\n", argv[0]);
//        exit (1);
//
//    
//        /*
//          fprintf (stderr, "Usage: %s poll|nopoll\n", argv[0]);
//
//          fprintf (stderr, "       [u_token=u_token]\n");
//          fprintf (stderr, "       [overwrite=0|1]\n");
//          fprintf (stderr, "       [removeSourceFiles=0|1]\n");
//          fprintf (stderr, "       fromSURL toSURL([lifetime=lifetime][,]\n");
//          fprintf (stderr, "       [fileStorageType=0|1|2][,]\n");
//          fprintf (stderr, "       [spaceToken=token][,]\n");
//          fprintf (stderr, "   [overwrite=0|1][,]\n");
//          fprintf (stderr, "       [dirOption={0|1,0|1,<numofLevels>][,] )\n");
//                
//        
//          //  fprintf (stderr, "usage: %s u_token fromSURLs toSURLs\n", argv[0]);
//          */
//
//        printChar();
//        printf("Error: \"fromSURL\" or \"toSURL\" missing!\n");
//        printLine();
//
//        exit (1);
//
//    }
//
//
//
//
//
//
//    //Check syntax for each _fromSURL_
//    
//    //DEBUG
//    //printf("===fromSurl syntax CHeck, arg:%d,%d \n",num_arg,nbfiles);
//    
//    for(j=0;j<nbfiles;j=j+2) {
//        if((j==0)&&( parseSURL_getEndpoint(argv[2+num_arg+j], &srm_endpoint, &sfn) < 0)) {
//            printLine();
//            //perror ("parsesurl");
//            exit (1);
//        } else if(parseSURL(argv[num_arg+2+j])<0) {
//            //perror ("parsesurl");
//            printLine();
//            exit(1);
//        }
//
//    }
//    
//    //DEBUG
//    //printf("===fromSurl syntax Done\n");
//    
//    //INitialize Soap Enviroment    
//    soap_init (&soap);
//
//#ifdef GSI_PLUGINS
///* Delegate proxy ---- TO BE CHANGED */
//    flags = CGSI_OPT_DISABLE_NAME_CHECK|CGSI_OPT_DELEG_FLAG;
//    soap_register_plugin_arg (&soap, client_cgsi_plugin, &flags);
//#endif
//
//    memset (&req, 0, sizeof(req));
//
//    if ((req.arrayOfFileRequests =
//         soap_malloc (&soap, sizeof(struct ns1__ArrayOfTCopyFileRequest))) == NULL ||
//        (req.arrayOfFileRequests->copyRequestArray =
//         soap_malloc (&soap, nbfiles * sizeof(struct ns1__TCopyFileRequest *))) == NULL) { 
//        perror ("malloc");
//        soap_end (&soap);
//        exit (1);
//    }
//
//    for (i = 0; i < nbfiles; i++) {
//        if ((req.arrayOfFileRequests->copyRequestArray[i] =
//             soap_malloc (&soap, sizeof(struct ns1__TCopyFileRequest))) == NULL) {
//            perror ("malloc");
//            soap_end (&soap);
//            exit (1);
//        }
//    }   
//
//    //DEBUG
//    //printf("===Set Parameter \n");
//    
//    //Set Size of INPUT Array
//    req.arrayOfFileRequests->__sizecopyRequestArray = nbfiles;
//
//    //Set UserTokenDescription
//    if ( ! strcmp(u_token, "") )
//        req.userRequestDescription = u_token;
////  else
////      req.userRequestDescription = "";
//    
//    //Set OverWrite
//    if ((req.overwriteOption =
//         soap_malloc (&soap, sizeof(enum ns1__TOverwriteMode))) == NULL ) { 
//        perror ("malloc");
//        soap_end (&soap);
//        exit (1);
//    }
//    
//    if ( overwrite==0||overwrite==1 ) 
//        *(req.overwriteOption) = overwrite;
////  else
////      *(req.overwriteOption) = 0;
//    
//    //RemoveSourceFiles
//
//    if ((req.removeSourceFiles =
//         soap_malloc (&soap, sizeof(enum xsd__boolean))) == NULL ) { 
//        perror ("malloc");
//        soap_end (&soap);
//        exit (1);
//    }
//
//    if ( removeSource==0||removeSource==1 ) 
//        *(req.removeSourceFiles) = removeSource;
////  else
////      *(req.removeSourceFiles) = 0;
//
//    //DEBUG
//    //printf("===Set Parameter Done\n");
//    
//
//    for (i = 0; i < nbfiles; i=i+2) {
//        
//        //Check syntax for each SURL and optional attribute specifiedi
//
//        if(parseCopyRequest(argv[i+num_arg+2+1],&LifeTime, &FileStorageType,SpaceToken,&overwrite,DirOption,toSURL)< 0 ) {
//            printChar();
//            printf("Error in parsing SURL: %s\n",argv[i+num_arg+2+1]);
//            //perror ("par");
//            exit (1);
//        }
//
//
//
//
//        reqfilep = req.arrayOfFileRequests->copyRequestArray[i];
//        memset (reqfilep, 0, sizeof(struct ns1__TCopyFileRequest));
//    
//        if ((reqfilep->toSURLInfo =
//             soap_malloc (&soap, sizeof(struct ns1__TSURLInfo))) == NULL ||
//            (reqfilep->toSURLInfo->SURLOrStFN =
//             soap_malloc (&soap, sizeof(struct ns1__TSURL))) == NULL) {
//            perror ("malloc");
//            soap_end (&soap);
//            exit (1);
//        }
//        if ((reqfilep->fromSURLInfo =
//             soap_malloc (&soap, sizeof(struct ns1__TSURLInfo))) == NULL ||
//            (reqfilep->fromSURLInfo->SURLOrStFN =
//             soap_malloc (&soap, sizeof(struct ns1__TSURL))) == NULL) {
//            perror ("malloc");
//            soap_end (&soap);
//            exit (1);
//        }
//        //INSERT FROM AND TO SURL
//
//        reqfilep->fromSURLInfo->SURLOrStFN->value = argv[i+2+num_arg];
//        //reqfilep->toSURLInfo->SURLOrStFN->value = argv[i+2+num_arg+1];
//        reqfilep->toSURLInfo->SURLOrStFN->value = toSURL;
//
//
//        //DEBUG
//        //  printf("fromSurl:%s toSURL:%s\n",argv[i+2+num_arg], argv[i+2+num_arg+1]);
//        
//
//        //INSERT DIR OPTION
//        
//        if ((reqfilep->dirOption =
//             soap_malloc (&soap, sizeof(struct ns1__TDirOption))) == NULL) {
//            perror ("malloc");
//            soap_end (&soap);
//            exit (1);
//        }
//        *reqfilep->dirOption = DirOption;
//   
//        //INSERT FileStorageType        
//
//        if ((reqfilep->fileStorageType =
//             soap_malloc (&soap, sizeof(enum ns1__TFileStorageType))) == NULL) {
//            perror ("malloc");
//            soap_end (&soap);
//            exit (1);
//        }
//        *reqfilep->fileStorageType = FileStorageType;
//   
//        //INSERT LIFETIME
//        if(LifeTime == -1)
//            reqfilep->lifetime = NULL;
//        else{
//            if ((reqfilep->lifetime =
//                 soap_malloc (&soap, sizeof(struct ns1__TLifeTimeInSeconds))) == NULL) {
//                perror ("malloc");
//                soap_end (&soap);
//                exit (1);
//            }else
//                reqfilep->lifetime->value = LifeTime;
//        }
//
//        /* 
//        //INSERT OVERWRIT
//        if ((reqfilep-> =  soap_malloc (&soap, sizeof(struct ns1__TOverwriteMode))) == NULL) {
//        perror ("malloc");
//        soap_end (&soap);
//        exit (1);
//        }
//                
//        *reqfilep->overwriteMode = overwrite;
//   
//        //INSERT TSpaceToken
//        if ((reqfilep-> =
//        soap_malloc (&soap, sizeof(struct ns1__TSpaceToken))) == NULL) {
//        perror ("malloc");
//        soap_end (&soap);
//        exit (1);
//        }
//        *reqfilep->spaceToken->value = SpaceToken;
//        */
//
//    }
//
//    
//    
//    if (soap_call_ns1__srmCopy (&soap, srm_endpoint, "Copy",
//                                &req, &rep)) {
//        soap_print_fault (&soap, stderr);
//        soap_print_fault_location (&soap, stderr);
//        soap_end (&soap);
//        exit (1);
//    }
//    reqstatp = rep.srmCopyResponse->returnStatus;
//    repfs = rep.srmCopyResponse->arrayOfFileStatuses;
//    if (rep.srmCopyResponse->requestToken) {
//        r_token = rep.srmCopyResponse->requestToken->value;
//        printChar();
//        printf ("Request Token [%s]\n", r_token);
//    }
//
//    printLine();
//
//    memset (&sreq, 0, sizeof(sreq));
//    sreq.requestToken = rep.srmCopyResponse->requestToken;
//
//    if(pollflg){
//        /* wait for file "ready" */
//        printChar();
//        printf("StatusOfCopyRequest:\n");
//
//        while (reqstatp->statusCode == SRM_USCOREREQUEST_USCOREQUEUED ||
//               reqstatp->statusCode == SRM_USCOREREQUEST_USCOREINPROGRESS ||
//               reqstatp->statusCode == SRM_USCOREREQUEST_USCORESUSPENDED ||
//               reqstatp->statusCode == SRM_USCOREABORTED) {
//            printChar();
//            printf("Request Status [%d] : [%s]\n", reqstatp->statusCode, reconvertStatusCode(reqstatp->statusCode));
//            sleep ((r++ == 0) ? 1 : DEFPOLLINT);
//            if (soap_call_ns1__srmStatusOfCopyRequest (&soap, srm_endpoint,
//                                                       "StatusOfCopyRequest", &sreq, &srep)) {
//                soap_print_fault (&soap, stderr);
//                soap_end (&soap);
//                exit (1);
//            }
//            reqstatp = srep.srmStatusOfCopyRequestResponse->returnStatus;
//            repfs = srep.srmStatusOfCopyRequestResponse->arrayOfFileStatuses;
//        }
//        printChar();
//        printf ("Request Status [%d] : [%s]\n", reqstatp->statusCode, reconvertStatusCode(reqstatp->statusCode));
//
//        if (reqstatp->statusCode != SRM_USCORESUCCESS &&
//            reqstatp->statusCode != SRM_USCOREDONE) {
//            if (reqstatp->explanation) {
//                printChar();
//                printf ("Explanation String : [%s]\n", reqstatp->explanation);
//            }
//            soap_end (&soap);
//            exit (1);
//        }
//        if (! repfs) {
//            printf ("arrayOfFileStatuses is NULL\n");
//            soap_end (&soap);
//            exit (1);
//        }
//
//        printLine();
//
//        for (i = 0; i < repfs->__sizecopyStatusArray; i++) {
//        
//            printChar();
//            printf("Request [%d]\n",i);
//
//            if (repfs->copyStatusArray[i]->toSURL) {
//                printChar();
//                printf ("Status [%d] : [%s]\n= Target SURL = [%s]\n= Explanation = [%s]\n", 
//                        (repfs->copyStatusArray[i])->status->statusCode,
//                        reconvertStatusCode(    (repfs->copyStatusArray[i])->status->statusCode ),
//                        (repfs->copyStatusArray[i])->toSURL->value,
//                        (repfs->copyStatusArray[i])->status->explanation);
//            } else if ((repfs->copyStatusArray[i])->status->explanation) {
//                printChar();
//                printf ("Status [%d] : [%s], Explanation = [%s]\n",
//                        (repfs->copyStatusArray[i])->status->statusCode,
//                        reconvertStatusCode((repfs->copyStatusArray[i])->status->statusCode),    
//                        (repfs->copyStatusArray[i])->status->explanation);
//            } else {
//                printChar();
//                printf ("Status [%d] : [%s],\n",
//                        (repfs->copyStatusArray[i])->status->statusCode,
//                        reconvertStatusCode((repfs->copyStatusArray[i])->status->statusCode));
//            }
//        
//            printLine();
//        }
//    }
//    soap_end (&soap);
//    exit (0);
}
