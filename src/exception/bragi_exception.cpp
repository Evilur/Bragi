#include "bragi_exception.h"
#include "util/color.h"
#include "locale/locale.h"

BragiException::BragiException(const char* const message, const Type type)
		: _message(message), _type(type) { }

dpp::message BragiException::GetMessage() const {
    /* Create an embed */
    dpp::embed result = dpp::embed()
            .set_color(Color::RED)
            .set_description(_message);

    /* If it is hard error, add a title */
    if (_type == HARD)
        result.set_title(_("**Error**"));

    /* Return the result */
    return result;
}
