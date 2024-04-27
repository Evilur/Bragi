#include "http_client.h"
#include "util/logger.h"

#include <asio.hpp>

HttpClient::HttpClient(const std::string &url) {
	/* Get delimiters */
	int first_del = url.find_first_of("://") + 3;
	int second_del = url.find_first_of('/', 8);

	/* Parse the url */
	_host = url.substr(first_del, second_del - first_del);
	_get = url.substr(second_del + 1);
	
	/* Init the stream */
	//_stream = asio::ip::tcp::iostream(_host, "443");
	
	/* Send the request */
	//_stream << "" << std::flush;
}

HttpClient::~HttpClient() { _stream.close(); }

void HttpClient::Read(char* buffer, int size) { _stream.read(buffer, size); }

bool HttpClient::CanRead() { return !_stream.eof(); }
