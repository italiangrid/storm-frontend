/*
 * Surl.cpp
 *
 *  Created on: Oct 15, 2013
 *      Author: andrea
 */
#include "Surl.hpp"

std::string storm::normalize_surl(std::string surl){

	static std::string const QUERY_PATTERN="/srm/managerv2?SFN=";
	static size_t const QUERY_PATTERN_SIZE = QUERY_PATTERN.size();

	if (surl.empty())
		return surl;

	size_t const pos = surl.find(QUERY_PATTERN);

	if (pos == std::string::npos)
		return surl;

	return surl.erase(pos, QUERY_PATTERN_SIZE);
}
