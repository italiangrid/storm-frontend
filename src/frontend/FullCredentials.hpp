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

#ifndef FULL_CREDENTIALS_HPP_
#define FULL_CREDENTIALS_HPP_

#include <globus_common.h>
#include <gssapi.h>
#include <globus_gsi_credential.h>
#include <globus_gss_assist.h>
#include <globus_gridmap_callout_error.h>

#include "gssapi_openssl.h"
#include "cgsi_plugin_int.h"

#include "Credentials.hpp"
#include "CredentialException.hpp"
#include <stdexcept>

namespace storm {

class FullCredentials : public Credentials{
public:
	FullCredentials(struct soap *soap);// : Credentials(soap){};

	~FullCredentials(){
		free(this->cert_chain);
	}

	char* getCertChain() {
		return cert_chain;
	}

private:
	char* cert_chain;

	static STACK_OF(X509) * gss_cred_extract_cert_chain(const globus_gsi_cred_handle_t gsi_cred) throw (CredentialException);
	static char * x509_convert_to_PEM(const X509 * x509, const STACK_OF(X509) * chain) throw (CredentialException);
	static gss_cred_id_t get_gss_cred_id(const gss_ctx_id_t gss_context)  throw (std::invalid_argument);
	static globus_gsi_cred_handle_t get_gss_cred_handle(const gss_cred_id_t gss_cred) throw (CredentialException);
	static X509 * gss_cred_extract_cert(const globus_gsi_cred_handle_t gsi_cred) throw (CredentialException);
};

}

#endif /*FULL_CREDENTIALS_HPP_*/
