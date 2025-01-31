#include "play.h"
#include "exception/bragi_exception.h"
#include "client/deezer_client.h"
#include "guild//guild_player.h"
#include "util/logger.h"

/* TODO: Sanitize the input data */

void Play::Exec(const dpp::slashcommand_t &event) {
	/* Set the bot to thinking state */
	event.thinking();

	/* Get query from the command parameter */
	std::string query = std::get<std::string>(event.get_parameter("query"));

	/* Send the message to the channel */
	try { event.edit_original_response(Exec(event.command.guild_id, event.command.usr.id, query, event.command.channel_id)); }
	catch (BragiException &exception) { event.edit_original_response(exception.Message()); }
}

void Play::Exec(const dpp::message_create_t &event, const std::string &argument) {
	/* Send the message to the channel */
	try { event.send(Exec(event.msg.guild_id, event.msg.author.id, argument, event.msg.channel_id)); }
	catch (BragiException &exception) { event.send(exception.Message()); }
}

dpp::message Play::Exec(const dpp::snowflake &guild_id, const dpp::snowflake &user_id, const std::string &query, const dpp::snowflake &channel_id) {
	/* If there isn't required parameters */
	if (query.empty()) throw BragiException(DIC_SLASH_NO_PARAMETER, channel_id, BragiException::HARD);

	/* Search the Deezer track */
	Track* track = DeezerClient::Search(query);

	/* If there is no such track */
	if (track == nullptr) throw BragiException(DIC_ERROR_TRACK_NOT_FIND, channel_id, BragiException::SOFT);

	/* Play the track or add it to the playlist */
	return GuildPlayer::Get(guild_id)->PlayCommand(user_id, channel_id, track);
}