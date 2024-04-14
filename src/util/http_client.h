#ifndef BRAGI_HTTP_CLIENT_H
#define BRAGI_HTTP_CLIENT_H

#include <boost/asio.hpp>

class HttpClient {
public:
	static const inline char* const HTTP_PORT = "80";
	static const inline char* const HTTP_VERSION = "1.1";
	static const inline char* const HTTP_END = "\r\n\r\n";
	
	explicit HttpClient(const std::string &url);
	~HttpClient();
	
	bool CanRead();
	std::string GetLine();
	void Read(char* buffer, int size);
private:
	boost::asio::ip::tcp::iostream _stream;
	std::string _url;
};

#endif
