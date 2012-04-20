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

#ifndef FILE_REQUEST_HPP
#define FILE_REQUEST_HPP

extern "C" {
#include <openssl/x509.h>
}

#include <mysql/mysqld_error.h>
#include <mysql/mysql.h>

// STL includes
#include <vector>
#include <string>
#include <exception>
#include <stdexcept>

#include <boost/smart_ptr/shared_ptr.hpp>

#include "srmv2H.h"
#include "storm_util.h"
#include "srm_server.h"

#include "soap_util.hpp"
#include "Credentials.hpp"
#include "sql_string.hpp"
#include "Surl.hpp"

/**
 * Abstract class representing a generic file request.
 */
namespace storm {
class invalid_request: public std::exception {
public:
    invalid_request() {
    }
    ;
    invalid_request(std::string reason) {
        errmsg = reason.c_str();
    }
    const char* what() const throw () {
        return errmsg;
    }
private:
    const char* errmsg;
};

class not_supported: public std::exception {
public:
    not_supported() {
    }
    ;
    not_supported(std::string reason) {
        errmsg = reason.c_str();
    }
    const char* what() const throw () {
        return errmsg;
    }
private:
    const char* errmsg;
};

class InvalidResponse: public std::exception {
public:
	InvalidResponse() {
    }
    ;
	InvalidResponse(std::string reason) {
        errmsg = reason.c_str();
    }
    const char* what() const throw () {
        return errmsg;
    }
private:
    const char* errmsg;
};

template<typename soap_in_t, typename soap_out_t>
class file_request {
public:
    file_request(struct soap* soapRequest) :
    	m_credentials(soapRequest), m_soapRequest(soapRequest),
    	m_status(SRM_USCOREREQUEST_USCOREQUEUED), m_builtResponse(NULL),
    	m_fileStorageType(DB_FILE_TYPE_UNKNOWN), m_desiredTotalRequestTime(-1) {
    }

    virtual ~file_request() {
    }

    Credentials getCredentials()
    {
    	return m_credentials;
    }

    std::string getClientDN() {
        return m_credentials.getDN();
    }

    std::vector<sql_string> getFQANsVector() {
		return m_credentials.getFQANsVector();
	}

    bool saveProxy() {
        return m_credentials.saveProxy(m_requestToken);
    }

    /**
     * Get the current status code of the file request
     */
    ns1__TStatusCode getStatus() {
        return m_status;
    }

    std::string getSurlsList()
    {
    	std::string builtList;
    	std::vector<SurlPtr>::const_iterator const vectorEnd = m_surls.end();
    	bool first = true;
    	for (std::vector<SurlPtr>::const_iterator i = m_surls.begin(); i != vectorEnd; ++i) {
    		if(first)
    		{
    			first = false;
    		}
    		else
    		{
    			builtList += ' ';
    		}
    		builtList += (*i)->getSurl();

    	}
    	return builtList;
    }

    int getSurlsNumber()
    {
    	return m_surls.size();
    }

    /**
     * Get the current explanation string
     */
    std::string getExplanation() {
        return m_explanation;
    }

    /**
     * get the current request token
     */
    std::string getRequestToken() {
        return m_requestToken;
    }

    void setRequestToken(std::string token)
    {
    	m_requestToken = sql_string(token);
    }

    void invalidateRequestToken()
    {
    	m_requestToken = sql_string("");
    }

    /**
     * get the current user token
     */
    std::string getUserToken() {
        return m_userToken;
    }

    /**
     * get the current space token
     */
    std::string getSpaceToken() {
        return m_spaceToken;
    }

    virtual void load(soap_in_t* req) throw (invalid_request) = 0;

    virtual bool supportsProtocolSpecification() = 0;

    void setProtocolVector(std::vector<sql_string>* protocolVector)
    {
    	m_protocols = *protocolVector;
    }

    std::vector<sql_string>* getRequestedProtocols() throw (std::logic_error)
	{
    	if(!this->supportsProtocolSpecification())
    	{
    		throw std::logic_error("Cannot get Requested Protocols when protocol specification is not supported");
    	}
    	return &(m_protocols);
	}

    void failRequest(std::string explaination) {
    	m_status = SRM_USCOREFAILURE;
    	m_explanation = explaination;
    	this->setGenericFailureSurls();
    }

    /*
     * Set the status code at SURL level to SRM_FAILURE to all requested SURLs
     */
    void setGenericFailureSurls()
    {
    	std::vector<SurlPtr>::const_iterator const vectorEnd = m_surls.end();
		for (std::vector<SurlPtr>::const_iterator i = m_surls.begin(); i != vectorEnd; ++i) {
			(*i)->setStatus(SRM_USCOREFAILURE);
		}
    }

    /*
    * Set the status code at SURL level to SRM_AUTHORIZATION_FAILURE to all requested SURLs
    */
    void setAuthorizationFailureSurls()
    {
    	std::vector<SurlPtr>::const_iterator const vectorEnd = m_surls.end();
    	for (std::vector<SurlPtr>::const_iterator i = m_surls.begin(); i != vectorEnd; ++i) {
    		(*i)->setStatus(SRM_USCOREAUTHORIZATION_USCOREFAILURE);
    	}
    }

    virtual void insertIntoDB(struct srm_dbfd* dbfd) throw (std::logic_error , storm_db::mysql_exception) = 0;

    soap_out_t* buildSpecificResponse(ns1__TStatusCode status, const char* const explanation) throw (InvalidResponse) {
		m_status = status;
		m_explanation = explanation;
		return this->buildResponse();
	}

    virtual soap_out_t* buildResponse() throw (std::logic_error , InvalidResponse) = 0;

    std::string sqlFormat(char c) {
        std::string formattedString("\'");
        formattedString.append(1, c);
        formattedString += '\'';
        return formattedString;
    }

    std::string sqlFormat(const std::string& s) {
        std::string formatted_s("\'");
        formatted_s += s;
        formatted_s += '\'';
        return formatted_s;
    }

    std::string sqlFormat(bool value) {
        return value ? "1" : "0";
    }

protected:
    soap* m_soapRequest;
    // -------------------------------
    // --- incoming request parameters
    typedef boost::shared_ptr<Surl> SurlPtr;
    std::vector<SurlPtr> m_surls;
    sql_string m_userToken; // User Token
    sql_string m_spaceToken; // Space Token
    char m_fileStorageType; // from --> ns1__TFileStorageType *desiredFileStorageType
    std::vector<sql_string> m_protocols; // --> contained in ns1__TTransferParameters* transferParameters;
    Credentials m_credentials; // --> if available built on char* authorizationID
    //NUOVI MIEI
	std::string m_userRequestDescription;
	storm_time_t m_desiredTotalRequestTime;
	ns1__ArrayOfTExtraInfo* m_storageSystemInfo; //ignored
	ns1__TRetentionPolicyInfo* m_targetFileRetentionPolicyInfo; // ignored
	//
    // --- parameters end
    // -------------------------------

    soap_out_t* m_builtResponse;
    //--------------------------------
    // --- response build elements
    ns1__TStatusCode m_status;
    std::string m_explanation;
    sql_string m_requestToken;
	// --- response elements end
    //--------------------------------

    //--------------------------------
    // --- internal use fields
    std::string m_requestType;
    // --- internal use fields end
    //--------------------------------

    void setFileStorageType(ns1__TFileStorageType type) throw (std::domain_error)
    {
    	 switch (type) {
			case VOLATILE:
				m_fileStorageType = DB_FILE_TYPE_VOLATILE;
				break;
			case DURABLE:
				m_fileStorageType = DB_FILE_TYPE_DURABLE;
				break;
			case PERMANENT:
				m_fileStorageType = DB_FILE_TYPE_PERMANENT;
				break;
			default:
				throw std::domain_error("Invalid desiredFileStorageType");
		}
    }
};
}

#endif // FILE_REQUEST_HPP
