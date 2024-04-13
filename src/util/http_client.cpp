#include "http_client.h"
#include "logger.h"


HttpClient::HttpClient(const std::string &url) : _stream(GetHost(url), "80") {
	_stream << "GET / HTTP/1.0\r\n\r\n" << std::flush;
	std::cout << _url;
}

std::string HttpClient::GetLine() {
	std::string result;
	std::getline(_stream, result);
	return result;
}

std::string HttpClient::GetHost(const std::string &url) {
	int begin = url.find_first_of("://") + 3;
	int end = url.find_first_of('/', 8) - begin;
	return url.substr(begin, end);
}
