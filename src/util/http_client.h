#ifndef BRAGI_HTTP_CLIENT_H
#define BRAGI_HTTP_CLIENT_H

#include <boost/asio.hpp>

class HttpClient {
public:
	static const inline char* HTTP_PORT = "80";
	
	explicit HttpClient(const std::string &url);
	
	bool CanRead();
	std::string ReadLine();
private:
	boost::asio::ip::tcp::iostream _stream;
	std::string _url;
};

#endif
