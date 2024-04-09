#include "leave.h"
#include "master.h"
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
	/* Get voice connections */
	dpp::voiceconn* voiceconn = ds_client->get_voice(guild_id);
	
	/* If the bot isn't in a voice channel */
	if (voiceconn == nullptr)
		throw BragiException(DIC_ERROR_BOT_IN_NOT_A_VOICE_CHANNEL, channel_id, ErrorType::Soft);

	ds_client->disconnect_voice(guild_id);
	return dpp::message(channel_id, DIC_LEFT);
}
