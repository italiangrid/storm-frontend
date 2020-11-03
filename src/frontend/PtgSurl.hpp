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

#ifndef PTG_SURL_HPP
#define PTG_SURL_HPP

#include <string>
#include <stdexcept>
#include "srmv2H.h"
#include "sql_string.hpp"
#include "Surl.hpp"

namespace storm {
class PtgSurl : public Surl  {
public:
	PtgSurl(std::string surl, ns1__TDirOption* dirOption): Surl(surl){
		this->init(dirOption);
	};

	PtgSurl(std::string& surl) : Surl(surl){
		this->init(NULL);
	};

	~PtgSurl(){};

	bool hasDirOption()
	{
		return m_dirOption;
	}

	bool isDirectory() throw (std::logic_error)
	{
		if(!this->hasDirOption())
		{
			throw std::logic_error("Cannot get Directory flag when Directory Option is not specified");
		}
		return m_directory;
	}

	bool isAllLevelRecursive() throw (std::logic_error)
	{
		if(!this->hasDirOption())
		{
			throw std::logic_error("Cannot get All level recursive flag when Directory Option is not specified");
		}
		return m_allLevelRecursive;
	}

	bool hasNumLevels()
	{
		return m_dirOption && m_numLevels > 0;
	}

	int getNumLevels() throw (std::logic_error)
	{
		if(!this->hasDirOption())
		{
			throw std::logic_error("Cannot get Number of levels when Directory Option is not specified");
		}
		return m_numLevels;
	}

private:
	bool m_dirOption;
	bool m_directory;
	bool m_allLevelRecursive;
	int m_numLevels; // -1 means not supplied

	void init(ns1__TDirOption *dirOption)
	{
		 if (dirOption == NULL) {
			 m_dirOption = false;
		} else {
			m_dirOption = true;
			m_directory = dirOption->isSourceADirectory;
			m_allLevelRecursive = (dirOption->allLevelRecursive != NULL ? (*(dirOption->allLevelRecursive) == true_) : false);
			if (dirOption->numOfLevels == NULL) {
				m_numLevels = -1;
			} else {
				m_numLevels = *(dirOption->numOfLevels);
			}
		}
	}
};
}
#endif // PTG_SURL_HPP
