#pragma once

#include "exception/exception.h"

class SocketException final : public Exception {
public:
    EXCEPTION_COMMON_CONTSTRUCT(SocketException);

    [[nodiscard]]
    const char* what() const noexcept override;
};

inline const char* SocketException::what() const
noexcept { return "Socket occur the exception"; }
