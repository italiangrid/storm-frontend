
#include "HttpPostClient.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <exception>

HttpPostClient::HttpPostClient(std::string url) {

    _curl = curl_easy_init();

    if (_curl == NULL) {
        throw new std::exception();
    }

    curl_easy_setopt(_curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(_curl, CURLOPT_WRITEDATA, _response);
    curl_easy_setopt(_curl, CURLOPT_PORT, 9996);

}

HttpPostClient::~HttpPostClient() {
    curl_easy_cleanup(_curl);
    free(*_response);
}

int HttpPostClient::callService(std::string data) {
    curl_easy_setopt(_curl, CURLOPT_POSTFIELDS, data.c_str());
    return curl_easy_perform(_curl);
}

std::string HttpPostClient::getResponse() {
    std::string response(*_response);
    return response;
}

long HttpPostClient::getHttpResponseCode() {
    long code;
    curl_easy_getinfo(_curl, CURLINFO_RESPONSE_CODE , &code );
    return code;
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
