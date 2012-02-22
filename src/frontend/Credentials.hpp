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

#ifndef CREDENTIALS_HPP_
#define CREDENTIALS_HPP_

// C includes
#include "srmv2H.h"

// STL includes
#include <string>
#include <vector>
#include <stdexcept>

// storm_db include
#include "mysql_query.hpp"

#include "sql_string.hpp"

#include <globus_common.h>
#include <gssapi.h>
#include <globus_gsi_credential.h>
#include <globus_gss_assist.h>
#include <globus_gridmap_callout_error.h>

#include "gssapi_openssl.h"
#include "CredentialException.hpp"
namespace storm {

class Credentials {
public:
	Credentials(struct soap *soap) ;

	~Credentials(){
		free(this->cert_chain);
	}

	void setDN(std::string dn) {
		_clientDN = dn;
	}

	std::string getDN() {
		return _clientDN;
	}

	std::vector<sql_string> getFQANsVector() {
		return _fqans_vector;
	}

	sql_string getFQANsOneString();

	char* getCertChanin() {
		return cert_chain;
	}

	bool saveProxy(std::string requestToken);

private:
	struct soap *_soap;
	std::string _clientDN;
	std::vector<sql_string> _fqans_vector;
	char* cert_chain;

	static STACK_OF(X509) * gss_cred_extract_cert_chain(const globus_gsi_cred_handle_t gsi_cred) throw (CredentialException);
	/*static int x509_convert_to_PEM(const X509 * x509,
			const STACK_OF(X509) * chain, char ** out_pem);*/
	static char * x509_convert_to_PEM(const X509 * x509, const STACK_OF(X509) * chain) throw (CredentialException);
	//static gss_cred_id_t get_gss_cred_id(const gss_ctx_id_t gss_context);
	static gss_cred_id_t get_gss_cred_id(const gss_ctx_id_t gss_context)  throw (std::invalid_argument);
	static globus_gsi_cred_handle_t get_gss_cred_handle(const gss_cred_id_t gss_cred) throw (CredentialException);
	/*static int gss_cred_extract_cert(const gss_cred_id_t gss_cred,
			X509 ** out_cert);*/
	static X509 * gss_cred_extract_cert(const globus_gsi_cred_handle_t gsi_cred) throw (CredentialException);
};

}

#endif /*CREDENTIALS_HPP_*/
