/*
 * token_validator.cpp
 *
 *  Created on: Oct 17, 2013
 *      Author: andrea
 */

#include "token_validator.hpp"
#include "boost/regex.hpp"

namespace storm {

namespace token {

bool valid(const std::string& token) {
	static const boost::regex e(
			"^[a-f0-9]{8}-[a-f0-9]{4}-[a-f0-9]{4}-[a-f0-9]{4}-[a-f0-9]{12}$",
			 boost::regex::perl|boost::regex::icase);
	return boost::regex_match(token, e);
}

int valid(const char* c_string){
	return valid(std::string(c_string));
}

}
}

int storm_token_valid(const char* token){
	return storm::token::valid(token);
}
