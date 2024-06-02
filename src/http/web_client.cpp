#include <regex>
#include "web_client.h"

WebClient::WebClient(const std::string &url) {
	/* Get delimiters */
	int first_del = url.find_first_of("://") + 3;
	int second_del = url.find_first_of('/', 8);

	/* Parse the url */
	_host = url.substr(first_del, second_del - first_del);
	_get = std::regex_replace(url.substr(second_del), std::regex("[[:space:]]"), "%20");
}