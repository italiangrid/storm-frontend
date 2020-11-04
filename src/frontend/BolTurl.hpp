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

#ifndef BOL_TURL_HPP
#define BOL_TURL_HPP


#include "Turl.hpp"
#include "storm_util.h"

namespace storm {

class BolTurl : public Turl  {
public:
	BolTurl(std::string turl, Surl surl, storm_size_t size):
			Turl(turl, surl), m_fileSize(size), m_estimatedWaitTime(-1), m_remainingPinLifetime(-1) {
	}

	BolTurl(std::string turl, Surl surl):
				Turl(turl, surl), m_fileSize(-1), m_estimatedWaitTime(-1), m_remainingPinLifetime(-1) {
	}

	BolTurl(Surl surl, storm_size_t size):
			Turl(surl), m_fileSize(size), m_estimatedWaitTime(-1), m_remainingPinLifetime(-1) {
	}

	BolTurl(Surl surl):
				Turl(surl), m_fileSize(-1), m_estimatedWaitTime(-1), m_remainingPinLifetime(-1) {
	}

	~BolTurl(){}

	storm_size_t getFileSize()
	{
		return m_fileSize;
	}

	bool hasFileSize()
	{
		return m_fileSize != -1;
	}

	void setEstimatedWaitTime(storm_time_t estimatedWaitTime)
	{
		m_estimatedWaitTime = estimatedWaitTime;
	}

	bool hasEstimatedWaitTime()
	{
		return m_estimatedWaitTime != -1;
	}

	storm_time_t getEstimatedWaitTime()
	{
		if(!this->hasEstimatedWaitTime())
		{
			throw std::logic_error("Cannot get Estimated Wait Time, field not initialized");
		}
		return m_estimatedWaitTime;
	}

	void setRemainingPinLifetime(storm_time_t remainingPinLifetime)
	{
		m_remainingPinLifetime = remainingPinLifetime;
	}

	bool hasRemainingPinLifetime()
	{
		return m_remainingPinLifetime != -1;
	}

	storm_time_t getRemainingPinLifetime()
	{
		if(!this->hasRemainingPinLifetime())
		{
			throw std::logic_error("Cannot get Remaining Pin Lifetime, field not initialized");
		}
		return m_remainingPinLifetime;
	}

private:
    storm_size_t m_fileSize;
    storm_time_t m_estimatedWaitTime; // -1 means unknown
    storm_time_t m_remainingPinLifetime; // -1 means unknown
};
}
#endif // PTG_TURL_HPP
