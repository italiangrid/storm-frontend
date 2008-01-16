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

#include <getopt.h>

#include <string>
#include <vector>
#include <iostream>

using namespace std;

#ifdef STRICT_CHECKING
#  define EXIT_FOR_CHECK(x) exit(x)
#else
#  define EXIT_FOR_CHECK(x) {x; cerr <<"STRICT_CHECKING DISABLED! Continuing"<<endl;}
#endif

std::string PROGNAME;

char* errors[34] = {
 "SRM_SUCCESS",
 "SRM_FAILURE",
 "SRM_AUTHENTICATION_FAILURE",
 "SRM_AUTHORIZATION_FAILURE",
 "SRM_INVALID_REQUEST",
 "SRM_INVALID_PATH",
 "SRM_FILE_LIFETIME_EXPIRED",
 "SRM_SPACE_LIFETIME_EXPIRED",
 "SRM_EXCEED_ALLOCATION",
 "SRM_NO_USER_SPACE",
 "SRM_NO_FREE_SPACE",
 "SRM_DUPLICATION_ERROR",
 "SRM_NON_EMPTY_DIRECTORY",
 "SRM_TOO_MANY_RESULTS",
 "SRM_INTERNAL_ERROR",
 "SRM_FATAL_INTERNAL_ERROR",
 "SRM_NOT_SUPPORTED",
 "SRM_REQUEST_QUEUED",
 "SRM_REQUEST_INPROGRESS",
 "SRM_REQUEST_SUSPENDED",
 "SRM_ABORTED",
 "SRM_RELEASED",
 "SRM_FILE_PINNED",
 "SRM_FILE_IN_CACHE",
 "SRM_SPACE_AVAILABLE",
 "SRM_LOWER_SPACE_GRANTED",
 "SRM_DONE",
 "SRM_PARTIAL_SUCCESS",
 "SRM_REQUEST_TIMED_OUT",
 "SRM_LAST_COPY",
 "SRM_FILE_BUSY",
 "SRM_FILE_LOST",
 "SRM_FILE_UNAVAILABLE",
 "SRM_CUSTOM_STATUS"
 };

std::string srmerror(int x)
{
    if(x > sizeof(errors))
        return "!!!INVALID ERROR NUMBER!!!";
    else
        return errors[x];
}

void usage()
{
    cout << "Usage: "<<PROGNAME<<" endpoint request_token [-p] [ -s sourceSURL1 -t targetSURL1] [...]"<<endl;
    return;
}
#include <stdarg.h>

template<typename soap_type_t>
soap_type_t * soap_calloc(struct soap *soap)
{
    soap_type_t *ptr;
    if(NULL == (ptr = static_cast<soap_type_t*>(soap_malloc(soap, sizeof(soap_type_t)))))
        throw ENOMEM;
    memset(ptr, 0, sizeof(soap_type_t));
    return ptr;
}

/* Template function which allocate a pointer to an ARRAY of
 * soap_type_t lenghty size */
template<typename soap_type_t>
soap_type_t ** soap_calloc(struct soap *soap, const int size)
{
    soap_type_t **ptr;
    if(NULL == (ptr = static_cast<soap_type_t**>(soap_malloc(soap, size * sizeof(soap_type_t*)))))
        throw ENOMEM;
    memset(ptr, 0, size * sizeof(soap_type_t*));
    return ptr;
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

void print_status(ns1__srmStatusOfCopyRequestResponse_ &srep)
{
    ns1__TReturnStatus *s =  
        srep.srmStatusOfCopyRequestResponse->returnStatus;

    cout << "Global request status: "<< s->statusCode <<" ("<<srmerror(s->statusCode)<<")"<<endl;
    if(s->explanation)
        cout << "explanation: "<<s->explanation<< endl;
    ns1__ArrayOfTCopyRequestFileStatus *fs = srep.srmStatusOfCopyRequestResponse->arrayOfFileStatuses;  
    if(NULL != fs){
        cout << fs->__sizestatusArray << " SURLs:"<<endl<<endl;
        for(int i=0; i<fs->__sizestatusArray; ++i){
            ns1__TCopyRequestFileStatus *f = fs->statusArray[i];
            cout << "SOURCE SURL "<<i<<": " << f->sourceSURL<<endl;
            cout << "TARGET SURL "<<i<<": " << f->targetSURL<<endl;
            s = f->status;
            cout << "status: "<< s->statusCode <<" ("<<srmerror(s->statusCode)<<")"<<endl;
            if(s->explanation)
                cout << "explanation: "<<s->explanation<<endl;
            if(f->fileSize)
                cout << "filesize: "<< *f->fileSize<<endl;
            else
                cout << "filesize not supplied"<<endl;
            if(f->estimatedWaitTime)
                cout << "estimated waiting time: "<< *f->estimatedWaitTime<<endl;
            else
                cout << "estimated waiting time not supplied"<<endl;
            if(f->remainingFileLifetime)
                cout << "remaining file lifetime: "<< *f->remainingFileLifetime<<endl;
            else
                cout << "remaining file lifetime not supplied"<<endl;
            cout << endl;
        }
        
    }else
        cout << "no information about single SURLs."<<endl;

}
int main(int argc,char ** argv)
{
    struct soap soap;
    PROGNAME=argv[0];

    if(argc < 3){
        exit(error_usage(1, "Missing argument.\n"));
    }

    soap_init (&soap);
#ifdef GSI_PLUGINS
    int flags = CGSI_OPT_DISABLE_NAME_CHECK | CGSI_OPT_DELEG_FLAG;
    soap_register_plugin_arg (&soap, client_cgsi_plugin, &flags);
#endif

    struct ns1__srmStatusOfCopyRequestRequest sreq;
    
    int index=1;
    char *endpoint = argv[index++];
    sreq.requestToken=soap_strdup(&soap, argv[index++]);

    bool pollflag;
    if(index<argc && string("-p") == argv[index]){
        pollflag=true;
        index++;
    }
    vector<string> sources;
    vector<string> targets;
    for(int i=index; i+3<argc; i+=4){
        if(string("-s") == argv[i] && string("-t") == argv[i+2]){
            sources.push_back(argv[i+1]);
            targets.push_back(argv[i+3]);           
        }
    }

    cout << "Calling endpoint '"<< endpoint<<"' for r_token '"<<sreq.requestToken<<"'";
    if(pollflag)
        cout <<" (polling)"<<endl;
    else
        cout << endl;
    if(sources.size()!=0){
        if(sources.size() != targets.size())
            exit(error_usage(1, "num of target != num of sources"));
        cout << "Query ONLY following SURLs: "<<endl;
        for(int i=0; i<sources.size(); ++i){
            cout << "  src: "<<sources[i]<<endl;
            cout << "  dst: "<<targets[i]<<endl;
        }
           
        // Fill array struct
        sreq.arrayOfSourceSURLs = soap_calloc<ns1__ArrayOfAnyURI>(&soap);
        sreq.arrayOfTargetSURLs = soap_calloc<ns1__ArrayOfAnyURI>(&soap);
        sreq.arrayOfSourceSURLs->__sizeurlArray = sources.size();
        sreq.arrayOfTargetSURLs->__sizeurlArray = sreq.arrayOfSourceSURLs->__sizeurlArray;
    
        sreq.arrayOfSourceSURLs->urlArray = soap_calloc<char>(&soap, sources.size());
        sreq.arrayOfTargetSURLs->urlArray = soap_calloc<char>(&soap, sources.size());

        for(int i=0; i<sources.size(); ++i){
            sreq.arrayOfSourceSURLs->urlArray[i] = soap_strdup(&soap, sources[i].c_str());
            sreq.arrayOfTargetSURLs->urlArray[i] = soap_strdup(&soap, targets[i].c_str());
        }
    }
    
    ns1__TReturnStatus *st;
    do{
        struct ns1__srmStatusOfCopyRequestResponse_ srep;   
        if (soap_call_ns1__srmStatusOfCopyRequest (&soap, endpoint,
                                                   "StatusOfCopyRequest", &sreq, &srep)) 
            {
            soap_print_fault (&soap, stderr);
            soap_end (&soap);
            exit (1);
        }
        print_status(srep);
        st = srep.srmStatusOfCopyRequestResponse->returnStatus;

    } while (pollflag && (st->statusCode == SRM_USCOREREQUEST_USCOREQUEUED 
                          || st->statusCode == SRM_USCOREREQUEST_USCOREINPROGRESS 
                          || st->statusCode == SRM_USCOREREQUEST_USCORESUSPENDED));

        soap_end (&soap);
    exit (0);
}




#include "srmv2H.h"
