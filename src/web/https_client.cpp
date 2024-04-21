#include "https_client.h"

#include <asio.hpp>
#include <asio/ssl.hpp>
#include <iostream>

HttpsClient::HttpsClient(const std::string &url) : WebClient(url) {
	asio::io_service svc;
	asio::ssl::context ctx(asio::ssl::context::method::sslv23_client);
	asio::ssl::stream<asio::ip::tcp::socket> ssock(svc, ctx);
	asio::ip::tcp::resolver resolver(svc);
	auto it = resolver.resolve({"cdn.discordapp.com", "443"});
	asio::connect(ssock.lowest_layer(), it);
	ssock.handshake(asio::ssl::stream_base::handshake_type::client);
	std::string request("GET /attachments/985959696160682114/1229492213122338847/CHSV.wav?ex=662fe0da&is=661d6bda&hm=7f04aa374fef529164557ad5f7c2d90a8fce43313286cba67290dfdd230ec26a&=null HTTP/1.1\n"
	                    "Host: cdn.discordapp.com\n"
	                    "Connection: close\r\n\r\n");
	asio::write(ssock, asio::buffer(request));
	std::string response;
	asio::error_code ec;
	do {
		char buf[1024];
		size_t bytes_transferred = ssock.read_some(asio::buffer(buf), ec);
		if (!ec) response.append(buf, buf + bytes_transferred);
	} while (!ec);
	std::cout << "Response received: '" << response << "'\n";
}

void HttpsClient::Read(char* buffer, int size) {

}

HttpsClient::~HttpsClient() = default;

bool HttpsClient::CanRead() { return false; }
