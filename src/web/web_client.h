#ifndef BRAGI_WEB_CLIENT_H
#define BRAGI_WEB_CLIENT_H

#include <string>

class WebClient {
public:
	explicit WebClient(const std::string &url);
	virtual ~WebClient() = default;
	
	virtual void Read(char* buffer, int size) = 0;
	virtual bool CanRead() = 0;
protected:
	std::string _host;
	std::string _get;
};

#endif
