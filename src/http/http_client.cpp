#include "http_client.h"
#include "util/logger.h"

#include <asio.hpp>

HttpClient::HttpClient(const std::string &url) : WebClient(url) {
	/* Init the stream */
	_stream = new asio::ip::tcp::iostream(_host, "80");

	/* Send the request */
	*_stream << "GET " << _get << " HTTP/1.1\n"
	         << "Host: " << _host << "\n"
			 << "Connection: close\n"
			 << "\r\n\r\n" << std::flush;

	ReadHeaders();
}

HttpClient::~HttpClient() {
	_stream->close();
	delete _stream;
	_stream = nullptr;
}

bool HttpClient::CanRead() const { return !_stream->eof(); }

void HttpClient::Read(char* buffer, const int size) { _stream->read(buffer, size); }

void HttpClient::ReadHeaders() {
	char buffer[1024];
	while (buffer[0] != '\r') _stream->getline(buffer, 1024);
}