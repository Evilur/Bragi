#pragma once

#include "util/color.h"
#include "locale/locale.h"

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

BragiException::BragiException(const char* const error_data,
                               const Type error_type)
    : _message(error_data), _type(error_type) {
}

dpp::message BragiException::GetMessage() const {
    /* Create an embed */
    dpp::embed result = dpp::embed()
                        .set_color(Color::RED)
                        .set_description(_message);

    /* If it is hard error, add a title */
    if (_type == MAJOR)
        result.set_title(_("**Error**"));

    /* Return the result */
    return result;
}
