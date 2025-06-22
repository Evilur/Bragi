#include "next.h"
#include "master.h"
#include "exception/bragi_exception.h"
#include "util/parser.h"
#include "base/bragi.h"

void Next::Exec(const dpp::slashcommand_t &event) {
	/* Set the bot to thinking state */
	event.thinking();

	/* Get number of tracks for skip (if exists) */
	short track_number = 0;
	dpp::command_value track_num_par = event.get_parameter("number");
	if (track_num_par.index() != 0) track_number = std::get<long>(track_num_par);

	/* Send the _message to the channel */
	try { event.edit_original_response(Bragi::Get(event.command.guild_id)->NextCommand(event.command.channel_id, track_number)); }
	catch (BragiException &exception) { event.edit_original_response(
                exception.GetMessage()); }
}

void Next::Exec(const dpp::message_create_t &event, const std::string &argument) {
	/* Get the track ordinal for researching (if exists) */
	unsigned short track_ordinal = argument.empty() ? 0 : Parser::ToInt16(argument.c_str() + argument.find_first_not_of(' '));

	/* Send the _message to the channel */
	try { event.send(Bragi::Get(event.msg.guild_id)->NextCommand(event.msg.channel_id, track_ordinal)); }
	catch (BragiException &exception) { event.send(exception.GetMessage()); }
}
