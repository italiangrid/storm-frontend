/*
 * base_request.hpp
 *
 *  Created on: Nov 7, 2013
 *      Author: andrea
 */

#ifndef BASE_REQUEST_HPP_
#define BASE_REQUEST_HPP_

#include "storm_exception.hpp"
#include "soap_util.hpp"
#include "srmlogit.h"
#include "token_validator.hpp"
#include "MonitoringHelper.hpp"

namespace storm{

namespace request{

template<typename request_t>
void register_request_error(const char* func_name, ns1__TStatusCode status,
        boost::posix_time::ptime start_time, std::string const& msg) {
    srmlogit(STORM_LOG_ERROR, func_name, msg.c_str());
    srmLogResponse(request_t::NAME.c_str(), status);
    storm::MonitoringHelper::registerOperationError(start_time,
            request_t::MONITOR_NAME.c_str());
}

inline void validate_token_description(const std::string& token_description) {
    if (!storm::token::description_valid(token_description))
        throw storm::invalid_request(
                "invalid token description: " + token_description);
}

inline void validate_token(const std::string& token) {
    if (!storm::token::valid(token))
        throw storm::invalid_request(
                "invalid token: " + token);
}
}
}

#endif /* BASE_REQUEST_HPP_ */
