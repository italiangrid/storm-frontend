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

#ifndef BOL_SURL_HPP
#define BOL_SURL_HPP

#include "PtgSurl.hpp"

namespace storm {
class BolSurl : public PtgSurl {
public:
	BolSurl(std::string surl, ns1__TDirOption* dirOption):
			PtgSurl(surl, dirOption), m_fileSize(-1), m_estimatedWaitTime(-1),
			m_remainingPinTime(-1) {
	}

	~BolSurl(){}

	unsigned long getFileSize()
	{
		return m_fileSize;
	}

	int getRemainingPinTime()
	{
		return m_remainingPinTime;
	}

	int getEstimatedWaitTime()
	{
		return m_estimatedWaitTime;
	}

	void setFileSize(unsigned long fileSize)
	{
		m_fileSize = fileSize;
	}

	void setRemainingPinTime(int remainingPinTime)
	{
		m_remainingPinTime = remainingPinTime;
	}

	void setEstimatedWaitTime(int estimatedWaitTime)
	{
		m_estimatedWaitTime = estimatedWaitTime;
	}

private:
	unsigned long m_fileSize;
	int m_remainingPinTime;
	int m_estimatedWaitTime;
};
}
#endif // BOL_SURL_HPP
