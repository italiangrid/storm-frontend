/*
 * storm_exception.hpp
 *
 *  Created on: Oct 18, 2013
 *      Author: andrea
 */

#ifndef STORM_EXCEPTION_HPP_
#define STORM_EXCEPTION_HPP_

#include <stdexcept>
namespace storm {

struct storm_error: public std::runtime_error {
	storm_error(std::string const& m) :
			runtime_error(m) {
	}
	virtual const char* what() throw () {
		return std::runtime_error::what();
	}
};

struct invalid_request: public storm_error {
	invalid_request(std::string const& m) :
			storm_error(m) {
	}
	virtual const char* what() throw () {
		return std::runtime_error::what();
	}
};

struct not_supported: public storm_error {
	not_supported(std::string const& m) :
			storm_error(m) {
	}
	virtual const char* what() throw () {
		return std::runtime_error::what();
	}
};

struct invalid_response: public storm_error {
	invalid_response(std::string const& m) :
			storm_error(m) {
	}
	virtual const char* what() throw () {
		return std::runtime_error::what();
	}
};

struct authorization_error: public storm_error {
	authorization_error(std::string const& m):
		storm_error(m) {}
	virtual const char* what() throw () {
		return std::runtime_error::what();
	}
};

struct argus_error: public storm_error {
	argus_error(std::string const& m):
		storm_error(m) {}
	virtual const char* what() throw () {
		return std::runtime_error::what();
	}
};


struct token_not_found: public invalid_request {
	token_not_found(std::string const& m) :
			invalid_request(m) {
	}
	virtual const char* what() throw () {
		return std::runtime_error::what();
	}
};

struct invalid_surl : public invalid_request {
	invalid_surl(std::string const& m) :
		invalid_request(m){}
	virtual const char* what() throw() {
		return std::runtime_error::what();
	}
};

struct invalid_turl : public invalid_request {
	invalid_turl(std::string const& m) :
		invalid_request(m){}
	virtual const char* what() throw() {
		return std::runtime_error::what();
	}
};
}
#endif /* STORM_EXCEPTION_HPP_ */
