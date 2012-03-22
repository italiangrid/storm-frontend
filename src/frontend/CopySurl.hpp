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

#ifndef __COPY_SURL_HPP
#define __COPY_SURL_HPP

#include <string>

#include "srmv2H.h"
#include "sql_string.hpp"
#include "PtgSurl.hpp"

namespace storm {
class CopySurl : public PtgSurl  {
public:
	CopySurl(std::string sourceSurl, std::string destinationSurl, ns1__TDirOption* dirOption)
	throw (InvalidSurl) : PtgSurl(sourceSurl, dirOption) {
		this->init(destinationSurl);
	};

	CopySurl(std::string& sourceSurl, std::string& destinationSurl)  throw (InvalidSurl) :
			PtgSurl(sourceSurl, NULL) {
		this->init(destinationSurl);
	};

	~CopySurl(){};

	sql_string getSourceSurl()
	{
		return this->getSurl();
	}

	sql_string getDestinationSurl()
	{
		return m_destinationSURL;
	}
private:
	sql_string m_destinationSURL;

	void init(std::string& destinationSurl) throw (InvalidSurl)
	{
		if(destinationSurl.empty())
		{
			throw InvalidSurl("Unable to create an empty destination SURL");
		}
		m_destinationSURL = sql_string(destinationSurl);
	}
};
}
#endif // __COPY_SURL_HPP
