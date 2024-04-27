#include "https_client.h"
#include "util/logger.h"

#include <asio.hpp>
#include <asio/ssl.hpp>
#include <iostream>

HttpsClient::HttpsClient(const std::string &url) : HttpClient(url) {
	/*asio::io_service svc;
	asio::ssl::context ctx(asio::ssl::context::method::sslv23_client);
	_socket = new asio::ssl::stream<asio::ip::tcp::socket>(svc, ctx);
	asio::connect(_socket->lowest_layer(), asio::ip::tcp::resolver(svc).resolve(_host, "443"));
	_socket->handshake(asio::ssl::stream_base::handshake_type::client);
	_socket->write_some(asio::buffer(GetRequest()));*/
}

HttpsClient::~HttpsClient() = default;

void HttpsClient::Read(char* buffer, int size) { }

bool HttpsClient::CanRead() { return false; }
