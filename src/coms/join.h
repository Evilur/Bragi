#ifndef BRAGI_JOIN_H
#define BRAGI_JOIN_H

#include "cluster.h"
#include "dispatcher.h"

class Join {
public:
	static void Exec(dpp::cluster &bot, const dpp::slashcommand_t &event);
	static void Exec(dpp::cluster &bot, const dpp::message_create_t &event);
private:
	static dpp::message Message(dpp::cluster &bot, dpp::snowflake guild_id, dpp::snowflake user_id, dpp::discord_client* discord_c, dpp::snowflake channel_id);
};


#endif
