#pragma once

#include <exception>

/**
 * A wrapper for the std::exception for sockets
 * @author Evilur <the.evilur@gmail.com>
 */
class SocketException final : std::exception {
public:
    SocketException() noexcept = default;
};
