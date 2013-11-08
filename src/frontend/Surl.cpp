/*
 * Surl.cpp
 *
 *  Created on: Oct 15, 2013
 *      Author: andrea
 */
#include "Surl.hpp"
#include "boost/regex.hpp"

static const std::string SRM_URL_REGEXP = "^srm://[A-Za-z0-9\\.\\-]+(:\\d{1,4})?/(srm/managerv2\\?SFN=)?[A-Za-z0-9\\._\\-/]*$";

bool srm_url_valid(std::string const& srm_url){
	static boost::regex const e(SRM_URL_REGEXP);
	return boost::regex_match(srm_url,e);
}

std::string storm::normalize_surl(std::string surl){

	if (!srm_url_valid(surl)){
		throw invalid_surl("invalid surl: "+surl);
	}

	static std::string const QUERY_PATTERN="/srm/managerv2?SFN=";
	static size_t const QUERY_PATTERN_SIZE = QUERY_PATTERN.size();

	if (surl.empty())
		return surl;

	size_t const pos = surl.find(QUERY_PATTERN);

	if (pos == std::string::npos)
		return surl;

	return surl.erase(pos, QUERY_PATTERN_SIZE);
}
