#ifndef HTTPPOSTCLIENT_H_
#define HTTPPOSTCLIENT_H_

#include "curl/curl.h"
#include <string>

class HttpPostClient {
public:
    HttpPostClient();
    virtual ~HttpPostClient();

    int callService(std::string data);
    std::string getResponse();
    std::string getHostname();
    long getHttpResponseCode();
    char* getUrl();
    std::string getPath();
    long getPort();
    void setHostname(std::string hostname);
    void setPath(std::string path);
    void setPort(long port);

private:
    static size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp);
    static size_t read_callback(void *ptr, size_t size, size_t nmemb, void *stream);

    CURL* _curl;
    struct curl_slist* _slist;

    char* _response;
    char* _url;
    long _port;
    std::string _hostname;
    std::string _path;

    class IndaputData {
    public:
        std::string data;
        bool endOfTransmission;
    } _inputData;
};

#endif /* HTTPPOSTCLIENT_H_ */
