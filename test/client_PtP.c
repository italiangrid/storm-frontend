/* Copyright (C) 2006 by Antonio Messina <antonio.messina@ictp.it> for
 * the ICTP project EGRID. */

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

#define _GNU_SOURCE
#include "srmv2H.h"
#include "srmSoapBinding.nsmap"
#include "storm_util.h"
#define DEFPOLLINT 1


#ifdef GSI_PLUGINS
#include "cgsi_plugin.h"
#endif

#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include <stdarg.h>

#ifdef STRICT_CHECKING
#  define EXIT_FOR_CHECK(x) exit(x)
#else
#  define EXIT_FOR_CHECK(x) {x; fprintf(stderr,"STRICT_CHECKING DISABLED! Continuing\n");}
#endif

int pollflag=0;
int sleep_time=DEFPOLLINT;

char *PROGNAME;

char *proto=NULL;
char *host=NULL;
char *port=NULL;
char *endpoint=NULL;
char *sfn=NULL;

void usage()
{
    fprintf(stdout,"Usage: %s [global options] [global surl options] endpoint surl1 [surl1 options] [surl2 [surl2 options]] ...\n",PROGNAME);
    fprintf(stdout,"\n\n"
            "GLOBAL OPTIONS:\n\n"
            "  -p      poll. Default: nopoll\n"
/*          "  -c  N   number of retrys. Default: 3 (implies -p)\n" */
            "  -c  N   update time in seconds\n"
            "  -i  S   user ID (probably ignored by the server)\n"
            "  -P  proto1,proto2,...\n"
            "          list of protocols\n"
            "  -o  C   overwrite mode. 'C' can be:\n"
            "             N = Never\n"
            "             A = Always\n"
            "             D = When files are Different\n"
            "  -r  N   retry time\n"
            "  -d  S   user request description\n"
            "  -S  S   storage system info\n"
            "  -l  N   lifetime in seconds\n"
            "  -h      show this help message\n"
            "  -s  S   space token\n"
            "\n\n"
            "SURL OPTIONS:\n\n"
            "  -t  C   file type. 'C' can be:\n"
            "             V = Volatile\n"
            "             P = Permanent\n"
            "             D = Durable\n"
            "  -z  N   size in bytes\n"
            "\n"
        );

    return;
}

int error_usage(int error_number, const char *format, ...)
{
    va_list args;
    va_start (args, format);
    vfprintf (stderr, format, args);
    va_end (args);
    puts("");
#ifdef STRICT_CHECKING
    puts("");
    usage();
#endif
    return error_number;
}

int error_(int error_number, const char *format, ...)
{
    va_list args;
    va_start (args, format);
    vfprintf (stderr, format, args);
    va_end (args);
    puts("\n");
    return error_number;
}


void fill_storage_type(char *option, struct soap *soap, struct ns1__srmPrepareToPutRequest *req)
{
    req->desiredFileStorageType = soap_malloc(soap, sizeof(enum ns1__TFileStorageType));
    switch(*option){
    case 'V':
    case 'v':
        *req->desiredFileStorageType = VOLATILE;
        break;
    case 'P':
    case 'p':
        *req->desiredFileStorageType = PERMANENT;
        break;
    case 'D':
    case 'd':
        *req->desiredFileStorageType = DURABLE;
        break;
    default:
        EXIT_FOR_CHECK(error_usage(-2,
                                   "Parsing error: unknown parameter %s for '-t' option",
                                   option));
        /* For testing purpouse, fill with the numeric value passed */
        *req->desiredFileStorageType = atoi(option);
    }
}

/* -1 == non ho chiamato parse_all_surl_option
   altrimenti: stato di ritorno di parse_all_surl_option

   nota: questa funzione modifica argv!!!
*/
int parse_global_options_PtP(int argc, 
                             char **argv, 
                             struct soap *soap, 
                             struct ns1__srmPrepareToPutRequest *request)
{
//    struct ns1__srmPrepareToPutRequest *request;
    struct ns1__ArrayOfTPutFileRequest *array;
    int size;
    int index=0;
    int got_endpoint=0;
    if(NULL == soap || NULL == request)
        return ENOMEM;
//    *request_p = request 

    if(argc<3){
#ifdef STRICT_CHECKING
        error_usage(-2,"Too few arguments");
#else
        fprintf(stderr, "Too few arguments but STRICT_CHECKING disabled. Continuing...\n");
#endif
    }


    /* Fill surl options */
    while(argc > ++index){
        if('-' != *argv[index]) /* surl found */
            if(1 == got_endpoint) /*returing to main() */
                return index;
            else{
                int ret=split_surl(argv[index],&proto,&host,&port,&endpoint,&sfn);
                if(SURL_VALID != ret || 
                   SURL_VALID != check_hostname(host) ||
                   SURL_VALID != check_port(port) ||
                   SURL_VALID != check_sfn(sfn))
                    {
                        printf("ret=%d, check_hostname(%s) = %d, check_port(%s) = %d, check_sfn(%s)=%d\n",
                               ret, host,check_hostname(host),port,check_port(port),sfn,check_sfn(sfn));
                        EXIT_FOR_CHECK(error_usage(-2,"Error parsing endpoint."));
                    }
                got_endpoint = 1;
                continue;
            }
        
        switch(*(argv[index]+1)){
        case 'p': /* poll flag */
            pollflag = 1;
            break;
        case 'i': /* user ID */
            index++;
            request->authorizationID=soap_strdup(soap,argv[index]);
            break;
        case 't':
            index++;
            fill_storage_type(argv[index], soap, request);
            break;
        case 'o': /* Overwrite Mode */
            size=sizeof(enum ns1__TOverwriteMode);
            index++;
            if(NULL == (request->overwriteOption = soap_malloc(soap,size)))
                exit(error_usage(-1,"Unable to allocate memory"));           
            switch(*argv[index]){
            case 'N':
            case 'n':
                *request->overwriteOption = NEVER;
                break;
            case 'A':
            case 'a':
                *request->overwriteOption = ALWAYS;
                break;
            case 'D':
            case 'd':
                *request->overwriteOption = WHEN_USCOREFILES_USCOREARE_USCOREDIFFERENT;
                break;
            default:
                EXIT_FOR_CHECK(error_usage(-2,
                                           "Parsing error: unknown parameter %s for '-o'",
                                           argv[index]));
                /* For testing purpouse, fill with the numeric value passed */
                *request->overwriteOption = atoi(argv[index]);
            }
            break;
        case 'P': /* Protocols */
        {           
            char *b,*e;
            int num=1;
            int i=0;
            index++;
            b = argv[index];
            while(NULL != (e = strchr(b,','))){             
                b=e+1;
                if('\0' != *b)
                    num++;
                else
                    break;
            }
            if(NULL == request->transferParameters){
                size = sizeof(struct ns1__TTransferParameters);
                if(NULL == (request->transferParameters = soap_malloc(soap,size)))
                    exit(error_usage(ENOMEM,"Unable to allocate memory"));
                memset(request->transferParameters,0,size);
            }
            size = sizeof(struct ns1__ArrayOfString);
            if(NULL == (request->transferParameters->arrayOfTransferProtocols = 
                        soap_malloc(soap, size)))
                exit(error_usage(ENOMEM,"Unable to allocate memory"));
            memset(request->transferParameters->arrayOfTransferProtocols,0,size);

            request->transferParameters->arrayOfTransferProtocols->__sizestringArray = num;
            
            size = sizeof(char*);
            request->transferParameters->arrayOfTransferProtocols->stringArray = soap_malloc(soap,num * size);
            memset(request->transferParameters->arrayOfTransferProtocols->stringArray,0,size * num);

            b=argv[index];
            for(i=0; i<num; i++){
                e=strchr(b,',');
                if(NULL != e)
                    *e='\0';
                request->transferParameters->arrayOfTransferProtocols->stringArray[i] = soap_strdup(soap,b);
                b=e+1;
            }
            
        }
            break;
/*         case 'S': /\* Storage System Info *\/ */
/*             size=sizeof(struct ns1__TStorageSystemInfo); */
/*             index++; */
/*             if(NULL == (request->storageSystemInfo = soap_malloc(soap,size))) */
/*                 exit(error_usage(-1,"Unable to allocate memory")); */
/*             memset(request->storageSystemInfo,0,size); */
/*             request->storageSystemInfo->value=soap_strdup(soap,argv[index]); */
/*             break; */
        case 'd':
            index++;
            request->userRequestDescription = soap_strdup(soap,argv[index]);
            break;
        case 'c':
            index++;
            sleep_time=atoi(argv[index]);
            break;
        case 'h':
            usage(argv[0]);
            exit(0);
            break;
            /* SURL global options */
        case 'l':
        {
            long long time=0;
            char *err;
            index++;
            request->desiredPinLifeTime = soap_malloc(soap, sizeof(int));
            time = strtoll(argv[index], &err, 10);
            if(time < 0)
                EXIT_FOR_CHECK(error_usage(-2,"Invalid desiredPinLifeTime value: %lld",time));
            if('\0' != *err)
                EXIT_FOR_CHECK(error_usage(-2,"Invalid char in desiredLifeTime value: %lld",time));
            *request->desiredPinLifeTime = time;
        }
            break;
        case 's':
            index++;
            if(argc>index)
                request->targetSpaceToken = soap_strdup(soap, argv[index]);
            else
                EXIT_FOR_CHECK(error_usage(-2, "Option -s requires a Space Token argument"));
            break;
            /* unknown options */
        default:
            EXIT_FOR_CHECK(error_usage(-2,"Parsing error: unknown option %s",argv[index]));
            break;
        }
    }

#ifdef STRICT_CHECKING
    return -1; /* surl missing */
#else
    return index;
#endif
}



/* Returns the next index to analyze. This function allocate the necessary  */
int parse_surl_options_PtP(int start,int argc, char **argv,struct soap *soap, struct ns1__TPutFileRequest **file_p)
{
    struct ns1__TPutFileRequest *file=NULL;
    const char *surl = argv[start];

    int filetype_filled=0;
    int lifetime_filled=0;
    int space_token_filled=0;
    int size_filled=0;

    if(NULL == soap || NULL == file_p)
        return -1;


    if(argc<start)
        return -1;

    if('-' == *argv[start])
        EXIT_FOR_CHECK(error_usage(-2,"Parsing error: %s must be a surl",argv[start]));

    /* Fill surl argument */
    /* malloc() TPutFileRequest */
    int size=sizeof(struct ns1__TPutFileRequest);
    if(NULL == (file = soap_malloc(soap,size)))
        exit(error_usage(ENOMEM,"Unable to allocate memory"));
    memset(file,0,size);
    *file_p = file;

    /* TODO parse surl to check syntax */
    file->targetSURL = soap_strdup(soap,argv[start]);

    ++start;
    if(start >= argc-1)
        return start;

    if(argv[start][0] == '-'){
        if(argv[start][1] == 'z' && argv[start][2]=='\0'){
            ++start;
            if(argc>start){
                long long size=0;
                char *err;
                file->expectedFileSize = soap_malloc(soap, sizeof(ULONG64));
                size = strtoll(argv[start],&err, 10);
                if(time < 0)
                    EXIT_FOR_CHECK(error_usage(-2,"Invalid expectedFileSize: %lld",size));
                if('\0' != *err)
                    EXIT_FOR_CHECK(error_usage(-2,"Invalid char in expectedFileSize value: %lld",size));
                *file->expectedFileSize = size;
            }else{
                EXIT_FOR_CHECK(error_usage(-2,"Parsing error: option -z requires an argument."));               
            }
        }else{
            EXIT_FOR_CHECK(error_usage(-2,"Parsing error: unknown option %s",argv[start]));
        }
        ++start;
    }
    
    /* Fill with global options, if any and if not specified for this
     * file*/

    return start;
}

struct list_of_filerequest {
    struct ns1__TPutFileRequest* value;
    struct list_of_filerequest* next;
};

/* returns the number of surl */
int parse_all_surl_option_PtP(int start, 
                          int argc, 
                          char **argv, 
                          struct soap *soap, 
                          struct ns1__ArrayOfTPutFileRequest **array_p)
{
   int count=0;
   int size;
   int i;
   struct list_of_filerequest *h=calloc(1,sizeof(struct list_of_filerequest));
   struct list_of_filerequest *curr = h;
   struct ns1__ArrayOfTPutFileRequest* array;

   if(NULL == soap || NULL == array_p)
       return ENOMEM;


   while(argc > start){
       curr->next = calloc(1,sizeof(struct list_of_filerequest));
       curr->next->value=NULL;
       curr=curr->next;
       start=parse_surl_options_PtP(start,argc,argv,soap,&curr->value);
       if(start>0)
           count++;
       else
           break;
   }

   size=sizeof(struct ns1__ArrayOfTPutFileRequest);
   if(NULL == (array = soap_malloc(soap, size)))
      exit(error_usage(ENOMEM,"Unable to allocate memory"));
   memset(array,0,size);

   *array_p = array;

   array->__sizerequestArray=count;
   size=sizeof(struct ns1__TPutFileRequest *);
   if(NULL == (array->requestArray = soap_malloc(soap, size * count))){
#ifdef STRICT_CHECKING
      exit(error_usage(ENOMEM,"Unable to allocate memory"));
#else
      return 0;
#endif
   }
   memset(array->requestArray,0,size*count);

   curr = h->next;
   for(i=0;i<count && curr != NULL;i++){
       array->requestArray[i] = curr->value;
       curr = curr->next;
   }
      
   return 0;
}



int main(int argc,char ** argv)
{
   struct ns1__srmPrepareToPutRequest *request;
   struct ns1__srmPrepareToPutResponse_ response_p;
   struct ns1__srmPrepareToPutResponse *response;
   int ret;
   struct soap soap;
   int flags;
   int size;

   PROGNAME=argv[0];

   soap_init (&soap);
   
#ifdef GSI_PLUGINS
/* Delegate proxy ---- TO BE CHANGED */
/*     flags = CGSI_OPT_DISABLE_NAME_CHECK|CGSI_OPT_DELEG_FLAG; *\/ */

   flags = CGSI_OPT_DISABLE_NAME_CHECK|CGSI_OPT_DELEG_FLAG;
   soap_register_plugin_arg (&soap, client_cgsi_plugin, &flags);
#endif
   size=sizeof(struct ns1__srmPrepareToPutRequest);
   if(NULL == (request = soap_malloc(&soap,size))){
       fprintf(stderr,"Unable to malloc() memory\n");
       return -1;
   }
   memset(request,0,size);
       

   /* Parsing options */
   ret = parse_global_options_PtP(argc,argv,&soap,request);
   if(ret < 0)
       return -1;
   ret = parse_all_surl_option_PtP(ret,argc,argv,&soap,&(request->arrayOfFileRequests));

    printf("Sending PtP request to: %s\n", endpoint);

   /* call soap */
   if (soap_call_ns1__srmPrepareToPut (&soap, endpoint, "PrepareToPut",
                                       request, &response_p)) {
       soap_print_fault (&soap, stderr);
       soap_print_fault_location (&soap, stderr);
       soap_end (&soap);
       exit (1);
   }

   response=response_p.srmPrepareToPutResponse;
   if (response->returnStatus->statusCode != SRM_USCORESUCCESS
       && response->returnStatus->statusCode != SRM_USCOREDONE
       && response->returnStatus->statusCode != SRM_USCOREREQUEST_USCOREQUEUED) {
       printf("Error: %d\n", response->returnStatus->statusCode);
       if (response->returnStatus->explanation) {
           printf ("Error: %s\n", response->returnStatus->explanation);
               
       }
       soap_end(&soap);
       exit (1);
   }

   if (response->requestToken) {
       char *r_token = response->requestToken;
       printf ("Request Token:     %s\n", r_token);
   }
   
   if (pollflag) {
       /* wait for file "ready" */
       struct soap soapr;
       struct ns1__srmStatusOfPutRequestResponse_ srep;
       struct ns1__srmStatusOfPutRequestRequest sreq;
       struct ns1__ArrayOfTPutRequestFileStatus *repfs;
       struct ns1__TReturnStatus *reqstatp = 
           response_p.srmPrepareToPutResponse->returnStatus;
       int soap_bool = 0;
       int r = 0;
       int i;
       memset (&sreq, 0, sizeof(sreq));
       sreq.requestToken = soap_strdup(&soapr, response_p.srmPrepareToPutResponse->requestToken);
       fprintf(stdout,"Polling...\n");

       soap_init (&soapr);
#ifdef GSI_PLUGINS
       flags = CGSI_OPT_DISABLE_NAME_CHECK | CGSI_OPT_DELEG_FLAG;
       soap_register_plugin_arg (&soapr, client_cgsi_plugin, &flags);
#endif
       while (reqstatp->statusCode == SRM_USCOREREQUEST_USCOREQUEUED ||
              reqstatp->statusCode == SRM_USCOREREQUEST_USCOREINPROGRESS ||
              reqstatp->statusCode == SRM_USCOREREQUEST_USCORESUSPENDED) {
           printf("...status: %s (%d)\n", reconvertStatusCode(reqstatp->statusCode), reqstatp->statusCode);

           sleep ((r++ == 0) ? 1 : DEFPOLLINT);
           sleep(sleep_time);
           if ( soap_bool ) soap_end (&soapr);
           soap_bool = 1;
           printf("Sending PtP request to: %s\n", endpoint);
           if (soap_call_ns1__srmStatusOfPutRequest (&soapr, endpoint,
                                                     "StatusOfPutRequest", &sreq, &srep)) {
               soap_print_fault (&soapr, stderr);
               soap_end (&soap);
               soap_end (&soapr);
               exit (1);
           }
           reqstatp = srep.srmStatusOfPutRequestResponse->returnStatus;
           repfs = srep.srmStatusOfPutRequestResponse->arrayOfFileStatuses;
       }
    
       printf("Request results:\n");
      
       if (reqstatp->statusCode != SRM_USCORESUCCESS &&
           reqstatp->statusCode != SRM_USCOREDONE) {
           if (reqstatp->explanation) {
               printf ("Error: %s\n", reqstatp->explanation);
               
           }
           soap_end (&soapr);
           soap_end(&soap);
           exit (1);
       }
      
       if (! repfs) {
           printf ("arrayOfFileStatuses is NULL\n");
           soap_end (&soapr);
           soap_end (&soap);
           exit (1);
       }
       
       printf("\nNumber of file in reply: %d\n",repfs->__sizestatusArray);
       for (i = 0; i < repfs->__sizestatusArray; i++) {        
           printf("%d) ",i+1);

           if(SRM_USCOREDONE == (repfs->statusArray[i])->status->statusCode
               || SRM_USCORESPACE_USCOREAVAILABLE == (repfs->statusArray[i])->status->statusCode){
               printf ("%s",(repfs->statusArray[i])->transferURL);
           }else{ /* Error */
               printf("ERROR (%d = %s)",
                      (repfs->statusArray[i])->status->statusCode,
                      reconvertStatusCode((repfs->statusArray[i])->status->statusCode));
               if ((repfs->statusArray[i])->status->explanation) 
                   printf(" %s ", (repfs->statusArray[i])->status->explanation);
               if(NULL != (repfs->statusArray[i])->transferURL)
                   printf ("(%s)",(repfs->statusArray[i])->transferURL);                           
           }
           puts("");
       }
   
       soap_end (&soapr);
   }


   soap_end (&soap);
   exit (0);
}
