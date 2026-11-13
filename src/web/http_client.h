#pragma once

#include "util/class.h"
#include "type/string.h"
#include "web/dns.h"

#include <cstdint>

/**
 * A simple http client
 * @author Evilur <the.evilur@gmail.com>
 */
class HttpClient final {
public:
    PREVENT_COPY_AND_MOVE(HttpClient);

    HttpClient(const char* domain, const char* path) noexcept;

    virtual ~HttpClient() noexcept = default;

protected:
    virtual consteval uint16_t GetPort() const noexcept;

private:
    const String domain;
    const uint32_t addr;
    const String path;
};

inline
HttpClient::HttpClient(const char* const domain,
                       const char* const path) noexcept
    : domain(domain), addr(DNS::Resolve(domain)), path(path) { }

inline consteval uint16_t HttpClient::GetPort() const noexcept {
    return htons(80);
}
