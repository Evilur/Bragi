#pragma once

#include "util/class.h"
#include "web/http_transport.h"

#include <cstdint>

class HttpEncoder {
public:
    ALLOW_COPY_ALLOW_MOVE(HttpEncoder);
    virtual ~HttpEncoder() = default;

    virtual void Send(HttpTransport& transport,
                      const char* buffer,
                      uint32_t buffer_size) noexcept = 0;

    virtual void Finish(HttpTransport& transport) noexcept = 0;

    class Plain;
    class Chunked;

protected:
    HttpEncoder() = default;
};

class HttpEncoder::Plain final : public HttpEncoder {
public:
    PREVENT_COPY_ALLOW_MOVE(Plain);

    void Send(HttpTransport& transport,
              const char* buffer,
              uint32_t buffer_size) noexcept override;

    void Finish(HttpTransport& transport) noexcept override;

private:
    Plain() = default;

public:
    static Plain& Instance() noexcept;
};

class HttpEncoder::Chunked final : public HttpEncoder {
public:
    PREVENT_COPY_ALLOW_MOVE(Chunked);

    void Send(HttpTransport& transport,
              const char* buffer,
              uint32_t buffer_size) noexcept override;

    void Finish(HttpTransport& transport) noexcept override;

private:
    Chunked() = default;

public:
    static Chunked& Instance() noexcept;
};

inline void HttpEncoder::Plain::Send(HttpTransport& transport,
                                     const char* const buffer,
                                     const uint32_t buffer_size)
noexcept { transport.Send(buffer, buffer_size); }

inline void HttpEncoder::Plain::Finish(HttpTransport& transport) noexcept { }

inline HttpEncoder::Plain& HttpEncoder::Plain::Instance() noexcept {
    static Plain instance;
    return instance;
}

inline void HttpEncoder::Chunked::Send(HttpTransport& transport,
                                       const char* const buffer,
                                       const uint32_t buffer_size) noexcept {
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
    transport.Send(chunk_size_ptr,
                   sizeof(chunk_size) - (chunk_size_ptr - chunk_size));
    transport.Send("\r\n");

    /* Send the body */
    transport.Send(buffer, buffer_size);
    transport.Send("\r\n");
}

inline void
HttpEncoder::Chunked::Finish(HttpTransport& transport)
noexcept { transport.Send("0\r\n\r\n");  }

inline HttpEncoder::Chunked& HttpEncoder::Chunked::Instance() noexcept {
    static Chunked instance;
    return instance;
}
