#ifndef BRAGI_HTTPS_CLIENT_H
#define BRAGI_HTTPS_CLIENT_H

#include "web_client.h"

#include <sstream>
#include <asio.hpp>
#include <asio/ssl.hpp>

class HttpsClient final : private WebClient {
public:
	explicit HttpsClient(const std::string &url);

	~HttpsClient() override;

	bool CanRead() const override;

	void Read(char* buffer, int size) override;

private:
	asio::error_code _ec;
	std::iostream* _stream;
	unsigned short _current_len = 0;
	asio::ssl::stream<asio::ip::tcp::socket>* _socket;

	void ReadHeaders() override;
};

#endif