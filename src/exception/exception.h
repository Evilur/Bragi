#pragma once

#include "util/class_traits.h"
#include "util/logger.h"

#include <exception>

#define EXCEPTION_COMMON_CONTSTRUCT(T)                                        \
template <typename... Args>                                                   \
T(const char* format, Args... args)                                           \
noexcept : Exception(format, args...) { }

/**
 * A base exception class
 * @author Evilur <the.evilur@gmail.com>
 */
class Exception : public std::exception {
public:
    ALLOW_COPY_ALLOW_MOVE(Exception);

    Exception() noexcept = delete;

    template <typename... Args>
    Exception(const char* format, Args... args) noexcept;

    virtual ~Exception() noexcept = default;

    [[nodiscard]]
    virtual const char* what() const noexcept override = 0;
};

template <typename... Args>
inline Exception::Exception(const char* const format, Args... args)
noexcept { ERROR_LOG(format, args...); }
