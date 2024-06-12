#include "http_client.h"
#include "util/logger.h"
#include "util/num_parser.h"

#include <asio.hpp>

HttpClient::HttpClient(const std::string &url, const std::string &headers, const std::string &body, const char* requset_type) {
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
	*_stream << requset_type << ' ' << get << " HTTP/1.1\n"
	         << "Host: " << host << '\n'
	         << "Connection: close\n";

	/* Add custom headers (if exists) */
	if (!headers.empty()) *_stream << headers << '\n';

	if (!body.empty()) {
		*_stream << "Content-Length: " << body.length() << '\n'
		         << "\r\n\r\n"
		         << body;
	}

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

const char* HttpClient::ReadAll() {
	/* If there is a right content size */
	if (_content_length > 0) {
		char* buffer = new char[_content_length];
		_stream->read(buffer, _content_length);
		return buffer;
	} else {  //Else there is chunked data
		char* buffer = nullptr;
		unsigned int current_size = 0;

		while (!_stream->eof()) {
			/* Get the chunk size */
			char line_buffer[1024];
			_stream->getline(line_buffer, 1024);
			unsigned int chunk_size = NumParser::HexToDec(line_buffer);
			if (chunk_size == 0) break;

			/* Read the chunk to the temp buffer */
			char* tmp_buffer = new char[current_size + chunk_size];
			_stream->read(tmp_buffer + current_size, chunk_size);
			_stream->ignore(2);  //Ingore the \r\n chars

			/* Update the current buffer */
			for (int i = 0; i < current_size; i++) tmp_buffer[i] = buffer[i];  //Pointer can be null only if the curretn size is 0 => loop will break instantly
			delete[] buffer;
			buffer = tmp_buffer;
			current_size += chunk_size;
		}

		return buffer;
	}
}

void HttpClient::ReadHeaders() {
	char buffer[1024];
	while (buffer[0] != '\r') {
		_stream->getline(buffer, 1024);
		if (std::strncmp(buffer, "Content-Length: ", 16) == 0) _content_length = NumParser::ToUInt32(buffer + 16);
	}
}