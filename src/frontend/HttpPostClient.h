#ifndef HTTPPOSTCLIENT_H_
#define HTTPPOSTCLIENT_H_

#include "curl/curl.h"
#include <string>

class HttpPostClient {
public:
	HttpPostClient(std::string url);
	virtual ~HttpPostClient();

	int callService(std::string data);
	std::string getResponse();
	std::string getHostname();
	long getHttpResponseCode();
	char* getUrl();
    std::string getPath();
    std::string getPort();
    void setHostname(std::string hostname);
    void setPath(std::string path);
    void setPort(std::string port);


private:
	static size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp);

	CURL* _curl;
	char** _response;

	char* _url;
	std::string _hostname;
	long _port;
	std::string _path;
};

#endif /* HTTPPOSTCLIENT_H_ */
