#include "dns.h"

#include "util/logger.hpp"

#include <netdb.h>
#include <stdexcept>

unsigned int DNS::Resolve(const char* hostname) {
    /* Try to resolve the domain name */
    const hostent* const hostent = gethostbyname(hostname);
    if (hostent == nullptr) {
        ERROR_LOG("Failed to resolve domain name: %s", hostname);
        throw std::runtime_error("Failed to resolve domain name");
    }

    /* Print the ip */
    const char* const ip = hostent->h_addr_list[0];
    TRACE_LOG("DNS resolve: %s -> %hhu.%hhu.%hhu.%hhu",
              hostname,
              ip[0],
              ip[1],
              ip[2],
              ip[3]);

    /* Return the result */
    const unsigned int* const result = (unsigned int*)ip;
    return *result;
}