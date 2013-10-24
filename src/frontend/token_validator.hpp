/*
 * token_validator.hpp
 *
 *  Created on: Oct 17, 2013
 *      Author: andrea
 */

#ifndef TOKEN_VALIDATOR_HPP_
#define TOKEN_VALIDATOR_HPP_

#ifdef __cplusplus
#include <string>

namespace storm{
	namespace token{
		bool valid(const std::string& token);
		int valid (const char* c_string);

		bool description_valid(const std::string& token_descs);
	}
}
#endif

#ifdef __cplusplus
extern "C"{
#endif

int storm_token_valid(const char* token);


#ifdef __cplusplus
}
#endif
#endif /* TOKEN_VALIDATOR_HPP_ */
