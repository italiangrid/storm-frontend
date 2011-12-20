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

#include <cgsi_plugin.h>
#include <argus/pep.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* attempt of getting the proxy
#include <globus/globus_gss_assist.h>
#include <globus/globus_gsi_credential.h>
#include <stdsoap2.h>

#include <openssl/x509.h>
#include <openssl/pem.h>
*/

#include "Authorization.hpp"
#include "srmlogit.h"

using namespace storm;

const std::string Authorization::DEFAULT_RESOURCE = "StoRM";
const std::string Authorization::DEFAULT_ACTION = "access";

/*
bool Authorization::isAuthorized(std::string resource, std::string action) throw (storm::AuthorizationException) {

	bool authorized = false;
	// create the XACML request
	xacml_request_t * request;
	xacml_response_t * response;
	std::string subjectid = credentials->getDN();
	fprintf(stdout, "Constructing Subject from DN %s\n", subjectid.c_str());
	int rc = Authorization::create_xacml_request(&request, subjectid.c_str(), resource.c_str(), action.c_str());
	if (rc != 0) {
		fprintf(stderr, "failed to create XACML request\n");
		std::string errMessage("failed to create XACML request\n");
		storm::AuthorizationException exc(errMessage);
		throw exc;
	}
	pep_error_t pep_rc = pep_authorize(pep, &request, &response);
	if (pep_rc != PEP_OK) {
		fprintf(stderr, "failed to authorize XACML request: %s\n",
				pep_strerror(pep_rc));
		std::string errMessage("failed to authorize XACML request\n");
		storm::AuthorizationException exc(errMessage);
		throw exc;
	}
	// parse and process XACML response
	xacml_decision_t decision;
	try {
		decision = process_xacml_response(response);
	} catch (storm::AuthorizationException& e) {
		fprintf(stderr, "failed to process XACML response. Received a storm::AuthorizationException.\n");
		throw e;
	}

	if (decision == XACML_DECISION_PERMIT) {
		fprintf(stdout, "User authorized\n");
		authorized = true;
	}
	else
	{
		fprintf(stderr, "User not authorized\n");
	}
	// delete resquest and response objs
	xacml_request_delete(request);
	xacml_response_delete(response);
	return authorized;
}
*/
/**
 * Return true if the user is blacklisted, false otherwise
 * We consider an user blacklisted if the authorization answer for DEFAULT_RESOURCE ans DEFAULT_ACTION is Deny;
 * */
bool Authorization::isBlacklisted() throw (storm::AuthorizationException)
{
	// create the XACML request
	char* funcName = "isBlacklisted";
	if(Authorization::blacklistRequested)
	{
		bool blacklisted = false;
		xacml_request_t * request;
		xacml_response_t * response;

		std::string subjectid = credentials->getDN();
		srmlogit(STORM_LOG_DEBUG, funcName, "Constructing Subject from DN %s\n", subjectid.c_str());
		int rc = Authorization::create_xacml_request(&request, subjectid.c_str(), Authorization::DEFAULT_RESOURCE.c_str(), Authorization::DEFAULT_ACTION.c_str());
		if (rc != 0) {
			srmlogit(STORM_LOG_ERROR, funcName, "Failed to create XACML request\n");
			std::string  errMessage("Failed to create XACML request\n");
			storm::AuthorizationException exc(errMessage);
			throw exc;
		}
		pep_error_t pep_rc = pep_authorize(pep, &request, &response);
		if (pep_rc != PEP_OK) {
			srmlogit(STORM_LOG_ERROR, funcName, "Failed to authorize XACML request: %s\n", pep_strerror(pep_rc));
			std::string errMessage("failed to authorize XACML request\n");
			storm::AuthorizationException exc(errMessage);
			throw exc;
		}
		xacml_decision_t decision;
		try {
			decision = process_xacml_response(response);
		} catch (storm::AuthorizationException& e) {
			srmlogit(STORM_LOG_ERROR, funcName, "Failed to process XACML response. Received a storm::AuthorizationException.\n");
			throw e;
		}
		// parse and process XACML response

		if (decision == XACML_DECISION_DENY) {
			srmlogit(STORM_LOG_INFO, funcName, "User blacklisted\n");
			blacklisted = true;
		}
		else
		{
			if(decision != XACML_DECISION_PERMIT && decision != XACML_DECISION_NOT_APPLICABLE)
			{
				srmlogit(STORM_LOG_WARNING, funcName, "Received error decision from Argus: %s\n Considering the user as not blacklisted" ,  Authorization::decision_tostring(decision));
			}
		}
		// delete resquest and response objs
		xacml_request_delete(request);
		xacml_response_delete(response);
		return blacklisted;
	}
	else
	{
		return false;
	}
}

/*
 * Creates a XACML Request containing a XACML Subject with the given subjectid, a XACML Resource
 * with the given resourceid and a XACML Action with the given actionid.
 *
 * @param [in/out] request address of the pointer to the XACML request object
 * @param [in] subjectid, a X.509 DN, attribute value of the XACML Request/Subject element
 * @param [in] resourceid  attribute value of the XACML Request/Resource element
 * @param [in] actionid  attribute value of the XACML Request/Action element
 * @return 0 on success or error code on failure.
 */
int Authorization::create_xacml_request(xacml_request_t ** request,const char * subjectid, const char * resourceid, const char * actionid)
{
    xacml_subject_t * subject;
    xacml_attribute_t * subject_attr_id;
    xacml_resource_t * resource;
    xacml_attribute_t * resource_attr_id;
    xacml_action_t * action;
    xacml_attribute_t * action_attr_id;

    char* funcName = "create_xacml_request";

    /* XACML Subject with subjectid Attribute value */
    subject= xacml_subject_create();
    if (subject == NULL) {
    	srmlogit(STORM_LOG_ERROR, funcName, "Can not create XACML Subject\n");
        return 1;
    }

    subject_attr_id= xacml_attribute_create(XACML_SUBJECT_ID);
    if (subject_attr_id == NULL) {
    	srmlogit(STORM_LOG_ERROR, funcName, "Can not create XACML Subject/Attribute:%s\n",XACML_SUBJECT_ID);
        xacml_subject_delete(subject);
        return 1;
    }
    // set X.509 DN value
    xacml_attribute_addvalue(subject_attr_id,subjectid);
	xacml_attribute_setdatatype(subject_attr_id,XACML_DATATYPE_STRING);
    xacml_subject_addattribute(subject,subject_attr_id);

   /* use this if you are sending the proxy
    subject_attr_id= xacml_attribute_create(XACML_SUBJECT_KEY_INFO);
	if (subject_attr_id == NULL) {
		fprintf(stderr,"can not create XACML Subject/Attribute:%s\n",XACML_SUBJECT_KEY_INFO);
		xacml_subject_delete(subject);
		return 1;
	}
	xacml_attribute_addvalue(subject_attr_id,subjectid);
	xacml_attribute_setdatatype(subject_attr_id,XACML_DATATYPE_STRING);
	xacml_subject_addattribute(subject,subject_attr_id);
	*/

    /* XACML Resource with resourceid Attribute value */
    resource= xacml_resource_create();
    if (resource == NULL) {
    	srmlogit(STORM_LOG_ERROR, funcName, "Can not create XACML Resource");
        xacml_subject_delete(subject);
        return 2;
    }
    resource_attr_id= xacml_attribute_create(XACML_RESOURCE_ID);
    if (resource_attr_id == NULL) {
    	srmlogit(STORM_LOG_ERROR, funcName, "Can not create XACML Resource/Attribute:%s\n",XACML_RESOURCE_ID);
        xacml_subject_delete(subject);
        xacml_resource_delete(resource);
        return 2;
    }
    xacml_attribute_addvalue(resource_attr_id,resourceid);
    xacml_resource_addattribute(resource,resource_attr_id);

    /* XACML Action with actionid Attribute value */
    action= xacml_action_create();
    if (action == NULL) {
    	srmlogit(STORM_LOG_ERROR, funcName, "Can not create XACML Action\n");
        xacml_subject_delete(subject);
        xacml_resource_delete(resource);
        return 3;
    }
    action_attr_id= xacml_attribute_create(XACML_ACTION_ID);
    if (action_attr_id == NULL) {
    	srmlogit(STORM_LOG_ERROR, funcName, "Can not create XACML Action/Attribute:%s\n",XACML_ACTION_ID);
        xacml_subject_delete(subject);
        xacml_resource_delete(resource);
        xacml_action_delete(action);
        return 3;
    }
    xacml_attribute_addvalue(action_attr_id,actionid);
    xacml_action_addattribute(action,action_attr_id);

    /* XACML Request with all elements */
    *request= xacml_request_create();
    if (*request == NULL) {
    	srmlogit(STORM_LOG_ERROR, funcName, "Can not create XACML Request\n");
        xacml_subject_delete(subject);
        xacml_resource_delete(resource);
        xacml_action_delete(action);
        return 4;
    }
    xacml_request_addsubject(*request,subject);
    xacml_request_addresource(*request,resource);
    xacml_request_setaction(*request,action);

    return 0;
}

/*
 * Returns the string representation of the decision.
 */
const char * Authorization::decision_tostring(xacml_decision_t decision) {
    switch(decision) {
    case XACML_DECISION_DENY:
        return "Deny";
        break;
    case XACML_DECISION_PERMIT:
        return "Permit";
        break;
    case XACML_DECISION_INDETERMINATE:
        return "Indeterminate";
        break;
    case XACML_DECISION_NOT_APPLICABLE:
        return "Not Applicable";
        break;
    default:
        return "ERROR (Unknown Decision)";
        break;
    }
}

/*
 * Returns the string representation of the fulfillOn.
 */
char * Authorization::fulfillon_tostring(xacml_fulfillon_t fulfillon) {
    switch(fulfillon) {
    case XACML_FULFILLON_DENY:
        return "Deny";
        break;
    case XACML_FULFILLON_PERMIT:
        return "Permit";
        break;
    default:
        return "ERROR (Unknown FulfillOn)";
        break;
    }
}

/*
 * Simply dump the XACML response.
 *
 * @param [in] response the XAXML response
 * @return 0 on success or error code on failure.
 */
xacml_decision_t Authorization::process_xacml_response(const xacml_response_t * response) throw (storm::AuthorizationException) {
	char* funcName = "process_xacml_response";
    if (response == NULL)
    {
    	srmlogit(STORM_LOG_ERROR, funcName, "Parameter response is NULL\n");
        std::string  errMessage("Response is NULL\n");
		storm::AuthorizationException exc(errMessage);
		throw exc;
    }
    xacml_decision_t decision;
    size_t resultsLength= xacml_response_results_length(response);
    srmlogit(STORM_LOG_DEBUG, funcName, "Response: %d results\n", (int)resultsLength);
    if(resultsLength == 0)
	{
    	srmlogit(STORM_LOG_ERROR, funcName, "Response contains no results\n");
        std::string  errMessage("response contains no results\n");
		storm::AuthorizationException exc(errMessage);
		throw exc;
	}
    if(resultsLength > 1)
    {
    	srmlogit(STORM_LOG_WARNING, funcName, "Received an unexpected number of results: %d\n", (int)resultsLength);
    }

    //cycle on the results
    for(int i= 0; i<resultsLength; i++)
    {
        xacml_result_t * result;
        xacml_status_t * status;
        xacml_statuscode_t * statuscode, * subcode;

        result= xacml_response_getresult(response,i);

        if(i == 0)
        {
			decision = xacml_result_getdecision(result);
			srmlogit(STORM_LOG_DEBUG, funcName, "The decision for resource %s is: %s\n", xacml_result_getresourceid(result) ,decision_tostring(decision));
			Authorization::printXamlResult(result);
        }
        else
        {
        	srmlogit(STORM_LOG_DEBUG, funcName, "Printing the content of a further response. It will not be considered\n");
        	Authorization::printXamlResult(result);
        }
    }
    return decision;
}


void Authorization::printXamlObligation(xacml_obligation_t * obligation)
{
	char* funcName = "printXamlObligation";
	srmlogit(STORM_LOG_DEBUG, funcName, "----Obligation BEGIN----\n");
	srmlogit(STORM_LOG_DEBUG, funcName, "Obligation id: %s\n",xacml_obligation_getid(obligation));
	srmlogit(STORM_LOG_DEBUG, funcName, "Obligation fulfillOn: %s\n",Authorization::fulfillon_tostring(xacml_obligation_getfulfillon(obligation)));
	size_t attrsLength= xacml_obligation_attributeassignments_length(obligation);
	srmlogit(STORM_LOG_DEBUG, funcName, "Obligation attributes length: %d\n",(int)attrsLength);
	//cycle over the obbligation attributes
	for (int i= 0; i<attrsLength; i++)
	{
		srmlogit(STORM_LOG_DEBUG, funcName, "++Attribute BEGIN++\n");
		xacml_attributeassignment_t * attr= xacml_obligation_getattributeassignment(obligation,i);
		srmlogit(STORM_LOG_DEBUG, funcName, "Attribute Id: %s\n",xacml_attributeassignment_getid(attr));
		srmlogit(STORM_LOG_DEBUG, funcName, "Attribute datatype: %s\n",xacml_attributeassignment_getdatatype(attr));
		srmlogit(STORM_LOG_DEBUG, funcName, "Attribute value: %s\n",xacml_attributeassignment_getvalue(attr));
		srmlogit(STORM_LOG_DEBUG, funcName, "++Attribute END++\n");
	}
	srmlogit(STORM_LOG_DEBUG, funcName, "----Obligation END----\n");
}

void Authorization::printXamlResult(xacml_result_t * result)
{
	char* funcName = "printXamlResult";
	srmlogit(STORM_LOG_DEBUG, funcName, "----Result BEGIN----\n");
	srmlogit(STORM_LOG_DEBUG, funcName, "Resource Id: %s\n", xacml_result_getresourceid(result));
	srmlogit(STORM_LOG_DEBUG, funcName, "Result decision: %s\n", decision_tostring(xacml_result_getdecision(result)));
	srmlogit(STORM_LOG_DEBUG, funcName, "Status message is: %s\n", xacml_status_getmessage(xacml_result_getstatus(result)));
	srmlogit(STORM_LOG_DEBUG, funcName, "Status code: %s\n", xacml_statuscode_getvalue(xacml_status_getcode(xacml_result_getstatus(result))));
	if (xacml_statuscode_getsubcode(xacml_status_getcode(xacml_result_getstatus(result))) != NULL)
	{
		srmlogit(STORM_LOG_DEBUG, funcName, "Status subcode: %s\n", xacml_statuscode_getvalue(xacml_statuscode_getsubcode(xacml_status_getcode(xacml_result_getstatus(result)))));
	}
	size_t obligationsLength= xacml_result_obligations_length(result);
	srmlogit(STORM_LOG_DEBUG, funcName, "Status obligations:\n");
    for(int i= 0; i<obligationsLength; i++)
    {
    	//we do not manage obligations
    	srmlogit(STORM_LOG_DEBUG, funcName, "Printing the content of an Obligation received. It will not be considered\n");
    	Authorization::printXamlObligation(xacml_result_getobligation(result,i));
    }
    srmlogit(STORM_LOG_DEBUG, funcName, "----Result END----\n");
}

/* attempt of getting the proxy
static char *server_plugin_id = "CGSI_PLUGIN_SERVER_1.0";

struct cgsi_plugin_data {
    int context_established;
    gss_cred_id_t credential_handle;
    gss_ctx_id_t  context_handle;
    int socket_fd;
    int (*fsend)(struct soap*, const char*, size_t);
    size_t (*frecv)(struct soap*, char*, size_t);
    int (*fopen)(struct soap*, const char*, const char*, int);
    int (*fclose)(struct soap*);
    char client_name[512];
    char server_name[512];
    char username[512];
    int nb_iter;
    int disable_hostname_check;
    int context_flags;
    int trace_mode;
    char trace_file[512];
    gss_cred_id_t deleg_credential_handle;
    int deleg_cred_set;
    // Pointers to VOMS data
    char *voname;
    char **fqan;
    int nbfqan;
    int disable_mapping;
    int disable_voms_check;
    int allow_only_self;
    int had_send_error;
    void *deleg_credential_token;
    size_t deleg_credential_token_len;
};

char* Credentials::getCertChain() {

	fprintf(stdout,"getCertChain START\n");
	char* certChain;
	struct cgsi_plugin_data *data;
	gss_ctx_id_t * context;
	gss_cred_id_t cred;
	globus_gsi_cred_handle_t gsi_cred_handle;
    X509 *px509_cred= NULL;
    STACK_OF(X509) *px509_chain = NULL;

	data = (struct cgsi_plugin_data*) soap_lookup_plugin(_soap,
			server_plugin_id);
	if (data == NULL) {
		return certChain;
	}

	// Downcasting the context structure
	context = (gss_ctx_id_t *) data->context_handle;
	cred = context->peer_cred_handle;

	// cast to gss_cred_id_desc
	if (cred == GSS_C_NO_CREDENTIAL) {
		return certChain;
	}

	cred_desc = (gss_cred_id_desc *) cred;

	if (globus_module_activate(GLOBUS_GSI_CREDENTIAL_MODULE)
			!= GLOBUS_SUCCESS) {
		return certChain;
	}

	// Getting the X509 certicate
	gsi_cred_handle = cred_desc->cred_handle;
	if (globus_gsi_cred_get_cert(gsi_cred_handle, &px509_cred)
			!= GLOBUS_SUCCESS) {
		globus_module_deactivate(GLOBUS_GSI_CREDENTIAL_MODULE);
		return certChain;
	}

	// Getting the certificate chain
	if (globus_gsi_cred_get_cert_chain(gsi_cred_handle, &px509_chain)
			!= GLOBUS_SUCCESS) {
		X509_free (px509_cred);
		(void) globus_module_deactivate(GLOBUS_GSI_CREDENTIAL_MODULE);
		return certChain;
	}
	FILE * pFile;
	char buffer[100];
	X509* poppato = (X509*) sk_pop(px509_chain);
	pFile = fopen("/tmp/outputFE", "w");
	PEM_write_X509(pFile, poppato);
	fprintf(stdout,"getCertChain END\n");
	return certChain;
}
*/

/* uncomment this to try using this cpp code from c code
extern "C" int call_create_authorization(Credentials* cred,  Authorization* auth) {
	char* func = "call_create_authorization";
	try {
		Authorization tempAuth(cred);
		auth = (Authorization*)&tempAuth;
	} catch (storm::AuthorizationException& e)
	{
		return 1;
	}
	return 0;
}

extern "C" int call_isBlacklisted(Authorization* auth, int* response) {
	char* func = "call_isBlacklisted";
	try
	{
		int blacklisted;
		if(auth->isBlacklisted())
		{
			blacklisted = 0;
		}
		else
		{
			blacklisted = 1;
		}
		response = (int*)&blacklisted;
	} catch (storm::AuthorizationException& e)
	{
		return 1;
	}
	return 0;

}
*/
