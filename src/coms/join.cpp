#include "join.h"
#include <coms/error.h>
#include <util/dictionary.h>

void Join::Exec(dpp::cluster &bot, const dpp::slashcommand_t &event) {
	event.reply(Message(bot, event.command.guild_id, event.command.usr.id, event.from, event.command.channel_id));
}

void Join::Exec(dpp::cluster &bot, const dpp::message_create_t &event) {
	event.send(Message(bot, event.msg.guild_id, event.msg.author.id, event.from, event.msg.channel_id));
}

dpp::message Join::Message(dpp::cluster &bot, const dpp::snowflake guild_id, const dpp::snowflake user_id, dpp::discord_client *discord_c, 
						   const dpp::snowflake channel_id) {
	dpp::guild *guild = dpp::find_guild(guild_id);
	dpp::channel *bot_vc = dpp::find_channel(guild->voice_members.find(bot.me.id)->second.channel_id);
	dpp::channel *user_vc = dpp::find_channel(guild->voice_members.find(user_id)->second.channel_id);

	/* If the user isn't in a voice channel */
	if (user_vc == nullptr)
		return Error::Force(DIC_ERROR_USER_NOT_IN_VOICE_CHANNEL, channel_id);

	/* If the user and a bot already in the same channel */
	if (bot_vc != nullptr && bot_vc->id == user_vc->id)
		return Error::Soft(DIC_ERROR_ALREADY_IN_CURRENT_CHANNEL, channel_id);

	/* If bot can not connect to the channel */
	if (!user_vc->get_user_permissions(&bot.me).can(dpp::p_connect))
		return Error::Force(DIC_ERROR_PERMISSION_DENIED, channel_id);

	/* If all is OK */
	discord_c->disconnect_voice(guild_id);	//Disconnect the old voice channel
	guild->connect_member_voice(user_id);  //Connect the new voice channel
	return dpp::message(channel_id, std::format(DIC_JOINED, user_vc->name));
}