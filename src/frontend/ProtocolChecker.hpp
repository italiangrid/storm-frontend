/*
 * ProtocolChecker.h
 *
 *  Created on: 18/lug/2011
 *      Author: Michele Dibenedetto
 */

#ifndef PROTOCOLCHECKER_H_
#define PROTOCOLCHECKER_H_

#include <vector>
#include "sql_string.hpp"

class ProtocolChecker {
public:

	/*
	 * Singleto instance
	 * 
	 * */
	static ProtocolChecker* getInstance();

	/*
	 * Initialize the object with the list (and its size) of the supported protocols
	 * 
	 * */
	void init(std::vector<std::string> const& protocolList);
	
	/*
	 * 
	 * Checks if the provided protocol vectopr contains unsupported protocols
	 * Return 0 if all protocols are valid
	 * Return 1 if at least 1 protocol is not valid
	 */
	int checkProtocols(std::vector<sql_string> const & protocolVector) const;
	/*
	 *
	 * Returns a new array containing the protocols from the provided array that are supported
	 * The returned array respects the order of the provided one
	 */
	std::vector<sql_string> removeUnsupportedProtocols(std::vector<sql_string> const& protocolVector) const;
	
	/*
	 * Prints the supported protocols
	 * */
	void printProtocols() const;

private:
	ProtocolChecker()	{}

	/*
	 * An array of supported protocols
	 * */
	std::vector<std::string> protocolVector;
	
	/*
	 * Singleton instance
	 * */
	static ProtocolChecker* instance;
};

#endif /* PROTOCOLCHECKER_H_ */
