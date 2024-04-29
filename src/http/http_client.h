#ifndef BRAGI_HTTP_CLIENT_H
#define BRAGI_HTTP_CLIENT_H

#include <asio.hpp>

class HttpClient final {
public:
	explicit HttpClient(const std::string &url);
	~HttpClient();
	
	void Read(char* buffer, int size);
	bool CanRead();
protected:
	std::string _host;
	std::string _get;
private:
	asio::ip::tcp::iostream* _stream;
};

#endif
