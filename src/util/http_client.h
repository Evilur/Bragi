#ifndef BRAGI_HTTP_CLIENT_H
#define BRAGI_HTTP_CLIENT_H

#include <boost/asio.hpp>

class HttpClient {
public:
	explicit HttpClient(const std::string &url);
	
	std::string GetLine();
private:
	boost::asio::ip::tcp::iostream _stream;
	std::string _url;
	
	static std::string GetHost(const std::string &url);
};

#endif
