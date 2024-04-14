#include "play_attachment.h"
#include "player/guild_player.h"
#include "exception/bragi_exception.h"
#include "player/attachment_track.h"
#include "util/logger.h"
#include "join.h"

void PlayAttachment::Exec(dpp::cluster &bot, const dpp::slashcommand_t &event) {
	
	
	return;
	try { event.reply(Exec(bot, event.command.usr.id, event.command.guild_id, event.command.channel_id, nullptr)); }
	catch (BragiException &exception) { event.reply(exception.GetMessage()); }
}

void PlayAttachment::Exec(dpp::cluster &bot, const dpp::message_create_t &event) {
	const dpp::attachment* attachment = event.msg.attachments.data();
	Track* track = nullptr;
	if (attachment != nullptr) track = new AttachmentTrack(event.msg.channel_id, attachment);
	
	try { event.send(Exec(bot, event.msg.author.id, event.msg.guild_id, event.msg.channel_id, track)); }
	catch (BragiException &exception) { event.send(exception.GetMessage()); }
}

dpp::message PlayAttachment::Exec(
		dpp::cluster &bot, const dpp::snowflake &user_id, const dpp::snowflake &guild_id, const dpp::snowflake &channel_id, Track *track) {
	dpp::message message;
	GuildPlayer* guild_player = GuildPlayer::Get(guild_id);
	guild_player->PlayTrack(bot, user_id, channel_id, track);
	return message;
}
