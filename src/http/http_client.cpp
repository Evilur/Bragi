#include "http_client.h"
#include "util/logger.h"

#include <asio.hpp>

HttpClient::HttpClient(const std::string &url) : WebClient(url) {
	/* Init the stream */
	_stream = new asio::ip::tcp::iostream(_host, "80");

	/* Send the request */
	*_stream << "GET " << _get << " HTTP/1.1\n"
	         << "Host: " << _host << "\n"
	         << "Accept: application/json, text/plain, */*\n"
	         << "Content-Type: text/plain;charset=UTF-8\n"
	         << "Cache-Control: max-age=0\n"
	         << "Accept-Language: en-US,en;q=0.9,en-US;q=0.8,en;q=0.7\n"
	         << "Accept-Charset: utf-8,ISO-8859-1;q=0.8,*;q=0.7\n"
	         << "User-Agent: Deezer/7.17.0.2 CFNetwork/1098.6 Darwin/19.0.0\n"
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