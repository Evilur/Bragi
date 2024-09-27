#include "next.h"
#include "master.h"
#include "exception/bragi_exception.h"
#include "util/parser.h"
#include "player/guild_player.h"

void Next::Exec(const dpp::slashcommand_t &event) {
	/* Set the bot to thinking state */
	event.thinking();

	/* Get number of tracks for skip (if exists) */
	/*int16 num_for_skip = 1;
	dpp::command_value num_for_skip_par = event.get_parameter("num");
	if (num_for_skip_par.index() != 0) num_for_skip = std::get<int64>(num_for_skip_par);*/

	/* Send the message to the channel */
	try { event.edit_original_response(GuildPlayer::Get(event.command.guild_id)->Next(event.command.channel_id, 0)); }
	catch (BragiException &exception) { event.edit_original_response(exception.Message()); }
}

void Next::Exec(const dpp::message_create_t &event, const std::string &argument) {
	/* Get the track ordinal for researching (if exists) */
	u_int16 track_ordinal = argument.empty() ? 0 : Parser::ToInt16(argument.c_str() + argument.find_first_not_of(' '));

	/* Send the message to the channel */
	try { event.send(GuildPlayer::Get(event.msg.guild_id)->Next(event.msg.channel_id, track_ordinal)); }
	catch (BragiException &exception) { event.send(exception.Message()); }
}