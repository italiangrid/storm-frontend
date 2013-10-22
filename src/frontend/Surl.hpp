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

#ifndef SURL_HPP
#define SURL_HPP

#include <string>

#include "srmv2H.h"
#include "sql_string.hpp"

namespace storm {

std::string normalize_surl(std::string surl);

class InvalidSurl: public std::exception {
public:
	InvalidSurl() { errmsg = 0;}

	InvalidSurl(std::string reason) {
        errmsg = reason.c_str();
    }
    const char *what() const throw () {
        return errmsg;
    }
private:
    const char *errmsg;
};

class Surl  {
public:
	Surl(std::string const& surl) throw (InvalidSurl) :
		m_surlString(normalize_surl(surl)),
		m_status(SRM_USCOREREQUEST_USCOREQUEUED){

		if(surl.empty())
		{
			throw InvalidSurl("Unable to create an empty SURL");
		}
	};

	virtual ~Surl(){};

	sql_string getSurl()
	{
		return m_surlString;
	}

	ns1__TStatusCode getStatus()
	{
		return m_status;
	}

	std::string  getExplanation()
	{
		return m_explanation;
	}

	void setStatus(ns1__TStatusCode newStatus)
	{
		m_status = newStatus;
	}

	void setExplanation(std::string const& explanation)
	{
		m_explanation = explanation;
	}

	bool operator< (Surl other) const {
		return m_surlString.size() < other.getSurl().size();
	}

private:
	sql_string m_surlString;
	ns1__TStatusCode m_status;
	std::string m_explanation;

};

}
#endif // SURL_HPP
