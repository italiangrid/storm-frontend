#include "ProtocolChecker.hpp"
#include "srmlogit.h"

#include <algorithm>

ProtocolChecker* ProtocolChecker::instance = NULL;

ProtocolChecker* ProtocolChecker::getInstance() {
	const char* func = "ProtocolChecker::getInstance";

	if (instance == NULL) {
		srmlogit(STORM_LOG_DEBUG, func, "Building ProtocolChecker Singleton instance\n");
		instance = new ProtocolChecker();
	}
	return instance;
}


void ProtocolChecker::init(std::vector<std::string> const& protocols) {

	const char* func = "ProtocolChecker::init<>";
	srmlogit(STORM_LOG_DEBUG, func, "Initializing ProtocolChecker\n");

	for (int i = 0, n = protocols.size(); i != n; ++i) {
		srmlogit(STORM_LOG_DEBUG, func, "Adding protocol %s to supported protocols list\n" , protocols[i].c_str());
		m_protocols.push_back(protocols[i]);
	}
	srmlogit(STORM_LOG_DEBUG, func, "ProtocolChecker initialization completed\n");
}

int ProtocolChecker::checkProtocols(std::vector<sql_string> const& protocols) const
{
	const char* func = "ProtocolChecker::checkProtocols<>";
	srmlogit(STORM_LOG_DEBUG, func, "Checking the provided protocols against supported ones\n");

	for (std::vector<sql_string>::const_iterator it = protocols.begin(), end = protocols.end(); it != end; ++it) {

		std::string const& protocol = *it;
		srmlogit(STORM_LOG_DEBUG, func, "Inspecting protocol - %s -\n" , protocol.c_str());
		//srmlogit(STORM_LOG_DEBUG, func, "Inspecting protocol - %s -\n" , it->c_str());
		if (std::find(m_protocols.begin(), m_protocols.end(), protocol) == m_protocols.end())
		{
			//some requested protocols are not in the local list
			srmlogit(STORM_LOG_DEBUG, func, "Protocol check failure, protocol - %s - not supported\n" , protocol.c_str());
			return 1;
		}
	}
	srmlogit(STORM_LOG_DEBUG, func, "Protocol check success\n");
	return 0;
}

std::vector<sql_string> ProtocolChecker::removeUnsupportedProtocols(std::vector<sql_string> const & protocolVector) const
{
	const char* func = "ProtocolChecker::removeUnsupportedProtocols<>";
	srmlogit(STORM_LOG_DEBUG, func, "Removing unsupported protocols from the provided list\n");
	std::vector<sql_string> filteredProtocolVector;
	for (std::vector<sql_string>::const_iterator it = protocolVector.begin(); it
				!= protocolVector.end(); ++it)
	{
		sql_string const& protocol = *it;
		if (std::find(m_protocols.begin(), m_protocols.end(), protocol) != m_protocols.end())
		{
			srmlogit(STORM_LOG_DEBUG, func, "Protocol - %s - is supported\n" , protocol.c_str());
			filteredProtocolVector.push_back(protocol);
		}
		else
		{
			srmlogit(STORM_LOG_DEBUG, func, "Removing unsupported protocol - %s -\n" , protocol.c_str());
		}
	}
	srmlogit(STORM_LOG_DEBUG, func, "Filtered protocol list ready\n");
	return filteredProtocolVector;
}

void ProtocolChecker::printProtocols() const
{
	const char* func = "ProtocolChecker::printProtocols<>";
	srmlogit(STORM_LOG_NONE, func, "Listing suported protocols\n");
	if(!(m_protocols.empty()))
	{
		for (std::vector<std::string>::const_iterator it = m_protocols.begin(), end = m_protocols.end(); it != end; ++it)
		{
			srmlogit(STORM_LOG_NONE, func, "Listing protocols: %s\n" , it->c_str());
		}
	}
	else
	{
		srmlogit(STORM_LOG_NONE, func, "No supported protocols available\n");
	}
}
