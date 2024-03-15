#include "leave.h"
#include "util/dictionary.h"
#include "coms/error.h"

void Leave::Exec(const dpp::slashcommand_t &event) {
	event.reply(Message(event.command.guild_id, event.command.channel_id, event.from));
}

void Leave::Exec(const dpp::message_create_t &event) {
	event.send(Message(event.msg.guild_id, event.msg.channel_id, event.from));
}

dpp::message Leave::Message(const dpp::snowflake guild_id, const dpp::snowflake channel_id, dpp::discord_client *discord_c) {
	/* Get voice connections */
	dpp::voiceconn* voiceconn = discord_c->get_voice(guild_id);
	
	/* If the bot isn't in a voice channel */
	if (voiceconn == nullptr)
		return Error::Soft(DIC_ERROR_BOT_IN_NOT_A_VOICE_CHANNEL, channel_id);
	
	discord_c->disconnect_voice(guild_id);
	return dpp::message(channel_id, DIC_LEFT);
}
