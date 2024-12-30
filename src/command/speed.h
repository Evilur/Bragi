#ifndef BRAGI_SPEED_H
#define BRAGI_SPEED_H

#include "master.h"

#include <dpp/dpp.h>

class Speed final {
public:
	static void Exec(const dpp::slashcommand_t &event);

	static void Exec(const dpp::message_create_t &event, const std::string &argument);

private:
	static dpp::message Exec(const dpp::snowflake &guild_id, const dpp::snowflake &channel_id, const long speed_percent);
};

#endif