/*
 * ProtocolChecker.cpp
 *
 *  Created on: 18/lug/2011
 *      Author: Michele Dibenedetto
 */

#include <string.h>
#include <algorithm>
#include <sstream>
#include <stdlib.h>

#include "srmlogit.h"
#include "ProtocolChecker.hpp"

ProtocolChecker* ProtocolChecker::instance = NULL;

ProtocolChecker* ProtocolChecker::getInstance() {
	const char* func = "ProtocolChecker::getInstance<>";

	if (instance == NULL) {
		srmlogit(STORM_LOG_DEBUG, func, "Building ProtocolChecker Singleton instance\n");
		instance = new ProtocolChecker();
	}
	return instance;
}


void ProtocolChecker::init(char*** protocolList, int protocolListSize) {

	const char* func = "ProtocolChecker::init<>";
	srmlogit(STORM_LOG_DEBUG, func, "Initializing ProtocolChecker\n");

	for (int i = 0; i < protocolListSize; i++) {
		srmlogit(STORM_LOG_DEBUG, func, "Adding protocol %s to supported protocols list\n" , (*protocolList)[i]);
		std::string current((*protocolList)[i]);
		this->protocolVector.push_back(current);
	}
	srmlogit(STORM_LOG_DEBUG, func, "ProtocolChecker initialization completed\n");
}

int ProtocolChecker::checkProtocols(std::vector<sql_string> const & protocolVector)
{
	const char* func = "ProtocolChecker::checkProtocols<>";
	srmlogit(STORM_LOG_DEBUG, func, "Checking the provided protocols against supported ones\n");

	for (std::vector<sql_string>::const_iterator it = protocolVector.begin(); it
				!= protocolVector.end(); ++it) {

		std::ostringstream oss;
		oss << *it;
		std::string protocolString = oss.str();
		srmlogit(STORM_LOG_DEBUG, func, "Inspecting protocol - %s -\n" , protocolString.c_str());
		//srmlogit(STORM_LOG_DEBUG, func, "Inspecting protocol - %s -\n" , it->c_str());
		if (std::find(this->protocolVector.begin(), this->protocolVector.end(), protocolString) == this->protocolVector.end())
		{
			//some requested protocols are not in the local list
			srmlogit(STORM_LOG_DEBUG, func, "Protocol check failure, protocol - %s - not supported\n" , protocolString.c_str());
			return 1;
		}
	}
	srmlogit(STORM_LOG_DEBUG, func, "Protocol check success\n");
	return 0;
}

std::vector<sql_string> ProtocolChecker::removeUnsupportedProtocols(std::vector<sql_string> const & protocolVector)
{
	const char* func = "ProtocolChecker::removeUnsupportedProtocols<>";
	srmlogit(STORM_LOG_DEBUG, func, "Removing unsupported protocols from the provided list\n");
	std::vector<sql_string> filteredProtocolVector;
	for (std::vector<sql_string>::const_iterator it = protocolVector.begin(); it
				!= protocolVector.end(); ++it)
	{
		std::ostringstream oss;
		oss << *it;
		std::string protocolString = oss.str();
		if (std::find(this->protocolVector.begin(), this->protocolVector.end(),
				protocolString) != this->protocolVector.end())
		{
			srmlogit(STORM_LOG_DEBUG, func, "Protocol - %s - is supported\n" , protocolString.c_str());
			filteredProtocolVector.push_back(*it);
		}
		else
		{
			srmlogit(STORM_LOG_DEBUG, func, "Removing unsupported protocol - %s -\n" , protocolString.c_str());
		}
	}
	srmlogit(STORM_LOG_DEBUG, func, "Filtered protocol list ready\n");
	return filteredProtocolVector;
}


void ProtocolChecker::printProtocols()
{
	const char* func = "ProtocolChecker::printProtocols<>";
	srmlogit(STORM_LOG_DEBUG, func, "Listying suported protocols\n");
	if(!(this->protocolVector.empty()))
	{
		bool read = false;
		for (std::vector<std::string>::iterator it = this->protocolVector.begin(); it
					!= this->protocolVector.end(); ++it)
		{
			read = true;
			std::ostringstream oss;
			oss << *it;
			std::string protocolString = oss.str();
			srmlogit(STORM_LOG_DEBUG, func, "Listying protocols: %s\n" , protocolString.c_str());
		}
		if(!read)
		{
			srmlogit(STORM_LOG_DEBUG, func, "Supported protocols vector not empty but no protocols read\n");
		}
	}
	else
	{
		srmlogit(STORM_LOG_DEBUG, func, "No supported protocols available\n");
	}
}
