#include "play_attachment.h"
#include "guild//guild_player.h"
#include "util/logger.h"
#include "coms/join.h"
#include <oggz/oggz.h>
#include <unistd.h>

void PlayAttachment::Exec(dpp::cluster &bot, const dpp::slashcommand_t &event) {
	event.reply(Exec(bot, event.command.usr.id, event.command.guild_id, event.command.channel_id, nullptr));
}

void PlayAttachment::Exec(dpp::cluster &bot, const dpp::message_create_t &event) {
	event.send(Exec(bot, event.msg.author.id, event.msg.guild_id, event.msg.channel_id, nullptr));
}

dpp::message PlayAttachment::Exec(dpp::cluster &bot, dpp::snowflake user_id, dpp::snowflake guild_id, dpp::snowflake channel_id,
                                  const Track *track) {
	dpp::message message;
	GuildPlayer::Get(guild_id)->PlayTrack(bot, user_id, channel_id, track, &message);
	return message;
}
