#include "play_attachment.h"
#include "player/guild_player.h"
#include "exception/bragi_exception.h"
#include "player/attachment_track.h"
#include "util/dictionary.h"
#include "util/logger.h"

void PlayAttachment::Exec(const dpp::slashcommand_t &event) {
	dpp::attachment* attachments[10];

	try { event.reply(Exec(event.command.guild_id, event.command.usr.id, event.command.channel_id, attachments, 0)); }
	catch (BragiException &exception) { event.reply(exception.Message()); }
}

void PlayAttachment::Exec(const dpp::message_create_t &event) {
	const unsigned char attachments_count = event.msg.attachments.size();
	dpp::attachment* attachments[10];
	for (unsigned char i = 0; i < attachments_count; i++) *attachments[i] = event.msg.attachments[i];

	try { event.send(Exec(event.msg.guild_id, event.msg.author.id, event.msg.channel_id, attachments, attachments_count)); }
	catch (BragiException &exception) { event.send(exception.Message()); }
}

dpp::message PlayAttachment::Exec(const dpp::snowflake &guild_id, const dpp::snowflake &user_id, const dpp::snowflake &channel_id,
                                  const dpp::attachment* const* const attachments, const unsigned char attachment_count) {
	if (attachment_count == 0) throw BragiException(DIC_ERROR_NO_ATTACHMENTS, channel_id, HARD);
	return GuildPlayer::Get(guild_id)->HandleTrack(user_id, channel_id, new AttachmentTrack(channel_id, attachments[0]));
}