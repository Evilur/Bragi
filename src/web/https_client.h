#ifndef BRAGI_HTTPS_CLIENT_H
#define BRAGI_HTTPS_CLIENT_H

#include "web_client.h"

#include <string>

class HttpsClient final : WebClient {
public:
	explicit HttpsClient(const std::string &url);
	~HttpsClient() override;
	
	void Read(char *buffer, int size) override;
	bool CanRead() override;
};

#endif
