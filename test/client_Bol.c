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
#include <stdio.h>
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
            "  -c  N   update time in seconds\n"
            "  -i  S   user ID (probably ignored by the server)\n"
            "  -P  proto1,proto2,...\n"
            "          list of protocols\n"
            "  -r  N   retry time\n"
            "  -d  S   user request description\n"
/*             "  -S  S   storage system info\n" */
            "  -t  C   file type. 'C' can be:\n"
            "             V = Volatile\n"
            "             P = Permanent\n"
            "             D = Durable\n"
            "  -h      show this help message\n"
            "  -l  N   lifetime in seconds\n"
            "  -s  S   space token\n"
            "\n\n"
            "SURL OPTIONS:\n\n"
            "  -D      Do a recursive BringOnline (default: not specified\n"
            "  -R      Scan *ALL* level (default: false)\n"
            "  -L  N   max number of levels (default: 1.) \n"
            "          If '-R' is supplied, then '-L' option should be \n"
            "          ignored by the server\n"
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

char *global_filetype=NULL;
char *global_lifetime=NULL;
char *global_space_token=NULL;
int global_recursive=0;
int global_is_directory=0;
char *global_level_number=NULL;

void fill_storage_type(char *option, struct soap *soap, struct ns1__srmBringOnlineRequest *file)
{
    file->desiredFileStorageType = soap_malloc(soap, sizeof(enum ns1__TFileStorageType));
    if(NULL == file->desiredFileStorageType)
        exit(error_usage(ENOMEM,"Unable to allocate memory"));
    
    switch(*option){
    case 'V':
    case 'v':
        *file->desiredFileStorageType = VOLATILE;
        break;
    case 'P':
    case 'p':
        *file->desiredFileStorageType = PERMANENT;
        break;
    case 'D':
    case 'd':
        *file->desiredFileStorageType = DURABLE;
        break;
    default:
        EXIT_FOR_CHECK(error_usage(-2,
                                   "Parsing error: unknown parameter %s for '-t' option",
                                   option));
        /* For testing purpouse, fill with the numeric value passed */
        *file->desiredFileStorageType = atoi(option);
    }
}

void fill_lifetime(char *option, struct soap *soap, struct ns1__srmBringOnlineRequest *file)
{
    char *err;
    long long lifetime=0;
    int size=sizeof(int);
    if(NULL == (file->desiredLifeTime = soap_malloc(soap,size)))
        exit(error_usage(ENOMEM,"Unable to allocate memory"));
    memset(file->desiredLifeTime,0,size);

    lifetime = strtoll(option, &err, 10);
    if(lifetime < 0)
        EXIT_FOR_CHECK(error_usage(-2,"Invalid lifetime value: %lld",lifetime));
    if('\0' != *err)
        EXIT_FOR_CHECK(error_usage(-2,"Invalid char in lifetime value: %lld",lifetime));
    *file->desiredLifeTime=lifetime;
}

/* -1 == non ho chiamato parse_all_surl_option
   altrimenti: stato di ritorno di parse_all_surl_option

   nota: questa funzione modifica argv!!!
*/
int parse_global_options_PtG(int argc, 
                             char **argv, 
                             struct soap *soap, 
                             struct ns1__srmBringOnlineRequest *request)
{
//    struct ns1__srmBringOnlineRequest *request;
    struct ns1__ArrayOfTGetFileRequest *array;
    int size;
    int index=0;
    int got_endpoint=0;
    if(NULL == soap || NULL == request)
        return ENOMEM;
//    *request_p = request 

    if(argc<3){
        error_usage(-2,"Too few arguments");
#ifndef STRICT_CHECKING
        usage(argv[0]);
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
        case 'r': /* Retry time */
        {
            char *err;
            long long totalRetryTime=0;
            index++;

            totalRetryTime = strtoll(argv[index], &err, 10);
            if(totalRetryTime < 0)
                EXIT_FOR_CHECK(error_usage(-2,"Invalid totalRetryTime value: %lld",totalRetryTime));
            if('\0' != *err)
                EXIT_FOR_CHECK(error_usage(-2,"Invalid char in totalRetryTime value: %lld",totalRetryTime));
            request->desiredTotalRequestTime = soap_malloc(soap, sizeof(int));
            *request->desiredTotalRequestTime=totalRetryTime;
            break;
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
            
            size = sizeof(struct ns1__TTransferParameters);
            if(NULL == (request->transferParameters = soap_malloc(soap,size)))
                exit(error_usage(ENOMEM,"Unable to allocate memory"));
            memset(request->transferParameters,0,size);
            
            size = sizeof(struct ns1__ArrayOfString);
            if(NULL == (request->transferParameters->arrayOfTransferProtocols = soap_malloc(soap,size)))
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
        case 't':    /* Fill File storage type */
            index++;
            /* File storage type is mandatory in case of put. If not supplied,
             * soap_call_* crashes! */
            fill_storage_type(argv[index],soap, request);
            break;
        case 'l':
            index++;
            fill_lifetime(argv[index], soap, request);
            break;
        case 's':
            index++;
            global_space_token = argv[index];
            break;
        case 'R':
            global_recursive=1;
            break;
        case 'D':
            global_is_directory=1;
            break;
        case 'L':
            index++;
            global_level_number = argv[index];
            break;
            /* unknown options */
        default:
            EXIT_FOR_CHECK(error_usage(-2,"Parsing error: unknown option %s",argv[index]));
            break;
        }
    }

    return -1; /* surl missing */
}


/* void fill_space_token(char *option, struct soap *soap, struct ns1__TGetFileRequest *file) */
/* { */
/*     int size=sizeof(struct ns1__TSpaceToken); */
/*     if(NULL == (file->spaceToken = soap_malloc(soap,size))) */
/*         exit(error_usage(ENOMEM,"Unable to allocate memory")); */
/*     memset(file->spaceToken,0,size); */
/*     file->spaceToken->value = soap_strdup(soap,option); */
/* } */


void fill_recursive(struct soap *soap, struct ns1__TGetFileRequest *file)
{
    if(NULL == file)
        return;
    if(NULL == file->dirOption){
        int size=sizeof(struct ns1__TDirOption);
        file->dirOption = soap_malloc(soap,size);
        memset(file->dirOption,0,size);
    }
    if(NULL == file->dirOption->allLevelRecursive)
        file->dirOption->allLevelRecursive = 
            soap_malloc(soap,sizeof(enum xsd__boolean));
    *(file->dirOption->allLevelRecursive) = true_;  
}

void fill_is_directory(struct soap *soap, struct ns1__TGetFileRequest *file)
{
    if(NULL == file)
        return;
    if(NULL == file->dirOption){
        int size=sizeof(struct ns1__TDirOption);
        file->dirOption = soap_malloc(soap,size);
        memset(file->dirOption,0,size);
    }
    file->dirOption->isSourceADirectory = true_;    
}

void fill_levels_number(char *option, struct soap *soap, struct ns1__TGetFileRequest *file)
{
    int levelnumber=0;
    char *err;
    if(NULL == file)
        return;
    if(NULL == file->dirOption){
        int size=sizeof(struct ns1__TDirOption);
        file->dirOption = soap_malloc(soap,size);
        memset(file->dirOption,0,size);
    }

    levelnumber = (int) strtol(option, &err, 10);
    if(levelnumber < 0)
        EXIT_FOR_CHECK(error_usage(-2,"Invalid size value: %d",levelnumber));
    if('\0' != *err)
        EXIT_FOR_CHECK(error_usage(-2,"Invalid char in size value: %d",levelnumber));   
    file->dirOption->numOfLevels = soap_malloc(soap, sizeof(int));
    *file->dirOption->numOfLevels = levelnumber;
}

/* Returns the next index to analyze. This function allocate the necessary  */
int parse_surl_options_PtG(int start,int argc, char **argv,struct soap *soap, struct ns1__TGetFileRequest **file_p)
{
    struct ns1__TGetFileRequest *file=NULL;
    const char *surl = argv[start];

    int filetype_filled=0;
    int lifetime_filled=0;
    int space_token_filled=0;
    int recursive_filled = 0;
    int is_directory_filled = 0;
    int level_number_filled = 0;
    if(NULL == soap || NULL == file_p)
        return -1;


    if(argc<start)
        return -1;

    if('-' == *argv[start])
        EXIT_FOR_CHECK(error_usage(-2,"Parsing error: %s must be a surl",argv[start]));

    /* Fill surl argument */
    /* malloc() TGetFileRequest */
    int size=sizeof(struct ns1__TGetFileRequest);
    if(NULL == (file = soap_malloc(soap,size)))
        exit(error_usage(ENOMEM,"Unable to allocate memory"));
    memset(file,0,size);
    *file_p = file;

    /* TODO parse surl to check syntax */
    file->sourceSURL = soap_strdup(soap,argv[start]);

    /* Fill argument options */
    for(++start; start < argc && '-' == *(argv[start]); start++){
        switch(*(argv[start]+1)){
/*         case 's':  /\* space token *\/ */
/*             start++; */
/*             fill_space_token(argv[start], soap, file); */
/*             space_token_filled=1; */
/*             break; */
        case 'R': /* recursive */
            fill_recursive(soap,file);
            recursive_filled = 1;
            break;
        case 'D': /* isDirectory? */
            fill_is_directory(soap,file);
            is_directory_filled = 1;
            break;
        case 'L': /* max level number */
            fill_levels_number(argv[++start],soap,file);
            level_number_filled = 1;
            break;
        case 'h':
            usage(argv[0]);
            exit(0);
            break;          
        default:
            EXIT_FOR_CHECK(error_usage(-2,"Parsing error: unknown option %s",argv[start]));
            break;
        }
    }
    
    /* Fill with global options, if any and if not specified for this
     * file*/
/*     if(NULL != global_space_token && 0 == space_token_filled) */
/*         fill_space_token(global_space_token, soap, file); */
    if(1 == global_recursive && 0 == recursive_filled)
        fill_recursive(soap, file);
    if(1 == global_is_directory && 0 == is_directory_filled)
        fill_is_directory(soap, file);
    if(NULL != global_level_number && 0 == level_number_filled)
        fill_levels_number(global_level_number, soap, file);

    return start;
}

struct list_of_filerequest {
    struct ns1__TGetFileRequest* value;
    struct list_of_filerequest* next;
};

/* returns the number of surl */
int parse_all_surl_option_PtG(int start, 
                          int argc, 
                          char **argv, 
                          struct soap *soap, 
                          struct ns1__ArrayOfTGetFileRequest **array_p)
{
    int count=0;
    int size;
    int i;
    struct list_of_filerequest *h=calloc(1,sizeof(struct list_of_filerequest));
    struct list_of_filerequest *curr = h;
    struct ns1__ArrayOfTGetFileRequest* array;

    if(NULL == soap || NULL == array_p)
        return ENOMEM;


    while(argc > start){
        curr->next = calloc(1,sizeof(struct list_of_filerequest));
        curr->next->value=NULL;
        curr=curr->next;
        start=parse_surl_options_PtG(start,argc,argv,soap,&curr->value);
        if(start>0)
            count++;
        else
            break;
    }

    size=sizeof(struct ns1__ArrayOfTGetFileRequest);
    if(NULL == (array = soap_malloc(soap, size)))
       exit(error_usage(ENOMEM,"Unable to allocate memory"));
    memset(array,0,size);

    *array_p = array;

    array->__sizerequestArray=count;
    size=sizeof(struct ns1__TGetFileRequest *);
    if(NULL == (array->requestArray = soap_malloc(soap, size * count)))
       exit(error_usage(ENOMEM,"Unable to allocate memory"));
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
    struct ns1__srmBringOnlineRequest *request;
    struct ns1__srmBringOnlineResponse_ response_p;
    struct ns1__srmBringOnlineResponse *response;
    int ret;
    struct soap soap;
    int flags;
    int size;

    PROGNAME=argv[0];

    soap_init (&soap);
    
#ifdef GSI_PLUGINS
    flags = CGSI_OPT_DISABLE_NAME_CHECK | CGSI_OPT_DELEG_FLAG;
    soap_register_plugin_arg (&soap, client_cgsi_plugin, &flags);
#endif
    size=sizeof(struct ns1__srmBringOnlineRequest);
    if(NULL == (request = soap_malloc(&soap,size))){
        fprintf(stderr,"Unable to malloc() memory\n");
        return -1;
    }
    memset(request,0,size);
        

    /* Parsing options */
    ret = parse_global_options_PtG(argc,argv,&soap,request);
    if(ret < 0)
        return -1;
    ret = parse_all_surl_option_PtG(ret,argc,argv,&soap,&(request->arrayOfFileRequests));

    /* call soap */
    if (soap_call_ns1__srmBringOnline (&soap, endpoint, "BringOnline",
                                        request, &response_p)) {
        soap_print_fault (&soap, stderr);
        soap_print_fault_location (&soap, stderr);
        soap_end (&soap);
        exit (1);
    }

    response=response_p.srmBringOnlineResponse;
    if (response->requestToken) {
        char *r_token = response->requestToken;
        printf ("Request Token:     %s\n", r_token);
    }
    if (response->returnStatus->statusCode != SRM_USCOREREQUEST_USCOREQUEUED &&
        response->returnStatus->statusCode != SRM_USCOREREQUEST_USCOREINPROGRESS &&
        response->returnStatus->statusCode != SRM_USCOREREQUEST_USCORESUSPENDED) {
        if(NULL != response->returnStatus->explanation)
            fprintf(stderr,"Error: %s\n",response->returnStatus->explanation);
        else
            fprintf(stderr,"Error %d\n",response->returnStatus->statusCode);
    }
    
    if (pollflag) {
        /* wait for file "ready" */
        struct soap soapr;
        struct ns1__srmStatusOfBringOnlineRequestResponse_ srep;
        struct ns1__srmStatusOfBringOnlineRequestRequest sreq;
        struct ns1__ArrayOfTGetRequestFileStatus *repfs;
        struct ns1__TReturnStatus *reqstatp = 
            response_p.srmBringOnlineResponse->returnStatus;
        int soap_bool = 0;
        int r = 0;
        int i;
        memset (&sreq, 0, sizeof(sreq));
        sreq.requestToken = soap_strdup(&soapr,response_p.srmBringOnlineResponse->requestToken);
        fprintf(stdout,"Polling...\n");

        soap_init (&soapr);
#ifdef GSI_PLUGINS
        flags = CGSI_OPT_DISABLE_NAME_CHECK;
        soap_register_plugin_arg (&soapr, client_cgsi_plugin, &flags);
#endif
        while (reqstatp->statusCode == SRM_USCOREREQUEST_USCOREQUEUED ||
               reqstatp->statusCode == SRM_USCOREREQUEST_USCOREINPROGRESS ||
               reqstatp->statusCode == SRM_USCOREREQUEST_USCORESUSPENDED) {
            printf("...status: %s (%d)\n", reconvertStatusCode(reqstatp->statusCode), reqstatp->statusCode);

//            sleep ((r++ == 0) ? 1 : DEFPOLLINT);
            sleep(sleep_time);
            if ( soap_bool ) soap_end (&soapr);
            soap_bool = 1;
            if (soap_call_ns1__srmStatusOfBringOnlineRequest (&soapr, endpoint,
                                                      "StatusOfBringOnlineRequest", &sreq, &srep)) {
                soap_print_fault (&soapr, stderr);
                soap_end (&soap);
                soap_end (&soapr);
                exit (1);
            }
            reqstatp = srep.srmStatusOfBringOnlineRequestResponse->returnStatus;
            repfs = srep.srmStatusOfBringOnlineRequestResponse->arrayOfFileStatuses;
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

/*             if(SRM_USCOREDONE == (repfs->statusArray[i])->status->statusCode){ */
/*                 printf ("%s",(repfs->statusArray[i])->transferURL); */
/*             }else{ /\* Error *\/ */
/*                 printf("ERROR (%d = %s)", */
/*                        (repfs->statusArray[i])->status->statusCode, */
/*                        reconvertStatusCode((repfs->statusArray[i])->status->statusCode)); */
/*                 if ((repfs->statusArray[i])->status->explanation)  */
/*                     printf(" %s ", (repfs->statusArray[i])->status->explanation); */
/*                 printf ("(%s)",(repfs->statusArray[i])->transferURL);                            */
/*             } */
            puts("");
        }
    
        soap_end (&soapr);
    }


    soap_end (&soap);
    exit (0);
}
