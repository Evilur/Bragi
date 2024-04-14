#ifndef BRAGI_PING_H
#define BRAGI_PING_H

#include <dpp/dpp.h>
#include <dpp/dispatcher.h>

class Ping {
public:
	static void Exec(dpp::cluster &bot, const dpp::slashcommand_t &event);
	static void Exec(dpp::cluster &bot, const dpp::message_create_t &event);
private:
	/* return the message with status of the executing */
	static dpp::message Exec(dpp::cluster &bot, const dpp::snowflake &channel_id);
};


#endif
