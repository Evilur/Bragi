#pragma once

#include "util/class.h"
#include "util/logger.h"
#include "web/dns_exception.h"

#include <netdb.h>

class DNS final {
public:
    PREVENT_INSTANTIATION(DNS);

    static uint16_t Resolve(const char* hostname);
};

inline uint16_t DNS::Resolve(const char* hostname) {
    /* Try to resolve the domain name */
    const hostent* const hostent = gethostbyname(hostname);
    if (hostent == nullptr) throw DNSException(hostname);

    /* Print the ip */
    TRACE_LOG("DNS resolve: %s -> %hhu.%hhu.%hhu.%hhu",
              hostname,
              hostent->h_addr_list[0][0],
              hostent->h_addr_list[0][1],
              hostent->h_addr_list[0][2],
              hostent->h_addr_list[0][3]);

    /* Return the result */
    return *(unsigned int*)hostent->h_addr_list[0];
}
