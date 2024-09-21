#ifndef BRAGI_SKIP_H
#define BRAGI_SKIP_H

#include <dpp/dpp.h>

class Skip final {
public:
	static void Exec(const dpp::slashcommand_t &event);

	static void Exec(const dpp::message_create_t &event, const std::string &argument);

private:
	static dpp::message Exec(const dpp::snowflake &guild_id, const dpp::snowflake &channel_id, const int num_for_skip);
};

#endif