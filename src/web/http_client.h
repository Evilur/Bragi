#pragma once

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

protected:
    virtual uint16_t Port() const noexcept;

private:
    HttpRequest _request;
    HttpTransport _transport;
};

inline HttpClient::HttpClient(const char* const domain,
                              const char* const path,
                              const char* const method) noexcept
    : _request(domain, path, method),
    _transport(DNS::Resolve(domain), Port()) { }

inline HttpClient::HttpClient(const HttpRequest& request) noexcept
    : _request(request),
    _transport(DNS::Resolve(request.domain), Port()) { }

inline HttpRequest& HttpClient::Request() noexcept { return _request; }

inline uint16_t HttpClient::Port() const noexcept { return htons(80); }
