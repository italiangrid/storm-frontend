#ifndef MONITOR_NOT_ENABLED_EXCEPTION_HPP_
#define MONITOR_NOT_ENABLED_EXCEPTION_HPP_

#include <stdexcept>

namespace storm {

struct MonitorNotEnabledException: std::runtime_error
{
	MonitorNotEnabledException(std::string const& text) : std::runtime_error(text)
	{
	}
};

}
#endif /*MONITOR_NOT_ENABLED_EXCEPTION_HPP_*/
