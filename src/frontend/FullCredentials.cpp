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

#include "FullCredentials.hpp"
#include "srmlogit.h"
#include <openssl/stack.h>
#include <openssl/x509.h>

using namespace storm;


FullCredentials::FullCredentials(struct soap *soap) : Credentials(soap) {
	static const char* const funcName = "Credentials()";
	struct cgsi_plugin_data *data;
	data = (struct cgsi_plugin_data*)soap_lookup_plugin(soap, "CGSI_PLUGIN_SERVER_1.0");
	if (data == NULL) {
		srmlogit(STORM_LOG_ERROR, funcName, "Credentials: could not get CGSI_PLUGIN_SERVER_1 data structure\n");
		return;
	}
	gss_ctx_id_t gss_context = (gss_ctx_id_desc *) data->context_handle;
	gss_cred_id_t cred;
	try
	{
		cred = FullCredentials::get_gss_cred_id(gss_context);
	}
	catch(std::invalid_argument &e)
	{
		srmlogit(STORM_LOG_ERROR, funcName, "Credentials: unable to get gss credential gss context: %s\n" , e.what());
		return;
	}
	try
	{
		globus_gsi_cred_handle_t gsi_cred = get_gss_cred_handle(cred);
		X509* x509 = FullCredentials::gss_cred_extract_cert(gsi_cred);
		STACK_OF(X509)* chain = FullCredentials::gss_cred_extract_cert_chain(gsi_cred);
		if(x509 == NULL || chain == NULL)
		{
			srmlogit(STORM_LOG_ERROR, funcName, "Credentials: could not get cert and cert_chain from gsi credentials\n");
			if(x509 != NULL)
			{
				X509_free(x509);
			}
			if(chain != NULL)
			{
				sk_X509_pop_free(chain,X509_free);
			}
			return;
		}
		cert_chain = FullCredentials::x509_convert_to_PEM(x509,chain);
		sk_X509_pop_free(chain,X509_free);
		X509_free(x509);
	}
	catch(CredentialException &e)
	{
		srmlogit(STORM_LOG_ERROR, funcName, "Credentials: unable to get cert chain from gss context: %s\n" , e.what());
		return;
	}
}

/**
 * Returns the gss_cred_id handle from the GSS context.
 */
gss_cred_id_t FullCredentials::get_gss_cred_id(const gss_ctx_id_t gss_context)  throw (std::invalid_argument)
{
	static char * funcName = "get_gss_cred_id";
    if (gss_context==NULL) {
    	srmlogit(STORM_LOG_ERROR, funcName, "Credentials: null parameter\n");
    	throw std::invalid_argument("Credentials: null parameter\n");
    }
    else {
        return (gss_cred_id_t)gss_context->peer_cred_handle;
    }
}

globus_gsi_cred_handle_t FullCredentials::get_gss_cred_handle(const gss_cred_id_t gss_cred) throw (CredentialException)
{
    // function name for error macros
    static char * funcName = "get_gss_cred_handle";
    /* Internally a gss_cred_id_t type is a pointer to a gss_cred_id_desc */

    globus_gsi_cred_handle_t gsi_cred;
    /* cast to gss_cred_id_desc */
    if (gss_cred != GSS_C_NO_CREDENTIAL) {
    	gss_cred_id_desc * cred_desc = (gss_cred_id_desc *) gss_cred;
    	gsi_cred = cred_desc->cred_handle;
		if (gsi_cred == NULL) {
			throw CredentialException(
					"Can not get GSI credentials handle from GSS credentials\n");
		}
	} else {
		throw CredentialException("No GSS credentials available\n");
	}
    return gsi_cred;
}

X509 * FullCredentials::gss_cred_extract_cert(const globus_gsi_cred_handle_t gsi_cred) throw (CredentialException)
{
    // function name for error macros
    static char * funcName = "gss_cred_extract_cert";
    /* Internally a gss_cred_id_t type is a pointer to a gss_cred_id_desc */

    X509* cert;
	if (globus_gsi_cred_get_cert(gsi_cred, &cert) != 0) {
		throw CredentialException(
				"Can not extract cert from GSI credentials\n");
	}else {
		srmlogit(STORM_LOG_DEBUG, funcName,
				"Cert credentials obtained\n");
	}
    return cert;
}

STACK_OF(X509) * FullCredentials::gss_cred_extract_cert_chain(const globus_gsi_cred_handle_t gsi_cred) throw (CredentialException)
{
    // function name for error macros
    static char * funcName = "gss_cred_extract_cert_chain";
    STACK_OF(X509) * chain;

    // internally a gss_cred_id_t type is a pointer to a gss_cred_id_desc

	if (globus_gsi_cred_get_cert_chain(gsi_cred, &chain)!= 0) {
		throw CredentialException(
				" globus_gsi_cred_get_cert_chain failure\n");
	} else {
		srmlogit(STORM_LOG_DEBUG, funcName,"Cert chain obtained\n");
	}
    return chain;
}

char * FullCredentials::x509_convert_to_PEM(const X509 * x509, const STACK_OF(X509) * chain) throw (CredentialException)
{
    // function name for error macros
    static char * funcName = "x509_convert_to_PEM";
    char * pem;

    BIO * bio = BIO_new(BIO_s_mem());
    if (bio==NULL) {
		throw CredentialException("Credentials: unable to create the BIO\n");
    }
    if (PEM_write_bio_X509(bio, (X509 *)x509) != 1) {
        BIO_free(bio);
        throw CredentialException("Credentials: unable to write the PEM on the BIO\n");
    }

    int chain_l= sk_X509_num(chain);
    for (int i = 0; i < chain_l; i++) {
		X509 * x509elt = sk_X509_value(chain,i);
		if (x509elt == NULL)
			break;
		if (PEM_write_bio_X509(bio, x509elt) != 1) {
			BIO_free(bio);
			throw CredentialException("Credentials: unable to write x509 element on the BIO\n");
		}
	}
    // bug fix: BIO_get_mem_data returns effective buffer length!!!!
    char *buffer= NULL;
    long buffer_l= 0;
    if ((buffer_l= BIO_get_mem_data(bio,&buffer)) <= 0) {
        BIO_free(bio);
        throw CredentialException("Credentials: unable to put BIO content in a buffer\n");
    }

    // bug fix: BIO_get_mem_data returns effective buffer length!!!!
    pem= strndup(buffer,buffer_l);
    if (pem==NULL) {
		BIO_free(bio);
		throw CredentialException("Credentials: unable to duplicate the PEM buffer string\n");
    }
    BIO_free(bio);
    return pem;
}

