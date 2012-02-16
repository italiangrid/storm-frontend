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

#ifndef FILE_REQUEST_HPP
#define FILE_REQUEST_HPP

extern "C" {
#include <openssl/x509.h>
}

#include "srmv2H.h"
#include "storm_util.h"

#include <string>
#include <vector>

#include <mysql/mysqld_error.h>
#include <mysql/mysql.h>

#include <exception>
#include <stdexcept>

#include "soap_util.hpp"
#include "sql_string.hpp"
#include "Credentials.hpp"

/**
 * Abstract class representing a generic file request.
 */
namespace storm {
class invalid_request: public std::exception {
public:
    invalid_request() {
    }
    ;
    invalid_request(std::string reason) {
        errmsg = reason.c_str();
    }
    const char *what() const throw () {
        return errmsg;
    }
private:
    const char *errmsg;
};

class not_supported: public std::exception {
public:
    not_supported() {
    }
    ;
    not_supported(std::string reason) {
        errmsg = reason.c_str();
    }
    const char *what() const throw () {
        return errmsg;
    }
private:
    const char *errmsg;
};

template<typename soap_in_t, typename soap_out_t>
class file_request {
public:
    file_request(struct soap *soap) :
        _credentials(soap), _soap(soap) {
    }
    ;

    virtual ~file_request() {
    }
    ;

    /**
     * Get the current client DN
     */
    std::string getClientDN() {
        return _credentials.getDN();
    }
    ;

    void setClientDN(const char* clientDN) {
        _credentials.setDN(std::string(clientDN));
    }
    ;

    bool saveProxy() {
        _credentials.saveProxy(_r_token);
    }
    ;

    /**
     * Get the current status code of the file request
     */
    ns1__TStatusCode status() {
        return _status;
    }
    ;

    /**
     * Set the current status code of the file request
     * @param i an integer
     * @return i an integer: the current status code
     */
    ns1__TStatusCode status(int i) {
        ns1__TStatusCode old = _status;
        _status = static_cast<ns1__TStatusCode> (i);
        return old;
    }
    ;

    /**
     * Get the current explanation string
     */
    std::string explanation() {
        return _explanation;
    }
    ;

    /**
     * Set the current status explanation string
     * @param s a std::string
     * @return the old explanation string of the instance
     */
    std::string explanation(std::string s) {
        std::string old = _explanation;
        _explanation = s;
        return old;
    }
    ;

    std::string sql_format(char c) {
        std::string s = "'";

        s.append(1, c);
        s.append("'");
        return s;
    }
    ;

    std::string sql_format(const std::string& s) {
        std::string formatted_s = "'";

        formatted_s.append(s);
        formatted_s.append("'");
        return formatted_s;
    }
    ;

    std::string sql_format(bool value) {
        if (value) {
            return "1";
        }
        return "0";
    }
    ;

    /**
     * get the current request token
     */
    std::string r_token() {
        return _r_token;
    }
    ;

    /**
     * Set the current request token
     * @param s a std::string
     * @return the old request token of the instance
     */
    std::string r_token(std::string s) {
        std::string old = _r_token;
        _r_token = s;
        return old;
    }
    ;

    /**
     * get the current user token
     */
    std::string u_token() {
        return _u_token;
    }
    ;

    /**
     * Set the current user token
     * @param s a std::string
     * @return the old user token of the instance
     */
    std::string u_token(std::string s) {
        std::string old = _u_token;
        _u_token = s;
        return old;
    }
    ;

    /**
     * get the current space token
     */
    std::string s_token() {
        return _s_token;
    }
    ;

    /**
     * Set the current space token
     * @param s a std::string
     * @return the old user token of the instance
     */
    std::string s_token(std::string s) {
        std::string old = _s_token;
        _s_token = s;
        return old;
    }
    ;

    /**
     * Set the proxy date
     * @param date char*
     * @return old date
     */
    char* proxy_date(char * date) {
        char* old = date;
        _proxy_date = date;
        return old;
    }
    ;

    /**
     * get the current date
     */
    char* proxy_date() {
        return _proxy_date;
    }
    ;

    /**
     * Set the proxy
     * @param date X509*
     * @return void
     */
    void proxy(X509* p) {
        _proxy = p;
    }
    ;

    /**
     * get the current proxy
     */
    X509* proxy() {
        return _proxy;
    }
    ;

    std::vector<sql_string> getFQANsVector() {
        return _credentials.getFQANsVector();
    }
    ;

    char overwrite() {
        return _overwrite;
    }
    ;

    char overwrite(char o) {
        char old = _overwrite, _overwrite = o;
        return old;
    }
    ;

    virtual void load(soap_in_t *req) = 0;
    virtual bool supportsProtocolSpecification() = 0;
    virtual std::vector<sql_string>* getRequestedProtocols() = 0;
    virtual void setProtocolVector(std::vector<sql_string>* protocolVector) = 0;

    /*
     * Set the status code at SURL level to SRM_FAILURE to all requested SURLs
     */
    virtual void setGenericFailureSurls() = 0;
    virtual void insert(struct srm_dbfd *dbfd) = 0;
    virtual soap_out_t *response() = 0;
    soap_out_t *error_response(ns1__TStatusCode s, const char * const expl) {
        _status = s;
        _explanation = expl;
        return response();
    }
    ;

protected:
    soap* _soap;
    storm::Credentials _credentials;
    sql_string _client_dn;
    std::vector<sql_string> _fqans;
    std::string _fqans_one_string; // Temporary hack: inserted in the BLOB column of the DB
    std::string _explanation;
    ns1__TStatusCode _status; // Staus of the SRM request
    sql_string _r_token; // Request Token
    sql_string _u_token; // User Token
    sql_string _s_token; // Space Token
    struct srm_dbfd *_db;
    // std::string _proxy;
    storm_time_t _retrytime;
    storm_time_t _pinLifetime; // -1 = not specified
    storm_time_t _remainingTime; // -1 = not specified
    storm_time_t _lifetime;
    int _n_files;
    int _n_completed;
    int _n_waiting;
    int _n_failed;
    char _f_type;
    char _overwrite;
    std::string _r_type;

    std::string _response_error;
    int _response_errno;

    //User Credential
    char *_proxy_date;
    X509 *_proxy;
};
}

#endif
