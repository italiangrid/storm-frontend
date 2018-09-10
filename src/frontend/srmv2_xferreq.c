/* Copyright (c) Istituto Nazionale di Fisica Nucleare (INFN). 2006-2010.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/

#include "storm_functions.h"
#include "srm_server.h"
#include "srmv2H.h"
#include "storm_util.h"
#include "srmlogit.h"
#include "xmlrpc_decode.h"
#include "xmlrpc_encode.h"
#include "frontend_version.h"
#include <xmlrpc-c/util.h>

extern int nb_supported_protocols;
extern char **supported_protocols;

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


int ns1__srmReleaseFiles_impl (struct soap *soap,
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
    xmlrpc_value *result;

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
    XMLRPC_ASSERT_ENV_OK((&env));

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
        if (error != ENCODE_ERR_MISSING_PARAM && (! DONT_FAIL_FOR_AUTHORIZATION_ID)) {
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
    srmlogit(STORM_LOG_DEBUG, func, "Making RPC.\n");
    result = xmlrpc_client_call(&env, xmlrpc_endpoint, methodName, "(S)", inputParam);
    rpcResponseHandler_ReleaseFiles(xmlrpc_endpoint, methodName, NULL,
                              &ReleaseFilesResponseHandlerInput, &env, result);

    xmlrpc_DECREF(inputParam);
    xmlrpc_DECREF(result);

    if (ReleaseFilesResponseHandlerInput.RPCTerminated == 2) {
    	srmlogit(STORM_LOG_ERROR, func, "Request done. Error: out of memory.\n");
        return(SOAP_EOM);
    }

    srmlogit(STORM_LOG_DEBUG, func, "Request done. Status: %s\n", reconvertStatusCode(repp->returnStatus->statusCode));
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


int ns1__srmPutDone_impl(struct soap *soap, struct ns1__srmPutDoneRequest *req, struct ns1__srmPutDoneResponse_ *rep)
{
    static const char *func = "PutDone";
    static const char *methodName = "synchcall.putDone";
    struct ns1__srmPutDoneResponse *repp;
    struct RPC_ResponseHandlerInput_PutDone PutDoneResponseHandlerInput;
    int error;
    xmlrpc_env env;
    xmlrpc_value *inputParam;
    xmlrpc_value *result;

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
    XMLRPC_ASSERT_ENV_OK((&env));

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
        if (error != ENCODE_ERR_MISSING_PARAM && (! DONT_FAIL_FOR_AUTHORIZATION_ID)) {
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
    srmlogit(STORM_LOG_DEBUG, func, "Making RPC.\n");
    result = xmlrpc_client_call(&env, xmlrpc_endpoint, methodName, "(S)", inputParam);
    rpcResponseHandler_PutDone(xmlrpc_endpoint, methodName, NULL,
                              &PutDoneResponseHandlerInput, &env, result);

    xmlrpc_DECREF(inputParam);
    xmlrpc_DECREF(result);

    if (PutDoneResponseHandlerInput.RPCTerminated == 2) {
    	srmlogit(STORM_LOG_ERROR, func, "Request done. Error: out of memory.\n");
        return(SOAP_EOM);
    }

    srmlogit(STORM_LOG_DEBUG, func, "Request done. Status: %s\n", reconvertStatusCode(repp->returnStatus->statusCode));

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


int ns1__srmAbortRequest_impl(struct soap *soap,
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
    xmlrpc_value *result;

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
    XMLRPC_ASSERT_ENV_OK((&env));

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
        if (error != ENCODE_ERR_MISSING_PARAM && (! DONT_FAIL_FOR_AUTHORIZATION_ID)) {
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
    srmlogit(STORM_LOG_DEBUG, func, "Making RPC.\n");
    result = xmlrpc_client_call(&env, xmlrpc_endpoint, methodName, "(S)", inputParam);
    rpcResponseHandler_AbortRequest(xmlrpc_endpoint, methodName, NULL,
                              &AbortRequestResponseHandlerInput, &env, result);

    xmlrpc_DECREF(inputParam);
    xmlrpc_DECREF(result);

    if (AbortRequestResponseHandlerInput.RPCTerminated == 2) {
    	srmlogit(STORM_LOG_ERROR, func, "Request done. Error: out of memory.\n");
        return(SOAP_EOM);
    }

    srmlogit(STORM_LOG_DEBUG, func, "Request done. Status: %s\n", reconvertStatusCode(repp->returnStatus->statusCode));

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


int ns1__srmAbortFiles_impl(struct soap *soap,
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
    xmlrpc_value *result;

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
    XMLRPC_ASSERT_ENV_OK((&env));

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
        if (error != ENCODE_ERR_MISSING_PARAM && (! DONT_FAIL_FOR_AUTHORIZATION_ID)) {
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
    srmlogit(STORM_LOG_DEBUG, func, "Making RPC.\n");
    result = xmlrpc_client_call(&env, xmlrpc_endpoint, methodName, "(S)", inputParam);
    rpcResponseHandler_AbortFiles(xmlrpc_endpoint, methodName, NULL,
                              &AbortFilesResponseHandlerInput, &env, result);

    xmlrpc_DECREF(inputParam);
    xmlrpc_DECREF(result);

    if (AbortFilesResponseHandlerInput.RPCTerminated == 2) {
    	srmlogit(STORM_LOG_ERROR, func, "Request done. Error: out of memory.\n");
        return(SOAP_EOM);
    }

    srmlogit(STORM_LOG_DEBUG, func, "Request done. Status: %s\n", reconvertStatusCode(repp->returnStatus->statusCode));

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


int ns1__srmExtendFileLifeTime_impl(struct soap *soap,
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
    xmlrpc_value *result;

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
    XMLRPC_ASSERT_ENV_OK((&env));

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
        if (error != ENCODE_ERR_MISSING_PARAM && (! DONT_FAIL_FOR_AUTHORIZATION_ID)) {
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
    srmlogit(STORM_LOG_DEBUG, func, "Making RPC.\n");
    result = xmlrpc_client_call(&env, xmlrpc_endpoint, methodName, "(S)", inputParam);
    rpcResponseHandler_ExtendFileLifeTime(xmlrpc_endpoint, methodName, NULL,
                              &ExtendFileLifeTimeResponseHandlerInput, &env, result);

    xmlrpc_DECREF(inputParam);
    xmlrpc_DECREF(result);

    if (ExtendFileLifeTimeResponseHandlerInput.RPCTerminated == 2) {
    	srmlogit(STORM_LOG_ERROR, func, "Request done. Error: out of memory.\n");
        return(SOAP_EOM);
    }

    srmlogit(STORM_LOG_DEBUG, func, "Request done. Status: %s\n", reconvertStatusCode(repp->returnStatus->statusCode));

    return(SOAP_OK);
}

/**
 * This function is to discover what transfer protocols are supported.
 */
int ns1__srmGetTransferProtocols_impl(struct soap* soap,
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

static int set_version_info(struct soap* soap, struct ns1__srmPingResponse *repp);

/*
int get_ns1__srmPingResponse(struct soap* soap, struct ns1__srmPingResponse *repp)
{
	if (NULL == (repp = soap_malloc(soap, sizeof(struct ns1__srmPingResponse)))) {
	        return (SOAP_EOM);
	}
	return (SOAP_OK);
}
*/
/**
 * This functin is used to check the state of the SRM. It works as an "are you alive" type of call.
*/
int ns1__srmPing_impl(struct soap* soap, struct ns1__srmPingRequest *req, struct ns1__srmPingResponse_ *rep)
{
    static const char *func = "Ping";
    static const char *commandPrefix = "CMD:";
    static const char *methodName = "synchcall.ping";
    struct ns1__srmPingResponse *repp;
    struct RPC_ResponseHandlerInput_Ping PingResponseHandlerInput;
    int error;
    char return_version_info;
    char clientdn[256];
    xmlrpc_env env;
    xmlrpc_value *inputParam;
    xmlrpc_value* result;

    /************************ Allocate response structure *******************************/
    if (NULL == (repp = soap_malloc(soap, sizeof(struct ns1__srmPingResponse)))) {
        return (SOAP_EOM);
    }
    repp->versionInfo = NULL;
    repp->otherInfo = NULL;

    /* Assign the repp response structure to the output parameter rep */
    rep->srmPingResponse = repp;

    /* Initialize xmlrpc error-handling environment. */
    xmlrpc_env_init(&env);

    /*Initialize xmlrpc input structure*/
    inputParam = xmlrpc_struct_new(&env);
    XMLRPC_ASSERT_ENV_OK((&env));

    if (req->authorizationID != NULL) {
        // Pass it to the BE and see what happen...
        return_version_info = 0;
        int prefix_lenght = strlen(commandPrefix);
        if (strncmp(req->authorizationID, commandPrefix, prefix_lenght) == 0) {

        }
    } else {
        req->authorizationID = "KEY=BE-Version";
        return_version_info = 1;
    }

    /**************************** Encode VOMS attibutes ***************************/
    error = encode_VOMSAttributes(func, &env, soap, NULL, inputParam);
    if (error) {
        srmlogit(STORM_LOG_ERROR, func, "Error encoding VOMS attributes\n");
        xmlrpc_DECREF(inputParam);
        xmlrpc_env_clean(&env);
        return(SOAP_OK);
    }

    /** OPTIONAL ************ (1) Encode authorizationID (char *) ****************************/
    error = encode_string(func, &env, req->authorizationID, SRM_PARAM_authorizationID, inputParam);
    if (error) {
        if (error != ENCODE_ERR_MISSING_PARAM && (! DONT_FAIL_FOR_AUTHORIZATION_ID)) {
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
    srmlogit(STORM_LOG_DEBUG, func, "Making RPC.\n");
    result = xmlrpc_client_call(&env, xmlrpc_endpoint, methodName,
                                "(S)", inputParam);
    rpcResponseHandler_Ping(xmlrpc_endpoint, methodName, NULL,
                            &PingResponseHandlerInput, &env, result);

    xmlrpc_DECREF(inputParam);
    xmlrpc_DECREF(result);

    if (PingResponseHandlerInput.RPCTerminated == 2) {
    	srmlogit(STORM_LOG_ERROR, func, "Request done. Error: out of memory.\n");
        return(SOAP_EOM);
    }

    if (return_version_info) {
        error = set_version_info(soap, repp);
        if (error != SOAP_OK) {
            return error;
        }
    }
    srmlogit(STORM_LOG_DEBUG, func, "Request done.\n");

    return(SOAP_OK);
}

// Function used by ns1__srmPing()
int set_version_info(struct soap* soap, struct ns1__srmPingResponse *repp) {
    struct ns1__TExtraInfo** extraInfoArray;
    char *be_version = "ERROR";
    char *be_os_distribution = "ERROR";
    char *be_key = "BE-Version";
    char *be_os_key = "BE-OS-Distribution";
    char *version_temp = "<FE:%s><BE:%s>";
    char version[50];
    int i;

    // Get the version of the BE and set the information to be returned.
    if (repp->otherInfo != NULL) {
        if (repp->otherInfo->extraInfoArray != NULL) {
            //Introduced BE version check 
            for (i=0; i< repp->otherInfo->__sizeextraInfoArray; i++) {
                srmlogit(STORM_LOG_DEBUG, "set_version_info", "index %d key %s value %s\n", i, repp->otherInfo->extraInfoArray[i]->key, repp->otherInfo->extraInfoArray[i]->value);
                if (strcmp(repp->otherInfo->extraInfoArray[i]->key,be_key)==0 && repp->otherInfo->extraInfoArray[i]->value != NULL) {
                   srmlogit(STORM_LOG_DEBUG, "set_version_info", "be key  %s\n", repp->otherInfo->extraInfoArray[i]->key);
                   be_version = repp->otherInfo->extraInfoArray[i]->value;
                   srmlogit(STORM_LOG_DEBUG, "set_version_info", "be version  %s\n", be_version);
                }
                else if (strcmp(repp->otherInfo->extraInfoArray[i]->key,be_os_key)==0 && repp->otherInfo->extraInfoArray[i]->value != NULL) {
                   srmlogit(STORM_LOG_DEBUG, "set_version_info", "be os key  %s\n", repp->otherInfo->extraInfoArray[i]->key);
                   be_os_distribution = repp->otherInfo->extraInfoArray[i]->value;
                   srmlogit(STORM_LOG_DEBUG, "set_version_info", "be os distribution  %s\n", be_os_distribution);
                }
            }
            //Obsoleted
            //if (repp->otherInfo->extraInfoArray[0]->value != NULL) {
            //    be_version = repp->otherInfo->extraInfoArray[0]->value;
            //}
        }
    }
    repp->versionInfo = "v2.2";

    // Allocate memory for the response structure
    repp->otherInfo = soap_malloc(soap, sizeof(struct ns1__ArrayOfTExtraInfo));
    if (NULL == repp->otherInfo) {
        return(SOAP_EOM);
    }
    repp->otherInfo->extraInfoArray = soap_malloc(soap, 3*sizeof(struct ns1__TExtraInfo));
    if (NULL == repp->otherInfo->extraInfoArray) {
        return(SOAP_EOM);
    }
    repp->otherInfo->__sizeextraInfoArray = 3;

    extraInfoArray = repp->otherInfo->extraInfoArray;

    // Set backend type
    extraInfoArray[0] = soap_malloc(soap, sizeof(struct ns1__TExtraInfo));
    extraInfoArray[0]->key = "backend_type";
    extraInfoArray[0]->value = "StoRM";
    // Set backend version
    extraInfoArray[1] = soap_malloc(soap, sizeof(struct ns1__TExtraInfo));
    extraInfoArray[1]->key = "backend_version";
    sprintf(version, version_temp, frontend_version, be_version);
    extraInfoArray[1]->value = soap_strdup(soap, version);
    // Set backend os distribution
    extraInfoArray[2] = soap_malloc(soap, sizeof(struct ns1__TExtraInfo));
    extraInfoArray[2]->key = "backend_os_distribution";
    extraInfoArray[2]->value = soap_strdup(soap, be_os_distribution);

    return SOAP_OK;
}


int ns1__srmSuspendRequest_impl(struct soap *soap,
                           struct ns1__srmSuspendRequestRequest *req,
                           struct ns1__srmSuspendRequestResponse_ *rep)
{
    static const char *func = "SuspendRequest";
    struct ns1__srmSuspendRequestResponse *repp;

    if ((repp = soap_malloc(soap, sizeof(struct ns1__srmSuspendRequestResponse))) == NULL ||
        (repp->returnStatus = soap_malloc (soap, sizeof(struct ns1__TReturnStatus))) == NULL) {
        return(SOAP_EOM);
    }

    repp->returnStatus->explanation = NULL;
    repp->returnStatus->statusCode = SRM_USCORENOT_USCORESUPPORTED;
    rep->srmSuspendRequestResponse = repp;

    srmlogit(STORM_LOG_DEBUG, func, "Result: SRM_NOT_SUPPORTED");

    return(SOAP_OK);
}


int ns1__srmResumeRequest_impl(struct soap *soap,
                          struct ns1__srmResumeRequestRequest *req,
                          struct ns1__srmResumeRequestResponse_ *rep)
{
    static const char *func = "ResumeRequest";
    struct ns1__srmResumeRequestResponse *repp;

    if ((repp = soap_malloc(soap, sizeof(struct ns1__srmResumeRequestResponse))) == NULL ||
        (repp->returnStatus = soap_malloc (soap, sizeof(struct ns1__TReturnStatus))) == NULL) {
        return(SOAP_EOM);
    }

    repp->returnStatus->explanation = NULL;
    repp->returnStatus->statusCode = SRM_USCORENOT_USCORESUPPORTED;
    rep->srmResumeRequestResponse = repp;

    srmlogit(STORM_LOG_DEBUG, func, "Result: SRM_NOT_SUPPORTED");

    return(SOAP_OK);
}

/* uncomment this to try using this cpp code from c code
struct Credentials;
struct Authorization;
int isBlacklisted(struct soap *soap)
{
	struct Credentials* c;
	struct Authorization* a;
	int* blacklisted;
	char *func = "isBlacklisted";
	srmlogit(STORM_LOG_ERROR, func, "calling call_create_credentials\n");
	if(call_create_credentials(soap, c) == 0)
	{
		srmlogit(STORM_LOG_ERROR, func, "calling call_create_authorization\n");
		int lalla = call_create_authorization(c, a);
		if(lalla == 0)
		{
			srmlogit(STORM_LOG_ERROR, func, "calling call_isBlacklisted\n");
			if (call_isBlacklisted(a, blacklisted) == 0)
			{
				srmlogit(STORM_LOG_ERROR, func, "fine is black\n");
				return *blacklisted;
			} else
			{
				srmlogit(STORM_LOG_ERROR, func, "fine is NOT black\n");
				return 0;
			}
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return 0;
	}
}
*/
