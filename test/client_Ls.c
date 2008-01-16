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


char *PROGNAME;
char *endpoint=NULL;
int verbose=0;

char *host=NULL;
char *port=NULL;

char *global_storage_sysinfo=NULL;

#ifdef STRICT_CHECKING
#  define EXIT_FOR_CHECK(x) exit(x)
#else
#  define EXIT_FOR_CHECK(x) {x; fprintf(stderr,"STRICT_CHECKING DISABLED! Continuing\n");}
#endif

void usage()
{
    fprintf(stdout,"Usage: %s [global options] [global surl options] endpoint surl1 [surl1 options] [surl2 [surl2 options]] ...\n",PROGNAME);
    fprintf(stdout,"\n\n"
            "GLOBAL OPTIONS:\n\n"
            "  -i  S   user ID (probably ignored by the server)\n"
            "  -l      verbose listing\n"
            "  -n  N   level of recursion. Implies '-r':\n"
            "            N >= 0 means only N level of directory\n"
            "            N <  0 means infinite recursion\n"
            "  -r      recursive listing. If -n is not supplied, then\n"
            "          infinite recursion will be requested\n"
            "  -c  N   count (do anyone know the meaing of this field???)\n"
            "  -t  C   file type. 'C' can be:\n"
            "             V = Volatile\n"
            "             P = Permanent\n"
            "             D = Durable\n"
            "  -o  N   offset\n"
            "  -h      show this help message\n"
            "\n\n"
            "SURL OPTIONS:\n\n"
            "  -S  S   storage system info\n"
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

void fill_storage_type(char *option, struct soap *soap, struct ns1__srmLsRequest *request)
{
    int size=sizeof(enum ns1__TFileStorageType);
    if(NULL == (request->fileStorageType=soap_malloc(soap,size)))   
        exit(error_usage(ENOMEM,"Unable to allocate memory"));
    memset(request->fileStorageType,0,size);

    switch(*option){
    case 'V':
    case 'v':
        *request->fileStorageType = Volatile_;
        break;
    case 'P':
    case 'p':
        *request->fileStorageType = Permanent_;
        break;
    case 'D':
    case 'd':
        *request->fileStorageType = Durable_;
        break;
    default:
        EXIT_FOR_CHECK(error_usage(-2,
                                   "Parsing error: unknown parameter %s for '-t' option",
                                   option));
        /* For testing purpouse, fill with the numeric value passed */
        *request->fileStorageType = atoi(option);
    }
}


int parse_global_options_Ls(int argc, 
                             char **argv, 
                             struct soap *soap, 
                             struct ns1__srmLsRequest *request)
{
    int i=0;
    int got_endpoint=0;
    int size;
    if(NULL == soap || NULL == request)
        return ENOMEM;

    while(argc > ++i){
        if('-' != *argv[i]){ /* surl found */
            if(1 == got_endpoint){ /*returing to main() */
                return i;
            }else{
                int ret=split_surl(argv[i],NULL,&host,&port,&endpoint,NULL);
                if(SURL_VALID != ret || 
                   SURL_VALID != check_hostname(host) ||
                   SURL_VALID != check_port(port))
                    {
                        printf("ret=%d, check_hostname(%s) = %d, check_port(%s) = %d\n",
                               ret, host,check_hostname(host),port,check_port(port));
                        EXIT_FOR_CHECK(error_usage(-2,"Error parsing endpoint."));
                    }
                got_endpoint = 1;
                continue;
            }
        }
        switch(*(argv[i]+1)){
        case 'i': /* user ID */
            size=sizeof(struct ns1__TUserID);
            i++;
            if(argc<=i)
                EXIT_FOR_CHECK(error_usage(-2,"Missing argument for option '-i'"));

            if(NULL == (request->userID = soap_malloc(soap,size)))
                exit(error_usage(-1,"Unable to allocate memory"));
            memset(request->userID,0,size);
            request->userID->value=soap_strdup(soap,argv[i]);
            break;
        case 't':
            i++;
            if(argc<=i)
                EXIT_FOR_CHECK(error_usage(-2,"Missing argument for option '-t'"));
            fill_storage_type(argv[i], soap, request);
            break;
        case 'c':
            i++;
            if(argc<=i)
                EXIT_FOR_CHECK(error_usage(-2,"Missing argument for option '-c'"));
            if(NULL == (request->count=soap_malloc(soap,sizeof(int))))
                exit(error_usage(-1,"Unable to allocate memory"));
            { 
                long long count = 0;
                char *err;
                count = strtoll(argv[i],&err,10);
                if(count < 0)
                    EXIT_FOR_CHECK(error_usage(-2,"Invalid count value: %lld",count));
                if('\0' != *err)
                    EXIT_FOR_CHECK(error_usage(-2,"Invalid char in count value: %lld",count));
                *request->count = count;
            }
            break;
        case 'o':
            i++;
            if(argc<=i)
                EXIT_FOR_CHECK(error_usage(-2,"Missing argument for option '-o'"));
            if(NULL == (request->offset=soap_malloc(soap,sizeof(int))))
                exit(error_usage(-1,"Unable to allocate memory"));
            { 
                long long offset = 0;
                char *err;
                offset = strtoll(argv[i],&err,10);
                if(offset < 0)
                    EXIT_FOR_CHECK(error_usage(-2,"Invalid offset value: %lld",offset));
                if('\0' != *err)
                    EXIT_FOR_CHECK(error_usage(-2,"Invalid char in offset value: %lld",offset));
                *request->offset = offset;
            }
            break;
        case 'r':
            if(NULL == request->numOfLevels){
                if(NULL == (request->allLevelRecursive=soap_malloc(soap,sizeof(enum xsd__boolean*))))
                    exit(error_usage(-1,"Unable to allocate memory"));
                *request->allLevelRecursive = true_;
            }
            break;
        case 'n':
            i++;
            if(argc<=i)
                EXIT_FOR_CHECK(error_usage(-2,"Missing argument for option '-n'"));

            { 
                long long recursion = 0;
                char *err;
                recursion = strtoll(argv[i],&err,10);
                if('\0' != *err)
                    EXIT_FOR_CHECK(error_usage(-2,"Invalid char in recursion value: %lld",recursion));
                if(recursion < 0){
                    if(NULL == (request->allLevelRecursive=soap_malloc(soap,sizeof(enum xsd__boolean*))))
                        exit(error_usage(-1,"Unable to allocate memory"));
                    *request->allLevelRecursive = true_;
                }else{
                    if(NULL == (request->numOfLevels=soap_malloc(soap,sizeof(int))))
                        exit(error_usage(-1,"Unable to allocate memory"));
                    *request->numOfLevels = recursion;
                    if(request->allLevelRecursive &&  *request->allLevelRecursive == true_)
                        *request->allLevelRecursive = false_;
                }
            }
            break;
        case 'l':
            verbose=1;
            if(NULL == (request->fullDetailedList=soap_malloc(soap,sizeof(enum xsd__boolean*))))
                exit(error_usage(-1,"Unable to allocate memory"));
            *request->fullDetailedList = true_;
            break;          
        case 'h':
            usage(argv[0]);
            exit(0);
            break;
        default:
            EXIT_FOR_CHECK(error_usage(-2,"Parsing error: unknown option %s",argv[i]));
            break;
        }
    }
    EXIT_FOR_CHECK(error_usage(-2,"Missing arguments"));
    return -1;
}
       


int parse_surl_options_Ls(int start,
                           int argc, 
                           char **argv,
                           struct soap *soap, 
                           struct ns1__TSURLInfo **file_p)
{
    struct ns1__TSURLInfo *file=NULL;

        if(NULL == soap || NULL == file_p)
        return -1;
    if(argc<start)
        return -1;
    if('-' == *argv[start])
        EXIT_FOR_CHECK(error_usage(-2,"Parsing error: %s must be a surl",argv[start]));


    int size=sizeof(struct ns1__TSURLInfo);
    if(NULL == (file = soap_malloc(soap,size)))
        exit(error_usage(ENOMEM,"Unable to allocate memory"));
    memset(file,0,size);
    *file_p = file;

    size=sizeof(struct ns1__TSURL);
    if(NULL == (file->SURLOrStFN = soap_malloc(soap,size)))
        exit(error_usage(ENOMEM,"Unable to allocate memory"));
    memset(file->SURLOrStFN,0,size);
    file->SURLOrStFN->value=soap_strdup(soap,argv[start]);

    for(++start; start < argc && '-' == *(argv[start]); start++){
        switch(*(argv[start]+1)){
        case 'S': /* Storage System Info */
            size=sizeof(struct ns1__TStorageSystemInfo);
            start++;
            if(argc<=start){
                EXIT_FOR_CHECK(error_usage(-2,"Missing argument for option '-S'"));
            }
            if(NULL == (file->storageSystemInfo = soap_malloc(soap,size)))
                exit(error_usage(-1,"Unable to allocate memory"));
            memset(file->storageSystemInfo,0,size);
            file->storageSystemInfo->value=soap_strdup(soap,argv[start]);
            break;
        case 'h':
            usage(argv[0]);
            exit(0);
            break;
        default:
            EXIT_FOR_CHECK(error_usage(-2,"Parsing error: unknown option %s",argv[start]));
        }
    }

    return start;
}


struct list_of_filerequest {
    struct ns1__TSURLInfo* value;
    struct list_of_filerequest* next;
};


int parse_all_surl_option_Ls(int start,
                              int argc,
                              char **argv,
                              struct soap *soap,
                              struct ns1__ArrayOfTSURLInfo** array_p)

{
    int count=0;
    int size;
    int i;

    struct list_of_filerequest *h=calloc(1,sizeof(struct list_of_filerequest));
    struct list_of_filerequest *curr = h;
    struct ns1__ArrayOfTSURLInfo *array;


    if(NULL == soap || NULL == array_p)
        return ENOMEM;


    while(argc > start){
        curr->next = calloc(1,sizeof(struct list_of_filerequest));
        curr->next->value=NULL;
        curr=curr->next;
        start=parse_surl_options_Ls(start,argc,argv,soap,&curr->value);
        if(start>0)
            count++;
        else
            break;
    }
    size=sizeof(struct ns1__ArrayOfTSURLInfo);
    if(NULL == (array = soap_malloc(soap, size)))
       exit(error_usage(ENOMEM,"Unable to allocate memory"));
    memset(array,0,size);

    *array_p = array;

    array->__sizesurlInfoArray=count;
    size=sizeof(struct ns1__TSURLInfo*);
    if(NULL == (array->surlInfoArray = soap_malloc(soap, size * count)))
       exit(error_usage(ENOMEM,"Unable to allocate memory"));
    memset(array->surlInfoArray,0,size*count);

    curr = h->next;
    for(i=0;i<count && curr != NULL;i++){
        array->surlInfoArray[i] = curr->value;
        curr = curr->next;
    }
       
    return 0;
}

void fprintf_file_array(FILE *io,struct ns1__ArrayOfTMetaDataPathDetail *list, const char *prefix);

const char header[]=
"file storage type\n"
"|  file type            life   life                           last\n"
"| /                     time   time               creat        mod\n"
"|| perm.       owner  assign   left     size       time       time      path     (surl)\n"
"++----------------------------------------------------------------------------------------\n";

void fprintf_file_details(FILE *io,
                          struct ns1__TMetaDataPathDetail *file, 
                          const char *prefix)
{            
    fprintf(io,prefix);

    if(NULL == file->status){
        fprintf(io,"Error in file");
        if(NULL != file->path)
            fprintf(io," %s",file->path);
        if(NULL != file->originalSURL && NULL != file->originalSURL->value)
            fprintf(io," (%s)",file->originalSURL->value);
        fprintf(io,"\n");
    }
    if(file->status && file->status->statusCode != SRM_USCORESUCCESS &&
       file->status->statusCode != SRM_USCOREDONE)
        if(file->status->statusCode == SRM_USCOREINVALID_USCOREPATH)
            fprintf(io,"No such file or directory\n");
        else
            fprintf(io,"     %s\n",reconvertStatusCode(file->status->statusCode));
    else{
        switch(*file->type){
        case File:
            fprintf(io, "-");
            break;
        case Directory:
            fprintf(io, "d");
            break;
        case Link:
            fprintf(io, "l");
            break;
        default:
            fprintf(io, "?");
            break;
        }

        switch(*file->fileStorageType){
        case Volatile:
            fprintf(io,"V");
            break;
        case Durable:
            fprintf(io,"D");
            break;
        case Permanent:
            fprintf(io,"P");
            break;
        default:
            fprintf(io,"?");
            break;
        }

        if(NULL != file->ownerPermission){
            switch(file->ownerPermission->mode){
            case None:
                fprintf(io,"---" );
                break;
            case X:
                fprintf(io,"--x" );
                break;
            case W:
                fprintf(io,"-w-" );
                break;
            case WX:
                fprintf(io,"-wx" );
                break;
            case R:
                fprintf(io,"r--" );
                break;
            case RX:
                fprintf(io,"r-x" );
                break;
            case RW:
                fprintf(io,"rw-" );
                break;
            case RWX:
                fprintf(io,"rwx" );
                break;
            default:
                fprintf(io,"???" );
            }
        }

        if(NULL != file->ownerPermission){
            switch(file->ownerPermission->mode){
            case None:
                fprintf(io,"---");
                break;
            case X:
                fprintf(io,"--x");
                break;
            case W:
                fprintf(io,"-w-");
                break;
            case WX:
                fprintf(io,"-wx");
                break;
            case R:
                fprintf(io,"r--");
                break;
            case RX:
                fprintf(io,"r-x");
                break;
            case RW:
                fprintf(io,"rw-");
                break;
            case RWX:
                fprintf(io,"rwx");
                break;
            default:
                fprintf(io,"???");
            }
        }
        
        if(NULL != file->owner)
            fprintf(io, "%12s ", file->owner->value);
        else
            fprintf(io, "USER UNKNOWN ");
        if(NULL != file->lifetimeAssigned)
            fprintf(io, "%7lld ", file->lifetimeAssigned->value);
        if(NULL != file->lifetimeLeft)
            fprintf(io, "%7lld ", file->lifetimeLeft->value);
        if(NULL != file->size)
            fprintf(io, "%7lld ", file->size->value);

        if(NULL != file->createdAtTime)
            fprintf(io, "%10s ", file->createdAtTime->value);
        else
            fprintf(io, "1970-01-01 ");

        if(NULL != file->lastModificationTime)
            fprintf(io, "%10s ", file->lastModificationTime->value);
        else
            fprintf(io, "1970-01-01 ");


        if(NULL != file->path && NULL != file->originalSURL)
            fprintf(io, "%s    (%s)\n", file->path, file->originalSURL->value);
        else
            fprintf(io, "%s    (\?\?\?)\n",file->path);
    }

    if(NULL != file->subPaths){
        int size=strlen(prefix);
        char *pre=malloc(size+2);
        strcpy(pre,prefix);
        strcat(pre," ");
        fprintf_file_array(io,file->subPaths,pre);
    }
}

void fprintf_file_array(FILE *io,struct ns1__ArrayOfTMetaDataPathDetail *list, const char *prefix)
{
    int i;
    for(i=0; i < list->__sizepathDetailArray; i++)
        if(0 == verbose)
            printf("%s\n",list->pathDetailArray[i]->path);
        else
            fprintf_file_details(stdout, list->pathDetailArray[i], prefix);     
}


main(int argc, char **argv)
{
    struct soap soap;
    int flags;
    int ret;
    int size;
    struct ns1__srmLsRequest *request;
    struct ns1__srmLsResponse_ response_p;
    struct ns1__srmLsResponse *response;

    PROGNAME=argv[0];

    soap_init (&soap);

#ifdef GSI_PLUGINS
/* Delegate proxy ---- TO BE CHANGED */
/*     flags = CGSI_OPT_DISABLE_NAME_CHECK|CGSI_OPT_DELEG_FLAG; */

    flags = CGSI_OPT_DISABLE_NAME_CHECK;
    soap_register_plugin_arg (&soap, client_cgsi_plugin, &flags);
#endif
    size=sizeof(struct ns1__srmLsRequest);
    if(NULL == (request = soap_malloc(&soap,size))){
        fprintf(stderr,"Unable to malloc() memory\n");
        return -1;
    }
    memset(request,0,size);

    ret = parse_global_options_Ls(argc,argv,&soap,request);
    if(ret < 0)
        return -1;

    ret = parse_all_surl_option_Ls(ret,argc,argv,&soap,&(request->paths));


    if (soap_call_ns1__srmLs (&soap, endpoint, "Ls",
                                        request, &response_p)) {
        soap_print_fault (&soap, stderr);
        soap_print_fault_location (&soap, stderr);
        soap_end (&soap);
        exit (1);
    }

    response=response_p.srmLsResponse;
    struct ns1__TReturnStatus *status=response->returnStatus;


    if(status->statusCode != SRM_USCORESUCCESS &&
       status->statusCode != SRM_USCOREDONE){
           printf("Error: %s (%d)\n",reconvertStatusCode(status->statusCode),
                  status->statusCode);
           if(NULL != status->explanation)
           printf("  (%s)\n",status->explanation);
       }
    if(NULL == response->details){
        printf("Error! No surl returned (NULL-pointer)\n");
        exit(1);
    }
    if(1 == verbose)
        printf(header);
    fprintf_file_array(stdout,response->details,"");   
}
