#include "join.h"
#include "master.h"
#include "util/bragi_exception.h"
#include "guild/guild_player.h"
#include "util/dictionary.h"

void Join::Exec(dpp::cluster &bot, const dpp::slashcommand_t &event) {
	/* Default user for connection */
	dpp::snowflake user_id = event.command.usr.id;
	
	/* Get user from the command parameter (if exists) */
	dpp::command_value user_par = event.get_parameter("user");
	if (user_par.index() != 0) user_id = event.command.get_resolved_user(std::get<dpp::snowflake>(user_par)).id;
	
	/* Send message to channel */
	try { event.reply(Exec(bot, event.command.guild_id, user_id, event.command.channel_id));	} 
	catch (BragiException& exception) { event.reply(exception.GetMessage()); }
}

void Join::Exec(dpp::cluster &bot, const dpp::message_create_t &event) {
	/* Default user for connection */
	dpp::snowflake user_id = event.msg.author.id;

	/* Get user from the message content (if exists) */
	if (!event.msg.mentions.empty()) user_id = event.msg.mentions.data()->second.user_id;

	/* Send message to channel */
	try { event.send(Exec(bot, event.msg.guild_id, user_id, event.msg.channel_id)); } 
	catch (BragiException& exception) { event.send(exception.GetMessage()); }
}

dpp::message Join::Exec(dpp::cluster &bot, dpp::snowflake guild_id, dpp::snowflake user_id, dpp::snowflake channel_id) {
	/* Get voice channels */
	dpp::guild* guild = dpp::find_guild(guild_id);
	dpp::channel* bot_vc = dpp::find_channel(guild->voice_members.find(bot.me.id)->second.channel_id);
	dpp::channel* user_vc = dpp::find_channel(guild->voice_members.find(user_id)->second.channel_id);
	
	/* Check bot for ready */
	GuildPlayer* guild_player = GuildPlayer::Get(guild_id);
	bool is_ready = guild_player->IsPLayerReady();
	
	/* If the user isn't in a voice channel */
	if (user_vc == nullptr)
		throw BragiException(DIC_ERROR_USER_NOT_IN_VOICE_CHANNEL, channel_id, ErrorType::Hard);

	/* If the user and a bot already in the same channel */
	if (is_ready && bot_vc != nullptr && bot_vc->id == user_vc->id)
		throw BragiException(DIC_ERROR_ALREADY_IN_CURRENT_CHANNEL, channel_id, ErrorType::Soft);
	
	/* If bot can not connect to the channel or speak there */
	if (!user_vc->get_user_permissions(&bot.me).can(dpp::p_connect) || !user_vc->get_user_permissions(&bot.me).can(dpp::p_speak))
		throw BragiException(DIC_ERROR_PERMISSION_DENIED, channel_id, ErrorType::Hard);
	
	/* If bot in the voice channel we need to disconnect */
	if (bot_vc != nullptr) ds_client->disconnect_voice(guild_id);

	/* If all is OK */
	guild_player->ConnectVoice(user_vc->id); //Connect the new voice channel
	return dpp::message(channel_id, std::format(DIC_JOINED, user_vc->name));
}