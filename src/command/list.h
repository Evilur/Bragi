#ifndef BRAGI_LIST_H
#define BRAGI_LIST_H

#include <dpp/dpp.h>

class List final {
public:
	static void Exec(const dpp::slashcommand_t &event);

	static void Exec(const dpp::message_create_t &event);

private:
	static dpp::message Exec(const dpp::snowflake &guild_id, const dpp::snowflake &channel_id);
};

#endif