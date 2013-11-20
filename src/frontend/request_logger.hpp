/*
 * request_logger.hpp
 *
 *  Created on: Oct 30, 2013
 *      Author: andrea
 */

#ifndef REQUEST_LOGGER_HPP_
#define REQUEST_LOGGER_HPP_

#include "srmlogit.h"
#include "get_socket_info.hpp"
#include "sync_requests.hpp"

namespace storm {

template<typename T>
void simple_logger(const char* func, T & request) {

	std::string ip = get_ip(request.getSoapRequest()).c_str();

	srmlogit(
	STORM_LOG_INFO, func, "Request: %s. IP: %s. Client DN: %s\n",
			T::NAME.c_str(), ip.c_str(), request.getClientDN().c_str());
}

template<typename T>
void surl_logger(const char* func, T & request) {
	std::string ip = get_ip(request.getSoapRequest()).c_str();

	srmlogit(
	STORM_LOG_INFO, func, "Request: %s. IP: %s. Client DN: %s. surl(s): %s\n",
			T::NAME.c_str(), ip.c_str(), request.getClientDN().c_str(),
			request.getSurlsList().c_str());
}

template<typename T>
void space_token_logger(const char* func, T & request) {

	std::string ip = get_ip(request.getSoapRequest()).c_str();

	srmlogit(
	STORM_LOG_INFO, func,
			"Request: %s. IP: %s. Client DN: %s. space token: %s\n",
			T::NAME.c_str(), ip.c_str(), request.getClientDN().c_str(),
			request.getSpaceToken().c_str());
}

template<typename T>
void space_token_description_logger(const char* func, T & request) {
	std::string ip = get_ip(request.getSoapRequest()).c_str();

	srmlogit(
	STORM_LOG_INFO, func,
			"Request: %s. IP: %s. Client DN: %s. space token description: %s\n",
			T::NAME.c_str(), ip.c_str(), request.getClientDN().c_str(),
			request.getUserSpaceTokenDescription().c_str());
}

template<typename T>
void space_token_list_logger(const char* func, T & request) {
	std::string ip = get_ip(request.getSoapRequest()).c_str();

	srmlogit(
	STORM_LOG_INFO, func,
			"Request: %s. IP: %s. Client DN: %s. space tokens: %s\n",
			T::NAME.c_str(), ip.c_str(), request.getClientDN().c_str(),
			request.getSpaceTokensList().c_str());
}

template<typename T>
void surl_req_token_logger(const char* func, T & request) {
	std::string ip = get_ip(request.getSoapRequest()).c_str();

	srmlogit(
	STORM_LOG_INFO, func,
			"Request: %s. IP: %s. Client DN: %s. surl(s): %s. token: %s\n",
			T::NAME.c_str(), ip.c_str(), request.getClientDN().c_str(),
			request.getSurlsList().c_str(), request.getRequestToken().c_str());
}

template<typename T>
void surl_space_token_logger(const char* func, T & request) {
	std::string ip = get_ip(request.getSoapRequest()).c_str();

	srmlogit(
	STORM_LOG_INFO, func,
			"Request: %s. IP: %s. Client DN: %s. surl(s): %s. space token: %s\n",
			T::NAME.c_str(), ip.c_str(), request.getClientDN().c_str(),
			request.getSurlsList().c_str(), request.getSpaceToken().c_str());
}

template<typename T>
void surl_target_space_token_logger(const char* func, T & request) {
	std::string ip = get_ip(request.getSoapRequest()).c_str();

	srmlogit(
	STORM_LOG_INFO, func,
			"Request: %s. IP: %s. Client DN: %s. surl(s): %s. space token: %s\n",
			T::NAME.c_str(), ip.c_str(), request.getClientDN().c_str(),
			request.getSurlsList().c_str(),
			request.getTargetSpaceToken().c_str());
}

template<typename T>
void req_token_logger(const char* func, T & request) {
	std::string ip = get_ip(request.getSoapRequest()).c_str();

	srmlogit(
	STORM_LOG_INFO, func, "Request: %s. IP: %s. Client DN: %s. token: %s\n",
			T::NAME.c_str(), ip.c_str(), request.getClientDN().c_str(),
			request.getRequestToken().c_str());

}

template<typename T>
void log_request_outcome(const char* func, T & request) {
	srmLogResponse(T::NAME.c_str(),request.getStatus());
}

template<typename T>
struct logging_traits {
	void log_request(const char* func, T & request) {
		simple_logger(func, request);
	}
};

template<>
struct logging_traits<storm::AbortFilesRequest> {
	void log_request(const char* func, storm::AbortFilesRequest & request) {
		surl_req_token_logger(func, request);
	}
};

template<>
struct logging_traits<storm::AbortRequestRequest> {
	void log_request(const char* func, storm::AbortRequestRequest & request) {
		req_token_logger(func, request);
	}
};

template<>
struct logging_traits<storm::MkdirRequest> {
	void log_request(const char* func, storm::MkdirRequest & request) {
		surl_logger(func, request);
	}
};

template<>
struct logging_traits<storm::RmRequest> {
	void log_request(const char* func, storm::RmRequest & request) {
		surl_logger(func, request);
	}
};

template<>
struct logging_traits<storm::RmdirRequest> {
	void log_request(const char* func, storm::RmdirRequest & request) {
		surl_logger(func, request);
	}
};

template<>
struct logging_traits<storm::MvRequest> {
	void log_request(const char* func, storm::MvRequest & request) {
		std::string ip = get_ip(request.getSoapRequest()).c_str();

		srmlogit(STORM_LOG_INFO,
				func,
				"Request: %s. IP: %s. Client DN: %s. from_surl: %s to_surl: %s\n",
				storm::MvRequest::NAME.c_str(),
				ip.c_str(),
				request.getClientDN().c_str(),
				request.from_surl().c_str(),
				request.to_surl().c_str());
	}
};

template<>
struct logging_traits<storm::SetPermissionRequest> {
	void log_request(const char* func, storm::SetPermissionRequest & request) {
		surl_logger(func, request);
	}
};

template<>
struct logging_traits<storm::CheckPermissionRequest> {
	void log_request(const char* func,
			storm::CheckPermissionRequest & request) {
		surl_logger(func, request);
	}
};

template<>
struct logging_traits<storm::GetPermissionRequest> {
	void log_request(const char* func, storm::GetPermissionRequest & request) {
		surl_logger(func, request);
	}
};

template<>
struct logging_traits<storm::StatusLsRequest> {
	void log_request(const char* func, storm::StatusLsRequest & request) {
		surl_req_token_logger(func, request);
	}
};

template<>
struct logging_traits<storm::StatusReserveSpaceRequest> {
	void log_request(const char* func,
			storm::StatusReserveSpaceRequest & request) {
		surl_req_token_logger(func, request);
	}
};

template<>
struct logging_traits<storm::StatusUpdateSpaceRequest> {
	void log_request(const char* func,
			storm::StatusUpdateSpaceRequest& request) {
		surl_req_token_logger(func, request);
	}
};

template<>
struct logging_traits<storm::StatusChangeSpaceForFilesRequest> {
	void log_request(const char* func,
			storm::StatusChangeSpaceForFilesRequest& request) {
		surl_req_token_logger(func, request);
	}
};

template<>
struct logging_traits<storm::SuspendRequestRequest> {
	void log_request(const char* func, storm::SuspendRequestRequest& request) {
		req_token_logger(func, request);
	}
};

template<>
struct logging_traits<storm::UpdateSpaceRequest> {
	void log_request(const char* func, storm::UpdateSpaceRequest& request) {
		space_token_logger(func, request);
	}
};

template<>
struct logging_traits<storm::ReleaseSpaceRequest> {
	void log_request(const char* func, storm::ReleaseSpaceRequest& request) {
		space_token_logger(func, request);
	}
};

template<>
struct logging_traits<storm::ReleaseFilesRequest> {
	void log_request(const char* func, storm::ReleaseFilesRequest& request) {
		if (request.hasRequestToken() && request.hasSurls())
			surl_req_token_logger(func, request);
		else if (request.hasRequestToken())
			req_token_logger(func, request);
		else
			surl_logger(func, request);
	}
};

template<>
struct logging_traits<storm::ExtendFileLifeTimeRequest> {
	void log_request(const char* func,
			storm::ExtendFileLifeTimeRequest& request) {
		if (request.hasRequestToken())
			surl_req_token_logger(func, request);
		else
			surl_logger(func, request);
	}
};

template<>
struct logging_traits<storm::PutDoneRequest> {
	void log_request(const char* func, storm::PutDoneRequest& request) {
		surl_req_token_logger(func, request);
	}
};

template<>
struct logging_traits<storm::PurgeFromSpaceRequest> {
	void log_request(const char* func, storm::PurgeFromSpaceRequest& request) {
		surl_space_token_logger(func, request);
	}
};

template<>
struct logging_traits<storm::GetSpaceMetaDataRequest> {
	void log_request(const char* func,
			storm::GetSpaceMetaDataRequest& request) {
		space_token_list_logger(func, request);
	}
};

template<>
struct logging_traits<storm::ExtendFileLifeTimeInSpaceRequest> {
	void log_request(const char* func,
			storm::ExtendFileLifeTimeInSpaceRequest& request) {
		surl_space_token_logger(func, request);
	}
};

template<>
struct logging_traits<storm::ChangeSpaceForFilesRequest> {
	void log_request(const char* func,
			storm::ChangeSpaceForFilesRequest& request) {
		surl_target_space_token_logger(func, request);
	}
};

template<>
struct logging_traits<storm::GetSpaceTokensRequest> {
	void log_request(const char* func, storm::GetSpaceTokensRequest& request) {
		space_token_description_logger(func, request);
	}
};

}

#endif /* REQUEST_LOGGER_HPP_ */
