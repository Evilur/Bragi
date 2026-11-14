#pragma once

#include "type/uniq_ptr.h"
#include "util/class.h"
#include "web/dns.h"
#include "web/http_request.h"
#include "web/http_transport.h"

#include <cstdint>

/**
 * A simple http client
 * @author Evilur <the.evilur@gmail.com>
 */
class HttpClient final {
public:
    PREVENT_COPY_AND_MOVE(HttpClient);

    HttpClient(const char* domain,
               const char* path,
               const char* method = "GET") noexcept;

    HttpClient(const HttpRequest& request) noexcept;

    virtual ~HttpClient() noexcept = default;

    HttpRequest& Request() noexcept;

    virtual void Write(const char* buffer, uint32_t buffer_size) noexcept;

    void Flush() noexcept;

protected:
    struct Body {
        PREVENT_COPY_AND_MOVE(Body);

        Body() = default;

        virtual ~Body() = default;

        virtual void Send(HttpTransport& transport,
                          const char* buffer,
                          uint32_t buffer_size) noexcept;

        virtual void Flush(HttpTransport& transport) noexcept;
    };

    virtual uint16_t Port() const noexcept;

    UniqPtr<Body> _request_body = nullptr;

private:
    struct ChunkedBody final : public Body {
        PREVENT_COPY_AND_MOVE(ChunkedBody);

        ChunkedBody() = default;

        void Send(HttpTransport& transport,
                  const char* buffer,
                  uint32_t buffer_size) noexcept override;

        void Flush(HttpTransport& transport) noexcept override;
    };

    HttpRequest _request;
    HttpTransport _transport;
};

inline HttpClient::HttpClient(const char* const domain,
                              const char* const path,
                              const char* const method) noexcept
    : _request(domain, path, method),
    _transport(DNS::Resolve(domain), Port()) { }

inline HttpClient::HttpClient(const HttpRequest& request) noexcept
    : _request(request), _transport(DNS::Resolve(request.domain), Port()) { }

inline HttpRequest& HttpClient::Request() noexcept { return _request; }

inline void HttpClient::Write(const char* const buffer,
                              const uint32_t buffer_size) noexcept {
    /* If we haven't sent the headers (and haven't init the body)
     * Do it now */
    if (_request_body == nullptr) {
        /* Init the request body */
        _request_body = _request.IsChunkedEncoding() ?
                        new ChunkedBody() : new Body();

        /* Send the request headers */
        _request.SendHttp11Headers(_transport);

        /* Reset the Content-Length */
        _request.ResetContentLength();
    }

    /* Send the request body */
    _request_body->Send(_transport, buffer, buffer_size);
}

inline void HttpClient::Flush()
noexcept { _request_body->Flush(_transport); _transport.Flush(); }

inline void HttpClient::Body::Send(HttpTransport& transport,
                                   const char* const buffer,
                                   const uint32_t buffer_size)
noexcept { transport.Send(buffer, buffer_size); }

inline void HttpClient::Body::Flush(HttpTransport& transport) noexcept { }

inline uint16_t HttpClient::Port() const noexcept { return htons(80); }

inline void HttpClient::ChunkedBody::Send(HttpTransport& transport,
                                          const char* const buffer,
                                          const uint32_t buffer_size)
noexcept {
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

inline void HttpClient::ChunkedBody::Flush(HttpTransport& transport)
noexcept { transport.Send("0\r\n\r\n"); }
