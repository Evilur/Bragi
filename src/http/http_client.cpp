#include "http_client.h"
#include "util/logger.h"

#include <asio.hpp>

HttpClient::HttpClient(const std::string &url) {
	/* Get delimiters */
	int first_del = url.find_first_of("://") + 3;
	int second_del = url.find_first_of('/', 8);

	/* Parse the url */
	_host = url.substr(first_del, second_del - first_del);
	_get = url.substr(second_del);
	
	/* Init the stream */
	_stream = new asio::ip::tcp::iostream(_host, "80");
	
	/* Send the request */
	*_stream << "GET " << _get << " HTTP/1.1\n" 
			 << "Host: " << _host << "\n"
			 << "Connection: close\n"
			 << "\r\n\r\n" << std::flush;
	
	char buffer[1024];
	while(buffer[0] != '\r') _stream->getline(buffer, 1024);
}

HttpClient::~HttpClient() {
	_stream->close();
	delete _stream;
	_stream = nullptr;
}

void HttpClient::Read(char* buffer, int size) { 
	_stream->read(buffer, size);
}

bool HttpClient::CanRead() { return !_stream->eof(); }
