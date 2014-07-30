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

#include <string>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>

#include <boost/format.hpp>
#include <boost/thread/tss.hpp>

#include <globus_common.h>
#include <gssapi.h>
#include <globus_gsi_credential.h>
#include <globus_gss_assist.h>
#include <globus_gss_assist.h>
#include <globus_gridmap_callout_error.h>

#include "gssapi_openssl.h"
#include "cgsi_plugin_int.h"

#include <argus/pep.h>

#include "Authorization.hpp"
#include "srmlogit.h"
#include "FrontendConfiguration.hpp"
#include "storm_exception.hpp"
#include "srmv2H.h"

#define authz_failure(msg)  log_failure_and_throw_authz_error( __func__ , msg )

static std::string const DEFAULT_AUTHORIZATION_RESOURCE = "StoRM";
static std::string const DEFAULT_AUTHORIZATION_ACTION = "access";
static std::string const DEFAULT_AUTHORIZATION_PROFILE = "http://glite.org/xacml/profile/grid-wn/1.0";

static boost::thread_specific_ptr<PEP> pep_handle(pep_destroy);

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
create_xacml_subject(std::string const& subjectid) {

  assert(!subjectid.empty());

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

  if (xacml_attribute_addvalue(subject_attr_id, subjectid.c_str()) != PEP_XACML_OK) {
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
create_xacml_resource(std::string const& resourceid) {

  assert(!resourceid.empty());

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

  if (xacml_attribute_addvalue(resource_attr_id, resourceid.c_str()) != PEP_XACML_OK) {
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
create_xacml_action(std::string const& actionid) {

  assert(!actionid.empty());

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

  if (xacml_attribute_addvalue(action_attr_id, actionid.c_str()) != PEP_XACML_OK) {
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
create_xacml_environment_profile(std::string const& profileid) {

  assert(!profileid.empty());

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

  if (xacml_attribute_addvalue(profile_attr_id, profileid.c_str()) != PEP_XACML_OK) {
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

  assert(subject && resource && action && environment);

  xacml_request_t * request = xacml_request_create();

  if (request == NULL) {
    authz_failure("Error creating XACML request.");
  }

  if (xacml_request_addsubject(request, subject) != PEP_XACML_OK) {
    xacml_request_delete(request);
    authz_failure("Error adding XACML request subject.");
  }

  if (xacml_request_addresource(request, resource) != PEP_XACML_OK) {
    xacml_request_delete(request);
    authz_failure("Error adding XACML request resource.");
  }

  if (xacml_request_setaction(request, action) != PEP_XACML_OK) {
    xacml_request_delete(request);
    authz_failure("Error adding XACML request action.");
  }

  if (xacml_request_setenvironment(request, environment) != PEP_XACML_OK) {
    xacml_request_delete(request);
    authz_failure("Error adding XACML request environment.");
  }

  return request;
}

static 
xacml_request_t*
create_xacml_request(std::string const& subject_value, std::string const& resourceid,
                     std::string const& actionid) {

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
        DEFAULT_AUTHORIZATION_PROFILE);

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
    boost::shared_ptr<xacml_response_t> response) {

  if (!response) {
    authz_failure("Cannot process a NULL xacml response.");
  }

  xacml_decision_t decision;
  size_t results_length = xacml_response_results_length(response.get());
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

    result = xacml_response_getresult(response.get(), i);

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

static
globus_gsi_cred_handle_t 
get_gsi_credential_from_soap(soap* soap){

  static const char* CGSI_PLUGIN_ID = "CGSI_PLUGIN_SERVER_1.0";

  struct cgsi_plugin_data *data =
      (struct cgsi_plugin_data*) soap_lookup_plugin(soap, CGSI_PLUGIN_ID);

  if (! data ){
    srmlogit(STORM_LOG_ERROR, __func__,"CGSI plugin not found!\n");
    throw storm::authorization_error("CGSI plugin not found!");
  }

  gss_ctx_id_t gss_context = data->context_handle;
  if (! gss_context ){
    srmlogit(STORM_LOG_ERROR, __func__,"Error extracting context from CGSI plugin: null context!\n");
    throw storm::authorization_error("Error extracting context from CGSI plugin: null context!\n");
  }

  gss_cred_id_t gss_cred = (gss_cred_id_t) gss_context->peer_cred_handle;

  if ( gss_cred == GSS_C_NO_CREDENTIAL ) {
    throw storm::authorization_error("No GSS credential found!");
  }

  return gss_cred->cred_handle;
}

static void
release_chain(STACK_OF(X509) *chain){
  sk_X509_pop_free(chain,X509_free);
}

static
std::string 
get_pem_from_cred_handle(const globus_gsi_cred_handle_t cred){

  STACK_OF(X509) *chain_ptr = 0;

  if (globus_gsi_cred_get_cert_chain(cred, &chain_ptr)) {
    throw storm::authorization_error("globus_gsi_cred_get_cert_chain failed!");
  }

  boost::shared_ptr<STACK_OF(X509)> chain(chain_ptr,release_chain);

  X509 *cert_ptr = 0;

  if (globus_gsi_cred_get_cert(cred, &cert_ptr)) {
    throw storm::authorization_error("globus_gsi_cred_get_cert failed!");
  }
  
  boost::shared_ptr<X509> cert(cert_ptr,X509_free);

  boost::shared_ptr<BIO> bio(BIO_new(BIO_s_mem()), BIO_free);

  if (!bio) {
    throw storm::authorization_error("BIO_new() failed!");
  }

  if (!PEM_write_bio_X509(bio.get(),cert.get())) {
    throw storm::authorization_error("PEM_write_bio_X509() failed!");
  }

  const int chain_size = sk_X509_num(chain.get());

  for (int i=0; i < chain_size; i++){
    X509 *chain_ele = sk_X509_value(chain.get(),i);

    if (!chain_ele) break;

    if (!PEM_write_bio_X509(bio.get(), chain_ele)){
      throw storm::authorization_error("PEM_write_bio_X509() failed!");
    }
  }
  
  char* pem_chain_buf = 0;
  long pem_chain_buf_len = BIO_get_mem_data(bio.get(),&pem_chain_buf);

  if (pem_chain_buf_len <= 0){
    throw storm::authorization_error("BIO_get_mem_data() failed!");
  }

  return std::string(pem_chain_buf, pem_chain_buf_len);
}

static 
std::string
get_pem_from_soap(soap *soap){
  return get_pem_from_cred_handle(get_gsi_credential_from_soap(soap));
}

static bool argus_check_enabled(){
  return FrontendConfiguration::getInstance()->getUserCheckBlacklist();
}

static void
set_pep_option(pep_option opt, std::string const& value){
  pep_error_t pep_rc = pep_setoption(pep_handle.get(), opt, value.c_str());
  if (pep_rc != PEP_OK){
    authz_failure(boost::format("Error setting pep option: %s") % pep_strerror(pep_rc));
  }
}

static void
make_pep(){

  pep_handle.reset(pep_initialize());

  if (!pep_handle.get()){
    authz_failure("Error building PEP client");
  }

  std::string pepd_endpoint =
      FrontendConfiguration::getInstance()->getArgusPepdEndpoint();

  if (pepd_endpoint.empty()) {
    authz_failure("No ARGUS PEPD endpoint configured.");
  }
  
  set_pep_option(PEP_OPTION_ENDPOINT_URL, pepd_endpoint);

  std::string key =
      FrontendConfiguration::getInstance()->getHostKeyFile();

  if (key.empty()) {
    authz_failure("Empty private key.");
  }

  set_pep_option(PEP_OPTION_ENDPOINT_CLIENT_KEY, key);

  std::string cert =
      FrontendConfiguration::getInstance()->getHostCertFile();

  if (cert.empty()) {
    authz_failure("certificate is misconfigured.");
  }

  set_pep_option(PEP_OPTION_ENDPOINT_CLIENT_CERT, cert);

  std::string ca_path = 
      FrontendConfiguration::getInstance()->getCaCertificatesFolder();

  if (ca_path.empty()){
    authz_failure("ca path is misconfigured.");
  }

  set_pep_option(PEP_OPTION_ENDPOINT_SERVER_CAPATH, ca_path);
}

namespace storm{
namespace authz{

bool is_blacklisted(soap* soap){

  if (!argus_check_enabled()) return false;

  std::string pem_chain = get_pem_from_soap(soap);

  std::string resource_id = 
      FrontendConfiguration::getInstance()->getArgusResourceId();

  if (resource_id.empty()){
    resource_id = DEFAULT_AUTHORIZATION_RESOURCE;
  }

  // Initialize thread local PEP handle, if not already done
  if (!pep_handle.get()){
    make_pep();
  }

  xacml_request_t * request_ptr = create_xacml_request(pem_chain, 
                                                       resource_id,
                                                       DEFAULT_AUTHORIZATION_ACTION);

  xacml_response_t *response_ptr = 0;

  pep_error_t pep_rc = pep_authorize(pep_handle.get(), &request_ptr, &response_ptr);

  boost::shared_ptr<xacml_request_t> request(request_ptr, 
                                             xacml_request_delete);

  boost::shared_ptr<xacml_response_t> response(response_ptr,
                                               xacml_response_delete);

  if (pep_rc != PEP_OK){
    authz_failure(boost::format("Failed to authorize XACML request: %s.")
                  % pep_strerror(pep_rc));
  }

  xacml_decision_t decision = process_xacml_response(response);

  return (decision != XACML_DECISION_PERMIT);

  return false;

}
}
}
