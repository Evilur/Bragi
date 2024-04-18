#ifndef BRAGI_WEB_CLIENT_H
#define BRAGI_WEB_CLIENT_H

class WebClient {
public:
	WebClient() = default;
	virtual ~WebClient() = default;
	
	virtual void Read(char* buffer, int size) = 0;
};

#endif
