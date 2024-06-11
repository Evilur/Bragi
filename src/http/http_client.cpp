#include "http_client.h"
#include "util/logger.h"

#include <asio.hpp>

HttpClient::HttpClient(const char* const url, const char* const headers, const char* const body) {
	/* Get the hostname and the get request from the url */
	const char* host_ptr = std::strncmp(url, "http://", 7) == 0 ? std::strchr(url, ':') + 3 : url;
	const char* get_ptr = std::strchr(host_ptr, '/');
	const unsigned short host_size = get_ptr - host_ptr;
	const unsigned short get_size = std::strlen(get_ptr);

	/* Write the data to the fields */
	std::strncpy(_host = new char[host_size + 1], host_ptr, host_size);
	std::strncpy(_get = new char[get_size + 1], get_ptr, get_size);
	_host[host_size] = '\0';
	_get[get_size] = '\0';

	/* Init the stream and send the request */
	_stream = new asio::ip::tcp::iostream(_host, "80");
	*_stream << "GET " << _get << " HTTP/1.1\n"
	         << "Host: " << _host << '\n'
	         << "Connection: close\n" << "\r\n\r\n" << std::flush;

	/* Add custom headers (if exists) */
	if (headers) *_stream << headers << '\n';

	/* Add a request body (if exists) */
	if (body) {
		*_stream << "Content-Length: " << std::strlen(body) << '\n'
		         << "\r\n\r\n" << body << std::flush;
	} else *_stream << "\r\n\r\n" << std::flush;

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