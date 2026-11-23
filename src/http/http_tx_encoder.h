#pragma once

#include "util/class.h"
#include "http/http_transport.h"

#include <cstdint>

class HttpTxEncoder {
public:
    ALLOW_COPY_ALLOW_MOVE(HttpTxEncoder);
    virtual ~HttpTxEncoder() = default;

    virtual void Write(HttpTransport* transport, char chr) const = 0;
    virtual void Write(HttpTransport* transport,
                       const char* buffer,
                       uint32_t buffer_size) const = 0;

    virtual void Finish(HttpTransport* transport) const = 0;

    class Plain;
    class Chunked;

protected:
    HttpTxEncoder() = default;
};

class HttpTxEncoder::Plain final : public HttpTxEncoder {
public:
    PREVENT_COPY_ALLOW_MOVE(Plain);

    void Write(HttpTransport* transport, char chr) const override;
    void Write(HttpTransport* transport,
               const char* buffer,
               uint32_t buffer_size) const override;

    void Finish(HttpTransport* transport) const noexcept override;

private:
    Plain() noexcept = default;

public:
    static Plain& Instance() noexcept;
};

class HttpTxEncoder::Chunked final : public HttpTxEncoder {
public:
    PREVENT_COPY_ALLOW_MOVE(Chunked);

    void Write(HttpTransport* transport, char chr) const override;
    void Write(HttpTransport* transport,
               const char* buffer,
               uint32_t buffer_size) const override;

    void Finish(HttpTransport* transport) const override;

private:
    Chunked() noexcept = default;

public:
    static Chunked& Instance() noexcept;
};

inline void HttpTxEncoder::Plain::Write(HttpTransport* const transport,
                                        char chr)
const { transport->RawWrite(chr); }

inline void HttpTxEncoder::Plain::Write(HttpTransport* const transport,
                                      const char* const buffer,
                                      const uint32_t buffer_size)
const { transport->RawWrite(buffer, buffer_size); }

inline void HttpTxEncoder::Plain::Finish(HttpTransport* const transport)
const noexcept { }

inline HttpTxEncoder::Plain& HttpTxEncoder::Plain::Instance() noexcept {
    static Plain instance;
    return instance;
}

inline void HttpTxEncoder::Chunked::Write(HttpTransport* const transport,
                                        char chr) const {
    transport->RawWrite("1\r\n");
    transport->RawWrite(chr);
    transport->RawWrite("\r\n");
}

inline void HttpTxEncoder::Chunked::Write(HttpTransport* const transport,
                                        const char* const buffer,
                                        const uint32_t buffer_size) const {
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
const { transport->RawWrite("0\r\n\r\n"); }

inline HttpTxEncoder::Chunked& HttpTxEncoder::Chunked::Instance() noexcept {
    static Chunked instance;
    return instance;
}
