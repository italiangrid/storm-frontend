/* Copyright (C) 2006 by Antonio Messina <antonio.messina@ictp.it> for the ICTP project EGRID. */
/*
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by the
 *  Free Software Foundation; either version 2 of the License, or (at your
 *  option) any later version.
 *
 *  This program is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include <stdio.h>
#include <limits.h>
#include <stdlib.h>

#define _GNU_SOURCE
#include "srmv2H.h"
#include "srmSoapBinding.nsmap"
#include "storm_util.h"



#ifdef GSI_PLUGINS
#include "cgsi_plugin.h"
#endif

char *PROGNAME;
char *endpoint=NULL;
char *token=NULL;
int verbose=0;

void usage(){
    fprintf(stdout,"Usage: %s [options] endpoint token [surl1] [surl2] [...]\n",PROGNAME);
    fprintf(stdout,"\n\n"
            "OPTIONS:\n\n"
            "  -i  S   user ID (probably ignored by the server)\n"
            "  -v      print more info\n"
            "  -h      show this help message\n"
            "\n"
        );

    return;
}


int fill_surl_array(int argc, 
                    char **argv, 
                    int start, 
                    struct soap *soap, 
                    struct ns1__srmStatusOfPutRequestRequest *request)
{
    int i;
    int num=0;
    int size;
    for(i=start;
        i<argc  &&  *argv[i]!='-';
        i++)
        num++;
    
    if(i<argc)
        return -1;

    if(NULL == request)
        return -1;
    size=sizeof(struct ns1__ArrayOfTPutRequestFileStatus);  
    if(NULL == (request->arrayOfToSURLs = soap_malloc(soap,size))){
        fprintf(stderr,"Unable to allocate memory\n");
        exit(-1);
    }

    memset(request->arrayOfToSURLs,0,size);

    request->arrayOfToSURLs->__sizesurlArray = num;

    size=sizeof(struct ns1__TSURL);
    if(NULL == (request->arrayOfToSURLs->surlArray = soap_malloc(soap,size * num))){
        fprintf(stderr,"Unable to allocate memory\n");
        exit(-1);
    }

    
    for(i=0;i<num;i++){
        if(NULL == (request->arrayOfToSURLs->surlArray[i] = soap_malloc(soap,size))){
            fprintf(stderr,"Unable to allocate memory\n");
            exit(-1);
        }
        
        request->arrayOfToSURLs->surlArray[i]->value = soap_strdup(soap,argv[i+start]);
    } 
}

int parse_options(int argc, 
                  char **argv, 
                  struct soap *soap, 
                  struct ns1__srmStatusOfPutRequestRequest *request)
{
    int i=0;
    for(i=1; i<argc; i++){
        if('-' != *argv[i]){
            char *tmp;
            if(NULL == endpoint){
                int ret = split_surl(argv[i],NULL,NULL,NULL,&endpoint,NULL);
                if(SURL_VALID != ret){
                    fprintf(stderr,"Endpoint '%s' invalid\n",argv[i]);
                    usage();
                    exit(1);
                }
                continue;
            }else{
                if(NULL == token){
                    token=argv[i];
                    continue;
                }else{
                    i=fill_surl_array(argc,argv,i,soap,request);
                    if(-1 == i){
                        fprintf(stderr,"Error parsing surl array\n");
                        usage();
                        exit(1);
                    }
                }
            }
        }
        switch(argv[i][1]){
        case 'i':
            if(i>=argc-1){
                fprintf(stderr,"Missing argument to '-i' option\n");
                usage();
                exit(1);
            }else{
                int size=sizeof(struct ns1__TUserID);
                i++;
                if(NULL == (request->userID = soap_malloc(soap,size))){
                    fprintf(stderr,"Unable to allocate memory");
                    exit(-1);
                }
                memset(request->userID,0,size);
                request->userID->value=soap_strdup(soap,argv[i]);
            }
            break;          
        case 'v':
            verbose=1;
            break;          
        case 'h':
            usage();
            exit(1);
        default:
            fprintf(stderr,"unknown option '%s'\n",argv[i]);
            usage();
            exit(1);
        }
    }


    /* Final check */
    if(NULL == endpoint){
        fprintf(stderr,"Missing endpoint\n");
        return -1;
    }
    if(NULL == token){
        fprintf(stderr,"Missing request token\n");
        return -1;
    }
}


main(int argc, char **argv)
{
//
//
//    PROGNAME=argv[0];
//
//    /* wait for file "ready" */
//    struct soap soap;
//    struct ns1__srmStatusOfPutRequestResponse_ srep;
//    struct ns1__srmStatusOfPutRequestRequest sreq;
//    struct ns1__ArrayOfTPutRequestFileStatus *repfs;
//    struct ns1__TReturnStatus *reqstatp;
//    int r = 0;
//    int i;
//    char *tmp;
//    int flags;
//
//    memset (&sreq, 0, sizeof(sreq));
//
//    soap_init (&soap);
//
//#ifdef GSI_PLUGINS
//    flags = CGSI_OPT_DISABLE_NAME_CHECK;
//    soap_register_plugin_arg (&soap, client_cgsi_plugin, &flags);
//#endif
//
//    r = parse_options(argc, argv,&soap,&sreq);
//
//    if (NULL == (sreq.requestToken =
//                 soap_malloc (&soap, sizeof(struct ns1__TRequestToken)))){
//        fprintf(stderr,"Unable to allocate memory!\n");
//        soap_end (&soap);
//        exit (1);
//    }
//
//    sreq.requestToken->value=soap_strdup(&soap, token);
//
//    if (soap_call_ns1__srmStatusOfPutRequest (&soap, endpoint,
//                                              "StatusOfPutRequest", &sreq, &srep)) {
//        soap_print_fault (&soap, stderr);
//        soap_end (&soap);
//        exit (1);
//    }
//    reqstatp = srep.srmStatusOfPutRequestResponse->returnStatus;
//    repfs = srep.srmStatusOfPutRequestResponse->arrayOfFileStatuses;
//    
//     
//    if(reqstatp->statusCode == SRM_USCOREREQUEST_USCOREQUEUED){
//        printf("Reqeust in progress\n");
//        soap_end(&soap);
//        exit(0);
//    }
//    
//    printf("Request results:\n");
//    if (reqstatp->statusCode != SRM_USCORESUCCESS &&
//        reqstatp->statusCode != SRM_USCOREDONE) {
//        if (reqstatp->explanation) {
//            printf ("  Error: %s (%d)\n", reqstatp->explanation, reqstatp->statusCode);                
//        }else
//            printf("  Error: %s (%d)\n", reconvertStatusCode(reqstatp->statusCode), reqstatp->statusCode);
//
//        soap_end (&soap);
//        exit (1);
//    }
//       
//    if (! repfs) {
//        printf ("Error: arrayOfFileStatuses is NULL\n");
//        soap_end (&soap);
//        exit (1);
//    }
//        
//    printf("\nNumber of file in reply: %d\n",repfs->__sizeputStatusArray);
//    for (i = 0; i < repfs->__sizeputStatusArray; i++) {        
//        printf("%d) ",i+1);
//
//        if(SRM_USCOREDONE == (repfs->putStatusArray[i])->status->statusCode){
//            printf ("%s",(repfs->putStatusArray[i])->transferURL->value);
//            if(verbose){
//                puts("");
//                if((repfs->putStatusArray[i])->siteURL)
//                    printf("  SURL: %s\n",(repfs->putStatusArray[i])->siteURL->value);
//                if((repfs->putStatusArray[i])->estimatedProcessingTime)
//                    printf("  Est. Processing Time:    %lld\n",*((repfs->putStatusArray[i])->estimatedProcessingTime));
//                if((repfs->putStatusArray[i])->estimatedWaitTimeOnQueue)
//                    printf("  Est. Wait Time on queue: %lld\n",*((repfs->putStatusArray[i])->estimatedWaitTimeOnQueue));
//                if((repfs->putStatusArray[i])->remainingPinTime)
//                    printf("  Remaining Pin Time:      %lld\n",*((repfs->putStatusArray[i])->remainingPinTime));
//                if((repfs->putStatusArray[i])->fileSize)
//                    printf("  File size:               %lld\n",*((repfs->putStatusArray[i])->fileSize));
//            }
//        }else{ /* Error */
//            printf("ERROR (%d = %s)",
//                   (repfs->putStatusArray[i])->status->statusCode,
//                   reconvertStatusCode((repfs->putStatusArray[i])->status->statusCode));
//            if ((repfs->putStatusArray[i])->status->explanation) 
//                printf(" %s ", (repfs->putStatusArray[i])->status->explanation);
//            if(NULL != (repfs->putStatusArray[i])->transferURL)
//                printf ("(%s)",(repfs->putStatusArray[i])->transferURL->value);                           
//        }
//        puts("");
//    }
//    
//    soap_end (&soap);
//    return 0;
//
}
