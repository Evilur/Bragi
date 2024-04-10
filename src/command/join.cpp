#include "join.h"
#include "master.h"
#include "util/bragi_exception.h"
#include "player/guild_player.h"
#include "util/dictionary.h"

void Join::Exec(dpp::cluster &bot, const dpp::slashcommand_t &event) {
	/* Default user for connection */
	dpp::snowflake user_id = event.command.usr.id;
	
	/* Get user from the command parameter (if exists) */
	dpp::command_value user_par = event.get_parameter("user");
	if (user_par.index() != 0) user_id = event.command.get_resolved_user(std::get<dpp::snowflake>(user_par)).id;
	
	/* Send message to channel */
	try { event.reply(Exec(bot, event.command.guild_id, user_id, event.command.channel_id));	} 
	catch (BragiException &exception) { event.reply(exception.GetMessage()); }
}

void Join::Exec(dpp::cluster &bot, const dpp::message_create_t &event) {
	/* Default user for connection */
	dpp::snowflake user_id = event.msg.author.id;

	/* Get user from the message content (if exists) */
	if (!event.msg.mentions.empty()) user_id = event.msg.mentions.data()->second.user_id;

	/* Send message to channel */
	try { event.send(Exec(bot, event.msg.guild_id, user_id, event.msg.channel_id)); } 
	catch (BragiException &exception) { event.send(exception.GetMessage()); }
}

dpp::message Join::Exec(dpp::cluster &bot, const dpp::snowflake &guild_id, const dpp::snowflake &user_id, const dpp::snowflake &channel_id) {
	return GuildPlayer::Get(guild_id)->Join(bot, user_id, channel_id);
}