#ifndef BRAGI_HTTP_CLIENT_EXCEPTION_H
#define BRAGI_HTTP_CLIENT_EXCEPTION_H

#include <exception>

enum HttpClientErrorCode : char { CON_CANNOT_BE_ESTABLISHED };

class WebClientException final : std::exception {
public:
	explicit WebClientException(HttpClientErrorCode e_code);
	
	HttpClientErrorCode GetCode();
	const char* GetData();
private:
	const HttpClientErrorCode _e_code;
	const char* _e_data;
	
	static const char* GetData(HttpClientErrorCode e_code);
};

#endif
