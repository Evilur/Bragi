#include <iostream>
#include "https_client.h"

HttpsClient::HttpsClient(const std::string &url) : WebClient(url) {
	_stream = new std::stringstream;

	asio::io_service svc;
	asio::ssl::context ctx(asio::ssl::context::method::sslv23_client);
	_socket = new asio::ssl::stream<asio::ip::tcp::socket>(svc, ctx);
	asio::ip::tcp::resolver resolver(svc);
	auto it = resolver.resolve({ _host, "443" });
	asio::connect(_socket->lowest_layer(), it);
	_socket->handshake(asio::ssl::stream_base::handshake_type::client);

	std::string request = std::format("GET {} HTTP/1.1\n"
	                                  "Host: {}\n"
	                                  "Connection: close\n"
	                                  "\r\n\r\n", _get, _host);
	asio::write(*_socket, asio::buffer(request));
}

HttpsClient::~HttpsClient() {
	delete _stream;
	_stream = nullptr;
}

bool HttpsClient::CanRead() const {
	return !_ec;
}

void HttpsClient::Read(char* buffer, const int size) {
	char tmp_buffer[1024];
	while (_current_len < size && !_ec) {
		int len = _socket->read_some(asio::buffer(tmp_buffer, 1024), _ec);
		_current_len += len;
		_stream->write(tmp_buffer, len);
	}

	_stream->read(buffer, size);
	_current_len -= size;
}