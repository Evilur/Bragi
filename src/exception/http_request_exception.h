#pragma once

#include <exception>

/**
 * A wrapper for the std::exception for http requests
 * @author Evilur <the.evilur@gmail.com>
 */
class HttpRequestException final : std::exception {
public:
    HttpRequestException() noexcept = default;
};
