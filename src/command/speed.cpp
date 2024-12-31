#include "speed.h"
#include "player/guild_player.h"
#include "exception/bragi_exception.h"
#include "util/parser.h"
#include "util/dictionary.h"

void Speed::Exec(const dpp::slashcommand_t &event) {
	/* Get the track speed in percents */
	long speed_percent = 100;
	dpp::command_value speed_percent_par = event.get_parameter("percent");
	if (speed_percent_par.index() != 0) speed_percent = std::get<long>(speed_percent_par);

	/* Reply the slash command */
	try { event.reply(Exec(event.command.guild_id, event.command.channel_id, speed_percent)); }
	catch (BragiException &exception) { event.reply(exception.Message()); }
}

void Speed::Exec(const dpp::message_create_t &event, const std::string &argument) {
	/* Get number of tracks for skip (if exists) */
	const long speed_percent = argument.empty() ? 100 : Parser::ToInt64(argument.c_str() + argument.find_first_not_of(' '));

	/* Send the output message to the channel */
	try { event.send(Exec(event.msg.guild_id, event.msg.channel_id, speed_percent)); }
	catch (BragiException &exception) { event.send(exception.Message()); }
}

dpp::message Speed::Exec(const dpp::snowflake &guild_id, const dpp::snowflake &channel_id, const long speed_percent) {
	if (speed_percent < 25) throw BragiException(DIC_SLASH_SPEED_LESS_THAN, channel_id, SOFT);
	else if (speed_percent > 250) throw BragiException(DIC_SLASH_SPEED_MORE_THAN, channel_id, SOFT);
	return GuildPlayer::Get(guild_id)->SetSpeed(channel_id, (byte)speed_percent);
}