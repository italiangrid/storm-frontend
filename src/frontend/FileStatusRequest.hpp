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

#ifndef FILE_STATUS_REQUEST_HPP
#define FILE_STATUS_REQUEST_HPP

#include "srmv2H.h"
#include "storm_functions.h"
#include "storm_util.h"
#include "srm_server.h"
#include "srmlogit.h"
#include "HttpPostClient.h"

#include <string>
#include <vector>
#include <set>
#include <map>

#include <exception>
#include <stdexcept>

#include "mysql_query.hpp"
#include "sql_string.hpp"

#include "soap_util.hpp"
#include "Credentials.hpp"
#include "FrontendConfiguration.hpp"
#include "Surl.hpp"
#include "Turl.hpp"

namespace storm {

typedef std::map<std::string, std::string> file_status_result_t;
typedef std::vector<file_status_result_t> file_status_results_t;

class TokenNotFound: public std::exception {
public:
	TokenNotFound() {
    }

	TokenNotFound(std::string reason) {
        errmsg = reason.c_str();
    }
    const char* what() const throw () {
        return errmsg;
    }

private:
    const char* errmsg;
};

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

template<typename soap_in_t, typename soap_out_t>
class FileStatusRequest {
public:

	FileStatusRequest(struct soap* soap, soap_in_t* req) throw (invalid_request) :
    	m_soapRequest(soap), m_credentials(soap), m_builtResponse(NULL) {
    	m_be_rest_port = (long) FrontendConfiguration::getInstance()->getRecalltablePort();
    	m_be_hostname = FrontendConfiguration::getInstance()->getXmlRpcHost();
    	m_remainingTotalRequestTime = -1;
    	this->loadCommonFields(req);
    }

    virtual ~FileStatusRequest() {
	}

    void loadCommonFields(soap_in_t* req) throw (invalid_request)
	{
    	if(req == NULL)
    	{
    		throw invalid_request("Received NULL request parameter");
    	}
    	if(req->requestToken == NULL)
    	{
    		throw invalid_request("ns1__srmStatusOfPutRequestRequest has NULLrequestToken");
    	}
    	m_requestToken = sql_string(req->requestToken);
    	if(req->authorizationID != NULL)
    	{
    		m_authorizationID = sql_string(req->authorizationID);
    	}
	}

    virtual soap_out_t* buildSpecificResponse(const ns1__TStatusCode srmStatusCode,
            const char* const srmStatusExplanation) throw (std::logic_error) {
        m_status = srmStatusCode;
        m_explanation = srmStatusExplanation;
        return buildResponse();
    }

    virtual soap_out_t* buildResponse() throw (std::logic_error) = 0;

    virtual void load(soap_in_t* req) = 0;

    virtual void loadFromDB(struct srm_dbfd *db) throw (TokenNotFound) = 0;

    virtual void fillCommonFields(file_status_results_t& statusResult) throw (std::logic_error)
	{
    	if (statusResult.size() == 0)
		{
    		srmlogit(STORM_LOG_ERROR, "storm::file_status::fillCommonFields()",
					"Received an empty query result\n");
			throw std::logic_error("Received an empty query result");
		}
		file_status_result_t row0(statusResult.at(0));

		if (row0["client_dn"].empty())
		{
			srmlogit(STORM_LOG_ERROR, "storm::file_status::fillCommonFields()",
					"No client_dn available in the query result\n");
			throw std::logic_error("No client_dn available in the query result");
		}

		m_storedClientDN = row0["client_dn"];

		if(row0["status"].empty())
		{
			srmlogit(STORM_LOG_ERROR, "storm::file_status::fillCommonFields()",
					"No request status available in the query result\n");
			throw std::logic_error("No request status available in the query result");
		}
		m_status = static_cast<ns1__TStatusCode> (atoi(row0["status"].c_str()));
		m_explanation = row0["errstring"];
		if(!row0["remainingTotalTime"].empty())
		{
			m_remainingTotalRequestTime = atoi(row0["remainingTotalTime"].c_str());
		}
	}

    sql_string getRequestToken()
    {
    	return m_requestToken;
    }

    bool hasSurls()
    {
    	return !m_surls.empty();
    }

    bool hasRemainingTotalRequestTime()
    {
    	return m_remainingTotalRequestTime != -1;
    }

    Credentials getCredentials()
    {
    	return m_credentials;
    }

    bool isAuthorized()
    {
    	if(m_storedClientDN != m_credentials.getDN())
		{
			srmlogit(STORM_LOG_INFO, "storm::file_status::fillCommonFields()",
					"The provided request token does not belong to the requesting user %s but to %s\n",
					m_credentials.getDN().c_str(), m_storedClientDN);
			return false;
		}
    	else
		{
    		return true;
		}
    }

    std::string getSurlsList()
    {
    	std::string builtList;
    	std::set<Surl>::const_iterator const vectorEnd = m_surls.end();
    	bool first = true;
    	for (std::set<Surl>::const_iterator i = m_surls.begin(); i != vectorEnd; ++i) {
    		Surl current = *i;
    		if(first)
    		{
    			first = false;
    		}
    		else
    		{
    			builtList += ' ';
    		}
    		builtList += current.getSurl();

    	}
    	return builtList;
    }

    int getSurlsNumber()
    {
    	return m_surls.size();
    }

protected:

    soap * m_soapRequest;
    // -------------------------------
    // --- incoming request parameters
    //
    //std::vector<Surl> m_surls;
    std::set<Surl> m_surls;
    sql_string m_requestToken;
    storm::Credentials m_credentials;
    sql_string m_authorizationID; //ignored
    //
	// --- parameters end
	// -------------------------------

    soap_out_t* m_builtResponse;
    typedef boost::shared_ptr<Turl> TurlPtr;
    //--------------------------------
    // --- response build elements
    ns1__TStatusCode m_status;
    std::string m_explanation;
    int m_remainingTotalRequestTime;
    std::set<TurlPtr> m_turls;
	// --- response elements end
    //--------------------------------

    //--------------------------------
    // --- internal use fields
    long m_be_rest_port;
	std::string m_be_hostname;
	std::string m_storedClientDN;
    // --- internal use fields end
    //--------------------------------

	bool hasMissingSurls()
	{
		return m_turls.size() < m_surls.size();
	}

	bool checkSurl(std::string surl) throw (std::logic_error) {
		std::set<TurlPtr>::const_iterator const vectorEnd = m_turls.end();
		for (std::set<TurlPtr>::const_iterator i = m_turls.begin(); i != vectorEnd; ++i) {
			storm::Turl* turl = dynamic_cast<storm::Turl*> (i->get());
			if(!turl)
			{
				throw std::logic_error("Unable to cast TurlPtr to PtpTurl, cast failure");
			}
			if(turl->getSurl().getSurl() == surl)
			{
				return true;
			}
		}
		return false;
	}

	virtual void addMissingSurls() throw (std::logic_error) = 0;

	 std::string sqlFormat(char c) {
		std::string formattedString("\'");
		formattedString.append(1, c);
		formattedString += '\'';
		return formattedString;
	}

	std::string sqlFormat(const std::string s) {
		std::string formatted_s("\'");
		formatted_s += s;
		formatted_s += '\'';
		return formatted_s;
	}

	std::string sqlFormat(bool value) {
		return value ? "1" : "0";
	}

	bool isSurlOnDisk(std::string surl) {

		bool result = false;
		try {

			HttpPostClient client;
			client.setHostname(m_be_hostname);
			client.setPort(m_be_rest_port);
			std::string data = "requestToken=" + m_requestToken + "\nsurl=" + surl;
			client.callService(data);

			long responseCode = client.getHttpResponseCode();
			srmlogit(STORM_LOG_DEBUG, "FileStatusRequest::isSurlOnDisk()", "Response code: %d\n", responseCode);

			if (responseCode == 200) {
				std::string response = client.getResponse();
				if (response.compare("true") == 0) {

					result = true;
					srmlogit(STORM_LOG_DEBUG2, "FileStatusRequest::isSurlOnDisk()", "Response=true for surl=%s\n",
							surl.c_str());
				} else {

					result = false;
					srmlogit(STORM_LOG_DEBUG2, "FileStatusRequest::isSurlOnDisk()", "Response=false for surl=%s\n",
							surl.c_str());
				}
			}
		} catch (std::exception& e) {
			srmlogit(STORM_LOG_ERROR, "FileStatusRequest::isSurlOnDisk()",
					"Curl: cannot create handle for HTTP client.\n");
			return false;
		}
		return result;
	}
};
}
#endif //FILE_STATUS_REQUEST_HPP
