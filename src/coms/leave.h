#ifndef BRAGI_LEAVE_H
#define BRAGI_LEAVE_H

#include <dpp/cluster.h>

class Leave {
public:
	static void Exec(const dpp::slashcommand_t &event);
	static void Exec(const dpp::message_create_t &event);
private:
	static dpp::message Message(dpp::snowflake guild_id, dpp::snowflake channel_id, dpp::discord_client *discord_c);
};

#endif
