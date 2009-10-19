
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
	long getHttpResponseCode();

private:
	static size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp);

	CURL* _curl;
	char** _response;
};

#endif /* HTTPPOSTCLIENT_H_ */
