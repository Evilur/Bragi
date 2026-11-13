#pragma once

#include "util/class.h"

#include <cstdint>

/**
 * A transport level for the http client
 * @author Evilur <the.evilur@gmail.com>
 */
class HttpTransport final {
public:
    PREVENT_COPY_AND_MOVE(HttpTransport);

    HttpTransport(uint32_t addr, uint16_t port) noexcept;

private:
    const uint32_t _addr;
    const uint16_t _port;
};

inline HttpTransport::HttpTransport(const uint32_t addr,
                                    const uint16_t port) noexcept
    : _addr(addr), _port(port) { }
