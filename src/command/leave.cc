#include "leave.h"
#include "base/bragi.h"
#include "exception/bragi_exception.h"

void Leave::Exec(const dpp::slashcommand_t &event) {
	try { event.reply(Exec(event.command.guild_id, event.command.channel_id)); }
	catch (BragiException &exception) { event.reply(exception.Message()); }
}

void Leave::Exec(const dpp::message_create_t &event) {
	try { event.send(Exec(event.msg.guild_id, event.msg.channel_id)); }
	catch (BragiException &exception) { event.send(exception.Message()); }
}

dpp::message Leave::Exec(const dpp::snowflake &guild_id, const dpp::snowflake &channel_id) {
	return dpp::message(channel_id, GuildPlayer::Get(guild_id)->Leave(channel_id));
}
