#ifndef BRAGI_HTTP_CLIENT_EXCEPTION_H
#define BRAGI_HTTP_CLIENT_EXCEPTION_H

#include <exception>

class WebClientException final : std::exception {
public:
	explicit WebClientException(const char* response, const char* url);
	
	void Log() const;
private:
	const char* const _response;
	const char* const _url;
};

#endif
