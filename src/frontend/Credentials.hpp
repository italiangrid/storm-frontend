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
// storm_db include
#include "mysql_query.hpp"

#include "sql_string.hpp"

namespace storm {

class Credentials {
public:
	Credentials(struct soap *soap) ;

	~Credentials(){}

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

	bool saveProxy(std::string requestToken);

private:
	struct soap *_soap;
	std::string _clientDN;
	std::vector<sql_string> _fqans_vector;
	};

}

#endif /*CREDENTIALS_HPP_*/
