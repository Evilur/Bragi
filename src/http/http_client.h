#ifndef BRAGI_HTTP_CLIENT_H
#define BRAGI_HTTP_CLIENT_H

#include <asio.hpp>

class HttpClient {
public:
	explicit HttpClient(const std::string &url, const std::string &headers = "", const std::string &body = "", const char* requset_type = "GET");

	~HttpClient();

	bool CanRead() const;

	void Read(char* buffer, int size);

	int PrevCount() const;

	/** Read all http document
	 * @warning return data is an allocated memory so you need to free it later
	 * @return a char aray with all http document data
	 */
	const char* ReadAll();

private:
	asio::ip::tcp::iostream* _stream = nullptr;
	unsigned int _content_length = 0;

	void ReadHeaders();
};

#endif