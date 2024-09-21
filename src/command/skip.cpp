#include "skip.h"
#include "exception/bragi_exception.h"
#include "player/guild_player.h"

void Skip::Exec(const dpp::slashcommand_t &event) {
	/* Send the message to the channel */
	try { event.reply(Exec(event.command.guild_id, event.command.channel_id, 1)); }
	catch (BragiException &exception) { event.reply(exception.Message()); }
}

void Skip::Exec(const dpp::message_create_t &event, const std::string &argument) {
	/* Send the message to the channel */
	try { event.reply(Exec(event.msg.guild_id, event.msg.channel_id, 1)); }
	catch (BragiException &exception) { event.reply(exception.Message()); }
}

dpp::message Skip::Exec(const dpp::snowflake &guild_id, const dpp::snowflake &channel_id, const int num_for_skip) {
	return GuildPlayer::Get(guild_id)->Skip(num_for_skip, channel_id);
}