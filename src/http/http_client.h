#ifndef BRAGI_HTTP_CLIENT_H
#define BRAGI_HTTP_CLIENT_H

#include <asio.hpp>

class HttpClient final {
public:
	explicit HttpClient(const char* url, const char* headers = nullptr, const char* body = nullptr);

	~HttpClient();

	bool CanRead() const;

	void Read(char* buffer, int size);

	int ReadAll(char*&buffer, int size, int delta_size = 1024 * 8);

private:
	char* _host;
	char* _get;
	asio::ip::tcp::iostream* _stream;

	void ReadHeaders();
};

#endif