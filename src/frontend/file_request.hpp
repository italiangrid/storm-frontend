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
#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/algorithm/string/replace.hpp>

#include "storm_exception.hpp"
#include "srmv2H.h"
#include "storm_util.h"
#include "srm_server.h"

#include "soap_util.hpp"
#include "Credentials.hpp"
#include "sql_string.hpp"
#include "Surl.hpp"
#include "token_validator.hpp"
#include "Monitor.hpp"

namespace storm {

typedef boost::shared_ptr<Surl> SurlPtr;

inline void validate_request_description(const std::string& request_description){
	if (! storm::token::description_valid(request_description))
		throw storm::invalid_request("invalid request description: "+request_description);
}

template<typename soap_in_t, typename soap_out_t>
class file_request {
public:
    file_request(struct soap* soapRequest) :
    	m_soapRequest(soapRequest),
    	m_credentials(soapRequest),
    	m_status(SRM_USCOREREQUEST_USCOREQUEUED),
    	m_builtResponse(NULL),
    	m_fileStorageType(DB_FILE_TYPE_UNKNOWN),
    	m_desiredTotalRequestTime(-1),
    	m_targetFileRetentionPolicyInfo(0),
    	m_storageSystemInfo(0){
    }

    virtual ~file_request() {
    }

    Credentials getCredentials(){ return m_credentials; }

    std::string getClientDN() { return m_credentials.getDN(); }

    std::vector<sql_string> getFQANsVector() { return m_credentials.getFQANsVector(); }

    bool saveProxy(){ return m_credentials.saveProxy(m_requestToken);}

    ns1__TStatusCode getStatus(){ return m_status; }

	std::string getSurlsList() {
		std::string result;
		std::vector<SurlPtr>::const_iterator const vectorEnd = m_surls.end();
		bool first = true;
		for (std::vector<SurlPtr>::const_iterator i = m_surls.begin();
				i != vectorEnd; ++i) {
			if (first) {
				first = false;
			} else {
				result += ' ';
			}
			result += (*i)->getSurl();

		}
		return result;
	}

    int getSurlsNumber(){ return m_surls.size();}

    std::string getExplanation(){ return m_explanation; }

    std::string getRequestToken(){ return m_requestToken; }

    void setRequestToken(std::string token){ m_requestToken = sql_string(token);}

    void invalidateRequestToken() { m_requestToken = sql_string("");}

    std::string getUserToken() { return m_userToken; }

    std::string getSpaceToken() { return m_spaceToken; }

    std::vector<sql_string> getRequestedProtocols(){
    	if (! supportsProtocolSpecification()) throw storm_error("Protocol specification not supported.");
    	return m_protocols;
    }

    void setProtocolVector(std::vector<sql_string> const& protocols){
    	m_protocols = protocols;
    }

    void failRequest(std::string explanation){
    	m_status = SRM_USCOREFAILURE;
    	m_explanation = explanation;
    	setGenericFailureSurls();
    }

    void setGenericFailureSurls(){
    	std::vector<SurlPtr>::const_iterator const vectorEnd = m_surls.end();
    	for (std::vector<SurlPtr>::const_iterator i = m_surls.begin(); i != vectorEnd; ++i) {
    		(*i)->setStatus(SRM_USCOREFAILURE);
    	}
    }

    void setAuthorizationFailureSurls(){
    	std::vector<SurlPtr>::const_iterator const vectorEnd = m_surls.end();
    	for (std::vector<SurlPtr>::const_iterator i = m_surls.begin(); i != vectorEnd; ++i) {
    			(*i)->setStatus(SRM_USCOREAUTHORIZATION_USCOREFAILURE);
    	}
    }

    soap_out_t* buildSpecificResponse(ns1__TStatusCode status, const char* const explanation) {
		m_status = status;
		m_explanation = explanation;
		return buildResponse();
	}

    std::string sqlFormat(char c){
    	std::string formattedString("\'");
    	formattedString.append(1, c);
    	formattedString += '\'';
    	return formattedString;
    }

    std::string sqlFormat(bool b){
    	return b ? "1" : "0";
    }

    std::string sqlFormat(std::string const& s){
    	std::string formatted_s = boost::replace_all_copy(s, "'", "''");
    	return '\'' + formatted_s + '\'';
    }

	void setFileStorageType(ns1__TFileStorageType type) {
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
			throw storm::storm_error("Invalid desiredFileStorageType");
		}
	}
    virtual void load(soap_in_t* req) = 0;

    virtual bool supportsProtocolSpecification() = 0;

    virtual void insertIntoDB(struct srm_dbfd* dbfd) = 0;

    virtual soap_out_t* buildResponse() = 0;

protected:
    soap* m_soapRequest;

    std::vector<SurlPtr> m_surls;
    sql_string m_userToken; // User Token
    sql_string m_spaceToken; // Space Token
    char m_fileStorageType; // from --> ns1__TFileStorageType *desiredFileStorageType
    std::vector<sql_string> m_protocols; // --> contained in ns1__TTransferParameters* transferParameters;


    Credentials m_credentials; // --> if available built on char* authorizationID

	std::string m_userRequestDescription;
	storm_time_t m_desiredTotalRequestTime;

	ns1__ArrayOfTExtraInfo* m_storageSystemInfo; //ignored
	ns1__TRetentionPolicyInfo* m_targetFileRetentionPolicyInfo; // ignored

    soap_out_t* m_builtResponse;

    ns1__TStatusCode m_status;

    std::string m_explanation;
    sql_string m_requestToken;
    std::string m_requestType;

};
}

#endif // FILE_REQUEST_HPP
