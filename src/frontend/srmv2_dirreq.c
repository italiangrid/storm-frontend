/*
 * $Id$
 */

/**
 * \file srmv2_dirreq.c
 * 
 * This file contains the functions implementing the "Directory Functions" section of
 * the SRM 2.2 specification.
 */

#include "storm_functions.h"
#include "srmv2H.h"
#include "xmlrpc_encode.h"
#include "xmlrpc_decode.h"
#include "srmlogit.h"
#include <xmlrpc-c/base.h>
#include <xmlrpc-c/client.h>


/* Defines for XML-RPM calls for FE-BE communication */

#define NAME "Xmlrpc-c Test Client"
//#define VERSION "1.0"
//#define RPC_ASYNCH_TIMEOUT 5

/*************************************************************************************/
/*                                  Directory Functions                              */
/*************************************************************************************/

/***********************         SRM v2.2 Mkdir            *************************/

struct RPC_ResponseHandlerInput_Mkdir {
    struct soap *soap;
    struct ns1__srmMkdirRequest *req;
    struct ns1__srmMkdirResponse *repp;
    int RPCTerminated;
};

/* Response handler for the RPC asynchronous call of the Mkdir function */
void rpcResponseHandler_Mkdir(const char         *serverUrl, 
                              const char         *method_name, 
                              const xmlrpc_value *param_array, 
                              const void         *user_data, 
                              const xmlrpc_env   *faultP, 
                              xmlrpc_value       *result)
{
    static const char *func = "rpcResponseHandler_Mkdir";
    struct RPC_ResponseHandlerInput_Mkdir *input;
    struct soap *soap;
    struct ns1__srmMkdirRequest *req;
    struct ns1__srmMkdirResponse *repp;
    int error;
    xmlrpc_env env;
    xmlrpc_value *globalReturnStatusP;
    
    input = (struct RPC_ResponseHandlerInput_Mkdir *) user_data;
    soap = input->soap;
    req = input->req;
    repp = input->repp;

    srmlogit(STORM_LOG_DEBUG, func, "Inside the response handler\n");
        
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
    error = decode_globalTReturnStatus(func,&env, soap, repp->returnStatus, result);
    if (error != 0) {
        repp->returnStatus->explanation = "Error: unable to parse the BE response or empty response";
        repp->returnStatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
        xmlrpc_env_clean(&env);
        input->RPCTerminated = 1;
        return;
    }
        
    xmlrpc_env_clean(&env);
    input->RPCTerminated = 1;

    srmlogit(STORM_LOG_DEBUG, func, "Response parsed\n");

    return;
}
 
/**
 * This function implements the XML-RPC call to the backend server for the Mkdir (SRM v2.1.1)
 * directory function.
 * @param soap the soap var
 * @param req input request structure
 * @param rep output response structure
 */                       
int ns1__srmMkdir(struct soap *soap, struct ns1__srmMkdirRequest *req, 
                  struct ns1__srmMkdirResponse_ *rep)
{
    static const char *func = "Mkdir";
    static const char *methodName_mkdir = "synchcall.mkdir";
    char clientdn[256], **fqans;
    int i, error, nbfqans;
    struct ns1__srmMkdirResponse *repp;
    struct RPC_ResponseHandlerInput_Mkdir MkdirResponseHandlerInput;
    xmlrpc_env env;
    xmlrpc_value *inputParam;

    /* Allocate response structure */
    if ((repp = soap_malloc(soap, sizeof(struct ns1__srmMkdirResponse))) == NULL)
        return(SOAP_EOM);
    if ((repp->returnStatus = soap_malloc(soap, sizeof(struct ns1__TReturnStatus))) == NULL) 
        return(SOAP_EOM);

    /* Assign the repp response structure to the output parameter rep */
    rep->srmMkdirResponse = repp;
    
    /* Initialize xmlrpc error-handling environment. */
    xmlrpc_env_init(&env);
    
    /* Initialize xmlrpc input structure */
    inputParam = xmlrpc_struct_new(&env);
    

    /**************************** Encode VOMS attibutes ***************************/
    error = encode_VOMSAttributes(func, &env, soap, req->authorizationID, inputParam);
    if (error) {
        repp->returnStatus->explanation = "Error encoding VOMS attributes"; 
        repp->returnStatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
        xmlrpc_DECREF(inputParam);
        /* Clean up our error-handling environment. */
        xmlrpc_env_clean(&env);
        return(SOAP_OK);
    }
 
    /******************************************************************************/
    /********          Encode parameters for Mkdir (SRM v2.2)              ********/  
    /******************************************************************************/
    /* IMPORTANT: Also for optional parameter, in case of parameter 
     * specified by client, IF error occours during xmlrpc parsing,
     *  we  prefere to abort the request execution.                               */

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

    /** MANDATORY ************ (2) Encode SURL ***************************************************/
    error = encode_string(func, &env, req->SURL, SRM_PARAM_SURL, inputParam);
    if (error) {
    	repp->returnStatus->statusCode = SRM_USCOREINVALID_USCOREREQUEST;
    	
        if (error == ENCODE_ERR_MISSING_PARAM) repp->returnStatus->explanation = "Error: parameter SURL is required";
        else repp->returnStatus->explanation = "Error encoding SURL parameter";
        
        xmlrpc_DECREF(inputParam);
        xmlrpc_env_clean(&env);
        return(SOAP_OK);
    }
    
    /** OPTIONAL ************* (3) Encode storageSystemInfo **************************************/
    error = encode_ArrayOfTExtraInfo(func, &env, req->storageSystemInfo, SRM_PARAM_storageSystemInfo, inputParam);
    if (error) {
        if (error != ENCODE_ERR_MISSING_PARAM) {
            repp->returnStatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
            repp->returnStatus->explanation = "Error encoding storageSystemInfo";
            xmlrpc_DECREF(inputParam);
            xmlrpc_env_clean(&env);
            return(SOAP_OK);
        }
        xmlrpc_env_clean(&env);
        xmlrpc_env_init(&env);
    }
    
    srmlogit(STORM_LOG_DEBUG, func, "RPC input param setted!\n");

    /* Clean up xmlrpc error-handling environment. */
    xmlrpc_env_clean(&env);
        
    /* Define the structure to give as input to the RPC response handler */
    MkdirResponseHandlerInput.soap = soap;
    MkdirResponseHandlerInput.req  = req;
    MkdirResponseHandlerInput.repp = repp;
    MkdirResponseHandlerInput.RPCTerminated = 0;
                                                                        
    /* Make remote procedure call, i.e. call Backend server */
    xmlrpc_client_call_asynch(xmlrpc_endpoint, methodName_mkdir, (void *) &rpcResponseHandler_Mkdir,
                              &MkdirResponseHandlerInput, "(S)", inputParam);
                              
    srmlogit(STORM_LOG_DEBUG, func, "Asynchronous RPC started\n");

    while (MkdirResponseHandlerInput.RPCTerminated == 0) 
        xmlrpc_client_event_loop_finish_asynch_timeout(RPC_ASYNCH_TIMEOUT);
    
    xmlrpc_DECREF(inputParam);

	srmlogit(STORM_LOG_INFO, func, "Request done. Status: %s\n", reconvertStatusCode(repp->returnStatus->statusCode));
	    
    return(SOAP_OK);
}

/***************************************************************************************/
/*************************         SRM v2.2 Rmdir              *************************/
/***************************************************************************************/

/* Data structure needed to give input parameters to the response handler function */
struct RPC_ResponseHandlerInput_Rmdir {
    struct soap *soap;
    struct ns1__srmRmdirRequest *req;
    struct ns1__srmRmdirResponse *repp;
    int RPCTerminated;
};

/* Response handler for the RPC asynchronous call of the Rmdir function */
void rpcResponseHandler_Rmdir(const char          *serverUrl, 
                              const char          *method_name, 
                              const xmlrpc_value  *param_array, 
                              const void          *user_data, 
                              const xmlrpc_env    *faultP, 
                              xmlrpc_value        *result)
{
    static const char *func= "rpcResponseHandler_Rmdir";
    struct RPC_ResponseHandlerInput_Rmdir *input;
    struct soap *soap;
    struct ns1__srmRmdirRequest *req;
    struct ns1__srmRmdirResponse *repp;
    xmlrpc_env env;
    int error = 0;
  
    input = (struct RPC_ResponseHandlerInput_Rmdir *) user_data;
    soap = input->soap;
    req = input->req;
    repp = input->repp;
    
    srmlogit(STORM_LOG_DEBUG, func, "Inside the response handler\n");
    
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
    error = decode_globalTReturnStatus(func,&env, soap, repp->returnStatus, result);
    if (error != 0) {
        repp->returnStatus->explanation = "Error: unable to parse the BE response or empty response";
        repp->returnStatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
        xmlrpc_env_clean(&env);
        input->RPCTerminated = 1;
        return;
    }

    /* Clean up our error-handling environment. */
    xmlrpc_env_clean(&env);
    input->RPCTerminated = 1;
    
    srmlogit(STORM_LOG_DEBUG, func, "Response parsed!\n");
    
    return;
}

int ns1__srmRmdir(struct soap *soap, struct ns1__srmRmdirRequest *req,
                  struct ns1__srmRmdirResponse_ *rep)
{
    static const char *methodName_rmdir = "synchcall.rmdir";
    char *func = "Rmdir";
    struct ns1__srmRmdirResponse *repp;
    struct RPC_ResponseHandlerInput_Rmdir RmdirResponseHandlerInput;
    int error;
    xmlrpc_env env;
    xmlrpc_value *inputParam;
  
    /* Allocate the response structure */
    if ((repp = soap_malloc(soap, sizeof(struct ns1__srmRmdirResponse))) == NULL)
        return(SOAP_EOM);
    if ((repp->returnStatus = soap_malloc(soap, sizeof(struct ns1__TReturnStatus))) == NULL)
        return(SOAP_EOM);
    
    /* Assign the repp response structure to the output parameter rep */
    rep->srmRmdirResponse = repp;
    
    /* Initialize xmlrpc error-handling environment. */
    xmlrpc_env_init(&env);
    
    /* Initialize xmlrpc input structure */
    inputParam = xmlrpc_struct_new(&env);
    
    /**************************** Encode VOMS attibutes ***************************/
    error = encode_VOMSAttributes(func, &env, soap, req->authorizationID, inputParam);
    if (error) {
        repp->returnStatus->explanation = "Error encoding VOMS attributes"; 
        repp->returnStatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
        xmlrpc_DECREF(inputParam);
        /* Clean up our error-handling environment. */
        xmlrpc_env_clean(&env);
        return(SOAP_OK);
    }

    /******************************************************************************/
    /********          Encode parameters for Rmdir (SRM v2.2)              ********/  
    /******************************************************************************/      
    
    /** OPTIONAL ************ (1) Encode authorizationID (char *) ****************************/
    error = encode_string(func, &env,req->authorizationID, SRM_PARAM_authorizationID, inputParam);
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
    
    /** MANDATORY ************ (2) Encode SURL ***************************************************/
    error = encode_string(func, &env, req->SURL, SRM_PARAM_SURL, inputParam);
    if (error) {  
        if (error == ENCODE_ERR_MISSING_PARAM) {
            repp->returnStatus->statusCode = SRM_USCOREINVALID_USCOREREQUEST;
            repp->returnStatus->explanation = "Error: parameter SURL is required";
        }
        else {
            repp->returnStatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
            repp->returnStatus->explanation = "Error encoding SURL parameter";
        }
        xmlrpc_DECREF(inputParam);
        xmlrpc_env_clean(&env);
        return(SOAP_OK);
    }
    
    
    /** OPTIONAL ************* (3) Encode storageSystemInfo **************************************/
    error = encode_ArrayOfTExtraInfo(func, &env, req->storageSystemInfo, SRM_PARAM_storageSystemInfo, inputParam);
    if (error) {
        if (error != ENCODE_ERR_MISSING_PARAM) {
            repp->returnStatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
            repp->returnStatus->explanation = "Error encoding storageSystemInfo";
            xmlrpc_DECREF(inputParam);
            xmlrpc_env_clean(&env);
            return(SOAP_OK);
        }
        xmlrpc_env_clean(&env);
        xmlrpc_env_init(&env);
    }
    
    /** OPTIONAL ************* (4) Encode recurisve **************************************/
    error = encode_bool(func, &env, req->recursive, "recursive", inputParam);
    if (error) {
        if (error != ENCODE_ERR_MISSING_PARAM) {
            repp->returnStatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
            repp->returnStatus->explanation = "Error encoding recurisve";
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
    RmdirResponseHandlerInput.soap = soap;
    RmdirResponseHandlerInput.req  = req;
    RmdirResponseHandlerInput.repp = repp;
    RmdirResponseHandlerInput.RPCTerminated = 0;
                                                                        
    /* Make remote procedure call, calling Backend server */
    xmlrpc_client_call_asynch(xmlrpc_endpoint, methodName_rmdir, (void *) &rpcResponseHandler_Rmdir,
                              &RmdirResponseHandlerInput, "(S)", inputParam);
                              
    srmlogit(STORM_LOG_DEBUG, func, "Asynchronous RPC started\n");

    while (RmdirResponseHandlerInput.RPCTerminated == 0) 
        xmlrpc_client_event_loop_finish_asynch_timeout(RPC_ASYNCH_TIMEOUT);
    
    xmlrpc_DECREF(inputParam);

	srmlogit(STORM_LOG_INFO, func, "Request done. Status: %s\n", reconvertStatusCode(repp->returnStatus->statusCode));
	
    return(SOAP_OK);
}

/***************************************************************************************/
/*************************           SRM v2.2 Rm             *************************/
/***************************************************************************************/

/* Data structure needed to give input parameters to the response handler function */
struct RPC_ResponseHandlerInput_Rm {
    struct soap *soap;
    struct ns1__srmRmRequest *req;
    struct ns1__srmRmResponse *repp;
    int RPCTerminated;
};

/* Response handler for the RPC asynchronous call of the Rm function */
void rpcResponseHandler_Rm(const char          *serverUrl, 
                           const char          *method_name, 
                           const xmlrpc_value  *param_array, 
                           const void          *user_data, 
                           const xmlrpc_env    *faultP, 
                           xmlrpc_value        *result)
{
    static const char *func = "rpcResponseHandler_Rm";
    struct RPC_ResponseHandlerInput_Rm *input;
    struct soap *soap;
    struct ns1__srmRmRequest *req;
    struct ns1__srmRmResponse *repp;
    struct ns1__TSURLReturnStatus *repfilep;
    int error;
    xmlrpc_env env;
    
    input = (struct RPC_ResponseHandlerInput_Rm *) user_data;
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
    error = decode_globalTReturnStatus(func,&env, soap, repp->returnStatus, result);
    if (error != 0) {
        repp->returnStatus->explanation = "Error: unable to parse the BE response or empty response";
        repp->returnStatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
        xmlrpc_env_clean(&env);
        input->RPCTerminated = 1;
        return;
    }
    
    /** OPTIONAL ************* (2) Decode arrayOfFileStatuses (in ArrayOfTSURLReturnStatus *) **********/
    error = decode_ArrayOfTSURLReturnStatus(func, &env, soap, &(repp->arrayOfFileStatuses), SRM_PARAM_arrayOfFileStatuses, result);
    if (error != 0) {
        if (error != DECODE_ERR_NOT_FOUND) {
        	repp->returnStatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
            if (error == DECODE_ERR_SOAP_MEMORY_ERROR) {
                repp->returnStatus->explanation = "Memory allocation error";
                input->RPCTerminated = 2;
            } else {
                repp->returnStatus->explanation = "Error parsing arrayOfFileStatuses";
                input->RPCTerminated = 1;
            }
            xmlrpc_env_clean(&env);
            return;
        }
    }
	
	xmlrpc_env_clean(&env);
    input->RPCTerminated = 1;
    return;
}



int ns1__srmRm(struct soap *soap, struct ns1__srmRmRequest *req,
               struct ns1__srmRmResponse_ *rep)
{
    static const char *func = "Rm";
    static const char *methodName_rm = "synchcall.rm";
    struct ns1__srmRmResponse *repp;
    struct RPC_ResponseHandlerInput_Rm RmResponseHandlerInput;
    int error = 0;
    xmlrpc_env env;
    xmlrpc_value *inputParam;
    
    /************************ Allocate response structure *******************************/
    if ((repp = soap_malloc(soap, sizeof(struct ns1__srmRmResponse))) == NULL)
        return (SOAP_EOM);
    /* Initialize the other filed of the repp structure */
    repp->arrayOfFileStatuses = NULL;
    
    if ((repp->returnStatus = soap_malloc(soap, sizeof(struct ns1__TReturnStatus))) == NULL)
        return (SOAP_EOM);

    /* Assign the repp response structure to the output parameter rep */
    rep->srmRmResponse = repp;
    
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
        /* Clean up our error-handling environment. */
        xmlrpc_env_clean(&env);
        return(SOAP_OK);
    }
    
    /******************************************************************************/
    /********          Encode parameters for Rm    (SRM v2.2)              ********/  
    /******************************************************************************/
    /** OPTIONAL ************ (1) Encode authorizationID (char *) ****************************/
    error = encode_string(func, &env,req->authorizationID, SRM_PARAM_authorizationID, inputParam);
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
    
    /** MANDATORY *********** (2) Encode arrayOfSURLs (TSurlInfo[]) ****************************/
    error = encode_ArrayOfAnyURI(func, &env, req->arrayOfSURLs, SRM_PARAM_arrayOfSURLs, inputParam);
    if (error) {  
        if (error == ENCODE_ERR_MISSING_PARAM) {
            repp->returnStatus->statusCode = SRM_USCOREINVALID_USCOREREQUEST;
            repp->returnStatus->explanation = "Error: parameter arrayOfSURLs is required";
        }
        else {
            repp->returnStatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
            repp->returnStatus->explanation = "Error encoding arrayOfSURLs parameter";
        }
        xmlrpc_DECREF(inputParam);
        xmlrpc_env_clean(&env);
        return(SOAP_OK);
    }
    
    /** OPTIONAL ************* (3) Encode storageSystemInfo **************************************/
    error = encode_ArrayOfTExtraInfo(func, &env, req->storageSystemInfo, SRM_PARAM_storageSystemInfo, inputParam);
    if (error) {
        if (error != ENCODE_ERR_MISSING_PARAM) {
            repp->returnStatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
            repp->returnStatus->explanation = "Error encoding storageSystemInfo";
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
    RmResponseHandlerInput.soap = soap;
    RmResponseHandlerInput.req  = req;
    RmResponseHandlerInput.repp = repp;
    RmResponseHandlerInput.RPCTerminated = 0;
                                                                        
    /* Make remote procedure call, i.e. call Backend server */
    xmlrpc_client_call_asynch(xmlrpc_endpoint, methodName_rm, (void *) &rpcResponseHandler_Rm,
                              &RmResponseHandlerInput, "(S)", inputParam);
                              
    srmlogit(STORM_LOG_DEBUG, func, "Asynchronous RPC started.\n");

    while (RmResponseHandlerInput.RPCTerminated == 0) 
        xmlrpc_client_event_loop_finish_asynch_timeout(RPC_ASYNCH_TIMEOUT);
    
    xmlrpc_DECREF(inputParam);

    if (RmResponseHandlerInput.RPCTerminated == 2) {
    	srmlogit(STORM_LOG_ERROR, func, "Request done. Error: out of memory.\n");
    	return(SOAP_EOM);
    }
    
    srmlogit(STORM_LOG_INFO, func, "Request done. Status: %s\n", reconvertStatusCode(repp->returnStatus->statusCode));
    
    return(SOAP_OK);
}


/***************************************************************************************/
/*************************           SRM v2.2 Ls               *************************/
/***************************************************************************************/

/* Data structure needed to give input parameters to the response handler function */
struct RPC_ResponseHandlerInput_Ls {
    struct soap *soap;
    struct ns1__srmLsRequest *req;
    struct ns1__srmLsResponse *repp;
    int RPCTerminated;
};

/* Response handler for the RPC asynchronous call of the Ls function */
void rpcResponseHandler_Ls(const char          *serverUrl, 
                           const char          *method_name, 
                           const xmlrpc_value  *param_array, 
                           const void          *user_data, 
                           const xmlrpc_env    *faultP, 
                           xmlrpc_value        *result)
{
    static const char *func = "rpcResponseHandler_Ls";
    struct RPC_ResponseHandlerInput_Ls *input;
    struct ns1__srmLsResponse *repp;
    struct ns1__srmLsRequest *req;
    struct soap *soap;
    xmlrpc_env env;
    int error = 0;
 
    srmlogit(STORM_LOG_DEBUG, func, "Response handler started.\n");

    input = (struct RPC_ResponseHandlerInput_Ls *) user_data;
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
    error = decode_globalTReturnStatus(func,&env, soap, repp->returnStatus, result);
    if (error != 0) {
        repp->returnStatus->explanation = "Error: unable to parse the BE response or empty response";
        repp->returnStatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
        xmlrpc_env_clean(&env);
        input->RPCTerminated = 1;
        return;
    }

    /** OPTIONAL ************* (2) Decode requestToken (in char *) ***************************/
    error = decode_string(func, &env, soap, &(repp->requestToken), "requestToken", result);
	if (error != 0) {
        if (error != DECODE_ERR_NOT_FOUND) {
        	repp->returnStatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
            if (error == DECODE_ERR_SOAP_MEMORY_ERROR) {
                repp->returnStatus->explanation = "Memory allocation error";
                input->RPCTerminated = 2;
            } else {
                repp->returnStatus->explanation = "Error parsing requestToken";
                input->RPCTerminated = 1;
            }
            xmlrpc_env_clean(&env);
            return;
        }
        xmlrpc_env_clean(&env);
        xmlrpc_env_init(&env);
    }
    
	/** OPTIONAL ************* (3) Decode details (in TMetaDataPathDetail *) *************************/    
    error =  decode_ArrayOfTMetaDataPathDetail(func, &env, soap, &(repp->details), SRM_PARAM_details, result);
    if (error != 0) {
        if (error != DECODE_ERR_NOT_FOUND) {
        	repp->returnStatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
            if (error == DECODE_ERR_SOAP_MEMORY_ERROR) {
                repp->returnStatus->explanation = "Memory allocation error";
                input->RPCTerminated = 2;
            } else {
                repp->returnStatus->explanation = "Error parsing details";
                input->RPCTerminated = 1;
            }
            xmlrpc_env_clean(&env);
            return;
        }
    }  
    
    /* Clean up our error-handling environment. */
    xmlrpc_env_clean(&env);
    input->RPCTerminated = 1;
    return;
}

int ns1__srmLs (struct soap *soap, struct ns1__srmLsRequest *req,
                struct ns1__srmLsResponse_ *rep)
{
    static const char *func = "Ls";
    static const char *methodName_ls = "synchcall.ls";
    struct RPC_ResponseHandlerInput_Ls LsResponseHandlerInput;
    struct ns1__srmLsResponse *repp;
    int error;
    xmlrpc_env env;
    xmlrpc_value *inputParam;

    /************************** Allocate response structure ***************************/
    if ((repp = soap_malloc(soap, sizeof(struct ns1__srmLsResponse))) == NULL)
        return (SOAP_EOM);
        
    /* Initialize the other filed of the repp structure */
    repp->details = NULL;
    repp->requestToken = NULL;
    
    if ((repp->returnStatus = soap_malloc(soap, sizeof(struct ns1__TReturnStatus))) == NULL)
        return (SOAP_EOM);
    
    /* Assign the repp response structure to the output parameter rep */
    rep->srmLsResponse = repp;
    
    /* Initialize xmlrpc error-handling environment. */
    xmlrpc_env_init(&env);
                                                                            
    inputParam = xmlrpc_struct_new(&env);
    
    
    /**************************** Encode VOMS attibutes ***************************/
    error = encode_VOMSAttributes(func, &env, soap, req->authorizationID, inputParam);
    if (error) {
        repp->returnStatus->explanation = "Error encoding VOMS attributes"; 
        repp->returnStatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
        xmlrpc_DECREF(inputParam);
        /* Clean up our error-handling environment. */
        xmlrpc_env_clean(&env);
        return(SOAP_OK);
    }
    
    /******************************************************************************/
    /********            Encode parameters for Ls (SRM v2.2)               ********/  
    /******************************************************************************/
    /* IMPORTANT: Also for optional parameter, in case of parameter 
     * specified by client, IF error occours during xmlrpc parsing,
     *  we  prefere to abort the request execution.                               */
      
    /** OPTIONAL ************ (1) Encode authorizationID (char *) ****************************/
    error = encode_string(func, &env,req->authorizationID, SRM_PARAM_authorizationID, inputParam);
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

    /** MANDATORY *********** (2) Encode arrayOfSURLs ***************************************/
    error = encode_ArrayOfAnyURI(func, &env, req->arrayOfSURLs, SRM_PARAM_arrayOfSURLs, inputParam);
    if (error) {  
        if (error == ENCODE_ERR_MISSING_PARAM) {
            repp->returnStatus->statusCode = SRM_USCOREINVALID_USCOREREQUEST;
            repp->returnStatus->explanation = "Error: parameter arrayOfSURLs is required";
        }
        else {
            repp->returnStatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
            repp->returnStatus->explanation = "Error encoding arrayOfSURLs parameter";
        }
        xmlrpc_DECREF(inputParam);
        xmlrpc_env_clean(&env);
        return(SOAP_OK);
    }
  
    /** OPTINAL ************** (3) Encode storageSystemInfo **************************************/
    error = encode_ArrayOfTExtraInfo(func, &env, req->storageSystemInfo, SRM_PARAM_storageSystemInfo, inputParam);
    if (error) {
        if (error != ENCODE_ERR_MISSING_PARAM) {
            repp->returnStatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
            repp->returnStatus->explanation = "Error encoding storageSystemInfo";
            xmlrpc_DECREF(inputParam);
            xmlrpc_env_clean(&env);
            return(SOAP_OK);
        }
        xmlrpc_env_clean(&env);
        xmlrpc_env_init(&env);
    }

    /** OPTINAL ************** (4) Encode fileStorageType **************************************/   
    error = encode_int(func, &env, (int*)req->fileStorageType, SRM_PARAM_fileStorageType, inputParam);
    if (error) {
        if (error != ENCODE_ERR_MISSING_PARAM) {
            repp->returnStatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
            repp->returnStatus->explanation = "Error encoding fileStorageType";
            xmlrpc_DECREF(inputParam);
            xmlrpc_env_clean(&env);
            return(SOAP_OK);
        }
        xmlrpc_env_clean(&env);
        xmlrpc_env_init(&env);
    }
    
    /** OPTIONAL ************* (5) Encode fullDetailedList **************************************/   
    error = encode_bool(func, &env, req->fullDetailedList, SRM_PARAM_fullDetailedList, inputParam);
    if (error) {
        if (error != ENCODE_ERR_MISSING_PARAM) {
            repp->returnStatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
            repp->returnStatus->explanation = "Error encoding fullDetailedList";
            xmlrpc_DECREF(inputParam);
            xmlrpc_env_clean(&env);
            return(SOAP_OK);
        }
        xmlrpc_env_clean(&env);
        xmlrpc_env_init(&env);
    }
  
    /** OPTIONAL ************* (6) Encode allLevelRecursive **************************************/   
    error = encode_bool(func, &env, req->allLevelRecursive, SRM_PARAM_allLevelRecursive, inputParam);
    if (error) {
        if (error != ENCODE_ERR_MISSING_PARAM) {
            repp->returnStatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
            repp->returnStatus->explanation = "Error encoding allLevelRecursive";
            xmlrpc_DECREF(inputParam);
            xmlrpc_env_clean(&env);
            return(SOAP_OK);
        }
        xmlrpc_env_clean(&env);
        xmlrpc_env_init(&env);
    }
    
    /** OPTIONAL ************* (7) Encode numOfLevels **************************************/   
    error = encode_int(func, &env, req->numOfLevels, SRM_PARAM_numOfLevels, inputParam);
    if (error) {
        if (error != ENCODE_ERR_MISSING_PARAM) {
            repp->returnStatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
            repp->returnStatus->explanation = "Error encoding numOfLevels";
            xmlrpc_DECREF(inputParam);
            xmlrpc_env_clean(&env);
            return(SOAP_OK);
        }
        xmlrpc_env_clean(&env);
        xmlrpc_env_init(&env);
    }
    
    /** OPTIONAL ************* (8) Encode offset **************************************/   
    error = encode_int(func, &env, req->offset, SRM_PARAM_offset, inputParam);
    if (error) {
        if (error != ENCODE_ERR_MISSING_PARAM) {
            repp->returnStatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
            repp->returnStatus->explanation = "Error encoding offset";
            xmlrpc_DECREF(inputParam);
            xmlrpc_env_clean(&env);
            return(SOAP_OK);
        }
        xmlrpc_env_clean(&env);
        xmlrpc_env_init(&env);
    }
    
    /** OPTIONAL ************* (9) Encode count **************************************/   
    error = encode_int(func, &env, req->count, SRM_PARAM_count, inputParam);
    if (error) {
        if (error != ENCODE_ERR_MISSING_PARAM) {
            repp->returnStatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
            repp->returnStatus->explanation = "Error encoding count";
            xmlrpc_DECREF(inputParam);
            xmlrpc_env_clean(&env);
            return(SOAP_OK);
        }
        xmlrpc_env_clean(&env);
        xmlrpc_env_init(&env);
    }
    
    
    srmlogit(STORM_LOG_DEBUG, func, "Input parameters encoded!\n");

    // Allow for a big array to be returned                                                          
    xmlrpc_limit_set(XMLRPC_XML_SIZE_LIMIT_ID, (100 * XMLRPC_XML_SIZE_LIMIT_DEFAULT));
   
    /* Clean up our error-handling environment. */
    xmlrpc_env_clean(&env);

    /* Define the structure to give as input to the RPC response handler */
    LsResponseHandlerInput.soap = soap;
    LsResponseHandlerInput.req  = req;
    LsResponseHandlerInput.repp = repp;
    LsResponseHandlerInput.RPCTerminated = 0;

    /* Make remote procedure call, i.e. call Backend server */
    xmlrpc_client_call_asynch(xmlrpc_endpoint, methodName_ls, (void *) rpcResponseHandler_Ls,
                              &LsResponseHandlerInput, "(S)", inputParam);

    srmlogit(STORM_LOG_INFO, func, "Asynchronous RPC started.\n");

    while (LsResponseHandlerInput.RPCTerminated == 0) 
        xmlrpc_client_event_loop_finish_asynch_timeout(RPC_ASYNCH_TIMEOUT);
    
    /* Free memory for xmlrpc_value* pointers */
    xmlrpc_DECREF(inputParam);
    
    if (LsResponseHandlerInput.RPCTerminated == 2) {
    	srmlogit(STORM_LOG_ERROR, func, "Request done. Error: out of memory.\n");
    	return(SOAP_EOM);
    }
    
	srmlogit(STORM_LOG_INFO, func, "Request done. Status: %s\n", reconvertStatusCode(repp->returnStatus->statusCode));
	    
    return(SOAP_OK);
}

/**
 *@TODO New function introduced in srm v2.2 
 */
int ns1__srmStatusOfLsRequest(struct soap *soap,
                              struct ns1__srmStatusOfLsRequestRequest *req,
                              struct ns1__srmStatusOfLsRequestResponse_ *rep)
{
    static const char *func = "StatusOfLsRequest";
    struct ns1__srmStatusOfLsRequestResponse *repp;
    
    /************************ Allocate response structure *******************************/
    if ((repp = soap_malloc(soap, sizeof(struct ns1__srmStatusOfLsRequestResponse))) == NULL)
        return (SOAP_EOM);
    repp->details = NULL;
    
    if ((repp->returnStatus = soap_malloc(soap, sizeof(struct ns1__TReturnStatus))) == NULL)
        return (SOAP_EOM);
    repp->returnStatus->explanation = "Not supported, srmLs is synchronous";
    repp->returnStatus->statusCode = SRM_USCORENOT_USCORESUPPORTED;

    /* Assign the repp response structure to the output parameter rep */
    rep->srmStatusOfLsRequestResponse = repp;
    
	srmlogit(STORM_LOG_INFO, func, "Request done. Status: %s\n", reconvertStatusCode(repp->returnStatus->statusCode));

    return (SOAP_OK);
}



/**
 * SrmMv()
 */

/* Data structure needed to give input parameters to the response handler function */
struct RPC_ResponseHandlerInput_Mv {
    struct soap *soap;
    struct ns1__srmMvRequest *req;
    struct ns1__srmMvResponse *repp;
    int RPCTerminated;
};

    
/* Response handler for the RPC asynchronous call of the SrmMv function */
void rpcResponseHandler_Mv (const char          *serverUrl, 
                              const char          *method_name, 
                              const xmlrpc_value  *param_array, 
                              const void          *user_data, 
                              const xmlrpc_env    *faultP, 
                              xmlrpc_value        *result)
{   
    static const char *func= "rpcResponseHandler_Mv";
    struct RPC_ResponseHandlerInput_Mv *input;
    struct soap *soap;
    struct ns1__srmMvRequest *req;
    struct ns1__srmMvResponse *repp;
    xmlrpc_env env;
    int error = 0;
  
    input = (struct RPC_ResponseHandlerInput_Mv *) user_data;
    soap = input->soap;
    req = input->req;
    repp = input->repp;
    
    srmlogit(STORM_LOG_DEBUG, func, "Inside the response handler: %s\n", req->fromSURL);
    
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

    /* Clean up our error-handling environment. */
    xmlrpc_env_clean(&env);
    input->RPCTerminated = 1;
    
    srmlogit(STORM_LOG_DEBUG, func, "Mv Response parsed! %s\n", req->fromSURL);
    
    return;
}


int ns1__srmMv (struct soap *soap, struct ns1__srmMvRequest *req, struct ns1__srmMvResponse_ *rep)
{
    static const char *methodName_mv = "synchcall.mv";
    char *func = "Mv";
    struct ns1__srmMvResponse *repp;
    struct RPC_ResponseHandlerInput_Mv MvResponseHandlerInput;
    int error;
    xmlrpc_env env;
    xmlrpc_value *inputParam;
  
    /* Allocate the response structure */
    if ((repp = soap_malloc(soap, sizeof(struct ns1__srmMvResponse))) == NULL)
        return(SOAP_EOM);
    if ((repp->returnStatus = soap_malloc(soap, sizeof(struct ns1__TReturnStatus))) == NULL)
        return(SOAP_EOM);
    
    /* Assign the repp response structure to the output parameter rep */
    rep->srmMvResponse = repp;
    
    /* Initialize xmlrpc error-handling environment. */
    xmlrpc_env_init(&env);
    
    /* Initialize xmlrpc input structure */
    inputParam = xmlrpc_struct_new(&env);
    
    /**************************** Encode VOMS attibutes ***************************/
    error = encode_VOMSAttributes(func, &env, soap, req->authorizationID, inputParam);
    if (error) {
        repp->returnStatus->explanation = "Error encoding VOMS attributes"; 
        repp->returnStatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
        xmlrpc_DECREF(inputParam);
        /* Clean up our error-handling environment. */
        xmlrpc_env_clean(&env);
        return(SOAP_OK);
    }

    /******************************************************************************/
    /********          Encode parameters for Mv (SRM v2.2)              ********/  
    /******************************************************************************/      
    
    /** OPTIONAL ************ (1) Encode authorizationID (char *) ****************************/
    error = encode_string(func, &env,req->authorizationID, SRM_PARAM_authorizationID, inputParam);
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
    
    /** MANDATORY ************ (2) Encode fromSURL ***************************************************/
    error = encode_string(func, &env, req->fromSURL, SRM_PARAM_fromSURL, inputParam);
    if (error) {  
        if (error == ENCODE_ERR_MISSING_PARAM) {
            repp->returnStatus->statusCode = SRM_USCOREINVALID_USCOREREQUEST;
            repp->returnStatus->explanation = "Error: parameter fromSURL is required";
        }
        else {
            repp->returnStatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
            repp->returnStatus->explanation = "Error encoding fromSURL parameter";
        }
        xmlrpc_DECREF(inputParam);
        xmlrpc_env_clean(&env);
        return(SOAP_OK);
    }
   
   
    /** MANDATORY ************ (3) Encode toSURL ***************************************************/
    error = encode_string(func, &env, req->toSURL, SRM_PARAM_toSURL, inputParam);
    if (error) {  
        if (error == ENCODE_ERR_MISSING_PARAM) {
            repp->returnStatus->statusCode = SRM_USCOREINVALID_USCOREREQUEST;
            repp->returnStatus->explanation = "Error: parameter toSURL is required";
        }
        else {
            repp->returnStatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
            repp->returnStatus->explanation = "Error encoding toSURL parameter";
        }
        xmlrpc_DECREF(inputParam);
        xmlrpc_env_clean(&env);
        return(SOAP_OK);
    }
    
    
    /** OPTIONAL ************* (4) Encode storageSystemInfo **************************************/
    error = encode_ArrayOfTExtraInfo(func, &env, req->storageSystemInfo, SRM_PARAM_storageSystemInfo, inputParam);
    if (error) {
        if (error != ENCODE_ERR_MISSING_PARAM) {
            repp->returnStatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
            repp->returnStatus->explanation = "Error encoding storageSystemInfo";
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
    MvResponseHandlerInput.soap = soap;
    MvResponseHandlerInput.req  = req;
    MvResponseHandlerInput.repp = repp;
    MvResponseHandlerInput.RPCTerminated = 0;
                                                                        
    /* Make remote procedure call, calling Backend server */
    xmlrpc_client_call_asynch(xmlrpc_endpoint, methodName_mv, (void *) &rpcResponseHandler_Mv,
                              &MvResponseHandlerInput, "(S)", inputParam);
                              
    srmlogit(STORM_LOG_DEBUG, func, "Asynchronous RPC started: %s\n", req->fromSURL);

    while (MvResponseHandlerInput.RPCTerminated == 0) 
        xmlrpc_client_event_loop_finish_asynch_timeout(RPC_ASYNCH_TIMEOUT);
    
    xmlrpc_DECREF(inputParam);

    srmlogit(STORM_LOG_INFO, func, "Request done. Status: %s\n", reconvertStatusCode(repp->returnStatus->statusCode));
    
    return(SOAP_OK);
}
