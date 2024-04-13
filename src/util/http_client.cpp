#include "http_client.h"
#include "logger.h"


HttpClient::HttpClient(const std::string &url) {
	/* Get delimiters */
	int first_del = url.find_first_of("://") + 3;
	int second_del = url.find_first_of('/', 8);
	
	/* Parse the url */
	std::string host = url.substr(first_del, second_del - first_del);
	std::string path = url.substr(second_del + 1);

	/* Init the stream */
	_stream = boost::asio::ip::tcp::iostream(host, "80");
	_stream << "GET /" << path << " HTTP/1.0\r\n\r\n" << std::flush;
}

bool HttpClient::CanRead() {
	return _stream.peek() != EOF;
}

std::string HttpClient::ReadLine() {
	std::string result;
	std::getline(_stream, result);
	return result;
}
