#include "play.h"
#include "exception/bragi_exception.h"
#include "client/deezer_client.h"
#include "player/guild_player.h"
#include "util/logger.h"

void Play::Exec(const dpp::slashcommand_t &event) {
	/* Get query from the command parameter */
	std::string query = std::get<std::string>(event.get_parameter("query"));

	/* Send the message to the channel */
	try { event.reply(Exec(event.command.guild_id, event.command.usr.id, query, event.command.channel_id)); }
	catch (BragiException &exception) { event.reply(exception.Message()); }
}

void Play::Exec(const dpp::message_create_t &event, const std::string &argument) {
	/* Send the message to the channel */
	try { event.send(Exec(event.msg.guild_id, event.msg.author.id, argument, event.msg.channel_id)); }
	catch (BragiException &exception) { event.send(exception.Message()); }
}

dpp::message Play::Exec(const dpp::snowflake &guild_id, const dpp::snowflake &user_id, const std::string &query, const dpp::snowflake &channel_id) {
	/* If there isn't required parameters */
	if (query.empty()) throw BragiException(DIC_SLASH_NO_PARAMETER, channel_id, HARD);

	/* Search the Deezer track */
	Track* track = DeezerClient::Search(query);

	/* If there is no such track */
	if (track == nullptr) throw BragiException(DIC_ERROR_TRACK_NOT_FIND, channel_id, SOFT);

	/* Play the track or add it to the playlist */
	return GuildPlayer::Get(guild_id)->HandleTrack(user_id, channel_id, track);
}