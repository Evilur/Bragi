#pragma once

#include "exception/http_exception.h"
#include "util/class.h"
#include "util/logger.h"
#include "type/string.h"

#include <arpa/inet.h>
#include <cassert>
#include <netdb.h>

class DNS final {
public:
    PREVENT_INSTANTIATION(DNS);

    [[nodiscard]]
    static sockaddr_in Resolve(const char* hostname);
};

inline sockaddr_in DNS::Resolve(const char* const hostname) {
    assert(hostname != nullptr && !String::Equal(hostname, ""));

    /* Setting up hints : 1 entry + IPv4 only */
    addrinfo hints {
        .ai_family = AF_INET,
        .ai_socktype = SOCK_STREAM
    };

    /* Save the result here */
    addrinfo* info;

    /* Perform DNS lookup */
    if (getaddrinfo(hostname, nullptr, &hints, &info) != 0 || !info)
        throw HttpException("Failed to resolve the ip for the '%s' domain",
                            hostname);

    /* Save the result to the stack */
    sockaddr_in addr = *(sockaddr_in*)info->ai_addr;
    TRACE_LOG("DNS resolve: %s -> %s", hostname, inet_ntoa(addr.sin_addr));

    /* Free the memory */
    freeaddrinfo(info);

    /* Return the result */
    return addr;
}
