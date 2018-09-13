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
#include "srmv2H.h"
#include "storm_util.h"
#include "srmlogit.h"
#include "xmlrpc_encode.hpp"
#include "xmlrpc_decode.hpp"
#include <xmlrpc-c/base.h>
#include <xmlrpc-c/client.h>
#include <xmlrpc-c/util.h>
#include "xmlrpc_client.hpp"

struct RPC_ResponseHandlerInput_ReserveSpace {
    struct soap *soap;
    struct ns1__srmReserveSpaceRequest *req;
    struct ns1__srmReserveSpaceResponse *repp;
    int RPCTerminated;
};

void rpcResponseHandler_ReserveSpace(const char         *serverUrl, 
                                     const char         *method_name, 
                                     const xmlrpc_value *param_array, 
                                     const void         *user_data, 
                                     const xmlrpc_env   *faultP, 
                                     xmlrpc_value       *result)
{
    static const char *func = "rpcResponseHandler_ReserveSpace";
    struct ns1__srmReserveSpaceRequest *req;
    struct ns1__srmReserveSpaceResponse *repp;
    struct RPC_ResponseHandlerInput_ReserveSpace *input;
    struct soap *soap;
    int error;
    xmlrpc_env env;
    
    input = (struct RPC_ResponseHandlerInput_ReserveSpace *) user_data;
    
    soap = input->soap;
    req = input->req;
    repp = input->repp;

    srmlogit(STORM_LOG_DEBUG, func, "Inside the response handler \n");
 
    if (faultP->fault_occurred) {
        srmlogit(STORM_LOG_ERROR, func, "ERROR: XML-RPC Fault: %s (code: %d)\n", faultP->fault_string, faultP->fault_code);
        repp->returnStatus->explanation = "Connection error to backend server. Check FE server log file for details.";
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
        /* Clean up our error-handling environment. */
        xmlrpc_env_clean(&env);
        input->RPCTerminated = 1;
        return;
    }
    
    /** OPTIONAL ************* (2) Decode requestToken (in char *) *********************************/
    error = decode_string(func, &env, soap, &(repp->requestToken), SRM_PARAM_requestToken, result);
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
    
    /** OPTIONAL ************* (3) Decode estimatedProcessingTime (in int *) *********************************/
    error = decode_int(func, &env, soap, &(repp->estimatedProcessingTime), SRM_PARAM_estimatedProcessingTime, result);
    if (error != 0) {
        if (error != DECODE_ERR_NOT_FOUND) {
            repp->returnStatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
            if (error == DECODE_ERR_SOAP_MEMORY_ERROR) {
                repp->returnStatus->explanation = "Memory allocation error";
                input->RPCTerminated = 2;
            } else {
                repp->returnStatus->explanation = "Error parsing estimatedProcessingTime";
                input->RPCTerminated = 1;
            }
            xmlrpc_env_clean(&env);
            return;
        }
        xmlrpc_env_clean(&env);
        xmlrpc_env_init(&env);
    }
    
    /** OPTIONAL ************* (4) Decode retentionPolicyInfo (in struct ns1__TRetentionPolicyInfo *) *******/
    error = decode_TRetentionPolicyInfo(func, &env, soap, &(repp->retentionPolicyInfo), SRM_PARAM_retentionPolicyInfo, result);
    if (error != 0) {
        if (error != DECODE_ERR_NOT_FOUND) {
            repp->returnStatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
            if (error == DECODE_ERR_SOAP_MEMORY_ERROR) {
                repp->returnStatus->explanation = "Memory allocation error";
                input->RPCTerminated = 2;
            } else {
                repp->returnStatus->explanation = "Error parsing retentionPolicyInfo";
                input->RPCTerminated = 1;
            }
            xmlrpc_env_clean(&env);
            return;
        }
        xmlrpc_env_clean(&env);
        xmlrpc_env_init(&env);
    }
    
    /** OPTIONAL ************* (5) Decode sizeOfTotalReservedSpace (in ULONG64 *) ***********************/
    error = decode_ULONG64(func, &env, soap, &(repp->sizeOfTotalReservedSpace), SRM_PARAM_sizeOfTotalReservedSpace, result);
    if (error != 0) {
        if (error != DECODE_ERR_NOT_FOUND) {
            repp->returnStatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
            if (error == DECODE_ERR_SOAP_MEMORY_ERROR) {
                repp->returnStatus->explanation = "Memory allocation error";
                input->RPCTerminated = 2;
            } else {
                repp->returnStatus->explanation = "Error parsing sizeOfTotalReservedSpace";
                input->RPCTerminated = 1;
            }
            xmlrpc_env_clean(&env);
            return;
        }
        xmlrpc_env_clean(&env);
        xmlrpc_env_init(&env);
    }
    
    /** OPTIONAL ************* (6) Decode sizeOfGuaranteedReservedSpace (in ULONG64 *) ***********************/
    error = decode_ULONG64(func, &env, soap, &(repp->sizeOfGuaranteedReservedSpace), SRM_PARAM_sizeOfGuaranteedReservedSpace, result);
    if (error != 0) {
        if (error != DECODE_ERR_NOT_FOUND) {
            repp->returnStatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
            if (error == DECODE_ERR_SOAP_MEMORY_ERROR) {
                repp->returnStatus->explanation = "Memory allocation error";
                input->RPCTerminated = 2;
            } else {
                repp->returnStatus->explanation = "Error parsing sizeOfGuaranteedReservedSpace";
                input->RPCTerminated = 1;
            }
            xmlrpc_env_clean(&env);
            return;
        }
        xmlrpc_env_clean(&env);
        xmlrpc_env_init(&env);
    }
    
    /** OPTIONAL ************* (7) Decode lifetimeOfReservedSpace (in int *) ***********************/
    error = decode_lifetimeValue(func, &env, soap, &(repp->lifetimeOfReservedSpace), SRM_PARAM_lifetimeOfReservedSpace, result);
    if (error != 0) {
        if (error != DECODE_ERR_NOT_FOUND) {
            repp->returnStatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
            if (error == DECODE_ERR_SOAP_MEMORY_ERROR) {
                repp->returnStatus->explanation = "Memory allocation error";
                input->RPCTerminated = 2;
            } else {
                repp->returnStatus->explanation = "Error parsing lifetimeOfReservedSpace";
                input->RPCTerminated = 1;
            }
            xmlrpc_env_clean(&env);
            return;
        }
        xmlrpc_env_clean(&env);
        xmlrpc_env_init(&env);
    }
    
    /** OPTIONAL ************* (8) Decode spaceToken (in char *) *********************************/
    error = decode_string(func, &env, soap, &(repp->spaceToken), SRM_PARAM_spaceToken, result);
    if (error != 0) {
        if (error != DECODE_ERR_NOT_FOUND) {
            repp->returnStatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
            if (error == DECODE_ERR_SOAP_MEMORY_ERROR) {
                repp->returnStatus->explanation = "Memory allocation error";
                input->RPCTerminated = 2;
            } else {
                repp->returnStatus->explanation = "Error parsing spaceToken";
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


int ns1__srmReserveSpace_impl(struct soap *soap,
                         struct ns1__srmReserveSpaceRequest *req,
                         struct ns1__srmReserveSpaceResponse_ *rep)
{
    static const char *func = "ReserveSpace";
    static const char *methodName_ReserveSpace = "synchcall.reserveSpace";
    struct ns1__srmReserveSpaceResponse *repp;
    struct RPC_ResponseHandlerInput_ReserveSpace ReserveSpaceResponseHandlerInput;
    int error;
    xmlrpc_env env;
    xmlrpc_value *inputParam;
    xmlrpc_value *result;

    repp = static_cast<ns1__srmReserveSpaceResponse*>(soap_malloc(soap,sizeof(ns1__srmReserveSpaceResponse)));
    if (repp == NULL) return SOAP_EOM;

    repp->returnStatus = static_cast<ns1__TReturnStatus*>(soap_malloc(soap, sizeof(ns1__TReturnStatus)));
    if (repp->returnStatus == NULL) return SOAP_EOM;

    repp->requestToken = NULL;
    repp->estimatedProcessingTime = NULL;
    repp->retentionPolicyInfo = NULL;
    repp->sizeOfTotalReservedSpace = NULL;
    repp->sizeOfGuaranteedReservedSpace = NULL;
    repp->lifetimeOfReservedSpace = NULL;
    repp->spaceToken = NULL;
    repp->returnStatus->explanation = NULL;

    /* Assign the repp response structure to the output parameter rep */
    rep->srmReserveSpaceResponse = repp;
   
    /* Initialize xmlrpc error-handling environment. */     
    xmlrpc_env_init(&env);
    
    /* Initialize xmlrpc input structure */
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
    /********       Encode parameters for ReserveSpace (SRM v2.2)          ********/  
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
    
    /** OPTIONAL ************ (2) Encode userSpaceTokenDescription (char *) ****************************/
    error = encode_userSpaceTokenDescription(func, &env, req->userSpaceTokenDescription, inputParam);
    if (error) {
        if (error != ENCODE_ERR_MISSING_PARAM) {
            repp->returnStatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
            repp->returnStatus->explanation = "Error encoding userSpaceTokenDescription";
            xmlrpc_DECREF(inputParam);
            xmlrpc_env_clean(&env);
            return(SOAP_OK);
        }
        xmlrpc_env_clean(&env);
        xmlrpc_env_init(&env);
    }
    
    /** MANDATORY *********** (3) Encode retentionPolicyInfo (TRetentionPolicyInfo *) ****************************/
    error = encode_retentionPolicyInfo(func, &env, req->retentionPolicyInfo, inputParam);
    if (error) {
        repp->returnStatus->statusCode = SRM_USCOREINVALID_USCOREREQUEST;
        if (error == ENCODE_ERR_MISSING_PARAM) repp->returnStatus->explanation = "Invalid request: retentionPolicyInfo must be specified";
        else repp->returnStatus->explanation = "Error encoding retentionPolicyInfo parameter";
        xmlrpc_DECREF(inputParam);
        xmlrpc_env_clean(&env);
        return(SOAP_OK);
    }
    
    /** OPTIONAL ************ (4) Encode desiredSizeOfTotalSpace (ULONG64 *) ****************************/
    error = encode_ULONG64(func, &env, req->desiredSizeOfTotalSpace, SRM_PARAM_desiredSizeOfTotalSpace, inputParam);
    if (error) {
        if (error != ENCODE_ERR_MISSING_PARAM) {
            repp->returnStatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
            repp->returnStatus->explanation = "Error encoding desiredSizeOfTotalSpace";
            xmlrpc_DECREF(inputParam);
            xmlrpc_env_clean(&env);
            return(SOAP_OK);
        }
        xmlrpc_env_clean(&env);
        xmlrpc_env_init(&env);
    }
    
    /** MANDATORY *********** (5) Encode desiredSizeOfGuaranteedSpace (ULONG64) ****************************/
    error = encode_ULONG64(func, &env, &(req->desiredSizeOfGuaranteedSpace), SRM_PARAM_desiredSizeOfGuaranteedSpace, inputParam);
    if (error) {
        repp->returnStatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
        if (error == ENCODE_ERR_MISSING_PARAM) repp->returnStatus->explanation = "Error: missing desiredSizeOfGuaranteedSpace (mandatory parameter)";
        else repp->returnStatus->explanation = "Error encoding desiredSizeOfGuaranteedSpace parameter";
        xmlrpc_DECREF(inputParam);
        xmlrpc_env_clean(&env);
        return(SOAP_OK);
    }
    
    /** OPTIONAL ************ (6) Encode desiredLifetimeOfReservedSpace (int *) ****************************/
    error = encode_lifetimeValue(func, &env, req->desiredLifetimeOfReservedSpace, SRM_PARAM_desiredLifetimeOfReservedSpace, inputParam);
    if (error) {
        if (error != ENCODE_ERR_MISSING_PARAM) {
            repp->returnStatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
            repp->returnStatus->explanation = "Error encoding desiredLifetimeOfReservedSpace";
            xmlrpc_DECREF(inputParam);
            xmlrpc_env_clean(&env);
            return(SOAP_OK);
        }
        xmlrpc_env_clean(&env);
        xmlrpc_env_init(&env);
    }
    
    /** OPTIONAL ***** (7) Encode arrayOfExpectedFileSizes (struct ns1__ArrayOfUnsignedLong *) ***************/
    error = encode_arrayOfUnsignedLong(func, &env, req->arrayOfExpectedFileSizes, SRM_PARAM_arrayOfExpectedFileSizes, inputParam);
    if (error) {
        if (error != ENCODE_ERR_MISSING_PARAM) {
            repp->returnStatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
            repp->returnStatus->explanation = "Error encoding arrayOfExpectedFileSizes";
            xmlrpc_DECREF(inputParam);
            xmlrpc_env_clean(&env);
            return(SOAP_OK);
        }
        xmlrpc_env_clean(&env);
        xmlrpc_env_init(&env);
    }
    
    /** OPTIONAL ***** (8) Encode storageSystemInfo (struct ns1__ArrayOfTExtraInfo *) ***************/
    if(req->storageSystemInfo != NULL && req->storageSystemInfo->__sizeextraInfoArray >= 1)
	{
		error = encode_ArrayOfTExtraInfo(func, &env, req->storageSystemInfo, "storageSystemInfo", inputParam);
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
	}
    
    /** OPTIONAL ***** (9) Encode transferParameters (struct ns1__TTransferParameters *) ***************/
    error = encode_TTransferParameters(func, &env, req->transferParameters, SRM_PARAM_transferParameters, inputParam);
    if (error) {
        if (error != ENCODE_ERR_MISSING_PARAM) {
            repp->returnStatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
            repp->returnStatus->explanation = "Error encoding transferParameters";
            xmlrpc_DECREF(inputParam);
            xmlrpc_env_clean(&env);
            return(SOAP_OK);
        }
    }
    
    /* Clean up xmlrpc error-handling environment. */
    xmlrpc_env_clean(&env);
    xmlrpc_env_init(&env);

    /* Define the structure to give as input to the RPC response handler */
    ReserveSpaceResponseHandlerInput.soap = soap;
    ReserveSpaceResponseHandlerInput.req  = req;
    ReserveSpaceResponseHandlerInput.repp = repp;
    ReserveSpaceResponseHandlerInput.RPCTerminated = 0;
                                                                        
    /* Make remote procedure call, i.e. call Backend server */
    srmlogit(STORM_LOG_DEBUG, func, "Making RPC.\n");

    xmlrpc_client* client = get_xmlrpc_client();
    xmlrpc_client_call2f(&env, client, xmlrpc_endpoint, methodName_ReserveSpace, &result, "(S)", inputParam);

    rpcResponseHandler_ReserveSpace(xmlrpc_endpoint, methodName_ReserveSpace, NULL,
                              &ReserveSpaceResponseHandlerInput, &env, result);
       
    xmlrpc_DECREF(inputParam);

    if (!env.fault_occurred) {
      xmlrpc_DECREF(result);
    }

    xmlrpc_env_clean(&env);
    
    srmlogit(STORM_LOG_DEBUG, func, "Request done. Status: %s\n", reconvertStatusCode(repp->returnStatus->statusCode));
    
    return(SOAP_OK);
}

/***************************************************************************************/
/************************ SRM v2.2   StatusOfReserveSpaceRequest ***********************/
/***************************************************************************************/

int ns1__srmStatusOfReserveSpaceRequest_impl(struct soap *soap,
                                        struct ns1__srmStatusOfReserveSpaceRequestRequest *req,
                                        struct ns1__srmStatusOfReserveSpaceRequestResponse_ *rep)
{
    static const char *func = "StatusOfReserveSpace";
    struct ns1__srmStatusOfReserveSpaceRequestResponse *repp;
    
    repp = static_cast<ns1__srmStatusOfReserveSpaceRequestResponse*>(soap_malloc(soap,sizeof(ns1__srmStatusOfReserveSpaceRequestResponse)));
    if (repp == NULL) return SOAP_EOM;

    repp->returnStatus = static_cast<ns1__TReturnStatus*>(soap_malloc(soap, sizeof(ns1__TReturnStatus)));
    if (repp->returnStatus == NULL) return SOAP_EOM;

    repp->returnStatus->statusCode = SRM_USCORENOT_USCORESUPPORTED;
    repp->returnStatus->explanation = "srmReserveSpace implementation is synchronous";
    rep->srmStatusOfReserveSpaceRequestResponse = repp;
    return(SOAP_OK);
}

/***************************************************************************************/
/*************************         SRM v2.2   ReleaseSpace     *************************/
/***************************************************************************************/

/* Data structure needed to give input parameters to the response handler function */
struct RPC_ResponseHandlerInput_ReleaseSpace {
    struct soap *soap;
    struct ns1__srmReleaseSpaceRequest *req;
    struct ns1__srmReleaseSpaceResponse *repp;
    int RPCTerminated;
};

/* Response handler for the RPC asynchronous call of the Rmdir function */
void rpcResponseHandler_ReleaseSpace(const char          *serverUrl, 
                                     const char          *method_name, 
                               		 const xmlrpc_value  *param_array, 
                               		 const void          *user_data, 
                               		 const xmlrpc_env    *faultP, 
                               		 xmlrpc_value        *result)
{
    static const char *func= "rpcResponseHandler_ReleaseSpace";
    struct RPC_ResponseHandlerInput_ReleaseSpace *input;
    struct soap *soap;
    struct ns1__srmReleaseSpaceRequest *req;
    struct ns1__srmReleaseSpaceResponse *repp;
    int error;
    xmlrpc_env env;
    
    input = (struct RPC_ResponseHandlerInput_ReleaseSpace *) user_data;
    soap = input->soap;
    req = input->req;
    repp = input->repp;
    
    srmlogit(STORM_LOG_DEBUG, func, "Inside the response handler.\n");
    
    if (faultP->fault_occurred) {
        srmlogit(STORM_LOG_ERROR, func, "ERROR: XML-RPC Fault: %s (code: %d)\n", faultP->fault_string, faultP->fault_code);
        repp->returnStatus->explanation = soap_strdup(soap, faultP->fault_string);
        repp->returnStatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
        input->RPCTerminated = 1;
        return;
    }
    /* Initialize the error-handling environment. */
    xmlrpc_env_init(&env);
    
    /************************* (1) Decode returnStatus (in TReturnStatus *) *************************/
    error = decode_globalTReturnStatus(func, &env, soap, repp->returnStatus, result);
    if (error != 0) {
        repp->returnStatus->explanation = "Error: unable to parse the BE response or empty response";
        repp->returnStatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
        /* Clean up our error-handling environment. */
        xmlrpc_env_clean(&env);
        input->RPCTerminated = 1;
        return;
    }
    
    /* Clean up xmlrpc error-handling environment. */
    xmlrpc_env_clean(&env);
    input->RPCTerminated = 1;
    
    srmlogit(STORM_LOG_DEBUG, func, "Response parsed.\n");
    
    return;
}


int ns1__srmReleaseSpace_impl(struct soap *soap,
                         struct ns1__srmReleaseSpaceRequest *req,
                         struct ns1__srmReleaseSpaceResponse_ *rep)
{
    static const char *func = "ReleaseSpace";
    static const char *methodName_RSpace = "synchcall.ReleaseSpace";
    struct ns1__srmReleaseSpaceResponse *repp;
    int error;
    struct RPC_ResponseHandlerInput_ReleaseSpace ReleaseSpaceResponseHandlerInput;
    xmlrpc_env env;
    xmlrpc_value *inputParam;
    xmlrpc_value *result;
    

    repp = static_cast<ns1__srmReleaseSpaceResponse*>(soap_malloc(soap,sizeof(ns1__srmReleaseSpaceResponse)));
    if (repp == NULL) return SOAP_EOM;

    repp->returnStatus = static_cast<ns1__TReturnStatus*>(soap_malloc(soap, sizeof(ns1__TReturnStatus)));
    if (repp->returnStatus == NULL) return SOAP_EOM;

    rep->srmReleaseSpaceResponse = repp;
    
    /* Initialize xmlrpc error-handling environment. */
    xmlrpc_env_init(&env);
    
    /* Initialize xmlrpc input structure */
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
    /********      Encode parameters for ReleaseSpace (SRM v2.2)          *********/  
    /******************************************************************************/

    /** OPTIONAL ************ (1) Encode authorizationID (char *) ****************************/
    error = encode_string(func, &env, req->authorizationID, SRM_PARAM_authorizationID, inputParam);
    if (error) {
        if (error != ENCODE_ERR_MISSING_PARAM && (! DONT_FAIL_FOR_AUTHORIZATION_ID)) {  // This is an optional parameter
            repp->returnStatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
            repp->returnStatus->explanation = "Error encoding authID parameter";
            xmlrpc_DECREF(inputParam);
            xmlrpc_env_clean(&env);
            return(SOAP_OK);
        }
    }
    
    /** MANDATORY *********** (2) Encode spaceToken (char *) ****************************/
    error = encode_string(func, &env, req->spaceToken, SRM_PARAM_spaceToken, inputParam);
    if (error) {
        if (error == ENCODE_ERR_MISSING_PARAM) {
            repp->returnStatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
            repp->returnStatus->explanation = "Error: missing spaceToken (mandatory parameter)";
            
        } else {
            repp->returnStatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
            repp->returnStatus->explanation = "Error encoding spaceToken parameter";
        }
        xmlrpc_DECREF(inputParam);
        xmlrpc_env_clean(&env);
        return(SOAP_OK);
    }
    
    /** OPTIONAL ************ (3) Encode storageSystemInfo (struct ns1__ArrayOfTExtraInfo *) ************/
    if(req->storageSystemInfo != NULL && req->storageSystemInfo->__sizeextraInfoArray >= 1)
	{
		error = encode_ArrayOfTExtraInfo(func, &env, req->storageSystemInfo , SRM_PARAM_storageSystemInfo, inputParam);
		if (error) {
			if (error != ENCODE_ERR_MISSING_PARAM) {
				repp->returnStatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
				repp->returnStatus->explanation = "Error encoding authID parameter";
				xmlrpc_DECREF(inputParam);
				xmlrpc_env_clean(&env);
				return(SOAP_OK);
			}
		}
	}
    
    /** OPTIONAL ************ (4) Encode forceFileRelease (enum xsd__boolean *) ************/
    error = encode_bool(func, &env, reinterpret_cast<unsigned int*>(req->forceFileRelease), "forceFileRelease", inputParam);
    if (error) {
        if (error != ENCODE_ERR_MISSING_PARAM) {
            repp->returnStatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
            repp->returnStatus->explanation = "Error encoding forceFileRelease parameter";
            xmlrpc_DECREF(inputParam);
            xmlrpc_env_clean(&env);
            return(SOAP_OK);
        }
    }
    
    /* Clean up xmlrpc error-handling environment. */
    xmlrpc_env_clean(&env);
    xmlrpc_env_init(&env);

    /* Define the structure to give as input to the RPC response handler */
    ReleaseSpaceResponseHandlerInput.soap = soap;
    ReleaseSpaceResponseHandlerInput.req  = req;
    ReleaseSpaceResponseHandlerInput.repp = repp;
    ReleaseSpaceResponseHandlerInput.RPCTerminated = 0;
                                                                        
    /* Make remote procedure call, calling Backend server */
    srmlogit(STORM_LOG_DEBUG, func, "Making RPC.\n");

    xmlrpc_client* client = get_xmlrpc_client();
    xmlrpc_client_call2f(&env, client, xmlrpc_endpoint, methodName_RSpace, &result, "(S)", inputParam);

    rpcResponseHandler_ReleaseSpace(xmlrpc_endpoint, methodName_RSpace, NULL,
                              &ReleaseSpaceResponseHandlerInput, &env, result);

    xmlrpc_DECREF(inputParam);

    if (!env.fault_occurred) {
      xmlrpc_DECREF(result);
    }

    xmlrpc_env_clean(&env);
    
    srmlogit(STORM_LOG_DEBUG, func, "Request done. Status: %s\n", reconvertStatusCode(repp->returnStatus->statusCode));
    
    return(SOAP_OK);
}

/***************************************************************************************/
/*************************         SRM v2.2   UpdateSpace      *************************/
/***************************************************************************************/

int ns1__srmUpdateSpace_impl(struct soap *soap,
                         struct ns1__srmUpdateSpaceRequest *req,
                         struct ns1__srmUpdateSpaceResponse_ *rep)
{
    static const char *func = "UpdateSpace";
    struct ns1__srmUpdateSpaceResponse *repp;
    
    repp = static_cast<ns1__srmUpdateSpaceResponse*>(soap_malloc(soap,sizeof(ns1__srmUpdateSpaceResponse)));
    if (repp == NULL) return SOAP_EOM;

    repp->returnStatus = static_cast<ns1__TReturnStatus*>(soap_malloc(soap, sizeof(ns1__TReturnStatus)));
    if (repp->returnStatus == NULL) return SOAP_EOM;

    repp->lifetimeGranted = NULL;
    repp->requestToken = NULL;
    repp->sizeOfGuaranteedSpace = NULL;
    repp->sizeOfTotalSpace = NULL;
    
    repp->returnStatus->explanation = "Not supported";
    repp->returnStatus->statusCode = SRM_USCORENOT_USCORESUPPORTED;

    /* Assign the repp response structure to the output parameter rep */
    rep->srmUpdateSpaceResponse = repp;
    
    srmlogit(STORM_LOG_DEBUG, func, "Not supported.\n");

    return (SOAP_OK);
}

/***************************************************************************************/
/************************* SRM v2.2   StatusOfUpdateSpaceRequest ***********************/
/***************************************************************************************/

int ns1__srmStatusOfUpdateSpaceRequest_impl(struct soap *soap,
                                       struct ns1__srmStatusOfUpdateSpaceRequestRequest *req,
                                       struct ns1__srmStatusOfUpdateSpaceRequestResponse_ *rep)
{
    static const char *func = "StatusOfUpdateSpace";
    struct ns1__srmStatusOfUpdateSpaceRequestResponse *repp;
    
    repp = static_cast<ns1__srmStatusOfUpdateSpaceRequestResponse*>(soap_malloc(soap,sizeof(ns1__srmStatusOfUpdateSpaceRequestResponse)));
    if (repp == NULL) return SOAP_EOM;

    repp->returnStatus = static_cast<ns1__TReturnStatus*>(soap_malloc(soap, sizeof(ns1__TReturnStatus)));
    if (repp->returnStatus == NULL) return SOAP_EOM;
    repp->lifetimeGranted = NULL;
    repp->sizeOfGuaranteedSpace = NULL;
    repp->sizeOfTotalSpace = NULL;
    
    repp->returnStatus->explanation = "Not supported";
    repp->returnStatus->statusCode = SRM_USCORENOT_USCORESUPPORTED;

    /* Assign the repp response structure to the output parameter rep */
    rep->srmStatusOfUpdateSpaceRequestResponse = repp;
    
    srmlogit(STORM_LOG_DEBUG, func, "Not supported.\n");

    return (SOAP_OK);
}

/***************************************************************************************/
/*************************     SRM v2.2   GetSpaceMetaData     *************************/
/***************************************************************************************/
struct RPC_ResponseHandlerInput_GetSpaceMetaData {
    struct soap *soap;
    struct ns1__srmGetSpaceMetaDataRequest *req;
    struct ns1__srmGetSpaceMetaDataResponse *repp;
    int RPCTerminated;
};

/* Response handler for the RPC asynchronous call of the GetSpaceMetaData function */
void rpcResponseHandler_GetSpaceMetaData(const char         *serverUrl, 
                                         const char         *method_name, 
                                         const xmlrpc_value *param_array, 
                                         const void         *user_data, 
                                         const xmlrpc_env   *faultP, 
                                         xmlrpc_value       *result)
{
    static const char *func = "rpcResponseHandler_GetSpaceMetaData";
    struct ns1__srmGetSpaceMetaDataRequest *req;
    struct ns1__srmGetSpaceMetaDataResponse *repp;
    struct RPC_ResponseHandlerInput_GetSpaceMetaData *input;
    struct soap *soap;
    int error;
    xmlrpc_env env;
    
    input = (struct RPC_ResponseHandlerInput_GetSpaceMetaData *) user_data;
    soap = input->soap;
    req = input->req;
    repp = input->repp;

    srmlogit(STORM_LOG_DEBUG, func, "Inside the response handler \n");
       
    if (faultP->fault_occurred) {
        srmlogit(STORM_LOG_ERROR, func, "ERROR: XML-RPC Fault: %s (code: %d)\n", faultP->fault_string, faultP->fault_code);
        repp->returnStatus->explanation = "Connection error to backend server. Check FE server log file for details.";
        repp->returnStatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
        input->RPCTerminated = 1;
        return;
    }
    
    xmlrpc_env_init(&env);
    
    /** MANDATORY ************ (1) Decode returnStatus (in TReturnStatus *) *********************************/
    error = decode_globalTReturnStatus(func, &env, soap, repp->returnStatus, result);
    if (error != 0) {
        repp->returnStatus->explanation = "Error: unable to parse the BE response or empty response";
        repp->returnStatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
        /* Clean up our error-handling environment. */
        xmlrpc_env_clean(&env);
        input->RPCTerminated = 1;
        return;
    }
    
    /** OPTIONAL ************* (2) Decode arrayOfSpaceDetails (in struct ns1__ArrayOfTMetaDataSpace *) ******/
    error = decode_ArrayOfTMetaDataSpace(func, &env, soap, &(repp->arrayOfSpaceDetails), SRM_PARAM_arrayOfSpaceDetails, result);
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
    
    input->RPCTerminated = 1;
    return;
}  
    
int ns1__srmGetSpaceMetaData_impl(struct soap *soap,
                             struct ns1__srmGetSpaceMetaDataRequest *req,
                             struct ns1__srmGetSpaceMetaDataResponse_ *rep)
{
    static const char *func = "GetSpaceMetaData";
    static const char *methodName_getSpace = "synchcall.getSpaceMetaData";
    struct ns1__srmGetSpaceMetaDataResponse *repp;
    struct RPC_ResponseHandlerInput_GetSpaceMetaData GetSpaceMetaDataResponseHandlerInput;
    int error;
    xmlrpc_env env;
    xmlrpc_value *inputParam;
    xmlrpc_value *result;
  
    repp = static_cast<ns1__srmGetSpaceMetaDataResponse*>(soap_malloc(soap,sizeof(ns1__srmGetSpaceMetaDataResponse)));
    if (repp == NULL) return SOAP_EOM;

    repp->returnStatus = static_cast<ns1__TReturnStatus*>(soap_malloc(soap, sizeof(ns1__TReturnStatus)));
    if (repp->returnStatus == NULL) return SOAP_EOM;

    repp->arrayOfSpaceDetails = NULL;
    rep->srmGetSpaceMetaDataResponse = repp;
    
    /* Initialize xmlrpc error-handling environment. */     
    xmlrpc_env_init(&env);
    
    /* Initialize xmlrpc input structure */
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
    /********       Encode parameters for GetSpaceMetaData (SRM v2.2)      ********/  
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

    /** MANDATORY ********** (2) Encode arrayOfSpaceTokens (struct ns1__ArrayOfString *) **********/
    error = encode_arrayOfString(func, &env, req->arrayOfSpaceTokens, SRM_PARAM_arrayOfSpaceTokens, inputParam);
    if (error) {
        repp->returnStatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
        if (error == ENCODE_ERR_MISSING_PARAM) repp->returnStatus->explanation = "Error: missing spaceToken (mandatory parameter)";
        else repp->returnStatus->explanation = "Error encoding spaceToken parameter";
        xmlrpc_DECREF(inputParam);
        xmlrpc_env_clean(&env);
        return(SOAP_OK);
    }
    
    /* Clean up xmlrpc error-handling environment. */
    xmlrpc_env_clean(&env);
    xmlrpc_env_init(&env);

    /* Define the structure to give as input to the RPC response handler */
    GetSpaceMetaDataResponseHandlerInput.soap = soap;
    GetSpaceMetaDataResponseHandlerInput.req  = req;
    GetSpaceMetaDataResponseHandlerInput.repp = repp;
    GetSpaceMetaDataResponseHandlerInput.RPCTerminated = 0;
 
    /* Make remote procedure call, i.e. call Backend server */
    srmlogit(STORM_LOG_DEBUG, func, "Making RPC.\n");

    xmlrpc_client* client = get_xmlrpc_client();
    xmlrpc_client_call2f(&env, client, xmlrpc_endpoint, methodName_getSpace, &result, "(S)", inputParam);

    rpcResponseHandler_GetSpaceMetaData(xmlrpc_endpoint, methodName_getSpace, NULL,
                              &GetSpaceMetaDataResponseHandlerInput, &env, result);
    
    xmlrpc_DECREF(inputParam);

    if (!env.fault_occurred) {
      xmlrpc_DECREF(result);
    }

    xmlrpc_env_clean(&env);
    
    srmlogit(STORM_LOG_DEBUG, func, "Request done. Status: %s\n", reconvertStatusCode(repp->returnStatus->statusCode));
    
    return(SOAP_OK);
}

/***************************************************************************************/
/*************************     SRM v2.2   GetSpaceTokens       *************************/
/***************************************************************************************/
struct RPC_ResponseHandlerInput_GetSpaceTokens {
    struct soap *soap;
    struct ns1__srmGetSpaceTokensRequest *req;
    struct ns1__srmGetSpaceTokensResponse *repp;
    int RPCTerminated;
};

/* Response handler for the RPC asynchronous call of the GetSpaceTokens function */
void rpcResponseHandler_GetSpaceTokens(const char         *serverUrl, 
                                         const char         *method_name, 
                                         const xmlrpc_value *param_array, 
                                         const void         *user_data, 
                                         const xmlrpc_env   *faultP, 
                                         xmlrpc_value       *result)
{
    static const char *func = "rpcResponseHandler_GetSpaceTokens";
    struct ns1__srmGetSpaceTokensRequest *req;
    struct ns1__srmGetSpaceTokensResponse *repp;
    struct RPC_ResponseHandlerInput_GetSpaceTokens *input;
    struct soap *soap;
    int error;
    xmlrpc_env env;
    
    input = (struct RPC_ResponseHandlerInput_GetSpaceTokens *) user_data;
    soap = input->soap;
    req = input->req;
    repp = input->repp;

    srmlogit(STORM_LOG_DEBUG, func, "Inside the response handler \n");
       
    if (faultP->fault_occurred) {
        srmlogit(STORM_LOG_ERROR, func, "ERROR: XML-RPC Fault: %s (code: %d)\n", faultP->fault_string, faultP->fault_code);
        repp->returnStatus->explanation = "Connection error to backend server. Check FE server log file for details.";
        repp->returnStatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
        input->RPCTerminated = 1;
        return;
    }
    
    xmlrpc_env_init(&env);
    
    /** MANDATORY ************ (1) Decode returnStatus (in TReturnStatus *) *********************************/
    error = decode_globalTReturnStatus(func, &env, soap, repp->returnStatus, result);
    if (error != 0) {
        repp->returnStatus->explanation = "Error: unable to parse the BE response or empty response";
        repp->returnStatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
        /* Clean up our error-handling environment. */
        xmlrpc_env_clean(&env);
        input->RPCTerminated = 1;
        return;
    }
    
    /** OPTIONAL ************* (2) Decode arrayOfSpaceDetails (in struct ns1__ArrayOfTTokensSpace *) ******/
    error = decode_ArrayOfString(func, &env, soap, &(repp->arrayOfSpaceTokens), SRM_PARAM_arrayOfSpaceTokens, result);
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
    
    input->RPCTerminated = 1;
    return;
}


int ns1__srmGetSpaceTokens_impl(struct soap *soap,
                           struct ns1__srmGetSpaceTokensRequest *req,
                           struct ns1__srmGetSpaceTokensResponse_ *rep)
{
    static const char *func = "GetSpaceTokens";
    static const char *methodName = "synchcall.getSpaceTokens";
    struct ns1__srmGetSpaceTokensResponse *repp;
    struct RPC_ResponseHandlerInput_GetSpaceTokens GetSpaceTokensResponseHandlerInput;
    int error;
    xmlrpc_env env;
    xmlrpc_value *inputParam;
    xmlrpc_value *result;
  
    repp = static_cast<ns1__srmGetSpaceTokensResponse*>(soap_malloc(soap,sizeof(ns1__srmGetSpaceTokensResponse)));
    if (repp == NULL) return SOAP_EOM;

    repp->returnStatus = static_cast<ns1__TReturnStatus*>(soap_malloc(soap, sizeof(ns1__TReturnStatus)));
    if (repp->returnStatus == NULL) return SOAP_EOM;

    repp->arrayOfSpaceTokens = NULL;
    rep->srmGetSpaceTokensResponse = repp;
    
    /* Initialize xmlrpc error-handling environment. */     
    xmlrpc_env_init(&env);
    
    /* Initialize xmlrpc input structure */
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
    /********       Encode parameters for GetSpaceTokens (SRM v2.2)        ********/  
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

    /** OPTIONAL ************ (2) Encode userSpaceTokenDescription (char *) ****************************/
    error = encode_string(func, &env, req->userSpaceTokenDescription, SRM_PARAM_userSpaceTokenDescription, inputParam);
    if (error) {
        if (error != ENCODE_ERR_MISSING_PARAM) {
            repp->returnStatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
            repp->returnStatus->explanation = "Error encoding userSpaceTokenDescription";
            xmlrpc_DECREF(inputParam);
            xmlrpc_env_clean(&env);
            return(SOAP_OK);
        }
        xmlrpc_env_clean(&env);
        xmlrpc_env_init(&env);
    }
    
    /* Clean up xmlrpc error-handling environment. */
    xmlrpc_env_clean(&env);
    xmlrpc_env_init(&env);

    /* Define the structure to give as input to the RPC response handler */
    GetSpaceTokensResponseHandlerInput.soap = soap;
    GetSpaceTokensResponseHandlerInput.req  = req;
    GetSpaceTokensResponseHandlerInput.repp = repp;
    GetSpaceTokensResponseHandlerInput.RPCTerminated = 0;
 
    /* Make remote procedure call, i.e. call Backend server */
    srmlogit(STORM_LOG_DEBUG, func, "Making RPC.\n");

    xmlrpc_client* client = get_xmlrpc_client();
    xmlrpc_client_call2f(&env, client, xmlrpc_endpoint, methodName, &result, "(S)", inputParam);

    rpcResponseHandler_GetSpaceTokens(xmlrpc_endpoint, methodName, NULL,
                              &GetSpaceTokensResponseHandlerInput, &env, result);
    
    xmlrpc_DECREF(inputParam);

    if (!env.fault_occurred) {
      xmlrpc_DECREF(result);
    }

    xmlrpc_env_clean(&env);
    
    srmlogit(STORM_LOG_DEBUG, func, "Request done. Status: %s\n", reconvertStatusCode(repp->returnStatus->statusCode));
    
    return(SOAP_OK);
}

/***************************************************************************************/
/*************************    SRM v2.2   ChangeSpaceForFiles   *************************/
/***************************************************************************************/

int ns1__srmChangeSpaceForFiles_impl(struct soap *soap,
                                struct ns1__srmChangeSpaceForFilesRequest *req,
                                struct ns1__srmChangeSpaceForFilesResponse_ *rep)
{
    static const char *func = "ChangeSpaceForFiles";
    struct ns1__srmChangeSpaceForFilesResponse *repp;
    

    repp = static_cast<ns1__srmChangeSpaceForFilesResponse*>(soap_malloc(soap,sizeof(ns1__srmChangeSpaceForFilesResponse)));
    if (repp == NULL) return SOAP_EOM;

    repp->returnStatus = static_cast<ns1__TReturnStatus*>(soap_malloc(soap, sizeof(ns1__TReturnStatus)));
    if (repp->returnStatus == NULL) return SOAP_EOM;
    repp->arrayOfFileStatuses = NULL;
    repp->estimatedProcessingTime = NULL;
    repp->requestToken = NULL;
    
    repp->returnStatus->explanation = "Not supported";
    repp->returnStatus->statusCode = SRM_USCORENOT_USCORESUPPORTED;

    /* Assign the repp response structure to the output parameter rep */
    rep->srmChangeSpaceForFilesResponse = repp;
    
    srmlogit(STORM_LOG_INFO, func, "Not supported.\n");

    return (SOAP_OK);
}

/***************************************************************************************/
/******************** SRM v2.2   StatusOfChangeSpaceForFilesRequest ********************/
/***************************************************************************************/

int ns1__srmStatusOfChangeSpaceForFilesRequest_impl(struct soap *soap,
                                               struct ns1__srmStatusOfChangeSpaceForFilesRequestRequest *req,
                                               struct ns1__srmStatusOfChangeSpaceForFilesRequestResponse_ *rep)
{
    static const char *func = "StatusOfChangeSpaceForFiles";
    struct ns1__srmStatusOfChangeSpaceForFilesRequestResponse *repp;
    
    repp = static_cast<ns1__srmStatusOfChangeSpaceForFilesRequestResponse*>(soap_malloc(soap,sizeof(ns1__srmStatusOfChangeSpaceForFilesRequestResponse)));
    if (repp == NULL) return SOAP_EOM;

    repp->returnStatus = static_cast<ns1__TReturnStatus*>(soap_malloc(soap, sizeof(ns1__TReturnStatus)));
    if (repp->returnStatus == NULL) return SOAP_EOM;
    repp->arrayOfFileStatuses = NULL;
    repp->estimatedProcessingTime = NULL;
    
    repp->returnStatus->explanation = "Not supported";
    repp->returnStatus->statusCode = SRM_USCORENOT_USCORESUPPORTED;

    /* Assign the repp response structure to the output parameter rep */
    rep->srmStatusOfChangeSpaceForFilesRequestResponse = repp;
    
    srmlogit(STORM_LOG_DEBUG, func, "Not supported.\n");

    return (SOAP_OK);
}

/***************************************************************************************/
/************************** SRM v2.2   ExtendFileLifeTimeInSpace ***********************/
/***************************************************************************************/

int ns1__srmExtendFileLifeTimeInSpace_impl(struct soap *soap,
                                      struct ns1__srmExtendFileLifeTimeInSpaceRequest *req,
                                      struct ns1__srmExtendFileLifeTimeInSpaceResponse_ *rep)
{
    static const char *func = "ExtendFileLifeTimeInSpace";
    struct ns1__srmExtendFileLifeTimeInSpaceResponse *repp;
    
    repp = static_cast<ns1__srmExtendFileLifeTimeInSpaceResponse*>(soap_malloc(soap,sizeof(ns1__srmExtendFileLifeTimeInSpaceResponse)));
    if (repp == NULL) return SOAP_EOM;

    repp->returnStatus = static_cast<ns1__TReturnStatus*>(soap_malloc(soap, sizeof(ns1__TReturnStatus)));
    if (repp->returnStatus == NULL) return SOAP_EOM;
    repp->arrayOfFileStatuses = NULL;
    
    repp->returnStatus->explanation = "Not supported";
    repp->returnStatus->statusCode = SRM_USCORENOT_USCORESUPPORTED;

    /* Assign the repp response structure to the output parameter rep */
    rep->srmExtendFileLifeTimeInSpaceResponse = repp;
    
    srmlogit(STORM_LOG_DEBUG, func, "Not supported.\n");

    return (SOAP_OK);
}

/***************************************************************************************/
/**************************       SRM v2.2   PurgeFromSpace      ***********************/
/***************************************************************************************/

int ns1__srmPurgeFromSpace_impl(struct soap *soap,
                           struct ns1__srmPurgeFromSpaceRequest *req,
                           struct ns1__srmPurgeFromSpaceResponse_ *rep)
{
    static const char *func = "PurgeFromSpace";
    struct ns1__srmPurgeFromSpaceResponse *repp;
    
    repp = static_cast<ns1__srmPurgeFromSpaceResponse*>(soap_malloc(soap,sizeof(ns1__srmPurgeFromSpaceResponse)));
    if (repp == NULL) return SOAP_EOM;

    repp->returnStatus = static_cast<ns1__TReturnStatus*>(soap_malloc(soap, sizeof(ns1__TReturnStatus)));
    if (repp->returnStatus == NULL) return SOAP_EOM;
    repp->arrayOfFileStatuses = NULL;
    
    repp->returnStatus->explanation = "Not supported";
    repp->returnStatus->statusCode = SRM_USCORENOT_USCORESUPPORTED;

    /* Assign the repp response structure to the output parameter rep */
    rep->srmPurgeFromSpaceResponse = repp;
    
    srmlogit(STORM_LOG_DEBUG, func, "Not supported.\n");

    return (SOAP_OK);
}

