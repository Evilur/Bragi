#ifndef BRAGI_PLAY_ATTACHMENT_H
#define BRAGI_PLAY_ATTACHMENT_H

#include "player/track.h"

#include <dpp/dpp.h>

/** Static class for slash/message "play-attachment" command */
class PlayAttachment final {
public:
	/** Exec the slash "play-attachment" command
	 * @param event The slash command event reference
	 */
	static void Exec(const dpp::slashcommand_t &event);
	/** Exec the message "play-attachment" command
	 * @param event The new message event reference
	 */
	static void Exec(const dpp::message_create_t &event);
private:
	/** Exec the any type of the command using the necessary data
	 * @param guild_id The snowflake reference with the guild ID
	 * @param user_id The snowflake reference with the user ID to connect to the voice channel
	 * @param channel_id The snowflake reference with the channel ID to write the message
	 * @param track The pointer to the track object to play
	 * @throws BragiException If cannot exec the command correctly
	 * @return The message object to send to the channel
	 */
	static dpp::message Exec(const dpp::snowflake &guild_id, const dpp::snowflake &user_id, const dpp::snowflake &channel_id, Track *track);
};

#endif
