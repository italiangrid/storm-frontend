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

#include "Credentials.hpp"
#include "srmlogit.h"
#include <cgsi_plugin.h>
#include "FrontendConfiguration.hpp"
#include "string.h"
#include "stdlib.h"
#include "cgsi_plugin_int.h"

using namespace storm;

Credentials::Credentials(struct soap *soap)
{
    static const char* const funcName = "Credentials()";
    char clientdn[256];
    char **fqans;
    int i, nbfqans, error;

    _soap = soap;

    // retrieving client DN
    _clientDN = string();
#if defined(GSI_PLUGINS)
    get_client_dn(_soap, clientdn, sizeof(clientdn));
    _clientDN = string(clientdn);
#endif

    // retrieving FQANs
    _fqans_vector = vector<sql_string>();
    fqans = NULL;

#if defined(GSI_PLUGINS)
    nbfqans = 0;
    // fqans will point to a memory area in the soap structure: it must not be freed
    fqans = get_client_roles(_soap, &nbfqans);

    if (fqans == NULL) {
        return;
    }

    /* Paranoic error check for the result returned by the get_client_roles() function */
    if ((nbfqans > 0) && (fqans == NULL)) {
        srmlogit(STORM_LOG_ERROR, funcName, "ERROR: FQAN not found (but they should exist): fqans=NULL\n");
    }

    for (i=0; i<nbfqans; i++) {
        if (fqans[i] == NULL) {
            srmlogit(STORM_LOG_ERROR, funcName, "Strange error: NULL FQAN\n");
        } else {
            _fqans_vector.push_back(fqans[i]);
        }
    }
    struct cgsi_plugin_data *data;
	data = (struct cgsi_plugin_data*)soap_lookup_plugin(soap, "CGSI_PLUGIN_SERVER_1.0");
	if (data == NULL) {
		srmlogit(STORM_LOG_ERROR, funcName, "Credentials: could not get data structure\n");
		return;
	}
	gss_ctx_id_t gss_context = (gss_ctx_id_desc *) data->context_handle;
	gss_cred_id_t cred = Credentials::get_gss_cred_id(gss_context);
	X509 * x509= NULL;
	if ((error= Credentials::gss_cred_extract_cert(cred, &x509)) != 0) {
		srmlogit(STORM_LOG_ERROR, funcName, "Credentials: gss_cred_extract_cert failure\n");
		return ;

	}
	STACK_OF(X509) * chain= NULL;
	if ((error= Credentials::gss_cred_extract_cert_chain(cred, &chain)) != 0) {
		srmlogit(STORM_LOG_ERROR, funcName, "Credentials: gss_cred_extract_cert_chain failure\n");
		return;
	}

	if ((error= Credentials::x509_convert_to_PEM(x509,chain,&cert_chain)) != 0) {
		srmlogit(STORM_LOG_ERROR, funcName, "Credentials: x509_convert_to_PEM failure\n");
		return;
	}
#endif
}

int Credentials::gss_cred_extract_cert(const gss_cred_id_t gss_cred, X509 ** out_cert)
{
    // function name for error macros
    static char * funcName = "gss_cred_extract_cert";
    int result = 0;
    /* Internally a gss_cred_id_t type is a pointer to a gss_cred_id_desc */
    gss_cred_id_desc * cred_desc= NULL;
    globus_gsi_cred_handle_t gsi_cred;


    /* cast to gss_cred_id_desc */
    if (gss_cred != GSS_C_NO_CREDENTIAL) {
        cred_desc = (gss_cred_id_desc *) gss_cred;
        gsi_cred = cred_desc->cred_handle;
        if ((result= globus_gsi_cred_get_cert(gsi_cred, out_cert)) != 0) {
        	srmlogit(STORM_LOG_ERROR, funcName, "Can not extract cert from GSI credential");
        	return result;
        }
    }
    else {
    	srmlogit(STORM_LOG_ERROR, funcName, "No GSS credential available");
    	return 1;
    }
    return result;
}

/**
 * Returns the gss_cred_id handle from the GSS context.
 */
gss_cred_id_t Credentials::get_gss_cred_id(const gss_ctx_id_t gss_context)
{
	static char * funcName = "get_gss_cred_id";
    if (gss_context==NULL) {
    	srmlogit(STORM_LOG_ERROR, funcName, "Credentials: null parameter\n");
        return NULL;
    }
    else {
        return (gss_cred_id_t)gss_context->peer_cred_handle;
    }
}

int Credentials::gss_cred_extract_cert_chain(const gss_cred_id_t gss_cred,STACK_OF(X509) **out_chain)
{
    // function name for error macros
    static char * funcName = "gss_cred_extract_cert_chain";
    int result= 0;

    // internally a gss_cred_id_t type is a pointer to a gss_cred_id_desc
    gss_cred_id_desc * cred_desc= NULL;
    globus_gsi_cred_handle_t gsi_cred;

    /* cast to gss_cred_id_desc */
    if (gss_cred != GSS_C_NO_CREDENTIAL) {
		cred_desc = (gss_cred_id_desc *) gss_cred;
		gsi_cred = cred_desc->cred_handle;
		if ((result = globus_gsi_cred_get_cert_chain(gsi_cred, out_chain))
				!= GLOBUS_SUCCESS) {
			srmlogit(STORM_LOG_ERROR, funcName,
					"Credentials: globus_gsi_cred_get_cert_chain failure\n");
		} else {
			srmlogit(STORM_LOG_INFO, funcName,
					"Credentials: cert chain obtained\n");
		}
	} else {
		srmlogit(STORM_LOG_ERROR, funcName,
				"Credentials: no credentials available: GSS_C_NO_CREDENTIAL\n");
	}
    return result;
}

int Credentials::x509_convert_to_PEM(const X509 * x509, const STACK_OF(X509) * chain, char ** out_pem)
{
    // function name for error macros
    static char * funcName = "x509_convert_to_PEM";
    globus_result_t result= GLOBUS_SUCCESS;
    int i, rc= 0;

    BIO * bio = BIO_new(BIO_s_mem());
    if (bio==NULL) {
		srmlogit(STORM_LOG_ERROR, funcName,
				"Credentials: unable to create the BIO\n");
        return result;
    }
    if ((rc= PEM_write_bio_X509(bio, (X509 *)x509)) != 1) {
		srmlogit(STORM_LOG_ERROR, funcName,
				"Credentials: unable to write the PEM on the BIO\n");
        BIO_free(bio);
        return result;
    }

    int chain_l= sk_X509_num(chain);
    for(i= 0; i<chain_l; i++) {
        X509 * x509elt= sk_X509_value(chain,i);
        if (x509elt == NULL) break;
        if ((rc= PEM_write_bio_X509(bio, x509elt)) != 1) {
    		srmlogit(STORM_LOG_ERROR, funcName,
    				"Credentials: unable to write x509 element on the BIO\n");
            BIO_free(bio);
            return result;
        }
    }
    // bug fix: BIO_get_mem_data returns effective buffer length!!!!
    char *buffer= NULL;
    long buffer_l= 0;
    if ((buffer_l= BIO_get_mem_data(bio,&buffer)) <= 0) {
		srmlogit(STORM_LOG_ERROR, funcName,
				"Credentials: unable to put BIO content in a buffer\n");
        BIO_free(bio);
        return result;
    }

    // bug fix: BIO_get_mem_data returns effective buffer length!!!!
    *out_pem= strndup(buffer,buffer_l);
    if (*out_pem==NULL) {
		srmlogit(STORM_LOG_ERROR, funcName,
				"Credentials: unable to duplicate the PEM buffer string\n");
    }
    BIO_free(bio);
    return result;
}


sql_string Credentials::getFQANsOneString()
{
    static const char* const funcName = "file_request::getFQANsOneString()";
    string returnString;
    int i;

    for (i=0; i<_fqans_vector.size(); i++) {
        returnString += _fqans_vector[i];
        returnString += "#";
    }

    // remove the last '#'
    if (returnString.length() > 0) {
        returnString.erase(returnString.length() - 1);
    }

    return sql_string(returnString);
};


//string Credentials::getFQANsOneString()
//{
//    static const char* const funcName = "Credentials::getFQANsOneString()";
//    string returnString;
//    int i;
//
//    for (i=0; i<_fqans_vector.size(); i++) {
//        returnString += _fqans_vector[i];
//        returnString += "#";
//    }
//
//    // remove the last '#'
//    if (returnString.length() > 0) {
//        returnString.erase(returnString.length() - 1);
//    }
//
//    return returnString;
//}

/* uncomment to have a method that checks if a proxy can be dumped
bool Credentials::canBeSaved()
{
	if (has_delegated_credentials(_soap)) {
		return true;

	} else {
		return false;
	}

}
*/

/**
 * Saves the proxy to the default directory only if the user has delegated
 * credentials. Returns "true" if the proxy is successfully saved or "false"
 * otherwise.
 **/
bool Credentials::saveProxy(std::string requestToken)
{
    static const char* const funcName = "Credentials::saveProxy()";
    bool result = false;

#if defined(GSI_PLUGINS)

    FrontendConfiguration* configuration = FrontendConfiguration::getInstance();

    /* Check for delegated credentials */
    if (has_delegated_credentials(_soap)) {
        /* Export client credential to proxy_filename */
        string proxy_filename = configuration->getProxyDir();

        proxy_filename += "/" + requestToken;
        char *tmp = strdup(proxy_filename.c_str()); // why export_delegated_cretentials() needs char* instead of const char *????
        int stat = export_delegated_credentials(_soap, tmp);
        if (getuid() != proxy_uid || getgid() != proxy_gid) {
            chown(proxy_filename.c_str(), proxy_uid, proxy_gid);
        }

        free(tmp);
        /* check export status */
        if (stat == 0) {
            srmlogit(STORM_LOG_DEBUG, funcName, "Proxy successfully written: %s\n", _clientDN.c_str());
            result = true;
        } else {
            srmlogit(STORM_LOG_ERROR, funcName, "Unable to write the proxy.\n");
        }

    } else { /* No delegation. */
        srmlogit(STORM_LOG_DEBUG, funcName, "%s: has NOT delegated credentials to server\n", _clientDN.c_str());
    }
#endif

    return result;
}

/*
char* Credentials::getProxy()
{

	char *token;
    size_t token_length;
    if (has_delegated_credentials(_soap))
    {
		if (get_delegated_credentials(_soap, (void **)&token, &token_length) < 0)
		{
			//error
		}
	}
	return token;
}
*/

//void getProxy() {
//
//
//#if defined(GSI_PLUGINS)
//    /* Check for delegated credentials */
//    if(has_delegated_credentials(soap)) {
//
//        /* Export to the DB instead of file */
//
//        /*
//         * The proxy will be writed to file, then it will be parsed by  the openssl functionalities.
//         * Once parsed into a X509 structure, if it is a voms proxy, the creation date will be retrived by the voms api,
//         * otherwise in case of simple proxy the expiration time will be retrived by openssl, and used tas discriminant into the DB.
//         */
//        int invalidCert = 0;
//        int *error;
//        int isVomsProxy = 1;
//
//        /* TODO: export to the DB instead of file */
//        /* Export client credential to proxy_filename */
//        // DO NOT USE SRMV2_PROXY_DIR... use FrontendConfiguration::getInstance()->getProxyDir();
//        std::string proxy_filename(SRMV2_PROXY_DIR);
//        srmlogit(STORM_LOG_INFO, func, "%s: %s\n", request.r_token().c_str(), "Client has delegated credentials to server");
//
//        proxy_filename += "/"+request.r_token();
//        char *tmp = strdup(proxy_filename.c_str()); // why export_delegated_cretentials() needs char* instead of const char *????
//
//        //Writing credential to file
//        int stat = export_delegated_credentials(soap, tmp);
//        if (getuid() != proxy_uid || getgid() != proxy_gid)
//            chown(proxy_filename.c_str(), proxy_uid, proxy_gid);
//
//        free(tmp);
//
//        /* check export status */
//        if (stat == 0)
//            srmlogit(STORM_LOG_INFO, func, "%s: Credential file written in %s\n", request.r_token().c_str(), proxy_filename.c_str());
//        else
//            srmlogit(STORM_LOG_ERROR, func, "%s: Unable to write credential file `%s'\n", request.r_token().c_str(),proxy_filename.c_str());
//
//        /* Read proxy certificate */
//        X509* x509p = NULL;
//        int errorVoms = 1;
//        int errorValue = 0;
//        int unexpectedError = 0;
//        char *date;
//
//        FILE * fd = fopen(proxy_filename.c_str(), "r");
//        if(!fd) {
//            srmlogit(STORM_LOG_INFO, func, "File not loaded \n");
//            invalidCert=1;
//        }
//
//        srmlogit(STORM_LOG_INFO, func, "Proxy readed start %s\n",proxy_filename.c_str());
//
//        //Get X509 structure from file
//        x509p  = PEM_read_X509(fd, NULL, 0,NULL);
//
//        if(!x509p) {
//            srmlogit(STORM_LOG_ERROR, func, "Fail to read the proxy certificate from file! \n");
//            invalidCert = 1;
//        } else
//            srmlogit(STORM_LOG_INFO, func, "Proxy certificate successfully readed from file. Principal name: %s \n",x509p->name);
//
//
//        /**
//         *  Check if the proxy is a simple X509 or a voms proxy with AC.
//         */
//
//        //Initialize voms data
//        struct vomsdata *vomsdatap = VOMS_Init(NULL, NULL);
//        if(vomsdatap==NULL) {
//            srmlogit(STORM_LOG_INFO, func, "Invalid voms initialization!!! \n");
//
//        }
//
//        STACK_OF(X 509) *chain = NULL;
//        //chain = load_chain(proxy_filename.c_str());
//
//        /*
//         * TODO this method should me integrate in a common file, as storm_utils.h
//         */
//
//        //Initialize the certificate Chain
//        STACK_OF(X509_INFO) *sk=NULL;
//
//        STACK_OF(X509) *stack=NULL, *ret=NULL;
//        BIO *in=NULL;
//        X509_INFO *xi;
//        int first = 1;
//
//        if(!(stack = sk_X509_new_null())) {
//            printf("memory allocation failure\n");
//            goto end;
//        }
//
//        if(!(in=BIO_new_file(proxy_filename.c_str(), "r"))) {
//            printf("error opening the file, %s\n",proxy_filename.c_str());
//            goto end;
//        }
//
//        /* This loads from a file, a stack of x509/crl/pkey sets */
//        if(!(sk=PEM_X509_INFO_read_bio(in,NULL,NULL,NULL))) {
//            printf("error reading the file, %s\n",proxy_filename.c_str());
//            goto end;
//        }
//
//        /* scan over it and pull out the certs */
//        while (sk_X509_INFO_num(sk)) {
//
//            /* skip first cert */
//            if (first) {
//                first = 0;
//                continue;
//            }
//            xi=sk_X509_INFO_shift(sk);
//            if (xi->x509 != NULL) {
//                sk_X509_push(stack,xi->x509);
//                xi->x509=NULL;
//            }
//            X509_INFO_free(xi);
//        }
//        if(!sk_X509_num(stack)) {
//            printf("no certificates in file, %s\n",proxy_filename.c_str());
//            sk_X509_free(stack);
//            goto end;
//        }
//        ret=stack;
//end:
//        BIO_free(in);
//        sk_X509_INFO_free(sk);
//        chain = ret;
//
//        srmlogit(STORM_LOG_INFO, func, "Certificate chain loaded  \n");
//
//        errorVoms = VOMS_Retrieve(x509p, chain,RECURSE_CHAIN, vomsdatap, &errorValue);
//        //errorVoms = VOMS_RetrieveFromProxy(RECURSE_CHAIN, vomsdatap, &errorValue);
//
//        if(errorVoms==0) {
//            //srmlogit(STORM_LOG_INFO, func, "Invalid voms return status Code %i \n", &errorValue);
//            char* errorMessage = VOMS_ErrorMessage(vomsdatap, errorValue, NULL, 0 );
//            srmlogit(STORM_LOG_INFO, func, "Error using VOMS API: %s\n", errorMessage);
//            srmlogit(STORM_LOG_INFO, func, "The proxy readed is NOT a VOMS proxy.\n");
//            isVomsProxy = 0;
//        } else {
//            srmlogit(STORM_LOG_INFO, func, "The proxy readed is a VOMS proxy. \n");
//            isVomsProxy = 1;
//        }
//
//        //VOMS certificate
//        if(isVomsProxy) {
//            //Getting voms information
//            voms* vomsd = VOMS_DefaultData(vomsdatap,error);
//            if(!vomsd) {
//                srmlogit(STORM_LOG_INFO, func, "NULL VOMS \n");
//                unexpectedError = 1;
//            } else {
//                srmlogit(STORM_LOG_INFO, func, "VOMS proxy creation time %s: \n", (vomsd->date1));
//                //Setting up date as the creation time for the voms proxy certificate.
//                date = strdup(proxy_filename.c_str());
//
//            }
//        }
//
//        //Non VOMS certificate
//        if ((!isVomsProxy)||(unexpectedError)) {
//            srmlogit(STORM_LOG_INFO, func,"NON VOMS proxy expiration time %s\n", (x509p->cert_info->validity->notAfter->data) );
//            //Setting up date as the exporation time for the voms proxy certificate.
//            date = strdup((const char*)x509p->cert_info->validity->notAfter->data);
//
//        }
//
//        //Removing proxy file
//        fclose(fd);
//        unlink(proxy_filename.c_str());
//
//        srmlogit(STORM_LOG_INFO, func, "Setting up date %s and proxy into the request structure \n", date);
//
//        //Check is something goes wrong in managing user credential.
//        if(invalidCert) {
//            srmlogit(STORM_LOG_ERROR, func,"Unrecoverable error managing user credential!\n");
//            return SOAP_OK;
//        }
//
//        //Setting up date and proxy into the request structure
//
//        //Get Proxy and Creation time into the request structure.
//
//        //Get Proxy:
//
//        //Get Creation time:
//        //Use voms api RetrieveByCred(...)
//
//        //Inser Proxy and Creation Time Array? into the request structure.
//
//        //request.time()
//        //request.proxy()
//    } else /* No delegation. */
//        srmlogit(STORM_LOG_INFO, func, "%s: Client has NOT delegated credentials to server\n", request.r_token().c_str());
//#endif
//
//
//}



// getVoms

//getVomsStart

//getVomsEnd


/* uncomment this to try using this cpp code from c code

extern "C" int call_create_credentials(struct soap *soap,  Credentials* cred) {
	char* func = "call_create_credentials";
	Credentials tempCred(soap);
	cred = (Credentials*)&tempCred;
	return 0;

}
*/
