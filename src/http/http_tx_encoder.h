#pragma once

#include "exception/http_exception.h"
#include "util/class.h"
#include "http/http_transport.h"

#include <cstdint>

class HttpTxEncoder {
public:
    PREVENT_COPY_AND_MOVE(HttpTxEncoder);
    virtual ~HttpTxEncoder() = default;

    virtual void Write(HttpTransport* transport, char chr) = 0;
    virtual void Write(HttpTransport* transport,
                       const char* buffer,
                       uint32_t buffer_size) = 0;

    virtual void Finish(HttpTransport* transport) = 0;

    class Plain;
    class Chunked;

protected:
    HttpTxEncoder() = default;
};

class HttpTxEncoder::Plain final : public HttpTxEncoder {
public:
    PREVENT_COPY_AND_MOVE(Plain);
    Plain(uint64_t content_length) noexcept;

    void Write(HttpTransport* transport, char chr) override;
    void Write(HttpTransport* transport,
               const char* buffer,
               uint32_t buffer_size) override;

    void Finish(HttpTransport* transport) override;

private:
    uint64_t _content_length;
};

class HttpTxEncoder::Chunked final : public HttpTxEncoder {
public:
    PREVENT_COPY_AND_MOVE(Chunked);
    Chunked() noexcept = default;

    void Write(HttpTransport* transport, char chr) override;
    void Write(HttpTransport* transport,
               const char* buffer,
               uint32_t buffer_size) override;

    void Finish(HttpTransport* transport) override;
};

inline HttpTxEncoder::Plain::Plain(const uint64_t content_length)
noexcept : _content_length(content_length) { }

inline void HttpTxEncoder::Plain::Write(HttpTransport* const transport,
                                        char chr) {
    if (_content_length == 0)
        throw HttpException("There are more data sent "
                            "that Content-Length specify");
    else --_content_length;
    transport->RawWrite(chr);
}

inline void HttpTxEncoder::Plain::Write(HttpTransport* const transport,
                                      const char* const buffer,
                                      const uint32_t buffer_size) {
    if (buffer_size > _content_length)
        throw HttpException("There are more data sent "
                            "that Content-Length specify");
    else _content_length -= buffer_size;
    transport->RawWrite(buffer, buffer_size);
}

inline void HttpTxEncoder::Plain::Finish(HttpTransport* const transport) {
    /* Check for the content length */
    if (_content_length > 0)
        throw HttpException("There are less data sent "
                            "that Content-Length specify");
}

inline void HttpTxEncoder::Chunked::Write(HttpTransport* const transport,
                                        char chr) {
    transport->RawWrite("1\r\n");
    transport->RawWrite(chr);
    transport->RawWrite("\r\n");
}

inline void HttpTxEncoder::Chunked::Write(HttpTransport* const transport,
                                        const char* const buffer,
                                        const uint32_t buffer_size) {
    /* Check for an empty chunk */
    if (buffer_size == 0) return;

    /* Create a buffer for the chunk size string representation */
    constexpr char hex_alpha[] = "0123456789abcdef";
    char chunk_size[sizeof(uint32_t) * 2];
    char* chunk_size_ptr = chunk_size + sizeof(chunk_size);

    /* Calculate the chunk size */
    {
        uint32_t value = buffer_size;
        do {
            --chunk_size_ptr;
            *chunk_size_ptr = hex_alpha[value & 0xF];
            value >>= 4;
        } while (value > 0);
    }

    /* Send the chunk size */
    transport->RawWrite(chunk_size_ptr,
                        sizeof(chunk_size) - (chunk_size_ptr - chunk_size));
    transport->RawWrite("\r\n");

    /* Send the body */
    transport->RawWrite(buffer, buffer_size);
    transport->RawWrite("\r\n");
}

inline void HttpTxEncoder::Chunked::Finish(HttpTransport* const transport)
{ transport->RawWrite("0\r\n\r\n"); }
