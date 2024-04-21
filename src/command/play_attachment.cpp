#include "play_attachment.h"
#include "player/guild_player.h"
#include "exception/bragi_exception.h"
#include "player/attachment_track.h"

void PlayAttachment::Exec(const dpp::slashcommand_t &event) {
	Track* track = new AttachmentTrack(event.command.channel_id, nullptr);
	
	try { event.reply(Exec(event.command.guild_id, event.command.usr.id, event.command.channel_id, track)); }
	catch (BragiException &exception) { event.reply(exception.GetMessage()); }
}

void PlayAttachment::Exec(const dpp::message_create_t &event) {
	Track* track = new AttachmentTrack(event.msg.channel_id, nullptr);
	
	try { event.send(Exec(event.msg.guild_id, event.msg.author.id, event.msg.channel_id, track)); }
	catch (BragiException &exception) { event.send(exception.GetMessage()); }
}

dpp::message PlayAttachment::Exec(const dpp::snowflake &guild_id, const dpp::snowflake &user_id, const dpp::snowflake &channel_id, Track *track) {
	GuildPlayer* guild_player = GuildPlayer::Get(guild_id);
	guild_player->PlayTrack(user_id, channel_id, track);
	return nullptr;
}
