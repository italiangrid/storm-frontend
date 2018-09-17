/*
 * token_validator.cpp
 *
 *  Created on: Oct 17, 2013
 *      Author: andrea
 */

#include "token_validator.hpp"
#include <boost/regex.hpp>

namespace storm {

namespace token {

static const std::string UUID_REGEXP =
		"^[a-f0-9]{8}-[a-f0-9]{4}-[a-f0-9]{4}-[a-f0-9]{4}-[a-f0-9]{12}$";

static const std::string DESCRIPTION_REGEXP =
		"^\\w(\\w|[-_\\.])+$";

bool valid(const std::string& token) {
	static const boost::regex e(UUID_REGEXP,
			 boost::regex::perl|boost::regex::icase);
	return boost::regex_match(token, e);
}


bool description_valid(const std::string& token_description){
	static const  boost::regex e(DESCRIPTION_REGEXP);
	return boost::regex_match(token_description,e);
}

int valid(const char* c_string){
	return valid(std::string(c_string));
}

}
}

int storm_token_valid(const char* token){
	return storm::token::valid(token);
}
