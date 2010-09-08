/*
 * storm_utils.hpp
 *
 *  Created on: Oct 19, 2009
 *      Author: alb
 */

#ifndef STORM_UTILS_HPP_
#define STORM_UTILS_HPP_

#include <sstream>

template<typename type_t>
bool string2num(type_t& x, const std::string& s) {
    std::istringstream ss(s);
    return (ss >> x).fail();
}

template<class T>
inline std::string to_string(const T& t) {
    std::stringstream ss;
    ss << t;
    return ss.str();
}

#endif /* STORM_UTILS_HPP_ */
