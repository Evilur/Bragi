#include "http_client.h"
#include "util/logger.h"
#include "exception/http_client_exception.h"

#include <asio.hpp>

HttpClient::HttpClient(const std::string &url) {
	/* Get delimiters */
	int first_del = url.find_first_of("://") + 3;
	int second_del = url.find_first_of('/', 8);
	
	/* Parse the url */
	std::string host = url.substr(first_del, second_del - first_del);
	std::string get = url.substr(second_del + 1);
	
	/* Init the stream */
	_stream = asio::ip::tcp::iostream(host, "80");
	_stream << "GET " << get << " HTTP/1.1\n"
			<< "Host:" << host << '\n'
			<< "Connection: close\n"
			<< "\r\n\r\n" << std::flush;
	
	/* Check for good stream */
	if (!_stream.good()) throw WebClientException(CON_CANNOT_BE_ESTABLISHED);
}

HttpClient::~HttpClient() { _stream.close(); }

void HttpClient::Read(char* buffer, int size) { 
	_stream.read(buffer, size); 
}
