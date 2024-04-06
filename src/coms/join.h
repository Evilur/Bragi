#ifndef BRAGI_JOIN_H
#define BRAGI_JOIN_H

#include <dpp/cluster.h>
#include <dpp/dispatcher.h>
#include <coroutine>

class Join {
public:
	static void Exec(dpp::cluster &bot, const dpp::slashcommand_t &event);
	static void Exec(dpp::cluster &bot, const dpp::message_create_t &event);
	
	/* return the message with status of the executing
	 * throw a BragiException */
	static dpp::message Exec(dpp::cluster &bot, dpp::snowflake guild_id, dpp::snowflake user_id, dpp::snowflake channel_id);
};


#endif
