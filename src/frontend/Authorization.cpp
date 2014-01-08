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
#include <boost/format.hpp>

#define authz_failure(msg)  log_failure_and_throw_authz_error( __func__ , msg )

static void log_failure_and_throw_authz_error(const char* func,
		const std::string& msg) {
	srmlogit(STORM_LOG_ERROR, func, "%s\n", msg.c_str());
	throw storm::authorization_error(msg);
}

static void log_failure_and_throw_authz_error(const char* func,
		boost::format& fmt) {
	log_failure_and_throw_authz_error(func, boost::str(fmt));
}

static std::string decision_tostring(xacml_decision_t decision) {

	switch (decision) {
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
		authz_failure("Unsupported xacml_decision_t received.");
	}
}

static std::string fulfillon_tostring(xacml_fulfillon_t fulfillon) {
	switch (fulfillon) {
	case XACML_FULFILLON_DENY:
		return "Deny";
		break;
	case XACML_FULFILLON_PERMIT:
		return "Permit";
		break;

	default:
		authz_failure("Unsupported xacml_fulfillon_t received.");
	}
}
static
void printXACMLObligation(xacml_obligation_t * obligation) {

	srmlogit(STORM_LOG_DEBUG, __func__, "----Obligation BEGIN----\n");
	srmlogit(STORM_LOG_DEBUG, __func__, "Obligation id: %s\n",
			xacml_obligation_getid(obligation));
	srmlogit(STORM_LOG_DEBUG, __func__, "Obligation fulfillOn: %s\n",
			fulfillon_tostring(xacml_obligation_getfulfillon(obligation)).c_str());

	size_t attrsLength = xacml_obligation_attributeassignments_length(
			obligation);

	srmlogit(STORM_LOG_DEBUG, __func__, "Obligation attributes length: %d\n",
			(int) attrsLength);

	for (int i = 0; i < attrsLength; i++) {
		srmlogit(STORM_LOG_DEBUG, __func__, "++Attribute BEGIN++\n");
		xacml_attributeassignment_t * attr =
				xacml_obligation_getattributeassignment(obligation, i);
		srmlogit(STORM_LOG_DEBUG, __func__, "Attribute Id: %s\n",
				xacml_attributeassignment_getid(attr));
		srmlogit(STORM_LOG_DEBUG, __func__, "Attribute datatype: %s\n",
				xacml_attributeassignment_getdatatype(attr));
		srmlogit(STORM_LOG_DEBUG, __func__, "Attribute value: %s\n",
				xacml_attributeassignment_getvalue(attr));
		srmlogit(STORM_LOG_DEBUG, __func__, "++Attribute END++\n");
	}

	srmlogit(STORM_LOG_DEBUG, __func__, "----Obligation END----\n");
}

static void printXACMLResult(xacml_result_t * result) {

	srmlogit(STORM_LOG_DEBUG, __func__, "----Result BEGIN----\n");

	srmlogit(STORM_LOG_DEBUG, __func__, "Resource Id: %s\n",
			xacml_result_getresourceid(result));

	srmlogit(STORM_LOG_DEBUG, __func__, "Result decision: %s\n",
			decision_tostring(xacml_result_getdecision(result)).c_str());

	srmlogit(STORM_LOG_DEBUG, __func__, "Status message is: %s\n",
			xacml_status_getmessage(xacml_result_getstatus(result)));

	srmlogit(STORM_LOG_DEBUG, __func__, "Status code: %s\n",
			xacml_statuscode_getvalue(
					xacml_status_getcode(xacml_result_getstatus(result))));

	if (xacml_statuscode_getsubcode(
			xacml_status_getcode(xacml_result_getstatus(result))) != NULL) {
		srmlogit(STORM_LOG_DEBUG, __func__, "Status subcode: %s\n",
				xacml_statuscode_getvalue(
						xacml_statuscode_getsubcode(
								xacml_status_getcode(
										xacml_result_getstatus(result)))));
	}

	size_t obligationsLength = xacml_result_obligations_length(result);
	srmlogit(STORM_LOG_DEBUG, __func__, "Status obligations:\n");
	for (int i = 0; i < obligationsLength; i++) {
		//we do not manage obligations
		srmlogit(STORM_LOG_DEBUG, __func__,
				"Printing the content of an Obligation received. It will not be considered\n");
		printXACMLObligation(xacml_result_getobligation(result, i));
	}
	srmlogit(STORM_LOG_DEBUG, __func__, "----Result END----\n");
}
static xacml_subject_t*
create_xacml_subject(const char * subjectid) {

	if (subjectid == NULL) {
		authz_failure("Invalid subject: NULL ");
	}

	xacml_subject_t * subject = xacml_subject_create();

	if (subject == NULL) {
		authz_failure("Error creating XACML subject.");
	}

	xacml_attribute_t * subject_attr_id = xacml_attribute_create(
			XACML_SUBJECT_KEY_INFO);

	if (subject_attr_id == NULL) {
		xacml_subject_delete(subject);
		authz_failure(boost::format("Error creating XACML subject attribute: %s") % XACML_SUBJECT_KEY_INFO);
	}

	if (xacml_attribute_setdatatype(subject_attr_id,
			XACML_DATATYPE_STRING) != PEP_XACML_OK) {
		xacml_attribute_delete(subject_attr_id);
		xacml_subject_delete(subject);
		authz_failure(boost::format("Error setting XACML subject data type: %s")
			% XACML_DATATYPE_STRING);
	}

	if (xacml_attribute_addvalue(subject_attr_id, subjectid) != PEP_XACML_OK) {
		xacml_attribute_delete(subject_attr_id);
		xacml_subject_delete(subject);
		authz_failure(
				boost::format("Error setting XACML subject attribute value: %s")
				% subjectid);
	}

	if (xacml_subject_addattribute(subject, subject_attr_id) != PEP_XACML_OK) {
		xacml_attribute_delete(subject_attr_id);
		xacml_subject_delete(subject);
		authz_failure("Error setting XACML subject attribute");
	}

	return subject;
}

static xacml_resource_t*
create_xacml_resource(const char * resourceid) {

	if (resourceid == NULL) {
		authz_failure("Illegal resourceid: NULL");
	}

	xacml_resource_t* resource = xacml_resource_create();

	if (resource == NULL) {
		authz_failure("Error creating XACML resource.");
	}

	xacml_attribute_t * resource_attr_id = xacml_attribute_create(
			XACML_RESOURCE_ID);

	if (resource_attr_id == NULL) {
		xacml_resource_delete(resource);
		authz_failure(
				boost::format("Error creating XACML resource attribute: %s")
				% XACML_RESOURCE_ID);
	}

	if (xacml_attribute_addvalue(resource_attr_id, resourceid) != PEP_XACML_OK) {
		xacml_attribute_delete(resource_attr_id);
		xacml_resource_delete(resource);
		authz_failure(
				boost::format("Error setting XACML resource attribute value: %s")
				% resourceid
				);
	}

	if (xacml_resource_addattribute(resource, resource_attr_id) != PEP_XACML_OK) {
		xacml_attribute_delete(resource_attr_id);
		xacml_resource_delete(resource);
		authz_failure("Error adding XACML resource attribute.");
	}

	return resource;
}


static xacml_action_t*
create_xacml_action(const char * actionid) {

	if (actionid == NULL) {
		authz_failure("Illegal actionid: NULL");
	}

	xacml_action_t * action = xacml_action_create();

	if (action == NULL) {
		authz_failure("Error creating XAMCL action.");
	}

	xacml_attribute_t * action_attr_id = xacml_attribute_create(
			XACML_ACTION_ID);

	if (action_attr_id == NULL) {
		xacml_action_delete(action);
		authz_failure(
				boost::format("Error creating XACML action attribute: %s")
				% XACML_ACTION_ID);
	}

	if (xacml_attribute_addvalue(action_attr_id, actionid) != PEP_XACML_OK) {
		xacml_attribute_delete(action_attr_id);
		xacml_action_delete(action);
		authz_failure(
				boost::format("Error setting XACML action attribute value: %s")
				% actionid);
	}

	if (xacml_action_addattribute(action, action_attr_id) != PEP_XACML_OK) {
		xacml_attribute_delete(action_attr_id);
		xacml_action_delete(action);
		authz_failure("Error adding XACML action attribute");
	}
	return action;
}

static xacml_environment_t*
create_xacml_environment_profile(const char * profileid) {

	if (profileid == NULL) {
		authz_failure("Null profile id.");
	}
	xacml_environment_t * environment = xacml_environment_create();
	if (environment == NULL) {
		authz_failure("Cannot create XACML Environment.");
	}
	xacml_attribute_t * profile_attr_id = xacml_attribute_create(
			XACML_GRIDWN_ATTRIBUTE_PROFILE_ID);

	if (profile_attr_id == NULL) {
		xacml_environment_delete(environment);
		authz_failure("Cannot create XACML Profile.");
	}

	if (xacml_attribute_addvalue(profile_attr_id, profileid) != PEP_XACML_OK) {
		xacml_attribute_delete(profile_attr_id);
		xacml_environment_delete(environment);
		authz_failure(boost::format("Error creating XACML profile object. %s") %
						profileid);
	}

	if (xacml_environment_addattribute(environment,
			profile_attr_id) != PEP_XACML_OK) {
		xacml_attribute_delete(profile_attr_id);
		xacml_environment_delete(environment);
		authz_failure("Error setting XACML profile attribute.");
	}

	return environment;
}

static xacml_request_t*
assemble_xacml_request(xacml_subject_t * subject, xacml_resource_t * resource,
		xacml_action_t * action, xacml_environment_t * environment) {

	if (subject == NULL || resource == NULL || action == NULL
			|| environment == NULL) {

		authz_failure("Received null arguments while assembling XACML request.");
	}
	xacml_request_t * request = xacml_request_create();

	if (request == NULL) {
		authz_failure("Error creating XACML request.");
	}

	if (subject != NULL
			&& xacml_request_addsubject(request, subject) != PEP_XACML_OK) {
		xacml_request_delete(request);
		authz_failure("Error adding XACML request subject.");
	}

	if (resource != NULL
			&& xacml_request_addresource(request, resource) != PEP_XACML_OK) {
		xacml_request_delete(request);
		authz_failure("Error adding XACML request resource.");
	}

	if (action != NULL
			&& xacml_request_setaction(request, action) != PEP_XACML_OK) {
		xacml_request_delete(request);
		authz_failure("Error adding XACML request action.");
	}

	if (environment != NULL
			&& xacml_request_setenvironment(request, environment)
					!= PEP_XACML_OK) {
		xacml_request_delete(request);
		authz_failure("Error adding XACML request environment.");
	}

	return request;
}

static xacml_request_t*
create_xacml_request(const char * subject_value, const char * resourceid,
		const char * actionid) {

	xacml_subject_t * subject = create_xacml_subject(subject_value);
	xacml_resource_t * resource = 0;

	try {
		resource = create_xacml_resource(resourceid);
	} catch (storm::authorization_error& e) {
		xacml_subject_delete(subject);
		throw e;
	}
	xacml_action_t * action;
	try {
		action = create_xacml_action(actionid);

	} catch (storm::authorization_error &e) {
		xacml_resource_delete(resource);
		xacml_subject_delete(subject);
		throw e;
	}

	xacml_environment_t * environment;
	try {
		environment = create_xacml_environment_profile(
				storm::DEFAULT_AUTHORIZATION_PROFILE.c_str());

	} catch (storm::authorization_error &e) {
		xacml_action_delete(action);
		xacml_resource_delete(resource);
		xacml_subject_delete(subject);
		throw e;
	}

	xacml_request_t* request;
	try {
		request = assemble_xacml_request(subject, resource, action,
				environment);

	} catch (storm::authorization_error &e) {
		xacml_environment_delete(environment);
		xacml_action_delete(action);
		xacml_resource_delete(resource);
		xacml_subject_delete(subject);
		throw e;
	}
	return request;
}

static xacml_decision_t process_xacml_response(
		const xacml_response_t * response) {

	if (response == NULL) {
		authz_failure("Cannot process a NULL xacml response.");
	}

	xacml_decision_t decision;
	size_t results_length = xacml_response_results_length(response);
	srmlogit(STORM_LOG_DEBUG, __func__, "Response: %d results\n",
			static_cast<int>(results_length));

	if (results_length == 0) {
		authz_failure("Response contained no results.");
	}

	if (results_length > 1) {
		srmlogit(STORM_LOG_WARNING, __func__,
				"Received an unexpected number of results: %d.\n",
				static_cast<int>(results_length));
	}

	for (int i = 0; i < results_length; i++) {

		xacml_result_t * result = 0;
		xacml_status_t * status = 0;
		xacml_statuscode_t * statuscode = 0, *subcode = 0;

		result = xacml_response_getresult(response, i);

		if (i == 0) {
			decision = xacml_result_getdecision(result);
			srmlogit(STORM_LOG_DEBUG, __func__,
					"The decision for resource %s is: %s\n",
					xacml_result_getresourceid(result),
					decision_tostring(decision).c_str());

			printXACMLResult(result);

		} else {
			srmlogit(STORM_LOG_DEBUG, __func__,
					"Printing the content of a further response. It will not be considered\n");

			printXACMLResult(result);
		}
	}

	return decision;
}

bool storm::Authorization::checkBlacklist(struct soap* soap) {

	bool response = false;
	if (FrontendConfiguration::getInstance()->getUserCheckBlacklist()) {
		FullCredentials cred(soap);
		Authorization auth((FullCredentials*) &cred);
		response = auth.isBlacklisted();
	}

	return response;
}

storm::Authorization::~Authorization() {
	pep_destroy(pep);
}

storm::Authorization::Authorization(FullCredentials *cred) :
		m_argusResourceId(DEFAULT_AUTHORIZATION_RESOURCE), credentials(cred) {

	using boost::format;
	using boost::str;
	blacklistRequested =
			FrontendConfiguration::getInstance()->getUserCheckBlacklist();

	if (blacklistRequested) {
		srmlogit(STORM_LOG_DEBUG, __func__, "Using %s\n", pep_version());
		pep = pep_initialize();

		if (pep == NULL) {
			authz_failure("Failed to create PEP client.");
		}

		std::string argusPepdEndpoint =
				FrontendConfiguration::getInstance()->getArgusPepdEndpoint();
		if (argusPepdEndpoint.empty()) {
			authz_failure("No ARGUS PEPD endpoint configured.");
		}

		pep_error_t pep_rc = pep_setoption(pep, PEP_OPTION_ENDPOINT_URL,
				argusPepdEndpoint.c_str());

		if (pep_rc != PEP_OK) {
			authz_failure(
					format("Error initializing Argus PEPD endpoint: %s. %s")
							% argusPepdEndpoint % pep_strerror(pep_rc));
		}

		std::string hostKey =
				FrontendConfiguration::getInstance()->getHostKeyFile();
		if (hostKey.empty()) {
			authz_failure("host certificate private key misconfigured.");
		}

		pep_rc = pep_setoption(pep, PEP_OPTION_ENDPOINT_CLIENT_KEY,
				hostKey.c_str());
		if (pep_rc != PEP_OK) {
			authz_failure(
					format("Error setting client key: %s. %s") % hostKey
							% pep_strerror(pep_rc));
		}

		std::string hostCert =
				FrontendConfiguration::getInstance()->getHostCertFile();

		if (hostCert.empty()) {
			authz_failure("host certificate is misconfigured.");
		}

		pep_rc = pep_setoption(pep, PEP_OPTION_ENDPOINT_CLIENT_CERT,
				hostCert.c_str());

		if (pep_rc != PEP_OK) {
			authz_failure(
					format("Error setting host certificate: %s. %s") % hostCert
							% pep_strerror(pep_rc));
		}

		std::string caCertFolder =
				FrontendConfiguration::getInstance()->getCaCertificatesFolder();

		if (caCertFolder.empty()) {
			authz_failure("CA folder misconfigured.");
		}

		// server certificate CA path for validation
		pep_rc = pep_setoption(pep, PEP_OPTION_ENDPOINT_SERVER_CAPATH,
				caCertFolder.c_str());

		if (pep_rc != PEP_OK) {
			authz_failure(
					format("Error setting CA folder: %s. %s") % caCertFolder
							% pep_strerror(pep_rc));
		}
		// debugging options
		pep_rc = pep_setoption(pep, PEP_OPTION_LOG_STDERR, stderr);

		if (pep_rc != PEP_OK) {
			authz_failure(
					format("PEP log initialization error: %s.")
							% pep_strerror(pep_rc));
		}

		pep_rc = pep_setoption(pep, PEP_OPTION_LOG_LEVEL, PEP_LOGLEVEL_WARN);

		if (pep_rc != PEP_OK) {
			authz_failure(
					format("Error setting PEP log level: %s.")
							% pep_strerror(pep_rc));
		}

		std::string argusResourceId =
				FrontendConfiguration::getInstance()->getArgusResourceId();
		if (!argusResourceId.empty()) {
			m_argusResourceId = argusResourceId;
		}
	}
}

/**
 * Return true if the user is blacklisted, false otherwise
 * */
bool storm::Authorization::isBlacklisted() {

	bool blacklisted = false;

	if (blacklistRequested) {

		xacml_response_t * response = NULL;

		xacml_request_t * request = create_xacml_request(
				credentials->getCertChain(),
				m_argusResourceId.c_str(),
				storm::DEFAULT_AUTHORIZATION_ACTION.c_str());

		pep_error_t pep_rc = pep_authorize(pep, &request, &response);

		if (pep_rc != PEP_OK) {
			xacml_request_delete(request);
			xacml_response_delete(response);

			authz_failure(
					boost::format("Failed to authorize XACML request: %s.")
							% pep_strerror(pep_rc));
		}

		xacml_decision_t decision;

		try {

			decision = process_xacml_response(response);

		} catch (storm::authorization_error& e) {
			xacml_request_delete(request);
			xacml_response_delete(response);
			throw e;
		}

		blacklisted = (decision != XACML_DECISION_PERMIT);
		xacml_request_delete(request);
		xacml_response_delete(response);
	}

	return blacklisted;
}
