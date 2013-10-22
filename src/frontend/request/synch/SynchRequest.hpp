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

#ifndef SYNCH_REQUEST_HPP
#define SYNCH_REQUEST_HPP

// STL includes
#include <string>
#include <exception>
#include <stdexcept>
#include <set>

#include "storm_util.h"
#include "soap_util.hpp"
#include "synch.hpp"
#include "Credentials.hpp"
#include "srmlogit.h"
#include "token_validator.hpp"
#include "storm_exception.hpp"

/**
 * Abstract class representing a generic file request.
 */
namespace storm {

template<typename soap_in_t, typename soap_out_t, typename soap_out_root_t>
class SynchRequest {
public:
	SynchRequest (struct soap* soapRequest, soap_in_t* request, std::string name, std::string monitorName) :
		m_soapRequest(soapRequest),
		m_credentials(soapRequest),
		m_request(request),
		m_builtResponse(NULL),
		m_name(name),
		m_monitorName(monitorName),
    	m_status(SRM_USCOREREQUEST_USCOREQUEUED){
		this->loadCommonFields(request);
    }

    virtual ~SynchRequest() {
    }

    soap* getSoapRequest()
    {
    	return m_soapRequest;
    }

    soap_in_t* getRequest()
    {
    	return m_request;
    }

    std::string getName()
    {
    	return m_name;
    }

    std::string getMonitorName()
    {
    	return m_monitorName;
    }

    Credentials getCredentials()
    {
    	return m_credentials;
    }

    std::string getClientDN() {
        return m_credentials.getDN();
    }

    std::vector<std::string> getFQANsVector() {
		return m_credentials.getFQANsVector();
	}

    ns1__TStatusCode getStatus() {
        return m_status;
    }

    void setStatus(ns1__TStatusCode status) {
    	m_status = status;
    }

    void setExplanation(char* explanation) {
    	m_explanation = std::string(explanation);
    }

    std::string getExplanation() {
    	return m_explanation;
    }

    std::string getSurlsList()
    {
    	std::string builtList;
    	std::set<std::string>::const_iterator const vectorEnd = m_surls.end();
    	bool first = true;
    	for (std::set<std::string>::const_iterator i = m_surls.begin(); i != vectorEnd; ++i) {
    		if(first)
    		{
    			first = false;
    		}
    		else
    		{
    			builtList += ' ';
    		}
    		builtList += *i;

    	}
    	return builtList;
    }

    int getSurlsNumber()
    {
    	return m_surls.size();
    }

    bool hasSurls()
	{
		return !m_surls.empty();
	}

    void failRequest(std::string explaination) {
    	m_status = SRM_USCOREFAILURE;
    	m_explanation = explaination;
    }

    void setResponse(soap_out_t* response)
    {
    	m_builtResponse = response;
    }

    soap_out_t* getResponse()
    {
    	if(m_builtResponse == NULL)
    	{
    		buildResponse();
    	}
    	return m_builtResponse;
    }

    int buildSpecificResponse(ns1__TStatusCode status, const char* const explanation){
		m_status = status;
		m_explanation = explanation;
		return this->buildResponse();
	}

    virtual void load(soap_in_t* req) = 0;

    virtual int buildResponse() = 0;

    virtual int performXmlRpcCall(soap_out_root_t* response) = 0;

protected:
    soap* m_soapRequest;
    Credentials m_credentials;

    soap_in_t* m_request;
    soap_out_t* m_builtResponse;

    std::set<std::string> m_surls;

    std::string m_authorizationID;
    std::set<std::pair<char*,char*> > m_extraInfo;

    std::string m_name;
    std::string m_monitorName;

    ns1__TStatusCode m_status;

    std::string m_explanation;



    bool convertBoolean(xsd__boolean boolean) {
    	switch (boolean) {
    	case true_:
    		return true;
    	case false_:
    		return false;
    	default:
    		srmlogit(STORM_LOG_ERROR, "storm::SynchRequest::convertBoolean()", "xsd__boolean has an unknown value %u\n", boolean);
    		break;
    	}
    	return false;
    }

    void validate_token(const std::string& token){
    	if (! storm::token::valid(token)){
    		throw storm::invalid_request("invalid token: "+token);
    	}
    }

private:
    void loadCommonFields(soap_in_t* request) throw (invalid_request)
    {
    	if(request == NULL)
		{
			throw invalid_request("Received NULL request parameter");
		}

		if(request->authorizationID != NULL)
		{
			m_authorizationID = request->authorizationID;
		}
    }
};
}

#endif // SYNCH_REQUEST_HPP
