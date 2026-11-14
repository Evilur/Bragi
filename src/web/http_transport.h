#pragma once

#include "util/class.h"

#include <cstdint>
#include <cstdio>

/**
 * A transport level for the http client
 * @author Evilur <the.evilur@gmail.com>
 */
class HttpTransport final {
public:
    PREVENT_COPY_AND_MOVE(HttpTransport);

    HttpTransport(uint32_t addr, uint16_t port) noexcept;

    void Send(const char* buffer, uint32_t buffer_size) noexcept;

    void Flush() noexcept;

private:
    const uint32_t _addr;
    const uint16_t _port;
};

inline HttpTransport::HttpTransport(const uint32_t addr,
                                    const uint16_t port) noexcept
    : _addr(addr), _port(port) { }

inline void HttpTransport::Send(const char* const buffer,
                                const uint32_t buffer_size) noexcept {
    fwrite(buffer, 1, buffer_size, stdout);
}

inline void HttpTransport::Flush() noexcept {
    fflush(stdout);
}
