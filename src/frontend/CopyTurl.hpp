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

#ifndef COPY_TURL_HPP
#define COPY_TURL_HPP


#include "Turl.hpp"
#include "storm_util.h"

namespace storm {

class CopyTurl : public Turl  {
public:
	CopyTurl(std::string turl, Surl sourceSurl, std::string destinationSurl, storm_size_t size):
			Turl(turl, sourceSurl), m_destinationSURL(destinationSurl), m_fileSize(size), m_estimatedWaitTime(-1), m_remainingFileLifetime(-1) {
		if(destinationSurl.empty())
		{
			throw invalid_surl("Unable to create a TURL with an empty destination SURL");
		}
	}

	CopyTurl(std::string turl, Surl sourceSurl, std::string destinationSurl):
				Turl(turl, sourceSurl), m_destinationSURL(destinationSurl), m_fileSize(-1), m_estimatedWaitTime(-1), m_remainingFileLifetime(-1) {
		if(destinationSurl.empty())
		{
			throw invalid_surl("Unable to create a TURL with an empty destination SURL");
		}
	}

	CopyTurl(Surl sourceSurl, std::string destinationSurl, storm_size_t size):
			Turl(sourceSurl), m_destinationSURL(destinationSurl), m_fileSize(size), m_estimatedWaitTime(-1), m_remainingFileLifetime(-1) {
		if(destinationSurl.empty())
		{
			throw invalid_surl("Unable to create a TURL with an empty destination SURL");
		}
	}

	CopyTurl(Surl sourceSurl, std::string destinationSurl):
				Turl(sourceSurl), m_destinationSURL(destinationSurl), m_fileSize(-1), m_estimatedWaitTime(-1), m_remainingFileLifetime(-1) {
		if(destinationSurl.empty())
		{
			throw invalid_surl("Unable to create a TURL with an empty destination SURL");
		}
	}

	~CopyTurl(){}

	Surl getSourceSurl()
	{
		return getSurl();
	}

	sql_string getDestinationSurl()
	{
		return m_destinationSURL;
	}

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

	storm_time_t getEstimatedWaitTime() throw (std::logic_error)
	{
		if(!this->hasEstimatedWaitTime())
		{
			throw std::logic_error("Cannot get Estimated Wait Time, field not initialized");
		}
		return m_estimatedWaitTime;
	}

	void setRemainingFileLifetime(storm_time_t remainingFileLifetime)
	{
		m_remainingFileLifetime = remainingFileLifetime;
	}

	bool hasRemainingFileLifetime()
	{
		return m_remainingFileLifetime != -1;
	}

	storm_time_t getRemainingFileLifetime() throw (std::logic_error)
	{
		if(!this->hasRemainingFileLifetime())
		{
			throw std::logic_error("Cannot get Remaining Pin Lifetime, field not initialized");
		}
		return m_remainingFileLifetime;
	}

private:
	sql_string m_destinationSURL;
    storm_size_t m_fileSize;
    storm_time_t m_estimatedWaitTime; // -1 means unknown
    storm_time_t m_remainingFileLifetime; // -1 means unknown
};
}
#endif // COPY_TURL_HPP
