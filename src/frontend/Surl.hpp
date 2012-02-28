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

#ifndef __SURL_HPP
#define __SURL_HPP

#include <string>

#include "srmv2H.h"
#include "sql_string.hpp"

namespace storm {
class Surl  {
public:
	Surl(std::string surl):surlString(surl), status(SRM_USCOREREQUEST_USCOREQUEUED){};

	~Surl(){};

	sql_string getSurl()
	{
		return this->surlString;
	}

	ns1__TStatusCode getStatus()
	{
		return this->status;
	}

	std::string  getExplanation()
	{
		return this->explanation;
	}

	void setStatus(ns1__TStatusCode newStatus)
	{
		this->status = newStatus;
	}
private:
	sql_string surlString;
	ns1__TStatusCode status;
	std::string explanation;
};
}
#endif // __SURL_HPP
