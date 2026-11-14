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

    virtual void Send(const char* buffer, uint32_t buffer_size) noexcept;

    void Finish() noexcept;

protected:
    virtual uint16_t Port() const noexcept;

private:
    HttpRequest _request;
    UniqPtr<HttpTransport> _transport = nullptr;
};

inline HttpClient::HttpClient(const char* const domain,
                              const char* const path,
                              const char* const method) noexcept
    : _request(domain, path, method) { }

inline HttpClient::HttpClient(const HttpRequest& request) noexcept
    : _request(request) { }

inline HttpRequest& HttpClient::Request() noexcept { return _request; }

inline void HttpClient::Send(const char* const buffer,
                             const uint32_t buffer_size) noexcept {
    /* If we haven't sent the headers yet (not initialized transport)
     * Do it now */
    if (_transport == nullptr) {
        /* Init the transport */
        _transport = new HttpTransport(
            DNS::Resolve(_request.domain),
            Port(),
            HttpTransport::PLAIN_BODY
        );

        /* Send the request headers */
        _request.SendHttp11Headers(*_transport);

        /* Change the encoding (if we need it) */
        if (_request.IsChunkedEncoding())
            _transport->SetEncoding(HttpTransport::CHUNKED);

        /* Else reset the Content-Length */
        else _request.ResetContentLength();
    }

    /* Send the body */
    _transport->Send(buffer, buffer_size);
}

inline void HttpClient::Finish()
noexcept {
    /* If we haven't sent the headers yet (not initialized transport)
     * Do it now */
    if (_transport == nullptr) {
        /* Init the transport */
        _transport = new HttpTransport(
            DNS::Resolve(_request.domain),
            Port(),
            HttpTransport::PLAIN_BODY
        );

        /* Set the 'Content-Length' to 0 and send the request headers */
        _request.SetContentLength(0);
        _request.SendHttp11Headers(*_transport);

        /* Reset the Content-Length */
        _request.ResetContentLength();
    }

    /* Finish the request */
    _transport->Finish();
}

inline uint16_t HttpClient::Port() const noexcept { return htons(80); }
