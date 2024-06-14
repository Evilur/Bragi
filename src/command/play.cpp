#include "play.h"
#include "exception/bragi_exception.h"

void Play::Exec(const dpp::slashcommand_t &event) {
	/* Send message to channel */
	try { event.reply(Exec(event.command.guild_id, event.command.usr.id, "NOT IMPLEMENTED YET", event.command.channel_id)); }
	catch (BragiException &exception) { event.reply(exception.Message()); }
}

void Play::Exec(const dpp::message_create_t &event, const std::string &argument) {
	/* Send message to channel */
	try { event.send(Exec(event.msg.guild_id, event.msg.author.id, argument, event.msg.channel_id)); }
	catch (BragiException &exception) { event.reply(exception.Message()); }
}

dpp::message Play::Exec(const dpp::snowflake &guild_id, const dpp::snowflake &user_id, const std::string &query, const dpp::snowflake &channel_id) {
	return nullptr;
}