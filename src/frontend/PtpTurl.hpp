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

#ifndef PTP_TURL_HPP
#define PTP_TURL_HPP


#include "PtgTurl.hpp"
#include "storm_util.h"

namespace storm {

class PtpTurl : public PtgTurl  {
public:
	PtpTurl(std::string turl, Surl surl, storm_size_t size, storm_size_t expectedFileSize):
				PtgTurl(turl, surl, size, expectedFileSize), m_remainingFileLifetime(-1) {
	}

	PtpTurl(std::string turl, Surl surl):
			PtgTurl(turl, surl), m_remainingFileLifetime(-1) {
	}


	PtpTurl(Surl surl, storm_size_t size, storm_size_t expectedFileSize):
				PtgTurl(surl, size, expectedFileSize), m_remainingFileLifetime(-1) {
	}

	PtpTurl(Surl surl):
			PtgTurl(surl), m_remainingFileLifetime(-1) {
	}

	~PtpTurl() {
	}


	void setRemainingFileLifetime(storm_time_t remainingFileLifetime)
	{
		m_remainingFileLifetime = remainingFileLifetime;
	}

	bool hasRemainingFileLifetime()
	{
		return m_remainingFileLifetime != -1;
	}

	storm_time_t getRemainingFileLifetime()
	{
		if(!this->hasRemainingFileLifetime())
		{
			throw std::logic_error("Cannot get Remaining File Lifetime, field not initialized");
		}
		return m_remainingFileLifetime;
	}

private:
    storm_time_t m_remainingFileLifetime; // -1 means unknown
};
}
#endif // PTP_TURL_HPP
