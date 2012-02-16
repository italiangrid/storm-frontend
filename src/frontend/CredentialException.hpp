#ifndef CREDENTIAL_EXCEPTION_HPP_
#define CREDENTIAL_EXCEPTION_HPP_

#include <exception>
#include <string>

namespace storm {

class CredentialException: public std::exception
{
public:
	CredentialException(std::string* text) {
		message = text;
	};

	CredentialException(std::string text) {
		message = &text;
	};

	~CredentialException() throw () {
		delete message;
	};

	const char* what() const throw () {
		return (*message).c_str();
	}
private:
	std::string* message;
};

}
#endif /*CREDENTIAL_EXCEPTION_HPP_*/
