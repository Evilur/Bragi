#include "loop.h"
#include "util/logger.h"

void Loop::Exec(const dpp::slashcommand_t &event) {
	/* Get the loop type slash command parameter */
	dpp::command_value loop_type_par = event.get_parameter("type");

	/* If there is no parameters */
	if (loop_type_par.index() == 0) {
		event.reply(GuildPlayer::Get(event.command.guild_id)->Loop(event.command.channel_id));
		return;
	}

	/* Get the loop type in string */
	const std::string loop_type_str = std::get<std::string>(loop_type_par);

	/* Get the loop type */
	GuildPlayer::LoopType loop_type;
	if (loop_type_str == "loop_track") loop_type = GuildPlayer::LoopType::TRACK;
	else if (loop_type_str == "loop_playlist") loop_type = GuildPlayer::LoopType::PLAYLIST;
	else loop_type = GuildPlayer::LoopType::DISABLED;

	/* Send the message to the channel */
	event.reply(GuildPlayer::Get(event.command.guild_id)->Loop(event.command.channel_id, loop_type));
}

void Loop::Exec(const dpp::message_create_t &event, const std::string &argument) {
	/* TODO: use argument */
	/* Send the message to the channel */
	event.send(GuildPlayer::Get(event.msg.guild_id)->Loop(event.msg.channel_id));
}