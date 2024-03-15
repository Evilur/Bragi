#include "leave.h"
#include "util/dictionary.h"
#include "coms/error.h"

void Leave::Exec(dpp::cluster &bot, const dpp::slashcommand_t &event) {
	event.reply(Message(bot, event.command.guild_id, event.command.channel_id, event.from));
}

void Leave::Exec(dpp::cluster &bot, const dpp::message_create_t &event) {
	event.send(Message(bot, event.msg.guild_id, event.msg.channel_id, event.from));
}

dpp::message Leave::Message(dpp::cluster &bot, const dpp::snowflake guild_id, const dpp::snowflake channel_id, dpp::discord_client *discord_c) {
	/* Get voice connections */
	dpp::guild* guild = dpp::find_guild(guild_id);
	dpp::channel* bot_vc = dpp::find_channel(guild->voice_members.find(bot.me.id)->second.channel_id);
	dpp::voiceconn* voiceconn = discord_c->get_voice(guild_id);
	
	/* If the bot isn't in a voice channel */
	if (bot_vc == nullptr)
		return Error::Soft(DIC_ERROR_BOT_IN_NOT_A_VOICE_CHANNEL, channel_id);
	
	/* If the bot has no voice connection we need to get a connection at first */
	if (voiceconn == nullptr)
		std::cout << "NEED TO JOIN THE CHANNEL\n";
	
	discord_c->disconnect_voice(guild_id);
	return dpp::message(channel_id, DIC_LEFT);
}
