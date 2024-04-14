#include "http_client.h"
#include "logger.h"

#include <exception/http_client_exception.h>

HttpClient::HttpClient(const std::string &url) {
	/* Get delimiters */
	int first_del = url.find_first_of("://") + 3;
	int second_del = url.find_first_of('/', 8);
	
	/* Parse the url */
	std::string host = url.substr(first_del, second_del - first_del);
	std::string path = url.substr(second_del + 1);

	/* Init the stream */
	_stream = boost::asio::ip::tcp::iostream(host, HTTP_PORT);
	_stream << "GET /" << path << " HTTP/" << HTTP_VERSION << HTTP_END << std::flush;
	
	/* Check for good stream */
	if (!_stream.good()) throw HttpClientException(CON_CANNOT_BE_ESTABLISHED);
	
	/* Read lines before there isn't an empty one (there is the end of the output headers) */
	while (_stream.peek() != '\r') GetLine();
	GetLine();
}

HttpClient::~HttpClient() { _stream.close(); }

bool HttpClient::CanRead() { return _stream.peek() != EOF; }

std::string HttpClient::GetLine() {
	std::string result;
	std::getline(_stream, result);
	return result;
}

void HttpClient::Read(char* buffer, int size) { _stream.read(buffer, size); }
