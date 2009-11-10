#ifndef FILE_STATUS_STATUS_HPP
#define FILE_STATUS_STATUS_HPP
// Copyright (C) 2006 by Antonio Messina <antonio.messina@ictp.it> for the ICTP project EGRID.

/*
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by the
 *  Free Software Foundation; either version 2 of the License, or (at your
 *  option) any later version.
 *
 *  This program is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include "srmv2H.h"
#include "storm_functions.h"
#include "storm_util.h"
#include "srm_server.h"
#include "srmlogit.h"

#include <string>
#include <vector>
#include <map>

#include <exception>
#include <stdexcept>

#include "mysql_query.hpp"
#include "sql_string.hpp"

#include "soap_util.hpp"
#include "Credentials.hpp"
#include "HttpPostClient.h"

typedef std::map<std::string, std::string> file_status_result_t;
typedef std::vector<file_status_result_t> file_status_results_t;
//#include "file_status_interface.hpp"

class token_not_found: public std::exception {
public:
    token_not_found() {
    }
    ;
    token_not_found(std::string reason) {
        errmsg = reason.c_str();
    }
    const char *what() const throw () {
        return errmsg;
    }

private:
    const char *errmsg;
};

namespace storm {
// Template Class file_status
template<typename soap_out_t>
class file_status {
public:
    /// Ctor taking DB connection parameters
    file_status(struct soap * soap) :
        _soap(soap), _results_filled(false), _credentials(soap) {
        _response = NULL;
    }
    ;

    std::string getClientDN() {
        return _credentials.getDN();
    }
    ;

    /// Construct from DB and client supplied requestToken

    /// Fill in SOAP output data structure
    virtual soap_out_t* response() {
    }
    ;

    virtual soap_out_t* error_response(const ns1__TStatusCode SRM_ERROR_CODE,
            const char * const SRM_ERROR_EXPLANATION) {
        _status = SRM_ERROR_CODE;
        _explanation = SRM_ERROR_EXPLANATION;
        return response();
    }

    virtual bool is_authorized(const std::string& clientdn) {
        return (_client_dn == clientdn);
    }

    virtual void load(struct srm_dbfd *db, const std::string &requestToken) {
        // Load from DB using the specified query and add the
        // results to _results

        srmlogit(STORM_LOG_DEBUG, "file_status::load", "file_status::load()\n");
        storm_db::vector_exec_query(db, _query, _results);

        if (!_results_filled) {
            try {
                _fill_common_value();
            } catch (...) {
            }
        }
    }

    bool isSurlOnDisk(std::string requestToken, std::string surl) {

        bool result = false;

        try {

            HttpPostClient client;

            client.setHostname(_recalltableHost);
            client.setPort(_recalltablePort);
            std::string data = "requestToken=" + requestToken + "\nsurl=" + surl;
            client.callService(data);

            long responseCode = client.getHttpResponseCode();
            srmlogit(STORM_LOG_DEBUG, "bol_status::isSurlOnDisk()", "Response code: %d\n", responseCode);

            if (responseCode == 200) {

                std::string response = client.getResponse();

                if (response.compare("true") == 0) {

                    result = true;

                    srmlogit(STORM_LOG_DEBUG2, "bol_status::isSurlOnDisk()", "Response=true for surl=%s\n",
                            surl.c_str());

                } else {

                    result = false;

                    srmlogit(STORM_LOG_DEBUG2, "bol_status::isSurlOnDisk()", "Response=false for surl=%s\n",
                            surl.c_str());

                }
            }

        } catch (exception& e) {
            srmlogit(STORM_LOG_ERROR, "bol_status::isSurlOnDisk()",
                    "Curl: cannot create handle for HTTP client.\n");
            return false;
        }

        return result;
    }

protected:
    virtual void _fill_common_value() {
        // Fill common value for all requests.
        _n_files = _results.size();
        if (_n_files == 0)
            throw ENOENT;
        file_status_result_t row0(_results.at(0));

        if (row0["client_dn"].size() == 0)
            throw ENOENT;
        _status = static_cast<ns1__TStatusCode> (atoi(row0["status"].c_str()));
        _explanation = row0["errstring"];
        _client_dn = row0["client_dn"];
        _r_token = row0["r_token"];
        _s_token = row0["s_token"];
        _u_token = row0["u_token"];
        _f_type = row0["config_FileStorageTypeID"][0];
        _overwrite = row0["config_OverwriteID"][0];
        _r_type = row0["config_RequestTypeID"];
        _access_pattern = row0["config_AccessPatternID"][0];

        _n_completed = atoi(row0["numOfCompleted"].c_str());
        _n_waiting = atoi(row0["numOfWaiting"].c_str());
        _n_failed = atoi(row0["numOfFailed"].c_str());

        // WARNING: this are all storm_time_t, not int!
        _remainingTime = atoi(row0["remainingTotalTime"].c_str());
        _pinLifetime = atoi(row0["pinLifetime"].c_str());
        _lifetime = atoi(row0["fileLifetime"].c_str());
        _retrytime = atoi(row0["retrytime"].c_str());
        _results_filled = true;
    }

    virtual void _empty_results() {
        _results.clear();
    }

    storm::Credentials _credentials;

    // altri campi comuni da 'request_queue'
    std::string _query;

    soap * _soap;
    struct srm_dbfd *_db;

    sql_string _client_dn;
    std::vector<std::string> _fqans;
    ns1__TStatusCode _status; // Staus of the SRM request
    std::string _explanation;
    sql_string _r_token; // Request Token
    std::string _u_token; // User Token
    std::string _s_token; // Space Token
    // std::string _proxy;
    char _f_type;
    char _overwrite;
    char _access_pattern;
    std::string _r_type;
    storm_time_t _retrytime;
    storm_time_t _pinLifetime; // -1 = not specified
    storm_time_t _remainingTime; // -1 = not specified
    storm_time_t _lifetime;
    int _n_files;
    int _n_completed;
    int _n_waiting;
    int _n_failed;
    int _n_aborted;

    file_status_results_t _results;
    bool _results_filled;
    soap_out_t* _response;
};

}

#endif //FILE_STATUS_STATUS_HPP
