#ifndef BRAGI_BRAGI_EXCEPTION_H
#define BRAGI_BRAGI_EXCEPTION_H

#include <dpp/dpp.h>

class BragiException final : std::exception {
public:
    enum Type { MINOR, MAJOR };

    BragiException(const char* error_data,
                   Type error_type);

    dpp::message GetMessage() const;

private:
    const char* const _message;
    const Type _type;
};

#endif