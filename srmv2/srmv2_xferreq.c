/*
 * $Id$
 */

#include <string.h>
#include <sys/types.h>
#include <uuid/uuid.h>
#include "Cnetdb.h"
#include "storm_functions.h"
#include "serrno.h"
#include "srm_server.h"
#include "srmv2H.h"
#include "storm_util.h"
#include "srmlogit.h"

#include "xmlrpc_decode.h"
#include "xmlrpc_encode.h"

#include <cgsi_plugin.h>

extern char db_pwd[33];
extern char db_srvr[33];
extern char db_user[33];
extern char localdomain[ST_MAXHOSTNAMELEN+1];
extern int nb_supported_protocols;
extern char **supported_protocols;

/*          Data Transfer Functions             */

static int marshall_CPR(struct soap *soap, struct ns1__TCopyRequestFileStatus *repfilep, struct storm_copy_filereq *cpr_entry)
{
    
    if (repfilep->fileSize = soap_malloc(soap, sizeof(unsignedLong64)))
        *(repfilep->fileSize) = cpr_entry->actual_size;
    
    if ((repfilep->sourceSURL = soap_strdup(soap, cpr_entry->from_surl)) == NULL)
        return(-1);
    
    if (repfilep->remainingFileLifetime = soap_malloc(soap, sizeof(int))) {
        if (cpr_entry->lifetime)
            *(repfilep->remainingFileLifetime) = *(cpr_entry->lifetime);
        else
            *(repfilep->remainingFileLifetime) = 0;
    }
            
    repfilep->status->statusCode = cpr_entry->status;
    
    if (*cpr_entry->errstring)
        repfilep->status->explanation = soap_strdup(soap, cpr_entry->errstring);
    
    if ((repfilep->targetSURL = soap_strdup(soap, cpr_entry->to_surl)) == NULL)
        return(-1);
    
    return (0);
}

static int marshall_GFR (struct soap *soap, struct ns1__TGetRequestFileStatus *repfilep, struct storm_get_filereq *gfr_entry, char *protocol)
{
    char *p;
    char turl[ST_MAXSFNLEN+1];
    
    if (NULL == repfilep || NULL == gfr_entry)
        return -1;

    if ((repfilep->fileSize = soap_malloc(soap, sizeof(unsignedLong64)))) 
        *(repfilep->fileSize) = gfr_entry->actual_size;
        
    if ((repfilep->sourceSURL = soap_strdup(soap, gfr_entry->from_surl)) == NULL)
        return(-1);
    
    if (repfilep->remainingPinTime = soap_malloc(soap, sizeof(int))) {
        if (gfr_entry->lifetime)
            *(repfilep->remainingPinTime) = *(gfr_entry->lifetime);
        else
            *(repfilep->remainingPinTime) = 0;
    }
    
    repfilep->status->statusCode = gfr_entry->status;
    
    if (*gfr_entry->errstring)
        repfilep->status->explanation = soap_strdup(soap, gfr_entry->errstring);
    
    if (*gfr_entry->pfn) {
        sprintf(turl,"%s",gfr_entry->pfn);
        repfilep->transferURL = soap_strdup(soap, turl);
    }
    
    return (0);
}

static int marshall_PFR(struct soap *soap, struct ns1__TPutRequestFileStatus *repfilep,
                 struct storm_put_filereq *pfr_entry, char *protocol)
{
    char *p;
    char turl[ST_MAXSFNLEN+1];

    if (repfilep->fileSize = soap_malloc(soap, sizeof(unsignedLong64)))
        *(repfilep->fileSize) = pfr_entry->actual_size;

    if ((repfilep->SURL = soap_strdup(soap, pfr_entry->to_surl)) == NULL)
        return(-1);
    
    if (repfilep->remainingPinLifetime = soap_malloc(soap, sizeof(int))) {
        if (pfr_entry->lifetime)
            *(repfilep->remainingPinLifetime) = *(pfr_entry->lifetime);
        else
            *(repfilep->remainingPinLifetime) = 0;
    }
    
    /*
     * @TODO FileLifetime, for the moment is equal to PinLifeTime
     */
     
    if (repfilep->remainingFileLifetime = soap_malloc(soap, sizeof(int))) {
        if (pfr_entry->lifetime)
            *(repfilep->remainingFileLifetime) = *(pfr_entry->lifetime);
        else
            *(repfilep->remainingFileLifetime) = 0;
    }
    
    
    repfilep->status->statusCode = pfr_entry->status;
    
    if (*pfr_entry->errstring)
        repfilep->status->explanation = soap_strdup(soap, pfr_entry->errstring);
    
    if (*pfr_entry->pfn) {
        sprintf(turl, "%s", pfr_entry->pfn);
        repfilep->transferURL = soap_strdup(soap, turl);
    }
    
    /*
     * @TODO ArrayOfTextraInfo
     */
    repfilep->transferProtocolInfo = NULL;
    
    return (0);
}



static int storm_req_fill_filetype(int ftype, struct storm_req *request)
{
    switch(ftype){
    case VOLATILE:
        request->f_type = DB_FILE_TYPE_VOLATILE;
        break;
    case DURABLE:
        request->f_type = DB_FILE_TYPE_DURABLE;
        break;
    case PERMANENT:
        request->f_type = DB_FILE_TYPE_PERMANENT;
        break;
    default:
        return EINVAL;
    }
    return 0;
}

/* Fill the storm_req struct "request" with protocols read from the
 * ns1__TTransferParameters struct.  In case of error, then the
 * function try to free() the memory already allocated.  To free() the
 * request->protocols array, please, call free_storm_req_protocols().
 */
static int storm_req_fill_transfer_params(const struct ns1__TTransferParameters *tparam, struct storm_req *request)
{
    int size=0, i=0;
    if(NULL == request || 
       NULL == tparam)
        return EINVAL;

    /* Fill Access Pattern */
    if(NULL != tparam->accessPattern){
        switch(*tparam->accessPattern){
        case TRANSFER_USCOREMODE:
            request->conn_type = DB_ACCESS_PATTERN_TRANSFER;
            break;
        case PROCESSING_USCOREMODE:
            request->conn_type = DB_ACCESS_PATTERN_PROCESS;
            break;
        default:
            return EINVAL;
        }
    }else
        request->conn_type = DB_ACCESS_PATTERN_UNKNOWN;

    /* Fill Connection Type */
    if(NULL != tparam->connectionType){
        switch(*tparam->connectionType){
        case WAN:
            request->conn_type = DB_CONN_TYPE_WAN;
            break;
        case LAN:
            request->conn_type = DB_CONN_TYPE_LAN;
            break;
        default:
            return EINVAL;
        }
    }else
        request->conn_type = DB_CONN_TYPE_UNKNOWN;

    /* Fill Array of Client Networks NOT IMPLEMENTED*/
    request->client_networks = NULL;

    /* Fill Protocol List */
    if(NULL == tparam->arrayOfTransferProtocols ||
       0 >= tparam->arrayOfTransferProtocols->__sizestringArray)
        return EINVAL;
    size = tparam->arrayOfTransferProtocols->__sizestringArray;
    request->protocols = calloc(size+1, sizeof(char *)); /* size+1 because this is a null-terminated array */
    if(NULL == request->protocols)
        return ENOMEM;

    for(i = 0; i< size; i++){
        request->protocols[i] = strdup(tparam->arrayOfTransferProtocols->stringArray[i]);
        if(NULL == request->protocols[i]){
            free_storm_req_transfer_params(request);
            free(request->protocols);
            return ENOMEM;
        }
    }

    request->nr_waiting = request->nbreqfiles;
    return 0;
}


/* This function return:
   0        Success
   EINVAL   invalid argument
   -1       invalid protocol list
   -2       invalid file type

    IGNORED char*                                authorizationID                0:1;//nillable
    ???IGNORED struct ns1__ArrayOfTGetFileRequest*  arrayOfFileRequests           ;
    char*                                userRequestDescription         0:1;//nillable
    IGNORED struct ns1__ArrayOfTExtraInfo*       storageSystemInfo              0:1;//nillable
    enum ns1__TFileStorageType*          desiredFileStorageType         0:1;//nillable
    IGNORED int*                                 desiredTotalRequestTime        0:1;//nillable
    int*                                 desiredPinLifeTime             0:1;//nillable
    IGNORED char*                                targetSpaceToken               0:1;//nillable
    IGNORED struct ns1__TRetentionPolicyInfo*    targetFileRetentionPolicyInfo  0:1;//nillable
    struct ns1__TTransferParameters*     transferParameters             0:1;//nillable

*/ 
static int storm_req_fill_from_get(struct ns1__srmPrepareToGetRequest *req, 
                                   struct storm_req *request)
{
    int ret;
    if(NULL == req 
       || NULL == request 
       || NULL == req->arrayOfFileRequests)
        return EINVAL;

    strncpy(request->r_type, DB_GET_REQUEST, sizeof(request->r_type)-1);

    /* Fill protocols */
    if(NULL == req->transferParameters)
        request->protocols = NULL;
    else{
        ret = storm_req_fill_transfer_params(req->transferParameters, request);
        if(0 != ret)
            return -1;
    }
    
    if(NULL != req->userRequestDescription) {
        // 'request->u_token' will have the correct syntax for the MYSQL query. 
        strncpy(&(request->u_token[1]), req->userRequestDescription, sizeof(request->u_token)-3);
        request->u_token[0] = '\'';
        request->u_token[strlen(request->u_token)] = '\'';
    }

    /* File Type */
    if(NULL != req->desiredFileStorageType){
        if(storm_req_fill_filetype(*req->desiredFileStorageType, request))
            return -2;
    }else
        request->f_type = DB_FILE_TYPE_UNKNOWN;

    /* desiredTotalRequestTime NOT IMPLEMENTED */
    request->retrytime = -1;

    /* desiredPinLifeTime */
    if(NULL != req->desiredPinLifeTime)
        request->pinlifetime = *req->desiredPinLifeTime;
    else
        request->pinlifetime = -1;
        
    /* targetSpaceToken NOT IMPLEMENTED */
    /* targetFileRetentionPolicyInfo NOT IMPLEMENTED */

    request->nbreqfiles = req->arrayOfFileRequests->__sizerequestArray;
    
    /* Fill other parameters with dummy value*/   
    request->status = SRM_USCOREREQUEST_USCOREQUEUED;
    memset(request->errstring, 0, sizeof(request->errstring));
    request->nr_completed = 0;
    request->nr_failed = 0;
    return 0;
}


static int gfr_entry_fill(struct ns1__TGetFileRequest *file, struct storm_get_filereq *gfr)
{
    if(NULL == gfr || NULL == file)
        return EINVAL;
    int i=0;
    strncpy(gfr->surl, file->sourceSURL, ST_MAXSFNLEN);

    gfr->status = SRM_USCOREREQUEST_USCOREQUEUED;

    if(NULL == file->dirOption)
        gfr->diroption = NULL;
    else{ /* fill DirOption fields */
        gfr->diroption = calloc(1, sizeof(struct storm_diroption));
        if(NULL == gfr->diroption)
            return ENOMEM;

        /* SourceADirectory */
        gfr->diroption->srcdir = file->dirOption->isSourceADirectory;

        /* AllLevelRecursive */
        if(NULL != file->dirOption->allLevelRecursive)
            gfr->diroption->recursive = *file->dirOption->allLevelRecursive;
        else
            gfr->diroption->recursive = -1;

        /* numOfLevels */
        if(NULL != file->dirOption->numOfLevels)
            gfr->diroption->numlevel = *file->dirOption->numOfLevels;
        else
            gfr->diroption->numlevel = -1;      
    }
    return 0;
}

/* This function allocate an array of struct storm_get_filereq. Please, call
 * gfr_entry_array_free to free() the memory allocated by this
 * function. 
 * 
 * This function returns:
 *    EINVAL  => invalid argument
 *    ENOMEM  => unable to allocate memory
 */
static int gfr_entry_array_fill(struct ns1__srmPrepareToGetRequest *req, 
                          struct storm_get_filereq ***gfr_array_ptr)
{
    int i;
    int size;
    struct storm_get_filereq **gfr_array;
    if(NULL == req ||
       NULL == req->arrayOfFileRequests ||
       NULL == gfr_array_ptr)
        return EINVAL;
    
    size = req->arrayOfFileRequests->__sizerequestArray;
    /* allocating memory for the array */
    *gfr_array_ptr =
        calloc(size+1, sizeof(struct storm_get_filereq *));  /* gfr_array will be NULL-terminated */
    if(NULL == *gfr_array_ptr)
        return ENOMEM;

    /* allocating and filling the members of the array */
    gfr_array = *gfr_array_ptr;
    for(i=0; i<size; i++){
        gfr_array[i] = calloc(req->arrayOfFileRequests->__sizerequestArray, sizeof(struct storm_get_filereq )); 
        if(NULL == gfr_array[i]){
            free_gfr_entry_array(*gfr_array_ptr);
            return ENOMEM;
        }
        
        if(0 != gfr_entry_fill(req->arrayOfFileRequests->requestArray[i], gfr_array[i])){
            free_gfr_entry_array(*gfr_array_ptr);
            return EINVAL;
        }
    }
    return 0;
}

static int ptgstatus_fill_from_getreq(struct soap *soap,
                                      struct ns1__srmStatusOfGetRequestResponse *rep,
                                      struct storm_get_filereq **gfr_array)
{
    

    return 0;
}

static int storm_req_fill_from_put()
{
    return 0;
}

static int storm_req_fill_from_copy()
{
    return 0;
}

/***************************************************************************************/
/*************************         SRM v2.2 ReleaseFiles       *************************/
/***************************************************************************************/

/* Data structure needed to give input parameters to the response handler function */
struct RPC_ResponseHandlerInput_ReleaseFiles {
    struct soap *soap;
    struct ns1__srmReleaseFilesRequest *req;
    struct ns1__srmReleaseFilesResponse *repp;
    int RPCTerminated;
};

/* Response handler for the RPC asynchronous call of the ReleaseFiles function */
void rpcResponseHandler_ReleaseFiles(const char          *serverUrl, 
                                     const char          *method_name, 
                                     const xmlrpc_value  *param_array, 
                                     const void          *user_data, 
                                     const xmlrpc_env    *faultP, 
                                     xmlrpc_value        *result)
{
    static const char *func = "rpcResponseHandler_ReleaseFiles";
    struct RPC_ResponseHandlerInput_ReleaseFiles *input;
    struct soap *soap;
    struct ns1__srmReleaseFilesRequest *req;
    struct ns1__srmReleaseFilesResponse *repp;
    int error;
    xmlrpc_env env;
    
    input = (struct RPC_ResponseHandlerInput_ReleaseFiles *) user_data;
    soap = input->soap;
    req = input->req;
    repp = input->repp;
    
    if (faultP->fault_occurred) {
        srmlogit(STORM_LOG_ERROR, func, "ERROR: XML-RPC Fault: %s (code: %d)\n", faultP->fault_string, faultP->fault_code);
        repp->returnStatus->explanation = soap_strdup(soap, faultP->fault_string);
        repp->returnStatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
        input->RPCTerminated = 1;
        return;
    }
    /* Initialize the error-handling environment. */
    xmlrpc_env_init(&env);
    
    /** MANDATORY ************ (1) Decode returnStatus (in TReturnStatus *) *************************/
    error = decode_globalTReturnStatus(func, &env, soap, repp->returnStatus, result);
    if (error != 0) {
        repp->returnStatus->explanation = "Error: unable to parse the BE response or empty response";
        repp->returnStatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
        xmlrpc_env_clean(&env);
        input->RPCTerminated = 1;
        return;
    }
    
    /** OPTIONAL ************* (2) Decode returnStatus (in TReturnStatus *) *************************/
    error = decode_ArrayOfTSURLReturnStatus(func, &env, soap, &(repp->arrayOfFileStatuses), SRM_PARAM_arrayOfFileStatuses, result);
    if (error != 0) {
        if (error != DECODE_ERR_NOT_FOUND) {
            repp->returnStatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
            if (error == DECODE_ERR_SOAP_MEMORY_ERROR) {
                repp->returnStatus->explanation = "Memory allocation error";
                input->RPCTerminated = 2;
            } else {
                repp->returnStatus->explanation = "Internal error while parsing the BE response";
                input->RPCTerminated = 1;
            }
            xmlrpc_env_clean(&env);
            return;
        }
    }
    
    srmlogit(STORM_LOG_DEBUG, func, "Response parsed!\n");
    
    xmlrpc_env_clean(&env);
    input->RPCTerminated = 1;
    return;
}

int ns1__srmReleaseFiles (struct soap *soap,
                          struct ns1__srmReleaseFilesRequest *req,
                          struct ns1__srmReleaseFilesResponse_ *rep)
{
    static const char *func = "ReleaseFiles";
    static const char *methodName = "synchcall.releaseFiles";
    struct ns1__srmReleaseFilesResponse *repp;
    struct RPC_ResponseHandlerInput_ReleaseFiles ReleaseFilesResponseHandlerInput;
    int error;
    xmlrpc_env env;
    xmlrpc_value *inputParam;
    
    /************************ Allocate response structure *******************************/
    if ((repp = soap_malloc(soap, sizeof(struct ns1__srmReleaseFilesResponse))) == NULL)
        return (SOAP_EOM);
    /* Initialize the other filed of the repp structure */
    repp->arrayOfFileStatuses = NULL;
    
    if ((repp->returnStatus = soap_malloc(soap, sizeof(struct ns1__TReturnStatus))) == NULL)
        return (SOAP_EOM);
    
    /* Assign the repp response structure to the output parameter rep */
    rep->srmReleaseFilesResponse = repp;
    
    /* Initialize xmlrpc error-handling environment. */
    xmlrpc_env_init(&env);

    /*Initialize xmlrpc input structure*/
    inputParam = xmlrpc_struct_new(&env);
    
    /**************************** Encode VOMS attibutes ***************************/
    error = encode_VOMSAttributes(func, &env, soap, req->authorizationID, inputParam);
    if (error) {
        repp->returnStatus->explanation = "Error encoding VOMS attributes"; 
        repp->returnStatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
        xmlrpc_DECREF(inputParam);
        xmlrpc_env_clean(&env);
        return(SOAP_OK);
    }
    
    /******************************************************************************/
    /********        Encode parameters for ReleaseFiles   (SRM v2.2)       ********/  
    /******************************************************************************/
    
    /** OPTIONAL ************ (1) Encode authorizationID (char *) ****************************/
    error = encode_string(func, &env, req->authorizationID, SRM_PARAM_authorizationID, inputParam);
    if (error) {
        if (error != ENCODE_ERR_MISSING_PARAM) {
            repp->returnStatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
            repp->returnStatus->explanation = "Error encoding authorizationID";
            xmlrpc_DECREF(inputParam);
            xmlrpc_env_clean(&env);
            return(SOAP_OK);
        }
        xmlrpc_env_clean(&env);
        xmlrpc_env_init(&env);
    }
    
    /** OPTIONAL ************ (2) Encode requestToken (char *) ********************************/
    error = encode_string(func, &env, req->requestToken, SRM_PARAM_requestToken, inputParam);
    if (error) {
        if (error != ENCODE_ERR_MISSING_PARAM) {
            repp->returnStatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
            repp->returnStatus->explanation = "Error encoding requestToken parameter";
            xmlrpc_DECREF(inputParam);
            xmlrpc_env_clean(&env);
            return(SOAP_OK);
        }
        xmlrpc_env_clean(&env);
        xmlrpc_env_init(&env);
    }
    
    /** OPTIONAL *********** (3) Encode arrayOfSURLs (ns1_ArrayOfAnyURI *) ********************************/
    error = encode_ArrayOfAnyURI(func, &env, req->arrayOfSURLs, SRM_PARAM_arrayOfSURLs, inputParam);
    if (error) {
        if (error != ENCODE_ERR_MISSING_PARAM) {
            repp->returnStatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
            repp->returnStatus->explanation = "Error encoding arrayOfSURLs parameter";
            xmlrpc_DECREF(inputParam);
            xmlrpc_env_clean(&env);
            return(SOAP_OK);
        }
        xmlrpc_env_clean(&env);
        xmlrpc_env_init(&env);
    }
    
    /** OPTIONAL ************ (4) Encode doRemove    ***************************************/
    error = encode_bool(func, &env, req->doRemove, "recursive", inputParam);
    if (error) {
        if (error != ENCODE_ERR_MISSING_PARAM) {
            repp->returnStatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
            repp->returnStatus->explanation = "Error encoding doRemove";
            xmlrpc_DECREF(inputParam);
            xmlrpc_env_clean(&env);
            return(SOAP_OK);
        }
        xmlrpc_env_clean(&env);
        xmlrpc_env_init(&env);
    }
    
    /* Clean up xmlrpc error-handling environment. */
    xmlrpc_env_clean(&env);

    /* Define the structure to give as input to the RPC response handler */
    ReleaseFilesResponseHandlerInput.soap = soap;
    ReleaseFilesResponseHandlerInput.req  = req;
    ReleaseFilesResponseHandlerInput.repp = repp;
    ReleaseFilesResponseHandlerInput.RPCTerminated = 0;
                                                                        
    /* Make remote procedure call, i.e. call Backend server */
    xmlrpc_client_call_asynch(xmlrpc_endpoint, methodName, (void *) &rpcResponseHandler_ReleaseFiles,
                              &ReleaseFilesResponseHandlerInput, "(S)", inputParam);
                              
    srmlogit(STORM_LOG_DEBUG, func, "Asynchronous RPC started.\n");

    while (ReleaseFilesResponseHandlerInput.RPCTerminated == 0) 
        xmlrpc_client_event_loop_finish_asynch_timeout(RPC_ASYNCH_TIMEOUT);
    
    xmlrpc_DECREF(inputParam);

    if (ReleaseFilesResponseHandlerInput.RPCTerminated == 2) {
    	srmlogit(STORM_LOG_ERROR, func, "Request done. Error: out of memory.\n");
        return(SOAP_EOM);
    }
    
    srmlogit(STORM_LOG_INFO, func, "Request done. Status: %s\n", reconvertStatusCode(repp->returnStatus->statusCode));
    
    return(SOAP_OK);
}

/***************************************************************************************/
/*************************         SRM v2.2 PutDone            *************************/
/***************************************************************************************/

/* Data structure needed to give input parameters to the response handler function */
struct RPC_ResponseHandlerInput_PutDone {
    struct soap *soap;
    struct ns1__srmPutDoneRequest *req;
    struct ns1__srmPutDoneResponse *repp;
    int RPCTerminated;
};

/* Response handler for the RPC asynchronous call of the PutDone function */
void rpcResponseHandler_PutDone(const char          *serverUrl, 
                                const char          *method_name, 
                                const xmlrpc_value  *param_array, 
                                const void          *user_data, 
                                const xmlrpc_env    *faultP, 
                                xmlrpc_value        *result)
{
    static const char *func = "rpcResponseHandler_PutDone";
    struct RPC_ResponseHandlerInput_PutDone *input;
    struct soap *soap;
    struct ns1__srmPutDoneRequest *req;
    struct ns1__srmPutDoneResponse *repp;
    int error;
    xmlrpc_env env;
    
    input = (struct RPC_ResponseHandlerInput_PutDone *) user_data;
    soap = input->soap;
    req = input->req;
    repp = input->repp;
    
    if (faultP->fault_occurred) {
        srmlogit(STORM_LOG_ERROR, func, "ERROR: XML-RPC Fault: %s (code: %d)\n", faultP->fault_string, faultP->fault_code);
        repp->returnStatus->explanation = soap_strdup(soap, faultP->fault_string);
        repp->returnStatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
        input->RPCTerminated = 1;
        return;
    }
    /* Initialize the error-handling environment. */
    xmlrpc_env_init(&env);
    
    /** MANDATORY ************ (1) Decode returnStatus (in TReturnStatus *) *************************/
    error = decode_globalTReturnStatus(func, &env, soap, repp->returnStatus, result);
    if (error != 0) {
        repp->returnStatus->explanation = "Error: unable to parse the BE response or empty response";
        repp->returnStatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
        xmlrpc_env_clean(&env);
        input->RPCTerminated = 1;
        return;
    }
    
    /** OPTIONAL ************* (2) Decode arrayOfFileStatuses (in ArrayOfTSURLReturnStatus *) *********/
    error = decode_ArrayOfTSURLReturnStatus(func, &env, soap, &(repp->arrayOfFileStatuses), SRM_PARAM_arrayOfFileStatuses, result);
    if (error != 0) {
        if (error != DECODE_ERR_NOT_FOUND) {
            repp->returnStatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
            if (error == DECODE_ERR_SOAP_MEMORY_ERROR) {
                repp->returnStatus->explanation = "Memory allocation error";
                input->RPCTerminated = 2;
            } else {
                repp->returnStatus->explanation = "Internal error while parsing the BE response";
                input->RPCTerminated = 1;
            }
            xmlrpc_env_clean(&env);
            return;
        }
    }
    
    srmlogit(STORM_LOG_DEBUG, func, "Response parsed!\n");
    
    xmlrpc_env_clean(&env);
    input->RPCTerminated = 1;
    return;
}

int ns1__srmPutDone(struct soap *soap, struct ns1__srmPutDoneRequest *req, struct ns1__srmPutDoneResponse_ *rep)
{
    static const char *func = "PutDone";
    static const char *methodName = "synchcall.putDone";
    struct ns1__srmPutDoneResponse *repp;
    struct RPC_ResponseHandlerInput_PutDone PutDoneResponseHandlerInput;
    int error;
    xmlrpc_env env;
    xmlrpc_value *inputParam;
    
    /************************ Allocate response structure *******************************/
    if ((repp = soap_malloc(soap, sizeof(struct ns1__srmPutDoneResponse))) == NULL)
        return (SOAP_EOM);
    /* Initialize the other filed of the repp structure */
    repp->arrayOfFileStatuses = NULL;
    
    if ((repp->returnStatus = soap_malloc(soap, sizeof(struct ns1__TReturnStatus))) == NULL)
        return (SOAP_EOM);

    /* Assign the repp response structure to the output parameter rep */
    rep->srmPutDoneResponse = repp;
    
    /* Initialize xmlrpc error-handling environment. */
    xmlrpc_env_init(&env);

    /*Initialize xmlrpc input structure*/
    inputParam = xmlrpc_struct_new(&env);
    
    /**************************** Encode VOMS attibutes ***************************/
    error = encode_VOMSAttributes(func, &env, soap, req->authorizationID, inputParam);
    if (error) {
        repp->returnStatus->explanation = "Error encoding VOMS attributes"; 
        repp->returnStatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
        xmlrpc_DECREF(inputParam);
        xmlrpc_env_clean(&env);
        return(SOAP_OK);
    }
    
    /******************************************************************************/
    /********        Encode parameters for PutDone   (SRM v2.2)            ********/  
    /******************************************************************************/
    
    /** OPTIONAL ************ (1) Encode authorizationID (char *) ****************************/
    error = encode_string(func, &env, req->authorizationID, SRM_PARAM_authorizationID, inputParam);
    if (error) {
        if (error != ENCODE_ERR_MISSING_PARAM) {
            repp->returnStatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
            repp->returnStatus->explanation = "Error encoding authorizationID";
            xmlrpc_DECREF(inputParam);
            xmlrpc_env_clean(&env);
            return(SOAP_OK);
        }
        xmlrpc_env_clean(&env);
        xmlrpc_env_init(&env);
    }
    
    /** OPTIONAL ************ (2) Encode requestToken (char *) ********************************/
    error = encode_string(func, &env, req->requestToken, SRM_PARAM_requestToken, inputParam);
    if (error) {
        if (error != ENCODE_ERR_MISSING_PARAM) {
            repp->returnStatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
            repp->returnStatus->explanation = "Error encoding requestToken parameter";
            xmlrpc_DECREF(inputParam);
            xmlrpc_env_clean(&env);
            return(SOAP_OK);
        }
        xmlrpc_env_clean(&env);
        xmlrpc_env_init(&env);
    }
    
    /** MANDATORY *********** (3) Encode arrayOfSURLs (ns1_ArrayOfAnyURI *) ********************************/
    error = encode_ArrayOfAnyURI(func, &env, req->arrayOfSURLs, SRM_PARAM_arrayOfSURLs, inputParam);
    if (error) {
        repp->returnStatus->statusCode = SRM_USCOREINVALID_USCOREREQUEST;
        
        if (error == ENCODE_ERR_MISSING_PARAM)
            repp->returnStatus->explanation = "Error: parameter arrayOfSURLs is required";
        else
            repp->returnStatus->explanation = "Error encoding arrayOfSURLs parameter";
        
        xmlrpc_DECREF(inputParam);
        xmlrpc_env_clean(&env);
        return(SOAP_OK);
    }
    
    /* Clean up xmlrpc error-handling environment. */
    xmlrpc_env_clean(&env);

    /* Define the structure to give as input to the RPC response handler */
    PutDoneResponseHandlerInput.soap = soap;
    PutDoneResponseHandlerInput.req  = req;
    PutDoneResponseHandlerInput.repp = repp;
    PutDoneResponseHandlerInput.RPCTerminated = 0;
                                                                        
    /* Make remote procedure call, i.e. call Backend server */
    xmlrpc_client_call_asynch(xmlrpc_endpoint, methodName, (void *) &rpcResponseHandler_PutDone,
                              &PutDoneResponseHandlerInput, "(S)", inputParam);
                              
    srmlogit(STORM_LOG_DEBUG, func, "Asynchronous RPC started.\n");

    while (PutDoneResponseHandlerInput.RPCTerminated == 0) 
        xmlrpc_client_event_loop_finish_asynch_timeout(RPC_ASYNCH_TIMEOUT);
    
    xmlrpc_DECREF(inputParam);

    if (PutDoneResponseHandlerInput.RPCTerminated == 2) {
    	srmlogit(STORM_LOG_ERROR, func, "Request done. Error: out of memory.\n");
        return(SOAP_EOM);
    }
    
    srmlogit(STORM_LOG_INFO, func, "Request done. Status: %s\n", reconvertStatusCode(repp->returnStatus->statusCode));
    
    return(SOAP_OK);
}

/***************************************************************************************/
/*************************         SRM v2.2 AbortRequest            *************************/
/***************************************************************************************/

/* Data structure needed to give input parameters to the response handler function */
struct RPC_ResponseHandlerInput_AbortRequest {
    struct soap *soap;
    struct ns1__srmAbortRequestRequest *req;
    struct ns1__srmAbortRequestResponse *repp;
    int RPCTerminated;
};

/* Response handler for the RPC asynchronous call of the AbortRequest function */
void rpcResponseHandler_AbortRequest(const char          *serverUrl, 
                                     const char          *method_name, 
                                     const xmlrpc_value  *param_array, 
                                     const void          *user_data, 
                                     const xmlrpc_env    *faultP, 
                                     xmlrpc_value        *result)
{
    static const char *func = "rpcResponseHandler_AbortRequest";
    struct RPC_ResponseHandlerInput_AbortRequest *input;
    struct soap *soap;
    struct ns1__srmAbortRequestRequest *req;
    struct ns1__srmAbortRequestResponse *repp;
    int error;
    xmlrpc_env env;
    
    input = (struct RPC_ResponseHandlerInput_AbortRequest *) user_data;
    soap = input->soap;
    req = input->req;
    repp = input->repp;
    
    if (faultP->fault_occurred) {
        srmlogit(STORM_LOG_ERROR, func, "ERROR: XML-RPC Fault: %s (code: %d)\n", faultP->fault_string, faultP->fault_code);
        repp->returnStatus->explanation = soap_strdup(soap, faultP->fault_string);
        repp->returnStatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
        input->RPCTerminated = 1;
        return;
    }
    /* Initialize the error-handling environment. */
    xmlrpc_env_init(&env);
    
    /** MANDATORY ************ (1) Decode returnStatus (in TReturnStatus *) *************************/
    error = decode_globalTReturnStatus(func, &env, soap, repp->returnStatus, result);
    if (error != 0) {
        repp->returnStatus->explanation = "Error: unable to parse the BE response or empty response";
        repp->returnStatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
        xmlrpc_env_clean(&env);
        input->RPCTerminated = 1;
        return;
    }
    
    srmlogit(STORM_LOG_DEBUG, func, "Response parsed!\n");
    
    xmlrpc_env_clean(&env);
    input->RPCTerminated = 1;
    return;
}

int ns1__srmAbortRequest(struct soap *soap,
                         struct ns1__srmAbortRequestRequest *req,
                         struct ns1__srmAbortRequestResponse_ *rep)
{
    static const char *func = "AbortRequest";
    static const char *methodName = "synchcall.abortRequest";
    struct ns1__srmAbortRequestResponse *repp;
    struct RPC_ResponseHandlerInput_AbortRequest AbortRequestResponseHandlerInput;
    int error;
    xmlrpc_env env;
    xmlrpc_value *inputParam;
    
    /************************ Allocate response structure *******************************/
    if ((repp = soap_malloc(soap, sizeof(struct ns1__srmAbortRequestResponse))) == NULL)
        return (SOAP_EOM);
    
    if ((repp->returnStatus = soap_malloc(soap, sizeof(struct ns1__TReturnStatus))) == NULL)
        return (SOAP_EOM);
    repp->returnStatus->explanation = NULL;

    /* Assign the repp response structure to the output parameter rep */
    rep->srmAbortRequestResponse = repp;
    
    /* Initialize xmlrpc error-handling environment. */
    xmlrpc_env_init(&env);

    /*Initialize xmlrpc input structure*/
    inputParam = xmlrpc_struct_new(&env);
    
    /**************************** Encode VOMS attibutes ***************************/
    error = encode_VOMSAttributes(func, &env, soap, req->authorizationID, inputParam);
    if (error) {
        repp->returnStatus->explanation = "Error encoding VOMS attributes"; 
        repp->returnStatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
        xmlrpc_DECREF(inputParam);
        xmlrpc_env_clean(&env);
        return(SOAP_OK);
    }
    
    /******************************************************************************/
    /********        Encode parameters for AbortRequest   (SRM v2.2)            ********/  
    /******************************************************************************/
    
    /** OPTIONAL ************ (1) Encode authorizationID (char *) ****************************/
    error = encode_string(func, &env, req->authorizationID, SRM_PARAM_authorizationID, inputParam);
    if (error) {
        if (error != ENCODE_ERR_MISSING_PARAM) {
            repp->returnStatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
            repp->returnStatus->explanation = "Error encoding authorizationID";
            xmlrpc_DECREF(inputParam);
            xmlrpc_env_clean(&env);
            return(SOAP_OK);
        }
        xmlrpc_env_clean(&env);
        xmlrpc_env_init(&env);
    }
    
    /** MANDATORY *********** (3) Encode requestToken (char *) ********************************/
    error = encode_string(func, &env, req->requestToken, SRM_PARAM_requestToken, inputParam);
    if (error) {
        repp->returnStatus->statusCode = SRM_USCOREINVALID_USCOREREQUEST;
        
        if (error == ENCODE_ERR_MISSING_PARAM)
            repp->returnStatus->explanation = "Error: parameter requestToken is required";
        else
            repp->returnStatus->explanation = "Error encoding requestToken parameter";
        
        xmlrpc_DECREF(inputParam);
        xmlrpc_env_clean(&env);
        return(SOAP_OK);
    }
    
    /* Clean up xmlrpc error-handling environment. */
    xmlrpc_env_clean(&env);

    /* Define the structure to give as input to the RPC response handler */
    AbortRequestResponseHandlerInput.soap = soap;
    AbortRequestResponseHandlerInput.req  = req;
    AbortRequestResponseHandlerInput.repp = repp;
    AbortRequestResponseHandlerInput.RPCTerminated = 0;
                                                                        
    /* Make remote procedure call, i.e. call Backend server */
    xmlrpc_client_call_asynch(xmlrpc_endpoint, methodName, (void *) &rpcResponseHandler_AbortRequest,
                              &AbortRequestResponseHandlerInput, "(S)", inputParam);
                              
    srmlogit(STORM_LOG_DEBUG, func, "Asynchronous RPC started.\n");

    while (AbortRequestResponseHandlerInput.RPCTerminated == 0) 
        xmlrpc_client_event_loop_finish_asynch_timeout(RPC_ASYNCH_TIMEOUT);
    
    xmlrpc_DECREF(inputParam);

    if (AbortRequestResponseHandlerInput.RPCTerminated == 2) {
    	srmlogit(STORM_LOG_ERROR, func, "Request done. Error: out of memory.\n");
        return(SOAP_EOM);
    }
    
    srmlogit(STORM_LOG_INFO, func, "Request done. Status: %s\n", reconvertStatusCode(repp->returnStatus->statusCode));
    
    return(SOAP_OK);
}

/***************************************************************************************/
/*************************         SRM v2.2 AbortFiles            *************************/
/***************************************************************************************/

/* Data structure needed to give input parameters to the response handler function */
struct RPC_ResponseHandlerInput_AbortFiles {
    struct soap *soap;
    struct ns1__srmAbortFilesRequest *req;
    struct ns1__srmAbortFilesResponse *repp;
    int RPCTerminated;
};

/* Response handler for the RPC asynchronous call of the AbortFiles function */
void rpcResponseHandler_AbortFiles(const char          *serverUrl, 
                                   const char          *method_name, 
                                   const xmlrpc_value  *param_array, 
                                   const void          *user_data, 
                                   const xmlrpc_env    *faultP, 
                                   xmlrpc_value        *result)
{
    static const char *func = "rpcResponseHandler_AbortFiles";
    struct RPC_ResponseHandlerInput_AbortFiles *input;
    struct soap *soap;
    struct ns1__srmAbortFilesRequest *req;
    struct ns1__srmAbortFilesResponse *repp;
    int error;
    xmlrpc_env env;
    
    input = (struct RPC_ResponseHandlerInput_AbortFiles *) user_data;
    soap = input->soap;
    req = input->req;
    repp = input->repp;
    
    if (faultP->fault_occurred) {
        srmlogit(STORM_LOG_ERROR, func, "ERROR: XML-RPC Fault: %s (code: %d)\n", faultP->fault_string, faultP->fault_code);
        repp->returnStatus->explanation = soap_strdup(soap, faultP->fault_string);
        repp->returnStatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
        input->RPCTerminated = 1;
        return;
    }
    /* Initialize the error-handling environment. */
    xmlrpc_env_init(&env);
    
    /** MANDATORY ************ (1) Decode returnStatus (in TReturnStatus *) *************************/
    error = decode_globalTReturnStatus(func, &env, soap, repp->returnStatus, result);
    if (error != 0) {
        repp->returnStatus->explanation = "Error: unable to parse the BE response or empty response";
        repp->returnStatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
        xmlrpc_env_clean(&env);
        input->RPCTerminated = 1;
        return;
    }
    
    /** OPTIONAL ************* (2) Decode arrayOfFileStatuses (in ArrayOfTSURLReturnStatus *) *********/
    error = decode_ArrayOfTSURLReturnStatus(func, &env, soap, &(repp->arrayOfFileStatuses), SRM_PARAM_arrayOfFileStatuses, result);
    if (error != 0) {
        if (error != DECODE_ERR_NOT_FOUND) {
            repp->returnStatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
            if (error == DECODE_ERR_SOAP_MEMORY_ERROR) {
                repp->returnStatus->explanation = "Memory allocation error";
                input->RPCTerminated = 2;
            } else {
                repp->returnStatus->explanation = "Internal error while parsing the BE response";
                input->RPCTerminated = 1;
            }
            xmlrpc_env_clean(&env);
            return;
        }
    }
    
    srmlogit(STORM_LOG_DEBUG, func, "Response parsed!\n");
    
    xmlrpc_env_clean(&env);
    input->RPCTerminated = 1;
    return;
}

int ns1__srmAbortFiles(struct soap *soap,
                       struct ns1__srmAbortFilesRequest *req,
                       struct ns1__srmAbortFilesResponse_ *rep)
{
    static const char *func = "AbortFiles";
    static const char *methodName = "synchcall.abortFiles";
    struct ns1__srmAbortFilesResponse *repp;
    struct RPC_ResponseHandlerInput_AbortFiles AbortFilesResponseHandlerInput;
    int error;
    xmlrpc_env env;
    xmlrpc_value *inputParam;
    
    /************************ Allocate response structure *******************************/
    if ((repp = soap_malloc(soap, sizeof(struct ns1__srmAbortFilesResponse))) == NULL)
        return (SOAP_EOM);
    repp->arrayOfFileStatuses = NULL;
    
    if ((repp->returnStatus = soap_malloc(soap, sizeof(struct ns1__TReturnStatus))) == NULL)
        return (SOAP_EOM);
    repp->returnStatus->explanation = NULL;

    /* Assign the repp response structure to the output parameter rep */
    rep->srmAbortFilesResponse = repp;
    
    /* Initialize xmlrpc error-handling environment. */
    xmlrpc_env_init(&env);

    /*Initialize xmlrpc input structure*/
    inputParam = xmlrpc_struct_new(&env);
    
    /**************************** Encode VOMS attibutes ***************************/
    error = encode_VOMSAttributes(func, &env, soap, req->authorizationID, inputParam);
    if (error) {
        repp->returnStatus->explanation = "Error encoding VOMS attributes"; 
        repp->returnStatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
        xmlrpc_DECREF(inputParam);
        xmlrpc_env_clean(&env);
        return(SOAP_OK);
    }
    
    /******************************************************************************/
    /********        Encode parameters for AbortFiles   (SRM v2.2)         ********/  
    /******************************************************************************/
    
    /** OPTIONAL ************ (1) Encode authorizationID (char *) ****************************/
    error = encode_string(func, &env, req->authorizationID, SRM_PARAM_authorizationID, inputParam);
    if (error) {
        if (error != ENCODE_ERR_MISSING_PARAM) {
            repp->returnStatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
            repp->returnStatus->explanation = "Error encoding authorizationID";
            xmlrpc_DECREF(inputParam);
            xmlrpc_env_clean(&env);
            return(SOAP_OK);
        }
        xmlrpc_env_clean(&env);
        xmlrpc_env_init(&env);
    }
    
    /** MANDATORY *********** (3) Encode requestToken (char *) ********************************/
    error = encode_string(func, &env, req->requestToken, SRM_PARAM_requestToken, inputParam);
    if (error) {
        repp->returnStatus->statusCode = SRM_USCOREINVALID_USCOREREQUEST;
        
        if (error == ENCODE_ERR_MISSING_PARAM)
            repp->returnStatus->explanation = "Error: parameter requestToken is required";
        else
            repp->returnStatus->explanation = "Error encoding requestToken parameter";
        
        xmlrpc_DECREF(inputParam);
        xmlrpc_env_clean(&env);
        return(SOAP_OK);
    }
    
    /** MANDATORY *********** (3) Encode arrayOfSURLs (ns1_ArrayOfAnyURI *) ********************************/
    error = encode_ArrayOfAnyURI(func, &env, req->arrayOfSURLs, SRM_PARAM_arrayOfSURLs, inputParam);
    if (error) {
        repp->returnStatus->statusCode = SRM_USCOREINVALID_USCOREREQUEST;
        
        if (error == ENCODE_ERR_MISSING_PARAM)
            repp->returnStatus->explanation = "Error: parameter arrayOfSURLs is required";
        else
            repp->returnStatus->explanation = "Error encoding arrayOfSURLs parameter";
        
        xmlrpc_DECREF(inputParam);
        xmlrpc_env_clean(&env);
        return(SOAP_OK);
    }
    
    /* Clean up xmlrpc error-handling environment. */
    xmlrpc_env_clean(&env);

    /* Define the structure to give as input to the RPC response handler */
    AbortFilesResponseHandlerInput.soap = soap;
    AbortFilesResponseHandlerInput.req  = req;
    AbortFilesResponseHandlerInput.repp = repp;
    AbortFilesResponseHandlerInput.RPCTerminated = 0;
                                                                        
    /* Make remote procedure call, i.e. call Backend server */
    xmlrpc_client_call_asynch(xmlrpc_endpoint, methodName, (void *) &rpcResponseHandler_AbortFiles,
                              &AbortFilesResponseHandlerInput, "(S)", inputParam);
                              
    srmlogit(STORM_LOG_DEBUG, func, "Asynchronous RPC started.\n");

    while (AbortFilesResponseHandlerInput.RPCTerminated == 0) 
        xmlrpc_client_event_loop_finish_asynch_timeout(RPC_ASYNCH_TIMEOUT);
    
    xmlrpc_DECREF(inputParam);

    if (AbortFilesResponseHandlerInput.RPCTerminated == 2) {
    	srmlogit(STORM_LOG_ERROR, func, "Request done. Error: out of memory.\n");
        return(SOAP_EOM);
    }
    
    srmlogit(STORM_LOG_INFO, func, "Request done. Status: %s\n", reconvertStatusCode(repp->returnStatus->statusCode));
    
    return(SOAP_OK);
}

/***************************************************************************************/
/*************************         SRM v2.2 ExtendFileLifeTime            *************************/
/***************************************************************************************/

/* Data structure needed to give input parameters to the response handler function */
struct RPC_ResponseHandlerInput_ExtendFileLifeTime {
    struct soap *soap;
    struct ns1__srmExtendFileLifeTimeRequest *req;
    struct ns1__srmExtendFileLifeTimeResponse *repp;
    int RPCTerminated;
};

/* Response handler for the RPC asynchronous call of the ExtendFileLifeTime function */
void rpcResponseHandler_ExtendFileLifeTime(const char          *serverUrl, 
                                           const char          *method_name, 
                                           const xmlrpc_value  *param_array, 
                                           const void          *user_data, 
                                           const xmlrpc_env    *faultP, 
                                           xmlrpc_value        *result)
{
    static const char *func = "rpcResponseHandler_ExtendFileLifeTime";
    struct RPC_ResponseHandlerInput_ExtendFileLifeTime *input;
    struct soap *soap;
    struct ns1__srmExtendFileLifeTimeRequest *req;
    struct ns1__srmExtendFileLifeTimeResponse *repp;
    int error;
    xmlrpc_env env;
    
    input = (struct RPC_ResponseHandlerInput_ExtendFileLifeTime *) user_data;
    soap = input->soap;
    req = input->req;
    repp = input->repp;
    
    if (faultP->fault_occurred) {
        srmlogit(STORM_LOG_ERROR, func, "ERROR: XML-RPC Fault: %s (code: %d)\n", faultP->fault_string, faultP->fault_code);
        repp->returnStatus->explanation = soap_strdup(soap, faultP->fault_string);
        repp->returnStatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
        input->RPCTerminated = 1;
        return;
    }
    /* Initialize the error-handling environment. */
    xmlrpc_env_init(&env);
    
    /** MANDATORY ************ (1) Decode returnStatus (in TReturnStatus *) *************************/
    error = decode_globalTReturnStatus(func, &env, soap, repp->returnStatus, result);
    if (error != 0) {
        repp->returnStatus->explanation = "Error: unable to parse the BE response or empty response";
        repp->returnStatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
        xmlrpc_env_clean(&env);
        input->RPCTerminated = 1;
        return;
    }
    
    /** OPTIONAL ************* (2) Decode arrayOfFileStatuses (in ArrayOfTSURLLifetimeReturnStatus *) *********/
    error = decode_ArrayOfTSURLLifetimeReturnStatus(func, &env, soap, &(repp->arrayOfFileStatuses), SRM_PARAM_arrayOfFileStatuses, result);
    if (error != 0) {
        if (error != DECODE_ERR_NOT_FOUND) {
            repp->returnStatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
            if (error == DECODE_ERR_SOAP_MEMORY_ERROR) {
                repp->returnStatus->explanation = "Memory allocation error";
                input->RPCTerminated = 2;
            } else {
                repp->returnStatus->explanation = "Internal error while parsing the BE response";
                input->RPCTerminated = 1;
            }
            xmlrpc_env_clean(&env);
            return;
        }
    }
    
    srmlogit(STORM_LOG_DEBUG, func, "Response parsed!\n");
    
    xmlrpc_env_clean(&env);
    input->RPCTerminated = 1;
    return;
}


int ns1__srmExtendFileLifeTime(struct soap *soap,
                               struct ns1__srmExtendFileLifeTimeRequest *req,
                               struct ns1__srmExtendFileLifeTimeResponse_ *rep)
{
    static const char *func = "ExtendFileLifeTime";
    static const char *methodName = "synchcall.extendFileLifeTime";
    struct ns1__srmExtendFileLifeTimeResponse *repp;
    struct RPC_ResponseHandlerInput_ExtendFileLifeTime ExtendFileLifeTimeResponseHandlerInput;
    int error;
    xmlrpc_env env;
    xmlrpc_value *inputParam;
    
    /************************ Allocate response structure *******************************/
    if ((repp = soap_malloc(soap, sizeof(struct ns1__srmExtendFileLifeTimeResponse))) == NULL)
        return (SOAP_EOM);
    repp->arrayOfFileStatuses = NULL;
    
    if ((repp->returnStatus = soap_malloc(soap, sizeof(struct ns1__TReturnStatus))) == NULL)
        return (SOAP_EOM);
    repp->returnStatus->explanation = NULL;

    /* Assign the repp response structure to the output parameter rep */
    rep->srmExtendFileLifeTimeResponse = repp;
    
    /* Initialize xmlrpc error-handling environment. */
    xmlrpc_env_init(&env);

    /*Initialize xmlrpc input structure*/
    inputParam = xmlrpc_struct_new(&env);
    
    /**************************** Encode VOMS attibutes ***************************/
    error = encode_VOMSAttributes(func, &env, soap, req->authorizationID, inputParam);
    if (error) {
        repp->returnStatus->explanation = "Error encoding VOMS attributes"; 
        repp->returnStatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
        xmlrpc_DECREF(inputParam);
        xmlrpc_env_clean(&env);
        return(SOAP_OK);
    }
    
    /******************************************************************************/
    /********    Encode parameters for ExtendFileLifeTime   (SRM v2.2)     ********/  
    /******************************************************************************/
    
    /** OPTIONAL ************ (1) Encode authorizationID (char *) ****************************/
    error = encode_string(func, &env, req->authorizationID, SRM_PARAM_authorizationID, inputParam);
    if (error) {
        if (error != ENCODE_ERR_MISSING_PARAM) {
            repp->returnStatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
            repp->returnStatus->explanation = "Error encoding authorizationID";
            xmlrpc_DECREF(inputParam);
            xmlrpc_env_clean(&env);
            return(SOAP_OK);
        }
        xmlrpc_env_clean(&env);
        xmlrpc_env_init(&env);
    }
    
    /** OPTIONAL ************ (2) Encode requestToken (char *) ********************************/
    error = encode_string(func, &env, req->requestToken, SRM_PARAM_requestToken, inputParam);
    if (error) {
        if (error != ENCODE_ERR_MISSING_PARAM) {
            repp->returnStatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
            repp->returnStatus->explanation = "Error encoding requestToken parameter";
            xmlrpc_DECREF(inputParam);
            xmlrpc_env_clean(&env);
            return(SOAP_OK);
        }
        xmlrpc_env_clean(&env);
        xmlrpc_env_init(&env);
    }
    
    /** MANDATORY *********** (3) Encode arrayOfSURLs (ns1_ArrayOfAnyURI *) ********************************/
    error = encode_ArrayOfAnyURI(func, &env, req->arrayOfSURLs, SRM_PARAM_arrayOfSURLs, inputParam);
    if (error) {
        repp->returnStatus->statusCode = SRM_USCOREINVALID_USCOREREQUEST;
        
        if (error == ENCODE_ERR_MISSING_PARAM)
            repp->returnStatus->explanation = "Error: parameter arrayOfSURLs is required";
        else
            repp->returnStatus->explanation = "Error encoding arrayOfSURLs parameter";
        
        xmlrpc_DECREF(inputParam);
        xmlrpc_env_clean(&env);
        return(SOAP_OK);
    }
    
    /** OPTIONAL ************ (4) Encode newFileLifeTime (int *)    *********************************/
    error = encode_lifetimeValue(func, &env, req->newFileLifeTime, SRM_PARAM_fileLifeTime, inputParam);
    if (error) {
        if (error != ENCODE_ERR_MISSING_PARAM) {
            repp->returnStatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
            repp->returnStatus->explanation = "Error encoding newFileLifeTime";
            xmlrpc_DECREF(inputParam);
            xmlrpc_env_clean(&env);
            return(SOAP_OK);
        }
        xmlrpc_env_clean(&env);
        xmlrpc_env_init(&env);
    }
    
    /** OPTIONAL ************ (5) Encode newPinLifeTime (int *)    *********************************/
    error = encode_lifetimeValue(func, &env, req->newPinLifeTime, SRM_PARAM_pinLifetime, inputParam);
    if (error) {
        if (error != ENCODE_ERR_MISSING_PARAM) {
            repp->returnStatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
            repp->returnStatus->explanation = "Error encoding newPinLifeTime";
            xmlrpc_DECREF(inputParam);
            xmlrpc_env_clean(&env);
            return(SOAP_OK);
        }
        xmlrpc_env_clean(&env);
        xmlrpc_env_init(&env);
    }
    
    /* Clean up xmlrpc error-handling environment. */
    xmlrpc_env_clean(&env);

    /* Define the structure to give as input to the RPC response handler */
    ExtendFileLifeTimeResponseHandlerInput.soap = soap;
    ExtendFileLifeTimeResponseHandlerInput.req  = req;
    ExtendFileLifeTimeResponseHandlerInput.repp = repp;
    ExtendFileLifeTimeResponseHandlerInput.RPCTerminated = 0;
                                                                        
    /* Make remote procedure call, i.e. call Backend server */
    xmlrpc_client_call_asynch(xmlrpc_endpoint, methodName, (void *) &rpcResponseHandler_ExtendFileLifeTime,
                              &ExtendFileLifeTimeResponseHandlerInput, "(S)", inputParam);
                              
    srmlogit(STORM_LOG_DEBUG, func, "Asynchronous RPC started.\n");

    while (ExtendFileLifeTimeResponseHandlerInput.RPCTerminated == 0) 
        xmlrpc_client_event_loop_finish_asynch_timeout(RPC_ASYNCH_TIMEOUT);
    
    xmlrpc_DECREF(inputParam);

    if (ExtendFileLifeTimeResponseHandlerInput.RPCTerminated == 2) {
    	srmlogit(STORM_LOG_ERROR, func, "Request done. Error: out of memory.\n");
        return(SOAP_EOM);
    }
    
    srmlogit(STORM_LOG_INFO, func, "Request done. Status: %s\n", reconvertStatusCode(repp->returnStatus->statusCode));
    
    return(SOAP_OK);
}

int ns1__srmSuspendRequest(struct soap *soap,
                           struct ns1__srmSuspendRequestRequest *req,
                           struct ns1__srmSuspendRequestResponse_ *rep)
{
    static const char *func = "SuspendRequest";
    struct hostent *hp;
    struct ns1__srmSuspendRequestResponse *repp;
    struct in_addr sin_addr;
    struct srm_srv_thread_info *thip = soap->user;
    char clientdn[256];
    char **fqans, *clienthost;
    int nbfqans;

/******* FLAVIA TO DO *******/
#if defined(GSI_PLUGINS)
    get_client_dn(soap, clientdn, sizeof(clientdn));
    fqans = get_client_roles(soap, &nbfqans);
#else
    if(NULL != req->authorizationID) strcpy(clientdn,req->authorizationID);
    else strcpy(clientdn,"DUMMY USER ID");
#endif
/******* FLAVIA TO DO *******/

    sin_addr.s_addr = htonl (soap->ip);
    if ((hp = Cgethostbyaddr((char *)(&sin_addr), sizeof(struct in_addr), AF_INET)) == NULL)
        clienthost = inet_ntoa (sin_addr);
    else
        clienthost = hp->h_name;
    srmlogit(STORM_LOG_INFO, func, "Request by %s from %s\n", clientdn, clienthost);

    if ((repp = soap_malloc(soap, sizeof(struct ns1__srmSuspendRequestResponse))) == NULL ||
        (repp->returnStatus = soap_malloc (soap, sizeof(struct ns1__TReturnStatus))) == NULL) {
        return(SOAP_EOM);
    }
    repp->returnStatus->explanation = NULL;
    repp->returnStatus->statusCode = SRM_USCORENOT_USCORESUPPORTED;
    rep->srmSuspendRequestResponse = repp;
    return(SOAP_OK);
}

int ns1__srmResumeRequest(struct soap *soap,
                          struct ns1__srmResumeRequestRequest *req,
                          struct ns1__srmResumeRequestResponse_ *rep)
{
    static const char *func = "ResumeRequest";
    struct hostent *hp;
    struct ns1__srmResumeRequestResponse *repp;
    struct in_addr sin_addr;
    struct srm_srv_thread_info *thip = soap->user;
    char clientdn[256];
    char **fqans, *clienthost;
    int nbfqans;

/******* FLAVIA TO DO *******/
#if defined(GSI_PLUGINS)
    get_client_dn(soap, clientdn, sizeof(clientdn));
    fqans = get_client_roles(soap, &nbfqans);
#else
    if(NULL != req->authorizationID) strcpy(clientdn,req->authorizationID);
    else strcpy(clientdn,"DUMMY USER ID");
#endif
/******* FLAVIA TO DO *******/

    sin_addr.s_addr = htonl (soap->ip);
    if ((hp = Cgethostbyaddr ((char *)(&sin_addr), sizeof(struct in_addr), AF_INET)) == NULL)
        clienthost = inet_ntoa (sin_addr);
    else
        clienthost = hp->h_name;
    srmlogit(STORM_LOG_INFO, func, "Request by %s from %s\n", clientdn, clienthost);

    if ((repp = soap_malloc(soap, sizeof(struct ns1__srmResumeRequestResponse))) == NULL ||
        (repp->returnStatus = soap_malloc (soap, sizeof(struct ns1__TReturnStatus))) == NULL) {
        return(SOAP_EOM);
    }
    repp->returnStatus->explanation = NULL;
    repp->returnStatus->statusCode = SRM_USCORENOT_USCORESUPPORTED;
    rep->srmResumeRequestResponse = repp;
    return(SOAP_OK);
}



/**
 * This function is to discover what transfer protocols are supported.
 */
int ns1__srmGetTransferProtocols(struct soap* soap,
                                 struct ns1__srmGetTransferProtocolsRequest *req,
                                 struct ns1__srmGetTransferProtocolsResponse_ *rep)
{
    static const char *func = "GetTransferProtocols";
    struct ns1__srmGetTransferProtocolsResponse *repp;
    struct ns1__TSupportedTransferProtocol **protocolArray;
    char **supported_protocols;
    int i, nb_supported_protocols;
    
    /************************ Allocate response structure *******************************/
    if ((repp = soap_malloc(soap, sizeof(struct ns1__srmGetTransferProtocolsResponse))) == NULL)
        return(SOAP_EOM);
    
    if ((repp->returnStatus = soap_malloc(soap, sizeof(struct ns1__TReturnStatus))) == NULL)
        return(SOAP_EOM);
        
    rep->srmGetTransferProtocolsResponse = repp;
    
    nb_supported_protocols = get_supported_protocols(&supported_protocols);
    
    if (nb_supported_protocols <= 0) {
        srmlogit (STORM_LOG_ERROR, func, "get_supported_protocols does not return any protocol");
        repp->returnStatus->statusCode = SRM_USCOREFAILURE;
        repp->returnStatus->explanation = "Error: list of supported protocols not found";
        return(SOAP_OK);
    }
    
    /* Allocate the response structure for the list of protocols */
    repp->protocolInfo = soap_malloc(soap, sizeof(struct ns1__ArrayOfTSupportedTransferProtocol));
    if (NULL == repp->protocolInfo)
        return(SOAP_EOM);
    
    repp->protocolInfo->protocolArray = soap_malloc(soap, nb_supported_protocols * sizeof(struct ns1__TSupportedTransferProtocol *));
    if (NULL == repp->protocolInfo->protocolArray)
        return(SOAP_EOM);
    repp->protocolInfo->__sizeprotocolArray = nb_supported_protocols;
    protocolArray = repp->protocolInfo->protocolArray;
    
    /* Set the protocol list to return */
    for (i=0; i<nb_supported_protocols; i++) {
        protocolArray[i] = soap_malloc(soap, sizeof(struct ns1__TSupportedTransferProtocol));
        protocolArray[i]->transferProtocol = soap_strdup(soap, supported_protocols[i]);
        protocolArray[i]->attributes = NULL;
    }
    
    free(supported_protocols);
    
    repp->returnStatus->statusCode = SRM_USCORESUCCESS;
    repp->returnStatus->explanation = "Success";
    
    return(SOAP_OK);
}

/***************************************************************************************/
/*************************         SRM v2.2 Ping               *************************/
/***************************************************************************************/

/* Data structure needed to give input parameters to the response handler function */
struct RPC_ResponseHandlerInput_Ping {
    struct soap *soap;
    struct ns1__srmPingRequest *req;
    struct ns1__srmPingResponse *repp;
    int RPCTerminated;
};

/* Response handler for the RPC asynchronous call of the Ping function */
void rpcResponseHandler_Ping(const char          *serverUrl, 
                             const char          *method_name, 
                             const xmlrpc_value  *param_array, 
                             const void          *user_data, 
                             const xmlrpc_env    *faultP, 
                             xmlrpc_value        *result)
{
    static const char *func = "rpcResponseHandler_Ping";
    struct RPC_ResponseHandlerInput_Ping *input;
    struct soap *soap;
    struct ns1__srmPingRequest *req;
    struct ns1__srmPingResponse *repp;
    int error;
    xmlrpc_env env;
    
    input = (struct RPC_ResponseHandlerInput_Ping *) user_data;
    soap = input->soap;
    req = input->req;
    repp = input->repp;
    
    if (faultP->fault_occurred) {
        srmlogit(STORM_LOG_ERROR, func, "ERROR: XML-RPC Fault: %s (code: %d)\n", faultP->fault_string, faultP->fault_code);
        repp->versionInfo = "ERROR!";
        input->RPCTerminated = 1;
        return;
    }
    /* Initialize the error-handling environment. */
    xmlrpc_env_init(&env);
    
    /** OPTIONAL ************* (1) Decode versionInfo (in char *) *********/
    error = decode_string(func, &env, soap, &(repp->versionInfo), "versionInfo", result);
    if (error != 0) {
        if (error != DECODE_ERR_NOT_FOUND) {
            if (error == DECODE_ERR_SOAP_MEMORY_ERROR) {
                repp->versionInfo = "Memory allocation error";
                input->RPCTerminated = 2;
            } else {
                repp->versionInfo = "Internal error while parsing the BE response";
                input->RPCTerminated = 1;
            }
            xmlrpc_env_clean(&env);
            return;
        }
        xmlrpc_env_clean(&env);
        xmlrpc_env_init(&env);
    }
    
    /** OPTIONAL ************* (2) Decode otherInfo (in ns1__ArrayOfTExtraInfo *) *********/
    error = decode_ArrayOfTExtraInfo(func, &env, soap, &(repp->otherInfo), SRM_PARAM_storageSystemInfo, result);
    if (error != 0) {
        if (error != DECODE_ERR_NOT_FOUND) {
            if (error == DECODE_ERR_SOAP_MEMORY_ERROR) {
                repp->versionInfo = "Memory allocation error";
                input->RPCTerminated = 2;
            } else {
                repp->versionInfo = "Internal error while parsing the BE response";
                input->RPCTerminated = 1;
            }
            xmlrpc_env_clean(&env);
            return;
        }
        xmlrpc_env_clean(&env);
        xmlrpc_env_init(&env);
    }
    
    srmlogit(STORM_LOG_DEBUG, func, "Response parsed!\n");
    
    xmlrpc_env_clean(&env);
    input->RPCTerminated = 1;
    return;
}

/**
 * This functin is used to check the state of the SRM. It works as an "are you alive" type of call.
*/ 
int ns1__srmPing(struct soap* soap, struct ns1__srmPingRequest *req, struct ns1__srmPingResponse_ *rep)
{
	static const char *func = "Ping";
	static const char *commandPrefix = "pippopluto";
	static const char *methodName = "synchcall.ping";
    struct ns1__TExtraInfo** extraInfoArray; 
    struct ns1__srmPingResponse *repp;
    struct RPC_ResponseHandlerInput_Ping PingResponseHandlerInput;
    int error;
    char askInfoToBE;
    char clientdn[256];
    xmlrpc_env env;
    xmlrpc_value *inputParam;
    
#if defined(GSI_PLUGINS)
    clientdn[0] = 0;
    get_client_dn(soap, clientdn, sizeof(clientdn));
    srmlogit(STORM_LOG_INFO, func, "Received Ping request from: %s\n", clientdn);  
#else
    if (req->authorizationID != NULL) {
        srmlogit(STORM_LOG_INFO, func, "Received Ping request from: %s\n", req->authorizationID);
    } else {
        srmlogit(STORM_LOG_INFO, func, "Received an anonymous Ping request.\n");
    }
#endif
    /************************ Allocate response structure *******************************/
    if (NULL == (repp = soap_malloc(soap, sizeof(struct ns1__srmPingResponse))))
        return (SOAP_EOM);
	repp->versionInfo = NULL;
    repp->otherInfo = NULL;
    
    /* Assign the repp response structure to the output parameter rep */
    rep->srmPingResponse = repp;
    
    /* Initialize xmlrpc error-handling environment. */
    xmlrpc_env_init(&env);

    /*Initialize xmlrpc input structure*/
    inputParam = xmlrpc_struct_new(&env);
    
    askInfoToBE = 0;
    if (req->authorizationID != NULL) {
    	if (strncmp(req->authorizationID, commandPrefix, strlen(commandPrefix)) == 0)
    		askInfoToBE = 1;
    }
    
    if (!askInfoToBE) {
    	repp->versionInfo = "v2.2";
        srmlogit(STORM_LOG_INFO, func, "Returning: %s\n", repp->versionInfo);
        
        //Adding the new field on TYPE and VERSION 
        repp->otherInfo = soap_malloc(soap, sizeof(struct ns1__ArrayOfTExtraInfo));
        if (NULL == repp->otherInfo)
            return(SOAP_EOM);
        repp->otherInfo->extraInfoArray = soap_malloc(soap, 2*sizeof(struct ns1__TExtraInfo));
        if (NULL == repp->otherInfo->extraInfoArray)
            return(SOAP_EOM);
        repp->otherInfo->__sizeextraInfoArray = 2;
        
        extraInfoArray = repp->otherInfo->extraInfoArray;
        
        //Set backend type
        extraInfoArray[0] = soap_malloc(soap, sizeof(struct ns1__TExtraInfo));
        extraInfoArray[0]->key = "backend_type";
        extraInfoArray[0]->value = "StoRM";
        //Set backend version
        extraInfoArray[1] = soap_malloc(soap, sizeof(struct ns1__TExtraInfo));
        extraInfoArray[1]->key = "backend_version";
        extraInfoArray[1]->value = "1.3.16";
               
    	return SOAP_OK;
        
    }
    
    /** OPTIONAL ************ (1) Encode authorizationID (char *) ****************************/
    error = encode_string(func, &env, req->authorizationID, SRM_PARAM_authorizationID, inputParam);
    if (error) {
        if (error != ENCODE_ERR_MISSING_PARAM) {
            repp->versionInfo= "Error encoding authorizationID";
            xmlrpc_DECREF(inputParam);
            xmlrpc_env_clean(&env);
            return(SOAP_OK);
        }
        xmlrpc_env_clean(&env);
        xmlrpc_env_init(&env);
    }
    
    /* Clean up xmlrpc error-handling environment. */
    xmlrpc_env_clean(&env);

    /* Define the structure to give as input to the RPC response handler */
    PingResponseHandlerInput.soap = soap;
    PingResponseHandlerInput.req  = req;
    PingResponseHandlerInput.repp = repp;
    PingResponseHandlerInput.RPCTerminated = 0;
                                                                        
    /* Make remote procedure call, i.e. call Backend server */
    xmlrpc_client_call_asynch(xmlrpc_endpoint, methodName, (void *) &rpcResponseHandler_Ping,
                              &PingResponseHandlerInput, "(S)", inputParam);
                              
    srmlogit(STORM_LOG_DEBUG, func, "Asynchronous RPC started.\n");

    while (PingResponseHandlerInput.RPCTerminated == 0) 
        xmlrpc_client_event_loop_finish_asynch_timeout(RPC_ASYNCH_TIMEOUT);
    
    xmlrpc_DECREF(inputParam);

    if (PingResponseHandlerInput.RPCTerminated == 2) {
    	srmlogit(STORM_LOG_ERROR, func, "Request done. Error: out of memory.\n");
        return(SOAP_EOM);
    }
    
    srmlogit(STORM_LOG_INFO, func, "Request done.\n");
    
    return(SOAP_OK);
}
