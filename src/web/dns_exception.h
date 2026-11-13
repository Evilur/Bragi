#pragma once

#include "util/logger.h"

#include <exception>

/**
 * An exception of the http client
 * @author Evilur <the.evilur@gmail.com>
 */
class DNSException final : std::exception {
public:
    DNSException(const char* hostname) noexcept;
};

inline
DNSException::DNSException(const char* const hostname) noexcept {
    ERROR_LOG("Failed to resolve the domain name: %s", hostname);
}
