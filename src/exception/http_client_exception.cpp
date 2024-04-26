#include "http_client_exception.h"
#include "util/logger.h"

#include <format>

WebClientException::WebClientException(const char* response, const char* url) : _response(response), _url(url) { }

void WebClientException::Log() const { Logger::Debug(std::format("URL: {}\nResponse: {}", _url, _response)); }