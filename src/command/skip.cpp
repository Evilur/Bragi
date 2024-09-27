#include "skip.h"
#include "exception/bragi_exception.h"
#include "player/guild_player.h"
#include "util/logger.h"
#include "util/parser.h"

void Skip::Exec(const dpp::slashcommand_t &event) {
	/* Get number of tracks for skip (if exists) */
	u_int16 num_for_skip = 1;
	dpp::command_value num_for_skip_par = event.get_parameter("num");
	if (num_for_skip_par.index() != 0) num_for_skip = std::get<int64>(num_for_skip_par);

	/* Send the message to the channel */
	try { event.reply(Exec(event.command.guild_id, event.command.channel_id, num_for_skip)); }
	catch (BragiException &exception) { event.reply(exception.Message()); }
}

void Skip::Exec(const dpp::message_create_t &event, const std::string &argument) {
	/* Get number of tracks for skip (if exists) */
	u_int16 num_for_skip = argument.empty() ? 1 : Parser::ToUInt16(argument.c_str() + argument.find_first_not_of(' '));

	/* Send the message to the channel */
	try { event.send(Exec(event.msg.guild_id, event.msg.channel_id, num_for_skip)); }
	catch (BragiException &exception) { event.send(exception.Message()); }
}

dpp::message Skip::Exec(const dpp::snowflake &guild_id, const dpp::snowflake &channel_id, const u_int16 num_for_skip) {
	return GuildPlayer::Get(guild_id)->Skip(channel_id, num_for_skip);
}