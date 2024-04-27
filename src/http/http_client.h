#ifndef BRAGI_HTTP_CLIENT_H
#define BRAGI_HTTP_CLIENT_H

#include <asio.hpp>

class HttpClient {
public:
	explicit HttpClient(const std::string &url);
	virtual ~HttpClient();
	
	virtual void Read(char* buffer, int size);
	virtual bool CanRead();
protected:
	std::string _host;
	std::string _get;
private:
	asio::ip::tcp::iostream _stream;
};

#endif
