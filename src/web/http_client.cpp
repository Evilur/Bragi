#include "http_client.h"
#include "util/logger.h"
#include "exception/http_client_exception.h"

#include <asio.hpp>

HttpClient::HttpClient(const std::string &url) : WebClient(url) {
	/* Init the stream */
	_stream = asio::ip::tcp::iostream(_host, "80");
	_stream << "GET " << _get << " HTTP/1.1\n"
			<< "Host:" << _host << '\n'
			<< "Connection: close\n"
			<< "\r\n\r\n" << std::flush;
	
	/* Check for good stream */
	if (!_stream.good()) throw WebClientException(CON_CANNOT_BE_ESTABLISHED);
	
	/* Skip http response headers */
	char buffer[1024];
	while (_stream.peek() != '\r') _stream.getline(buffer, 1024);
	_stream.getline(buffer, 1024);
}

inline HttpClient::~HttpClient() { _stream.close(); }

void HttpClient::Read(char* buffer, int size) { _stream.read(buffer, size); }

bool HttpClient::CanRead() { return !_stream.eof(); }
