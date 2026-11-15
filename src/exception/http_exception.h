#pragma once

#include "exception/exception.h"

class HttpException final : public Exception {
public:
    EXCEPTION_COMMON_CONTSTRUCT(HttpException);

    const char* what() const noexcept override;
};

inline const char* HttpException::what() const
noexcept { return "Http client the exception"; }
