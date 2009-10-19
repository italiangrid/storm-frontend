#include "HttpPostClient.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <exception>
#include "storm_utils.hpp"
#include "FrontendConfiguration.hpp"

HttpPostClient::HttpPostClient() {

    _path = std::string("/storm/checksum.json");
    _url = NULL;

    _curl = curl_easy_init();

    if (_curl == NULL) {
        throw new std::exception();
    }

    curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(_curl, CURLOPT_WRITEDATA, _response);

}

HttpPostClient::~HttpPostClient() {
    curl_easy_cleanup(_curl);
    free(*_response);
}

int HttpPostClient::callService(std::string data) {
    curl_easy_setopt(_curl, CURLOPT_URL, _url);
    curl_easy_setopt(_curl, CURLOPT_PORT, _port);
    curl_easy_setopt(_curl, CURLOPT_POSTFIELDS, data.c_str());
    return curl_easy_perform(_curl);
}

std::string HttpPostClient::getResponse() {
    std::string response(*_response);
    return response;
}

long HttpPostClient::getHttpResponseCode() {
    long code;
    curl_easy_getinfo(_curl, CURLINFO_RESPONSE_CODE, &code);
    return code;
}

char* HttpPostClient::getUrl() {
    if (_url != NULL) {
        return _url;
    }

    std::string url = std::string("http://");
    url.append(_hostname);
    url.append(":");
    url.append(to_string(_port));
    url.append(_path);

    _url = (char*) calloc(url.size() + 1, 1);
    strcpy(_url, url.c_str());

    return _url;
}

std::string HttpPostClient::getHostname() {
    return _hostname;
}

std::string HttpPostClient::getPath() {
    return _path;
}

long HttpPostClient::getPort() {
    return _port;
}

void HttpPostClient::setHostname(std::string hostname) {
    if (_url != NULL) {
        free(_url);
        _url = NULL;
    }
    _hostname = hostname;
}

void HttpPostClient::setPath(std::string path) {
    if (_url != NULL) {
        free(_url);
        _url = NULL;
    }
    _path = path;
}

void HttpPostClient::setPort(long port) {
    if (_url != NULL) {
        free(_url);
        _url = NULL;
    }
    _port = port;
}

size_t HttpPostClient::write_data(void* buffer, size_t size, size_t nmemb, void* userp) {

    char** p = (char**) userp;
    *p = (char*) calloc(nmemb, size);

    if (*p == NULL) {
        return 0;
    }

    size_t nbytes = size * nmemb;

    memcpy(*p, buffer, nbytes);

    return nbytes;
}
