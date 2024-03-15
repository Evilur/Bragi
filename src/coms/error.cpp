#include "error.h"
#include <util/color.h>
#include <util/dictionary.h>

dpp::message Error::Force(const char *e_data, const dpp::snowflake channel_id) {
	return dpp::message(channel_id, dpp::embed()
			.set_color(Color::RED)
			.set_title(DIC_ERROR)
			.set_description(e_data));
}

dpp::message Error::Soft(const char *e_data, dpp::snowflake channel_id) {
	return dpp::message(channel_id, dpp::embed()
			.set_color(Color::RED)
			.set_description(e_data));
}
