#ifndef MONITOR_NOT_ENABLED_EXCEPTION_HPP_
#define MONITOR_NOT_ENABLED_EXCEPTION_HPP_

#include <exception>
#include <string>

namespace storm {

class MonitorNotEnabledException: public std::exception
{
public:
	MonitorNotEnabledException(std::string text) {
		message = text;
	};

	~MonitorNotEnabledException() throw () {};

	const char* what() const throw () {
		return message.c_str();
	}
private:
	std::string message;
};

}
#endif /*MONITOR_NOT_ENABLED_EXCEPTION_HPP_*/
