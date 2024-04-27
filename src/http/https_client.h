#ifndef BRAGI_HTTPS_CLIENT_H
#define BRAGI_HTTPS_CLIENT_H

#include "http_client.h"

#include <string>
#include <asio.hpp>
#include <asio/ssl.hpp>

class HttpsClient final : HttpClient {
public:
	explicit HttpsClient(const std::string &url);
	~HttpsClient() override;
	
	void Read(char* buffer, int size) override;
	bool CanRead() override;
};

#endif
