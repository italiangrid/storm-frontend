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

#include "HttpPostClient.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <exception>
#include "storm_utils.hpp"
#include "FrontendConfiguration.hpp"
#include "srmlogit.h"

HttpPostClient::HttpPostClient() {

    _path = std::string("/recalltable/task");
    _url = NULL;
    _response = NULL;
    _curl = curl_easy_init();

    if (_curl == NULL) {
        throw new std::exception();
    }

    curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(_curl, CURLOPT_WRITEDATA, &_response);
    curl_easy_setopt(_curl, CURLOPT_READDATA, &_inputData);
    curl_easy_setopt(_curl, CURLOPT_READFUNCTION, read_callback);
    curl_easy_setopt(_curl, CURLOPT_UPLOAD, 1L);
    curl_easy_setopt(_curl, CURLOPT_PUT, 1L);

    _slist = NULL;
    _slist = curl_slist_append(_slist, "Expect:");
    _slist = curl_slist_append(_slist, "Content-Type: text/plain");
    curl_easy_setopt(_curl, CURLOPT_HTTPHEADER, _slist);

}

HttpPostClient::~HttpPostClient() {
    curl_slist_free_all(_slist);
    curl_easy_cleanup(_curl);
    free(_response);
    free(_url);
}

int HttpPostClient::callService(std::string data) {

    _inputData.data = data;
    _inputData.endOfTransmission = false;

    free(_response);
    _response = NULL;

    curl_easy_setopt(_curl, CURLOPT_URL, getUrl());

    return curl_easy_perform(_curl);
}

std::string HttpPostClient::getResponse() {
    std::string response(_response);
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
    *p = (char*) calloc(nmemb + 1, size);

    if (*p == NULL) {
        return 0;
    }

    size_t nbytes = size * nmemb;

    memcpy(*p, buffer, nbytes);

    return nbytes;
}

size_t HttpPostClient::read_callback(void *ptr, size_t size, size_t nmemb, void *stream) {

    HttpPostClient::IndaputData* inputData = (HttpPostClient::IndaputData*) stream;

    int inputDataSize = inputData->data.size();

    if (inputData->endOfTransmission) {
        srmlogit(STORM_LOG_DEBUG2, "HttpPutClient::read_callback()", "End of transmission\n");
        return 0;
    }

    if (inputDataSize > (nmemb * size)) {
        srmlogit(STORM_LOG_ERROR, "HttpPutClient::read_callback()",
                "Buffer too small, aborting HTTP PUT request for: %s\n", inputData->data.c_str());
        return 0;
    }

    srmlogit(STORM_LOG_DEBUG2, "HttpPutClient::read_callback()", "data: \"%s\"\n", inputData->data.c_str());

    strncpy((char *) ptr, inputData->data.c_str(), inputDataSize);

    inputData->endOfTransmission = true;

    return inputDataSize;
}
