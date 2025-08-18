#ifndef BRAGI_HTTP_CLIENT_H
#define BRAGI_HTTP_CLIENT_H
#include <netinet/in.h>

class String;

class HttpClient {
public:
    explicit HttpClient(const char* hostname,
                        const char* path,
                        const char* headers = "",
                        const char* method = "GET",
                        const char* body = "");

    ~HttpClient();

    bool End() const;

    unsigned int Read(char* out, unsigned int size);

    String ReadAll();

private:
    static inline unsigned short _port = htons(80);

    int _server_fd;

    static constexpr int BUFFER_SIZE = 256;
    char _buffer[BUFFER_SIZE];
    unsigned int _buffer_offset = 0;
    unsigned int _buffer_size = 0;

    unsigned short _status_code = 0;
    unsigned long _content_length = 0;

    bool _eof = false;

    bool _is_chunked = true;

    bool Write(const char* buffer,
               unsigned int buffer_size) const;
};

#endif