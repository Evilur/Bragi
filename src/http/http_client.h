#ifndef BRAGI_HTTP_CLIENT_H
#define BRAGI_HTTP_CLIENT_H

#include <asio.hpp>

class HttpClient final {
public:
	explicit HttpClient(const std::string &url, const std::string &headers = "");

	~HttpClient();

	bool CanRead() const;

	void Read(char* buffer, int size);

	int ReadAll(char*&buffer, int size, int delta_size = 1024 * 8);

private:
	asio::ip::tcp::iostream* _stream;

	void ReadHeaders();
};

#endif