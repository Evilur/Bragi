#include "join.h"
#include <coms/error.h>
#include <util/dictionary.h>

void Join::Exec(dpp::cluster &bot, const dpp::slashcommand_t &event) {
	event.reply(Message(event.command.guild_id, event.command.usr.id, event.from, event.command.channel_id));
}

void Join::Exec(dpp::cluster &bot, const dpp::message_create_t &event) {
	event.send(Message(event.msg.guild_id, event.msg.author.id, event.from, event.msg.channel_id));
}

dpp::message Join::Message(const dpp::snowflake guild_id, const dpp::snowflake user_id, dpp::discord_client *discord_c, const dpp::snowflake channel_id) {
	dpp::guild *guild = dpp::find_guild(guild_id);
	dpp::voiceconn *bot_vc = discord_c->get_voice(guild_id);
	dpp::channel *user_vc = dpp::find_channel(guild->voice_members.find(user_id)->second.channel_id);

	discord_c->disconnect_voice(guild_id);  //Disconnect the voice channel
	
	/* If the user is not in the voice channel */
	if (!guild->connect_member_voice(user_id))
		return Error::Force(DIC_ERROR_USER_NOT_IN_VOICE_CHANNEL, channel_id);

	return dpp::message(channel_id, std::format(DIC_JOINED, user_vc->name));
}