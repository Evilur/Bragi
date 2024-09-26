#ifndef BRAGI_HTTPS_CLIENT_H
#define BRAGI_HTTPS_CLIENT_H

#include <string>

/** !!!TEMPORARY!!! https client for debugging the bot */
class HttpsClient final {
public:
	explicit HttpsClient(const std::string &url, const std::string &headers = "", const std::string &body = "", const char* requset_type = "GET");

	/** Read all http document
	 * @warning return data is an allocated memory so you need to free it later
	 * @return a char aray with all http document data
	 */
	const char* ReadAll();

private:
	char* _data = nullptr;
};

#endif