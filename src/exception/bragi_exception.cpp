#include "bragi_exception.h"
#include "util/color.h"
#include "util/dictionary.h"

BragiException::BragiException(const char* e_data, const dpp::snowflake &channel_id, const ErrorType e_type)
													: _e_data(e_data), _channel_id(channel_id), _e_type(e_type) { }

dpp::message BragiException::GetMessage() {
	if (_e_type == ErrorType::HARD) return ForceError();
	else return WeakError();
}
													
dpp::message BragiException::ForceError() {
	return dpp::message(_channel_id, dpp::embed()
			.set_color(Color::RED)
			.set_title(DIC_ERROR)
			.set_description(_e_data));
}

dpp::message BragiException::WeakError() {
	return dpp::message(_channel_id, dpp::embed()
			.set_color(Color::RED)
			.set_description(_e_data));
}
