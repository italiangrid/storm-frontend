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

#ifndef TURL_HPP
#define TURL_HPP

#include <string>

#include "srmv2H.h"
#include "sql_string.hpp"
#include "storm_exception.hpp"

namespace storm {

class Turl {
public:
	Turl(std::string const& turl, Surl& surl): m_turlString(turl) , m_surl(surl), m_empty(false) {
		if(turl.empty())
		{
			throw invalid_turl("TURL is empty");
		}
	}

	Turl(Surl& surl):m_surl(surl) , m_empty(true){

	}

	virtual ~Turl(){};

	sql_string getTurl()
	{
		if(isEmpty())
		{
			throw std::logic_error("Cannot get the SURL string from an empty TURL object");
		}
		return m_turlString;
	}

	Surl getSurl()
	{
		return m_surl;
	}

	void setStatus(ns1__TStatusCode status)
	{
		m_surl.setStatus(status);
	}

	ns1__TStatusCode getStatus()
	{
		return m_surl.getStatus();
	}

	void setExplanation(std::string explanation)
	{
		m_surl.setExplanation(explanation);
	}

	std::string getExplanation()
	{
		return m_surl.getExplanation();
	}

	bool isEmpty()
	{
		return m_empty;
	}

private:
	sql_string m_turlString;
	Surl m_surl;
	bool m_empty; //true if the turl string is not available
};
}
#endif // TURL_HPP
