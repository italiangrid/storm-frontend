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

#include "Authorization.hpp"
#include "srmlogit.h"

using namespace storm;

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

	char* funcName = "isBlacklisted";
	bool blacklisted = false;
	if(Authorization::blacklistRequested)
	{
		xacml_response_t * response;
		//const char* subject = credentials->getDN().c_str();
		//srmlogit(STORM_LOG_DEBUG, funcName, "Constructing Subject from %s\n", subject);
		xacml_request_t * request = create_xacml_request(credentials->getCertChain(), DEFAULT_AUTHORIZATION_RESOURCE.c_str(), DEFAULT_AUTHORIZATION_ACTION.c_str());
		pep_error_t pep_rc = pep_authorize(pep, &request, &response);
		if (pep_rc != PEP_OK) {
			xacml_request_delete(request);
			xacml_response_delete(response);
			throw storm::AuthorizationException(new std::string("Failed to authorize XACML request: %s\n", pep_strerror(pep_rc)));
		}
		xacml_decision_t decision;
		 try
		{
			decision = process_xacml_response(response);
		}catch(AuthorizationException &e)
		{
			xacml_request_delete(request);
			xacml_response_delete(response);
			throw e;
		}
		blacklisted = evaluateResponse(&decision);
		xacml_request_delete(request);
		xacml_response_delete(response);
	}
	return blacklisted;
}

xacml_request_t* Authorization::create_xacml_request(const char * subject_value, const char * resourceid, const char * actionid) throw (storm::AuthorizationException)
{
    char* funcName = "create_xacml_request";
    xacml_subject_t * subject = create_xacml_subject(subject_value);
    xacml_resource_t * resource;
    try
    {
    	resource = create_xacml_resource(resourceid);
    }catch(AuthorizationException &e)
    {
    	xacml_subject_delete(subject);
    	throw e;
    }
    xacml_action_t * action;
    try
	{
    	action = create_xacml_action(actionid);
	}catch(AuthorizationException &e)
	{
		xacml_resource_delete(resource);
		xacml_subject_delete(subject);
		throw e;
	}
    xacml_environment_t * environment;
    try
   	{
    	environment = create_xacml_environment_profile(DEFAULT_AUTHORIZATION_PROFILE.c_str());
   	}catch(AuthorizationException &e)
   	{
   		xacml_action_delete(action);
   		xacml_resource_delete(resource);
   		xacml_subject_delete(subject);
   		throw e;
   	}
    xacml_request_t* request;
    try
	{
    	request = assemble_xacml_request(subject,resource,action,environment);
	}catch(AuthorizationException &e)
	{
		xacml_environment_delete(environment);
		xacml_action_delete(action);
		xacml_resource_delete(resource);
		xacml_subject_delete(subject);
		throw e;
	}
    return request;
}

xacml_subject_t * Authorization::create_xacml_subject(const char * subjectid) throw (storm::AuthorizationException) {
    static char * funcName = "create_xacml_subject";

    if (subjectid==NULL) {
    	throw storm::AuthorizationException("Illegal Argument. Received NULL Subject\n");
    }
    xacml_subject_t * subject= xacml_subject_create();
    if (subject==NULL) {
		throw storm::AuthorizationException("Cannot create XACML Subject\n");
    }
    //xacml_attribute_t * subject_attr_id= xacml_attribute_create(XACML_SUBJECT_ID);
    xacml_attribute_t * subject_attr_id= xacml_attribute_create(XACML_SUBJECT_KEY_INFO);
    if (subject_attr_id==NULL) {
    	xacml_subject_delete(subject);
    	throw storm::AuthorizationException(new std::string("Cannot create XACML %s Subject Attribute\n",XACML_SUBJECT_KEY_INFO));
    }
    if (xacml_attribute_setdatatype(subject_attr_id,XACML_DATATYPE_STRING) != PEP_XACML_OK) {
    	xacml_attribute_delete(subject_attr_id);
		xacml_subject_delete(subject);
    	throw storm::AuthorizationException(new std::string("Cannot set XACML %s Subject Attribute data type\n" , XACML_DATATYPE_STRING));
    }
    if (xacml_attribute_addvalue(subject_attr_id,subjectid) != PEP_XACML_OK) {
        xacml_attribute_delete(subject_attr_id);
        xacml_subject_delete(subject);
        throw storm::AuthorizationException(new std::string("Cannot set XACML %s Subject Attribute value\n" , subjectid));
    }
    if (xacml_subject_addattribute(subject,subject_attr_id) != PEP_XACML_OK) {
        xacml_attribute_delete(subject_attr_id);
        xacml_subject_delete(subject);
        throw storm::AuthorizationException("Cannot add XACML Subject Attribute\n");
    }
    return subject;
}

xacml_resource_t* Authorization::create_xacml_resource(const char * resourceid) throw (storm::AuthorizationException) {

	static char * funcName = "create_xacml_resource_id";

    if (resourceid==NULL) {
    	throw storm::AuthorizationException("Illegal Argument. Received NULL Resource_id\n");
    }
    xacml_resource_t* resource= xacml_resource_create();
    if (resource==NULL) {
    	throw storm::AuthorizationException("Cannot create XACML Resource\n");
    }
    xacml_attribute_t * resource_attr_id= xacml_attribute_create(XACML_RESOURCE_ID);
    if (resource_attr_id==NULL) {
        xacml_resource_delete(resource);
    	throw storm::AuthorizationException(new std::string("Cannot create XACML %s Resource Attribute\n" , XACML_RESOURCE_ID));
    }
    if (xacml_attribute_addvalue(resource_attr_id,resourceid) != PEP_XACML_OK) {
        xacml_attribute_delete(resource_attr_id);
        xacml_resource_delete(resource);
    	throw storm::AuthorizationException(new std::string("Cannot set XACML %s Resource Attribute value\n" , resourceid));
    }
    if (xacml_resource_addattribute(resource,resource_attr_id) != PEP_XACML_OK) {
        xacml_attribute_delete(resource_attr_id);
        xacml_resource_delete(resource);
    	throw storm::AuthorizationException("Cannot add XACML Resource Attribute\n");
    }
    return resource;
}

xacml_action_t * Authorization::create_xacml_action(const char * actionid) throw (storm::AuthorizationException) {

	static char * funcName = "create_xacml_action";

    if (actionid==NULL) {
    	throw storm::AuthorizationException("Illegal Argument. Received NULL Action_id\n");
    }
    xacml_action_t * action= xacml_action_create();
    if (action==NULL) {
    	throw storm::AuthorizationException("Cannot create XACML Action\n");
    }
    xacml_attribute_t * action_attr_id= xacml_attribute_create(XACML_ACTION_ID);
    if (action_attr_id==NULL) {
        xacml_action_delete(action);
        throw storm::AuthorizationException(new std::string("Cannot create XACML %s Action Attribute\n" , XACML_ACTION_ID));
    }
    if (xacml_attribute_addvalue(action_attr_id,actionid) != PEP_XACML_OK) {
        xacml_attribute_delete(action_attr_id);
        xacml_action_delete(action);
        throw storm::AuthorizationException(new std::string("Cannot set XACML %s Action Attribute value\n" , actionid));
    }
    if (xacml_action_addattribute(action,action_attr_id) != PEP_XACML_OK) {
		xacml_attribute_delete(action_attr_id);
		xacml_action_delete(action);
		throw storm::AuthorizationException("Cannot add XACML Action Attribute\n");
    }
    return action;
}

xacml_environment_t* Authorization::create_xacml_environment_profile(const char * profileid) throw (storm::AuthorizationException) {

    static char * funcName = "create_xacml_environment_profile";

    if (profileid == NULL) {
		throw storm::AuthorizationException("Illegal Argument. Received NULL Profile_id\n");
	}
    xacml_environment_t * environment = xacml_environment_create();
    if (environment==NULL) {
    	throw storm::AuthorizationException("Cannot create XACML Environment\n");
    }
    xacml_attribute_t * profile_attr_id= xacml_attribute_create(XACML_GRIDWN_ATTRIBUTE_PROFILE_ID);
    if (profile_attr_id==NULL) {
        xacml_environment_delete(environment);
        throw storm::AuthorizationException(new std::string("Cannot create XACML %s Profile Attribute\n" , XACML_GRIDWN_ATTRIBUTE_PROFILE_ID));
    }
    if (xacml_attribute_addvalue(profile_attr_id,profileid) != PEP_XACML_OK) {
        xacml_attribute_delete(profile_attr_id);
        xacml_environment_delete(environment);
        throw storm::AuthorizationException(new std::string("Cannot set XACML %s Profile Attribute value\n" , profileid));
    }
    if (xacml_environment_addattribute(environment,profile_attr_id) != PEP_XACML_OK) {
		xacml_attribute_delete(profile_attr_id);
		xacml_environment_delete(environment);
		throw storm::AuthorizationException("Cannot add XACML Environment Profile Attribute\n");
    }
    return environment;

}

xacml_request_t * Authorization::assemble_xacml_request(xacml_subject_t * subject, xacml_resource_t * resource, xacml_action_t * action, xacml_environment_t * environment) throw (storm::AuthorizationException) {

    static char * funcName = "assemble_xacml_request";
    if (subject  == NULL || resource  == NULL || action  == NULL || environment == NULL) {
    	throw storm::AuthorizationException("Illegal Argument. Received NULL parameters\n");
    }
    xacml_request_t * request= xacml_request_create();
    if (request==NULL) {
    	throw storm::AuthorizationException("Cannot create XACML Request\n");
    }
    if (subject!=NULL && xacml_request_addsubject(request,subject) != PEP_XACML_OK) {
        xacml_request_delete(request);
        throw storm::AuthorizationException("Cannot add XACML Request Subject\n");
    }
    if (resource!=NULL && xacml_request_addresource(request,resource)!=PEP_XACML_OK) {
        xacml_request_delete(request);
        throw storm::AuthorizationException("Cannot add XACML Request Resource\n");
    }
    if (action!=NULL && xacml_request_setaction(request,action)!=PEP_XACML_OK) {
        xacml_request_delete(request);
        throw storm::AuthorizationException("Cannot add XACML Request Action\n");
    }
    if (environment!=NULL && xacml_request_setenvironment(request,environment) != PEP_XACML_OK) {
		xacml_request_delete(request);
		throw storm::AuthorizationException("Cannot add XACML Request Environment\n");
	}
    return request;
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
			printXamlResult(result);
        }
        else
        {
        	srmlogit(STORM_LOG_DEBUG, funcName, "Printing the content of a further response. It will not be considered\n");
        	printXamlResult(result);
        }
    }
    return decision;
}

bool Authorization::evaluateResponse(xacml_decision_t* decision)
{
	char* funcName = "isBlacklisted";
	if (*decision == XACML_DECISION_PERMIT) {
		return false;
	}
	else
	{
		if(*decision != XACML_DECISION_DENY && *decision != XACML_DECISION_NOT_APPLICABLE)
		{
			srmlogit(STORM_LOG_WARNING, funcName, "Received unknown decision from Argus: %s\n Considering the user as not blacklisted" ,  Authorization::decision_tostring(*decision));
		}
	}
	return true;
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
