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
    class Reader {
    public:
        virtual ~Reader() noexcept = default;

        virtual unsigned int Read(HttpClient* http,
                                  char* out,
                                  unsigned int size) = 0;

        virtual String ReadAll(HttpClient* http) = 0;
    } *_reader = nullptr;

    class CompleteReader final : public Reader {
    public:
        ~CompleteReader() noexcept override = default;

        unsigned int Read(HttpClient* http,
                          char* out,
                          unsigned int size) override;

        String ReadAll(HttpClient* http) override;
    };

    class ChunkedReader final : public Reader {
    public:
        ~ChunkedReader() noexcept override = default;

        unsigned int Read(HttpClient* http,
                          char* out,
                          unsigned int size) override;

        String ReadAll(HttpClient* http) override;
    };

    static inline unsigned short _port = htons(80);

    int _server_fd;

    static constexpr int BUFFER_SIZE = 256;
    char _buffer[BUFFER_SIZE];
    unsigned int _buffer_offset = 0;
    unsigned int _buffer_size = 0;

    unsigned short _status_code = 0;
    unsigned long _content_length = 0;

    bool _eof = false;

    bool Write(const char* buffer,
               unsigned int buffer_size) const;
};

#endif