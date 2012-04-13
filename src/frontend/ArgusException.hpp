#ifndef ARGUS_EXCEPTION_HPP_
#define ARGUS_EXCEPTION_HPP_

#include <exception>
#include <string>

namespace storm {

class ArgusException: public std::exception
{
public:
	ArgusException(std::string* text) {
		message = text;
	};

	ArgusException(std::string text) {
		message = &text;
	};

	~ArgusException() throw () {
		delete message;
	};

	const char* what() const throw () {
		return (*message).c_str();
	}
private:
	std::string* message;
};

}
#endif /*ARGUS_EXCEPTION_HPP_*/
