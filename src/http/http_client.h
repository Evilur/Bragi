#ifndef BRAGI_HTTP_CLIENT_H
#define BRAGI_HTTP_CLIENT_H

#include "web_client.h"

#include <asio.hpp>

class HttpClient final : private WebClient {
public:
	explicit HttpClient(const std::string &url);

	~HttpClient() override;

	bool CanRead() const override;

	void Read(char* buffer, int size) override;

private:
	asio::ip::tcp::iostream* _stream;

	void ReadHeaders() override;
};

#endif