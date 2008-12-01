#define _GNU_SOURCE
#include <string.h>
#include <errno.h>
#include "srmv2H.h"

#include <unistd.h> /* need sysconf to know max hostname length */
#include "storm_util.h"


#include <ctype.h> /* need isprint, isalpha etc... */
extern int errno;

int convertPermission(char *mode)
{
    if (strcmp("None", mode) == 0) return NONE;
    if (strcmp("X", mode) == 0)    return X;
    if (strcmp("W", mode) == 0)    return W;
    if (strcmp("R", mode) == 0)    return R;
    if (strcmp("RX", mode) == 0)   return RX;
    if (strcmp("RW", mode) == 0)   return RW;
    if (strcmp("RWX", mode) == 0)  return RWX;
    return(NONE);
}

/** Converts the file storage type */
int convertFileStorageType(char *fstype)
{
    if (strcmp("Volatile_", fstype) == 0)  return VOLATILE;
    if (strcmp("Durable_", fstype) == 0)   return DURABLE;
    if (strcmp("Permanent_", fstype) == 0) return PERMANENT;
    if (strcmp("Unknown.", fstype) == 0)   return -1; 
    return -1;
}

int convertFileType(char* ftype)
{
    if (strcmp("File", ftype) == 0)      return FILE_;
    if (strcmp("Directory", ftype) == 0) return DIRECTORY;
    if (strcmp("Link", ftype) == 0)      return LINK;
    if (strcmp("Unknown.", ftype) == 0)  return -1;
    return -1;
}

/** Converts the file storage type */
int convertSpaceType(char *stype)
{
    if (strcmp("Volatile", stype) == 0)  return VOLATILE;
    if (strcmp("Durable", stype) == 0)   return DURABLE;
    if (strcmp("Permanent", stype) == 0) return PERMANENT;
    if (strcmp("Unknown.", stype) == 0)   return -1;
    return -1;
}
 
/* Converts the status code from char* format into a SRM status code */
int convertStatusCode(char*  code) 
{
    if (strcmp("SRM_SUCCESS", code) == 0)     return SRM_USCORESUCCESS; // 0
    else if (strcmp("SRM_FAILURE", code) == 0)     return SRM_USCOREFAILURE; // 1 
    else if (strcmp("SRM_AUTHENTICATION_FAILURE", code) == 0)     return SRM_USCOREAUTHENTICATION_USCOREFAILURE; // 2
    else if (strcmp("SRM_AUTHORIZATION_FAILURE", code) == 0)     return SRM_USCOREAUTHORIZATION_USCOREFAILURE; // 3
    else if (strcmp("SRM_INVALID_REQUEST", code) == 0)     return SRM_USCOREINVALID_USCOREREQUEST; // 4
    else if (strcmp("SRM_INVALID_PATH", code) == 0)     return SRM_USCOREINVALID_USCOREPATH; // 5
    else if (strcmp("SRM_FILE_LIFETIME_EXPIRED", code) == 0)     return SRM_USCOREFILE_USCORELIFETIME_USCOREEXPIRED; // 6
    else if (strcmp("SRM_SPACE_LIFETIME_EXPIRED", code) == 0)     return SRM_USCORESPACE_USCORELIFETIME_USCOREEXPIRED; // 7
    else if (strcmp("SRM_EXCEED_ALLOCATION", code) == 0)     return SRM_USCOREEXCEED_USCOREALLOCATION; // 8
    else if (strcmp("SRM_NO_USER_SPACE", code) == 0)     return SRM_USCORENO_USCOREUSER_USCORESPACE; // 9
    else if (strcmp("SRM_NO_FREE_SPACE", code) == 0)     return SRM_USCORENO_USCOREFREE_USCORESPACE; // 10
    else if (strcmp("SRM_DUPLICATION_ERROR", code) == 0)     return SRM_USCOREDUPLICATION_USCOREERROR; // 11
    else if (strcmp("SRM_NON_EMPTY_DIRECTORY", code) == 0)     return SRM_USCORENON_USCOREEMPTY_USCOREDIRECTORY; // 12
    else if (strcmp("SRM_TOO_MANY_RESULTS", code) == 0)     return SRM_USCORETOO_USCOREMANY_USCORERESULTS; // 13
    else if (strcmp("SRM_INTERNAL_ERROR", code) == 0)  return SRM_USCOREINTERNAL_USCOREERROR; // 14
    else if (strcmp("SRM_FATAL_INTERNAL_ERROR", code) == 0)     return SRM_USCOREFATAL_USCOREINTERNAL_USCOREERROR; // 15
    else if (strcmp("SRM_NOT_SUPPORTED", code) == 0)     return SRM_USCORENOT_USCORESUPPORTED; // 16
    else if (strcmp("SRM_REQUEST_QUEUED", code) == 0)     return SRM_USCOREREQUEST_USCOREQUEUED; // 17
    else if (strcmp("SRM_REQUEST_INPROGRESS", code) == 0)     return SRM_USCOREREQUEST_USCOREINPROGRESS; // 18
    else if (strcmp("SRM_REQUEST_SUSPENDED", code) == 0)     return SRM_USCOREREQUEST_USCORESUSPENDED; // 19
    else if (strcmp("SRM_ABORTED", code) == 0)     return SRM_USCOREABORTED; // 20
    else if (strcmp("SRM_RELEASED", code) == 0)     return SRM_USCORERELEASED; // 21
    else if (strcmp("SRM_FILE_PINNED", code) == 0)     return SRM_USCOREFILE_USCOREPINNED; // 22
    else if (strcmp("SRM_FILE_IN_CACHE", code) == 0)     return SRM_USCOREFILE_USCOREIN_USCORECACHE; // 23
    else if (strcmp("SRM_SPACE_AVAILABLE", code) == 0)     return SRM_USCORESPACE_USCOREAVAILABLE; // 24
    else if (strcmp("SRM_LOWER_SPACE_GRANTED", code) == 0)     return SRM_USCORELOWER_USCORESPACE_USCOREGRANTED; // 25
    else if (strcmp("SRM_DONE", code) == 0)        return SRM_USCOREDONE; // 26
    else if (strcmp("SRM_PARTIAL_SUCCESS", code) == 0) return SRM_USCOREPARTIAL_USCORESUCCESS; // 27
    else if (strcmp("SRM_REQUEST_TIMED_OUT", code) == 0) return SRM_USCOREREQUEST_USCORETIMED_USCOREOUT; // 28
    else if (strcmp("SRM_LAST_COPY", code) == 0) return SRM_USCORELAST_USCORECOPY; // 29
    else if (strcmp("SRM_FILE_BUSY", code) == 0) return SRM_USCOREFILE_USCOREBUSY; // 30
    else if (strcmp("SRM_FILE_LOST", code) == 0) return SRM_USCOREFILE_USCORELOST; // 31
    else if (strcmp("SRM_FILE_UNAVAILABLE", code) == 0) return SRM_USCOREFILE_USCOREUNAVAILABLE; // 32
    else return SRM_USCORECUSTOM_USCORESTATUS; // 33
}

char* sfnfromsurl (char *surl)
{
    char *p;
    
    if (strncmp (surl, "srm://", 6)) {
            errno = EINVAL;
            return (NULL);
    }
    if ((p = strstr (surl + 6, "?SFN="))) return (p + 5);
    
    if ((p = strchr (surl + 6, '/')) == NULL) {
            errno = EINVAL;
            return (NULL);
    }
    return (p);
}

char* reconvertStatusCode(int  code) 
{
    if (code == 0)  return "SRM_SUCCESS"; // 0
    if (code == 1)  return "SRM_FAILURE"; // 1 
    if (code == 2)  return "SRM_AUTHENTICATION_FAILURE"; // 2
    if (code == 3)  return "SRM_AUTHORIZATION_FAILURE"; // 3
    if (code == 4)  return "SRM_INVALID_REQUEST"; // 4
    if (code == 5)  return "SRM_INVALID_PATH"; // 5
    if (code == 6)  return "SRM_FILE_LIFETIME_EXPIRED"; // 6
    if (code == 7)  return "SRM_SPACE_LIFETIME_EXPIRED"; // 7
    if (code == 8)  return "SRM_EXCEED_ALLOCATION"; // 8
    if (code == 9)  return "SRM_NO_USER_SPACE"; // 9
    if (code == 10) return "SRM_NO_FREE_SPACE"; // 10
    if (code == 11) return "SRM_DUPLICATION_ERROR"; // 1
    if (code == 12) return "SRM_NON_EMPTY_DIRECTORY"; // 12
    if (code == 13) return "SRM_TOO_MANY_RESULTS"; // 13
    if (code == 14) return "SRM_INTERNAL_ERROR"; // 14
    if (code == 15) return "SRM_FATAL_INTERNAL_ERROR"; // 15
    if (code == 16) return "SRM_NOT_SUPPORTED"; // 16
    if (code == 17) return "SRM_REQUEST_QUEUED"; // 17
    if (code == 18) return "SRM_REQUEST_INPROGRESS"; // 18
    if (code == 19) return "SRM_REQUEST_SUSPENDED"; // 19
    if (code == 20) return "SRM_ABORTED"; // 20
    if (code == 21) return "SRM_RELEASED"; // 21
    if (code == 22) return "SRM_FILE_PINNED"; // 22
    if (code == 23) return "SRM_FILE_IN_CACHE"; // 23
    if (code == 24) return "SRM_SPACE_AVAILABLE"; // 24
    if (code == 25) return "SRM_LOWER_SPACE_GRANTED"; // 25
    if (code == 26) return "SRM_DONE"; // 26
    if (code == 27) return "SRM_PARTIAL_SUCCESS"; // 27
    if (code == 28) return "SRM_REQUEST_TIMED_OUT"; // 28
    if (code == 29) return "SRM_LAST_COPY"; // 29
    if (code == 30) return "SRM_FILE_BUSY"; // 30
    if (code == 31) return "SRM_FILE_LOST"; // 31
    if (code == 32) return "SRM_FILE_UNAVAILABLE"; // 32
    if (code == 33) return "SRM_CUSTOM_STATUS"; // 33
}


void printLine() {
    printf("======================================================\n"); 
}

void printChar() {
    printf("= ");
}


int parseSURL_getEndpoint (const char *surl, char **endpoint, char **sfn)
{
    int len;
    int lenp;
    char *p;
    char* dot;
    static char srm_ep[256];

    if (strncmp (surl, "srm://", 6)) {
        //Verify if SURL start with srm:// prefix
        printChar();
        printf("Error in parsing SURL: %s\n",surl);
        printChar();
        printf("SURL must start whit \"srm://\" prefix!\n");    
        errno = EINVAL;
        return (-1);
    }
    //Verify if SFN is in query mode
    if (p = strstr (surl + 6, "?SFN=")) {
        *sfn = p + 5;
    } else if (p = strchr (surl + 6, '/')) {
        //Verify if SFN contains "/" char
        *sfn = p;
    } else {
        //SFN specified not well formed! 
        printChar();
        printf("Error in parsing SURL: %s\n",surl);
        printChar();
        printf("SURL must be in the form: \"srm://host:port/site_file_name\"!\n");
        errno = EINVAL;
        return (-1);
    }

    //Verify if SURL dose not contains more than two :
    if(dot = strstr(surl + 6,":")) {
        if ((dot = strstr(dot+1 ,":"))) {
        
            printChar();
            printf("Error in parsing SURL: %s\n",surl);
            printChar();
            printf("SURL must be in the form: \"srm://host:port/site_file_name\"!\n");
            printChar();
            printf("To much \":\" char found.\n");
            errno = EINVAL;
            return (-1);            
        }
    } else {
        printChar();
        printf("Error in parsing SURL: %s\n",surl);
        printChar();
        printf("SURL must be in the form: \"srm://host:port/site_file_name\"!\n");
        printChar();
        printf("Port not found.\n");
        errno = EINVAL;
        return (-1);     
    }
    

#ifdef GSI_PLUGINS
    strcpy (srm_ep, "httpg://");
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
    //printf("ENDPOINT:%s",*endpoint);
    return (0);

}

//Used by SrmReserveSpace to parse Surl
int parseSURL_noSFN_getEndpoint (const char *surl, char **endpoint)
{
    int len;
    int lenp;
    const char *p;
    char* dot;
    char* slash;
    static char srm_ep[256];
    
    if (strncmp (surl, "srm://", 6)) {
        //Verify if SURL start with srm:// prefix
        printChar();
        printf("Error in parsing SURL: %s\n",surl);
        printChar();
        printf("SURL must start whit \"srm://\" prefix!\n");    
        errno = EINVAL;
        return (-1);
    }

    //Verify if SURL dose not contains more than two :
    if(dot = strstr(surl + 6,":")) {
        if ((dot = strstr(dot+1 ,":"))) {
        
            printChar();
            printf("Error in parsing SURL: %s\n",surl);
            printChar();
            printf("SURL must be in the form: \"srm://host:port\"!\n");
            printChar();
            printf("To much \":\" char found.\n");
            errno = EINVAL;
            return (-1);            
        }
    } else {
        printChar();
        printf("Error in parsing SURL: %s\n",surl);
        printChar();
        printf("SURL must be in the form: \"srm://host:port\"!\n");
        printChar();
        printf("Port not found.\n");
        errno = EINVAL;
        return (-1);     
    }
    //CHeck if SURL specified contains also SFN part, not needed in space reservation
    if (slash = strstr(surl+6,"/")) {
        printChar();
        printf("Error in parsing SURL: %s\n",surl);
        printChar();
        printf("SURL must be in the form: \"srm://host:port\"!\n");
        printChar();
        printf("site_file_name must not be specified.\n");
        errno = EINVAL;
        return (-1);     
    }


        

#ifdef GSI_PLUGINS
    strcpy (srm_ep, "httpg://");
    lenp = 8;
#else
    strcpy (srm_ep, "http://");
    lenp = 7;
#endif
    //End of String.
    p = surl + strlen(surl);    

    len = p - surl - 6;

    if (lenp + len >= sizeof(srm_ep)) {
        printChar();
        printf("Error Parsing SURL!Endpoint exceeded Size allowed! :%i\n",lenp+len);
        errno = EINVAL;
        return (-1);
    }

    strncpy (srm_ep + lenp, surl + 6, len);
    *(srm_ep + lenp + len) = '\0';
    if (strchr (srm_ep + lenp, '/') == NULL) {
        if (strlen (SRM_EP_PATH) + lenp + len >= sizeof(srm_ep)) {
            printChar();
            printf("Error Parsing SURL!\n");
            errno = EINVAL;
            return (-1);
        }
        strcat (srm_ep, SRM_EP_PATH);
    }
    
    *endpoint = srm_ep;
    //printf("ENDPOINT:%s",*endpoint);
    return (0);

}


int parseSURL(const char *surl)
{
    int len;
    int lenp;
    char *p;
    static char srm_ep[256];
    char* dot;
    if (strncmp (surl, "srm://", 6)) {
        //Verify if SURL start with srm:// prefix
        printChar();
        printf("Error in parsing SURL: %s\n",surl);
        printChar();
        printf("SURL must start whit \"srm://\" prefix!\n");    
        errno = EINVAL;
        return (-1);
    }
    //Verify if SFN is in query mode
    if (p = strstr (surl + 6, "?SFN=")) {
            ;//*sfn = p + 5;
    } else if (p = strchr (surl + 6, '/')) {
        //Verify if SFN contains "/" char
        ;//*sfn = p;
    } else {
        //SFN specified not well formed! 
        printChar();
        printf("Error in parsing SURL: %s\n",surl);
        printChar();
        printf("SURL must be in the form: \"srm://host:port/site_file_name\"!\n");
        errno = EINVAL;
        return (-1);
    }

    //Verify if SURL dose not contains more than two :
    if(dot = strstr(surl + 6,":")) {
            if ((dot = strstr(dot+1,":"))) {
            
                printChar();
                printf("Error in parsing SURL: %s\n",surl);
                printChar();
                printf("SURL must be in the form: \"srm://host:port/site_file_name\"!\n");
                printChar();
                printf("To much \":\" char found.\n");
                errno = EINVAL;
                return (-1);
            }           
    } else {
            printChar();
            printf("Error in parsing SURL: %s\n",surl);
            printChar();
            printf("SURL must be in the form: \"srm://host:port/site_file_name\"!\n");
            printChar();
            printf("Port not found.\n");
            errno = EINVAL;
            return (-1);     
    }
    
    if(strlen(surl)>SURL_MAXSIZE) {
        
            printChar();
            printf("Error in parsing SURL: %s\n",surl);
            printChar();
            printf("SURL too long!\n");
            errno = EINVAL;
            return (-1);     
    }

    return (0);

}

/* The check_* functions returns SURL_VALID (==0) in case of
 * success. In case of error returns a bitfield value != 0
 */

/* Check if the hostname is syntactically valid 
   return a bitfield with this possible bits:
   SURL_VALID in case of success,
   A bitfield containing:
   SURL_INVALID_HOSTNAME in case of wrong char in hostname
   SURL_EMPTY_HOSTNAME if the hostname is empty (i.e. hostname is a NULL pointer)
   SURL_INVALID_HOSTNAME can have other */
int check_dns(const char *hostname){
    /* check if hostname contains wrong char */
    /* cfr RFC 952 */
    int i;
    long hl;
    const char *p;
    if(NULL == hostname)
        return SURL_EMPTY_HOSTNAME;
    p=hostname;
    if(! isalpha(*p))
        return SURL_INVALID_HOSTNAME|SURL_INVALID_CHAR; /* hostname must begin with an alpha char */

    /* get host specific hostname max length. */
    hl=sysconf(_SC_HOST_NAME_MAX);
    if(-1 == hl)
        hl=255;

    /* check for invalid char */
    for(i=0; '\0' != *p; i++){
        if(i > hl)
            return SURL_INVALID_HOSTNAME|SURL_STRING_TOO_LONG; /* hostname too long */ 
        if(! isalnum(*p) &&  *p != '.' && *p != '-')
            return SURL_INVALID_HOSTNAME|SURL_INVALID_CHAR; /* invalid char in hostname */
        p++;
    }
    if(*(p-1) == '.') /* hostname must not end with a '.' */
        return SURL_INVALID_HOSTNAME|SURL_INVALID_CHAR;

    return SURL_VALID;
}
/* check if ip is a valid ip number.  */
int check_ip_number(const char *ip){
    int period=0;
    const char *p,*b,*e,*end;
    long n;

    if(NULL == ip)
        return SURL_EMPTY_HOSTNAME;
    p=ip;
    end=strchr(p,'\0');

    while(*p!='\0'){
        e=strchr(p,'.');
        if(NULL == e)
            e=end;
        else
            period++;
        for(b=p;b!=e;b++)
            if(!isdigit(*b))
                return SURL_INVALID_HOSTNAME|SURL_INVALID_CHAR;
        n=strtol(p,NULL,10);
        if(n < 0 || n > 255)
            return SURL_INVALID_HOSTNAME|SURL_STRING_TOO_LONG;
        p= (*e == '\0'? e: e+1);
    }
    if(period != 3)
        return SURL_INVALID_HOSTNAME|SURL_INVALID_CHAR; /* invalid number of '.' */
    
    return SURL_VALID;
}

int check_hostname(const char *hostname){
    int len;
    if(NULL == hostname)
        return SURL_EMPTY_HOSTNAME; /* missing hostname */
    len=strlen(hostname);
    if(len < 1)
        return SURL_INVALID_HOSTNAME|SURL_STRING_TOO_SHORT; /* hostname too short */

    /* if hostname begin with a digit, then assume it is an ip
     * number */
    if(isdigit(hostname[0]))
        return check_ip_number(hostname);
    else
        return check_dns(hostname);
}

int check_port(const char *port){
    const char *p;
    long pn;

    if(NULL == port)
        return SURL_EMPTY_PORT;
    p=port;
    while('\0' != *p)
        if(!isdigit(*(p++)))
            return SURL_INVALID_PORT|SURL_INVALID_CHAR;

    pn=strtol(port,NULL,10);
    if(pn <= 0 || pn >= MAX_PORT_NUMBER)
        return SURL_INVALID_PORT | SURL_STRING_TOO_LONG;

    return SURL_VALID;
}

int check_sfn(const char *sfn){
    const char *p;
    if(NULL == sfn)
        return SURL_EMPTY_SFN;
    
    p=sfn;
    /* check for invalid char */
    while('\0' != *p){
        if(! isprint(*p))
            return SURL_INVALID_SFN|SURL_INVALID_CHAR; /* invalid char in hostname */
        p++;
    }

    return SURL_VALID;
}

/* 

  This function split *surl into host, port, endpoint_path and
  sfn. The function allocate dynamically (using malloc()) necessary
  space, so caller function must free() all the char pointer
  passed. 

  The function expects a surl in the form:

     <proto>://[/]*<host>[:<port>][[<soap_endpoint>?SFN=]<sfn>

  The funciton return SURL_VALID in case of success. 1 in case of error. Errors may be:
  
  - missing protocol
  - protocol not followed by '://'
  - missing host
  - missing protocol
  - unable to allocate memory
*/
int split_surl(const char *surl, 
               char **proto, 
               char **host, 
               char **port, 
               char **endpoint_path, 
               char **sfn){
    int i;
    char *b;
    char *e;
    char *ep_begin=NULL,*ep_end=NULL;


    b=strchr(surl,':');
    if(NULL == b || b == surl)
        return SURL_INVALID_PROTOCOL; /* missing or wrong protocol specification */

    if(NULL != proto){
        *proto=strndup(surl,b-surl);
        if(NULL == *proto)
            return SURL_ENOMEM; /* unable to allocate memory */
    }
    
    if(*(b++)!=':' || *(b++)!= '/' || *(b++)!= '/')
        return SURL_INVALID_PROTOCOL; /* protocol not followed by '://' */
    
    while('/' == *b)
        b++; 
    /* b points to first not-slash after protocol */
    
    e=b;
    i=0;
    while('\0' != *b){
        if(':' == *b ||
           '/' == *b ||
           '?' == *b){
            break;
        }
        b++;
    }

    /* b point to first  char after the hostname */
    if(b-e<2)
        return SURL_EMPTY_HOSTNAME; /* hostname missing */

    ep_begin=e; /* set endpoint begin char to host */
    /* copy hostname in *host, if host is not NULL */
    if(NULL != host){
        *host=strndup(e,b-e);
        if(NULL == *host)
            return SURL_ENOMEM; /* unable to allocate memory */
    }
    if('\0' == *(b++))
        return SURL_VALID;

    /* fill port string, if present */
    e=b;
    if(':' == *(b-1)){ 
        while('\0' != *b){
            if('/' == *b ||
               '?' == *b){
                break;
            }
            b++;
        }
        if('\0' == *b)
            if(e == b)
                return SURL_INVALID; /*  invalid surl: only a colon after hostname */
        if(NULL != port){
            *port=strndup(e,b-e);
            if(NULL == *port)
                return SURL_ENOMEM; /* unable to allocate memory */
        }       
    }
    
    /* skip multiple slash before soap_endpoint_path, if any, but
     * preserve the last one */
    while('/' == *b && '/' == *(b+1))
        b++;

    e=strchr(b,'?');
    if(NULL != e){ /* fill endpoint */

        if(0 != strncmp(e,"?SFN=",5))
            return SURL_INVALID_SFN; /* invalid SFN */
        if(NULL != endpoint_path) /* set end char of endpoint */
            ep_end=e;
        
        e+=5; /* e point to first char after '?SFN=' */
        b=e;
    }else{
        ep_end=b;
    }

    /* b and e points to first sfn char */
    if(NULL != sfn){
        *sfn=strdup(b);
        if(NULL == *sfn)
            return SURL_ENOMEM; /* unable to allocate memory */
    }

    /* Fill *endpoint_path ep_begin and ep_end point to the begin and
       to the end of the host[+path] of the endpoint. We must add only
       the prefix... */
    
    if(NULL != endpoint_path && NULL != ep_begin && NULL != ep_end){
        int len=ep_end-ep_begin;
        int lenp;
        int lenpre=strlen(SRM_EP_PATH);
#ifdef GSI_PLUGINS
        const char *prefix="httpg://";
        lenp=8;
#else
        const char *prefix="http://";
        lenp=7;
#endif

        *endpoint_path = malloc(len+lenp+lenpre+1);
        if(NULL == *endpoint_path)
            return SURL_ENOMEM; /* unable to allocate memory */

        strncpy(*endpoint_path,prefix,lenp);
        
        strncpy(*endpoint_path+lenp,ep_begin,len);
        strncpy(*endpoint_path+lenp+len,SRM_EP_PATH,lenpre);
        (*endpoint_path)[len+lenp+lenpre]='\0';        
    }
    return SURL_VALID;
}


/*** Get chain ***/
STACK_OF(X509) *load_chain(const char *certfile)
{
  STACK_OF(X509_INFO) *sk=NULL;
  STACK_OF(X509) *stack=NULL, *ret=NULL;
  BIO *in=NULL;
  X509_INFO *xi;
  int first = 1;

  if(!(stack = sk_X509_new_null())) {
    printf("memory allocation failure\n");
    goto end;
  }

  if(!(in=BIO_new_file(certfile, "r"))) {
    printf("error opening the file, %s\n",certfile);
    goto end;
  }

  /* This loads from a file, a stack of x509/crl/pkey sets */
  if(!(sk=PEM_X509_INFO_read_bio(in,NULL,NULL,NULL))) {
    printf("error reading the file, %s\n",certfile);
    goto end;
  }

  /* scan over it and pull out the certs */
  while (sk_X509_INFO_num(sk)) {
    /* skip first cert */
    if (first) {
      first = 0;
      continue;
    }
    xi=sk_X509_INFO_shift(sk);
    if (xi->x509 != NULL) {
      sk_X509_push(stack,xi->x509);
      xi->x509=NULL;
    }
    X509_INFO_free(xi);
  }
  if(!sk_X509_num(stack)) {
    printf("no certificates in file, %s\n",certfile);
    sk_X509_free(stack);
    goto end;
  }
  ret=stack;
end:
  BIO_free(in);
  sk_X509_INFO_free(sk);
  return(ret);
}