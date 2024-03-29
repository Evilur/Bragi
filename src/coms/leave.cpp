#include "leave.h"
#include "master.h"
#include "util/dictionary.h"
#include "coms/error.h"

void Leave::Exec(const dpp::slashcommand_t &event) {
	event.reply(Message(event.command.guild_id, event.command.channel_id));
}

void Leave::Exec(const dpp::message_create_t &event) {
	event.send(Message(event.msg.guild_id, event.msg.channel_id));
}

dpp::message Leave::Message(const dpp::snowflake guild_id, const dpp::snowflake channel_id) {
	/* Get voice connections */
	dpp::voiceconn* voiceconn = ds_client->get_voice(guild_id);
	
	/* If the bot isn't in a voice channel */
	if (voiceconn == nullptr)
		return Error::Soft(DIC_ERROR_BOT_IN_NOT_A_VOICE_CHANNEL, channel_id);

	ds_client->disconnect_voice(guild_id);
	return dpp::message(channel_id, DIC_LEFT);
}
