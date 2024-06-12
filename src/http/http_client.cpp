#include "http_client.h"
#include "util/logger.h"
#include "util/num_parser.h"

#include <asio.hpp>

HttpClient::HttpClient(const std::string &url, const std::string &headers) {
	/* !!! DELETE THIS IN PRODUCTION !!! */
	if (url.starts_with("https://")) throw std::runtime_error("This if a fucking https, not a http!");
	/* !!! DELETE THIS IN PRODUCTION !!! */

	/* Get the hostname and the get request from the url */
	const std::string request = url.starts_with("http://") ? url.substr(7) : url;
	const int get_index = request.find('/');
	const std::string host = request.substr(0, get_index);
	const std::string get = request.substr(get_index);

	/* Init the stream and send the request */
	_stream = new asio::ip::tcp::iostream(host, "80");
	*_stream << "GET " << get << " HTTP/1.0\n"
	         << "Host: " << host << '\n'
	         << "Connection: close\n";

	/* Add custom headers (if exists) */
	if (!headers.empty()) *_stream << headers << '\n';

	/* Write an empty line for the http request */
	*_stream << "\r\n\r\n" << std::flush;

	ReadHeaders();
}

HttpClient::~HttpClient() {
	_stream->close();
	delete _stream;
	_stream = nullptr;
}

bool HttpClient::CanRead() const { return !_stream->eof(); }

void HttpClient::Read(char* buffer, const int size) { _stream->read(buffer, size); }

int HttpClient::ReadAll(char*&buffer, int size, const int delta_size) {
	Read(buffer, size);

	while (CanRead()) {
		char* temp_buffer = new char[size + delta_size];
		for (unsigned int i = 0; i < size; i++) temp_buffer[i] = buffer[i];
		delete[] buffer;
		buffer = temp_buffer;
		Read(buffer + size, delta_size);
		size += delta_size;
	}

	return size;
}

void HttpClient::ReadHeaders() {
	char buffer[1024];
	while (buffer[0] != '\r') _stream->getline(buffer, 1024);
}