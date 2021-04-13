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

#ifndef PTP_SURL_HPP
#define PTP_SURL_HPP

#include <stdexcept>

#include "srmv2H.h"
#include "Surl.hpp"
#include "storm_util.hpp"

namespace storm {
class PtpSurl : public Surl  {
public:
	PtpSurl(std::string surl, storm_size_t size) : Surl(surl) {
		this->init(size);
	};

	PtpSurl(std::string surl) : Surl(surl) {
		this->init(-1);
	};

	bool hasExpected_size()
	{
		return m_expected_size > 0;
	}

	storm_size_t getExpected_size()
	{
		if(!this->hasExpected_size())
		{
			throw std::logic_error("Cannot get Expected Size value when has not been specified");
		}
		return m_expected_size;
	}

private:
    storm_size_t m_expected_size;

    void init(storm_size_t size)
    {
    	m_expected_size = size;
    }
};
}
#endif // PTP_SURL_HPP
