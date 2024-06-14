#ifndef BRAGI_PLAY_H
#define BRAGI_PLAY_H

#include <dpp/dpp.h>

class Play final {
public:
	static void Exec(const dpp::slashcommand_t &event);

	static void Exec(const dpp::message_create_t &event, const std::string &argument);

private:
	static dpp::message Exec(const dpp::snowflake &guild_id, const dpp::snowflake &user_id, const std::string &query, const dpp::snowflake &channel_id);
};

#endif