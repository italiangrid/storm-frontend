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

#ifndef __PTG_SURL_HPP
#define __PTG_SURL_HPP

#include <string>

#include "srmv2H.h"
#include "sql_string.hpp"
#include "Surl.hpp"

namespace storm {
class PtgSurl : public Surl  {
public:
	PtgSurl(std::string surl, ns1__TDirOption *dirOption) : Surl(surl){
		this->init(dirOption);
	};

	PtgSurl(std::string surl) : Surl(surl){
		this->init(NULL);
	};

	~PtgSurl(){};

	bool hasDirOption()
	{
		return this->dirOption;
	}

	bool isDirectory()
	{
		return this->directory;
	}

	bool isAllLevelRecursive()
	{
		return this->allLevelRecursive;
	}

	bool hasNumLevels()
	{
		return this->numLevels > 0;
	}

	int getNumLevels()
	{
		return this->numLevels;
	}
private:
//	sql_string sourceSURL;
	bool dirOption;
	bool directory;
	bool allLevelRecursive;
	//int allLevelRecursive; // -1 means not supplied
	int numLevels; // -1 means not supplied
//	ns1__TStatusCode status;
//	std::string explanation;

	void init(ns1__TDirOption *dirOption)
	{
		 if (dirOption == NULL) {
			 this->dirOption = false;
		} else {
			this->dirOption = true;
			directory = dirOption->isSourceADirectory;
			allLevelRecursive = (dirOption->allLevelRecursive != NULL ? *(dirOption->allLevelRecursive) : false);
			/*
			if (dirOption->allLevelRecursive == NULL) {
				allLevelRecursive = -1;
			} else {
				if (*(dirOption->allLevelRecursive)) {
					allLevelRecursive = 1;
				} else {
					allLevelRecursive = 0;
				}
			}
			*/
			if (dirOption->numOfLevels == NULL) {
				numLevels = -1;
			} else {
				numLevels = *(dirOption->numOfLevels);
			}
		}
	}

};
}
#endif // __PTG_SURL_HPP
