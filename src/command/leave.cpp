#include "leave.h"
#include "master.h"
#include "player/guild_player.h"
#include "util/dictionary.h"
#include "util/bragi_exception.h"

void Leave::Exec(const dpp::slashcommand_t &event) {
	try { event.reply(Exec(event.command.guild_id, event.command.channel_id)); }
	catch (BragiException &exception) { event.reply(exception.GetMessage()); }
}

void Leave::Exec(const dpp::message_create_t &event) {
	try { event.send(Exec(event.msg.guild_id, event.msg.channel_id)); } 
	catch (BragiException &exception) { event.send(exception.GetMessage()); }
}

dpp::message Leave::Exec(const dpp::snowflake &guild_id, const dpp::snowflake &channel_id) {
	return GuildPlayer::Get(guild_id)->Leave(channel_id);
}