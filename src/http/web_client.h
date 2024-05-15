#ifndef BRAGI_WEB_CLIENT_H
#define BRAGI_WEB_CLIENT_H

#include <string>

class WebClient {
protected:
	explicit WebClient(const std::string &url);

	virtual ~WebClient() = default;

	virtual bool CanRead() const = 0;

	virtual void Read(char* buffer, int size) = 0;

	std::string _host;
	std::string _get;
};

#endif