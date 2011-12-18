#ifndef AUTHORIZATION_EXCEPTION_HPP_
#define AUTHORIZATION_EXCEPTION_HPP_

#include <exception>
#include <string>

namespace storm {

class AuthorizationException: public std::exception
{
public:
	AuthorizationException(std::string text) {
		message = text;
	};

	~AuthorizationException() throw () {};

	const char* what() const throw () {
		return message.c_str();
	}
private:
	std::string message;
};

}
#endif /*AUTHORIZATION_EXCEPTION_HPP_*/
