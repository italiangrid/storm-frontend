/* Copyright (c) Istituto Nazionale di Fisica Nucleare (INFN). 2006-2010.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/

#include "srmv2H.h"
#include "storm_util.hpp"
#include "srm_server.h"
#include <string.h>
#include <stdlib.h>
#include "srmlogit.h"
#include "storm_mysql.hpp"
#include <openssl/pem.h>

std::vector<std::string> get_supported_protocols(std::string const& server, std::string const& user, std::string const& pw)
{
    typedef std::vector<std::string> Protocols;
    Protocols result;

    srm_dbfd dbfd{};
    if (storm_opendb(server.c_str(), user.c_str(), pw.c_str(), &dbfd) < 0) {
        // log
        return result;
        // exception?
    }

    int const n_protocols = storm_list_protocol(&dbfd, NULL, 0, 0);

    if (n_protocols <= 0) {
        // log
        storm_closedb(&dbfd);
        return result;
        // exception?
    }

    char** protocols = new char*[n_protocols];
    int const protocol_len = 10;
    for (int i = 0; i != n_protocols; ++i) {
        protocols[i] = new char[protocol_len];
    }

    int const n_protocols2 = storm_list_protocol(&dbfd, protocols, n_protocols, protocol_len);
    // assert(n_protocols == n_protocols2);
    storm_closedb(&dbfd);

    if (n_protocols2 <= 0) {
        // log
        for (int i = 0; i != n_protocols; ++i) {
            delete [] protocols[i];
        }
        delete [] protocols;
        return result;
        // exception?
    }

    for (int i = 0; i != n_protocols; ++i) {
        result.push_back(std::string(protocols[i]));
        delete [] protocols[i];
    }
    delete [] protocols;

    return result;
}

/* Converts the status code from char* format into a SRM status code */
int convertStatusCode(char const* code) {
    if (strcmp("SRM_SUCCESS", code) == 0)
        return SRM_USCORESUCCESS; // 0
    else if (strcmp("SRM_FAILURE", code) == 0)
        return SRM_USCOREFAILURE; // 1
    else if (strcmp("SRM_AUTHENTICATION_FAILURE", code) == 0)
        return SRM_USCOREAUTHENTICATION_USCOREFAILURE; // 2
    else if (strcmp("SRM_AUTHORIZATION_FAILURE", code) == 0)
        return SRM_USCOREAUTHORIZATION_USCOREFAILURE; // 3
    else if (strcmp("SRM_INVALID_REQUEST", code) == 0)
        return SRM_USCOREINVALID_USCOREREQUEST; // 4
    else if (strcmp("SRM_INVALID_PATH", code) == 0)
        return SRM_USCOREINVALID_USCOREPATH; // 5
    else if (strcmp("SRM_FILE_LIFETIME_EXPIRED", code) == 0)
        return SRM_USCOREFILE_USCORELIFETIME_USCOREEXPIRED; // 6
    else if (strcmp("SRM_SPACE_LIFETIME_EXPIRED", code) == 0)
        return SRM_USCORESPACE_USCORELIFETIME_USCOREEXPIRED; // 7
    else if (strcmp("SRM_EXCEED_ALLOCATION", code) == 0)
        return SRM_USCOREEXCEED_USCOREALLOCATION; // 8
    else if (strcmp("SRM_NO_USER_SPACE", code) == 0)
        return SRM_USCORENO_USCOREUSER_USCORESPACE; // 9
    else if (strcmp("SRM_NO_FREE_SPACE", code) == 0)
        return SRM_USCORENO_USCOREFREE_USCORESPACE; // 10
    else if (strcmp("SRM_DUPLICATION_ERROR", code) == 0)
        return SRM_USCOREDUPLICATION_USCOREERROR; // 11
    else if (strcmp("SRM_NON_EMPTY_DIRECTORY", code) == 0)
        return SRM_USCORENON_USCOREEMPTY_USCOREDIRECTORY; // 12
    else if (strcmp("SRM_TOO_MANY_RESULTS", code) == 0)
        return SRM_USCORETOO_USCOREMANY_USCORERESULTS; // 13
    else if (strcmp("SRM_INTERNAL_ERROR", code) == 0)
        return SRM_USCOREINTERNAL_USCOREERROR; // 14
    else if (strcmp("SRM_FATAL_INTERNAL_ERROR", code) == 0)
        return SRM_USCOREFATAL_USCOREINTERNAL_USCOREERROR; // 15
    else if (strcmp("SRM_NOT_SUPPORTED", code) == 0)
        return SRM_USCORENOT_USCORESUPPORTED; // 16
    else if (strcmp("SRM_REQUEST_QUEUED", code) == 0)
        return SRM_USCOREREQUEST_USCOREQUEUED; // 17
    else if (strcmp("SRM_REQUEST_INPROGRESS", code) == 0)
        return SRM_USCOREREQUEST_USCOREINPROGRESS; // 18
    else if (strcmp("SRM_REQUEST_SUSPENDED", code) == 0)
        return SRM_USCOREREQUEST_USCORESUSPENDED; // 19
    else if (strcmp("SRM_ABORTED", code) == 0)
        return SRM_USCOREABORTED; // 20
    else if (strcmp("SRM_RELEASED", code) == 0)
        return SRM_USCORERELEASED; // 21
    else if (strcmp("SRM_FILE_PINNED", code) == 0)
        return SRM_USCOREFILE_USCOREPINNED; // 22
    else if (strcmp("SRM_FILE_IN_CACHE", code) == 0)
        return SRM_USCOREFILE_USCOREIN_USCORECACHE; // 23
    else if (strcmp("SRM_SPACE_AVAILABLE", code) == 0)
        return SRM_USCORESPACE_USCOREAVAILABLE; // 24
    else if (strcmp("SRM_LOWER_SPACE_GRANTED", code) == 0)
        return SRM_USCORELOWER_USCORESPACE_USCOREGRANTED; // 25
    else if (strcmp("SRM_DONE", code) == 0)
        return SRM_USCOREDONE; // 26
    else if (strcmp("SRM_PARTIAL_SUCCESS", code) == 0)
        return SRM_USCOREPARTIAL_USCORESUCCESS; // 27
    else if (strcmp("SRM_REQUEST_TIMED_OUT", code) == 0)
        return SRM_USCOREREQUEST_USCORETIMED_USCOREOUT; // 28
    else if (strcmp("SRM_LAST_COPY", code) == 0)
        return SRM_USCORELAST_USCORECOPY; // 29
    else if (strcmp("SRM_FILE_BUSY", code) == 0)
        return SRM_USCOREFILE_USCOREBUSY; // 30
    else if (strcmp("SRM_FILE_LOST", code) == 0)
        return SRM_USCOREFILE_USCORELOST; // 31
    else if (strcmp("SRM_FILE_UNAVAILABLE", code) == 0)
        return SRM_USCOREFILE_USCOREUNAVAILABLE; // 32
    else
        return SRM_USCORECUSTOM_USCORESTATUS; // 33
}

char const* reconvertStatusCode(int code) {
	switch (code) {
	case 0:
		return "SRM_SUCCESS";
	case 1:
		return "SRM_FAILURE";
	case 2:
		return "SRM_AUTHENTICATION_FAILURE";
	case 3:
		return "SRM_AUTHORIZATION_FAILURE";
	case 4:
		return "SRM_INVALID_REQUEST";
	case 5:
		return "SRM_INVALID_PATH";
	case 6:
		return "SRM_FILE_LIFETIME_EXPIRED";
	case 7:
		return "SRM_SPACE_LIFETIME_EXPIRED";
	case 8:
		return "SRM_EXCEED_ALLOCATION";
	case 9:
		return "SRM_NO_USER_SPACE";
	case 10:
		return "SRM_NO_FREE_SPACE";
	case 11:
		return "SRM_DUPLICATION_ERROR";
	case 12:
		return "SRM_NON_EMPTY_DIRECTORY";
	case 13:
		return "SRM_TOO_MANY_RESULTS";
	case 14:
		return "SRM_INTERNAL_ERROR";
	case 15:
		return "SRM_FATAL_INTERNAL_ERROR";
	case 16:
		return "SRM_NOT_SUPPORTED";
	case 17:
		return "SRM_REQUEST_QUEUED";
	case 18:
		return "SRM_REQUEST_INPROGRESS";
	case 19:
		return "SRM_REQUEST_SUSPENDED";
	case 20:
		return "SRM_ABORTED";
	case 21:
		return "SRM_RELEASED";
	case 22:
		return "SRM_FILE_PINNED";
	case 23:
		return "SRM_FILE_IN_CACHE";
	case 24:
		return "SRM_SPACE_AVAILABLE";
	case 25:
		return "SRM_LOWER_SPACE_GRANTED";
	case 26:
		return "SRM_DONE";
	case 27:
		return "SRM_PARTIAL_SUCCESS";
	case 28:
		return "SRM_REQUEST_TIMED_OUT";
	case 29:
		return "SRM_LAST_COPY";
	case 30:
		return "SRM_FILE_BUSY";
	case 31:
		return "SRM_FILE_LOST";
	case 32:
		return "SRM_FILE_UNAVAILABLE";
	case 33:
		return "SRM_CUSTOM_STATUS";
	default:
		srmlogit(STORM_LOG_WARNING, "reconvertStatusCode", "Received an unknown status code: %d\n", code);
		return "UNKNOWN";
	}
}
