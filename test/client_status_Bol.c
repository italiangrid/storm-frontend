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
                    struct ns1__srmStatusOfBringOnlineRequestRequest *request)
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
    size=sizeof(struct ns1__ArrayOfAnyURI);  
    if(NULL == (request->arrayOfSourceSURLs = soap_malloc(soap,size))){
        fprintf(stderr,"Unable to allocate memory\n");
        exit(-1);
    }

    memset(request->arrayOfSourceSURLs,0,size);

    request->arrayOfSourceSURLs->__sizeurlArray = num;

    if(NULL == (request->arrayOfSourceSURLs->urlArray = soap_malloc(soap, num * sizeof(char *)))){
        fprintf(stderr,"Unable to allocate memory\n");
        exit(-1);
    }


    for(i=0;i<num;i++)
       request->arrayOfSourceSURLs->urlArray[i] = soap_strdup(soap,argv[i+start]);

    return start+num+1;
}

int parse_options(int argc, 
                  char **argv, 
                  struct soap *soap, 
                  struct ns1__srmStatusOfBringOnlineRequestRequest *request)
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
            }else{
                if(NULL == token){
                    token=argv[i];
                }else{
                    i=fill_surl_array(argc,argv,i,soap,request);
                    if(0 >  i){
                        fprintf(stderr,"Error parsing surl array\n");
                        usage();
                        exit(1);
                    }
                    --i;
                }
            }
            continue;
        }
        switch(argv[i][1]){
        case 'i':
            if(i>=argc-1){
                fprintf(stderr,"Missing argument to '-i' option\n");
                usage();
                exit(1);
            }else{
                i++;
                request->authorizationID=soap_strdup(soap,argv[i]);
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


   PROGNAME=argv[0];

   /* wait for file "ready" */
   struct soap soap;
   struct ns1__srmStatusOfBringOnlineRequestResponse_ srep;
   struct ns1__srmStatusOfBringOnlineRequestRequest sreq;
   struct ns1__ArrayOfTBringOnlineRequestFileStatus *repfs;
   struct ns1__TReturnStatus *reqstatp;
   int r = 0;
   int i;
   char *tmp;
   int flags;

   memset (&sreq, 0, sizeof(sreq));

   soap_init (&soap);

#ifdef GSI_PLUGINS
   flags = CGSI_OPT_DISABLE_NAME_CHECK | CGSI_OPT_DELEG_FLAG;
   soap_register_plugin_arg (&soap, client_cgsi_plugin, &flags);
#endif

   r = parse_options(argc, argv,&soap,&sreq);
   
   sreq.requestToken = soap_strdup(&soap, token);


   if (soap_call_ns1__srmStatusOfBringOnlineRequest (&soap, endpoint,
                                             "StatusOfBringOnlineRequest", &sreq, &srep)) {
       soap_print_fault (&soap, stderr);
       soap_end (&soap);
       exit (1);
   }
   reqstatp = srep.srmStatusOfBringOnlineRequestResponse->returnStatus;
   repfs = srep.srmStatusOfBringOnlineRequestResponse->arrayOfFileStatuses;
               
   if(reqstatp->statusCode == SRM_USCOREREQUEST_USCOREQUEUED){
       printf("Reqeust in progress\n");
       soap_end(&soap);
       exit(0);
   }
   
   printf("Request results:\n");
   if (reqstatp->statusCode != SRM_USCORESUCCESS &&
       reqstatp->statusCode != SRM_USCOREDONE) {
       if (reqstatp->explanation) {
           printf ("  Error: %s (%d)\n", reqstatp->explanation, reqstatp->statusCode);                
       }else
           printf("  Error: %s (%d)\n", reconvertStatusCode(reqstatp->statusCode), reqstatp->statusCode);

       soap_end (&soap);
       exit (1);
   }
      
   if (! repfs) {
       printf ("Error: arrayOfFileStatuses is NULL\n");
       soap_end (&soap);
       exit (1);
   }
       
   printf("\nNumber of file in reply: %d\n",repfs->__sizestatusArray);
   for (i = 0; i < repfs->__sizestatusArray; i++) {        
       printf("%d) ",i+1);

       if(SRM_USCOREDONE == (repfs->statusArray[i])->status->statusCode){
           if(verbose){
               puts("");
               if((repfs->statusArray[i])->sourceSURL)
                   printf("  SURL: %s\n",(repfs->statusArray[i])->sourceSURL);
               if((repfs->statusArray[i])->estimatedWaitTime)
                   printf("  Est. Waiting Time:       %lld\n",*((repfs->statusArray[i])->estimatedWaitTime));
               if((repfs->statusArray[i])->remainingPinTime)
                   printf("  Remaining Pin Time:      %lld\n",*((repfs->statusArray[i])->remainingPinTime));
               if((repfs->statusArray[i])->fileSize)
                   printf("  File size:               %lld\n",*((repfs->statusArray[i])->fileSize));
           }
       }else{ /* Error */
           printf("ERROR (%d = %s)",
                  (repfs->statusArray[i])->status->statusCode,
                  reconvertStatusCode((repfs->statusArray[i])->status->statusCode));
           if ((repfs->statusArray[i])->status->explanation) 
               printf(" %s ", (repfs->statusArray[i])->status->explanation);
       }
       puts("");
   }
   
   soap_end (&soap);
   return 0;

}
