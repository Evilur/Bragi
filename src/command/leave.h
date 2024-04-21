#ifndef BRAGI_LEAVE_H
#define BRAGI_LEAVE_H

#include <dpp/dpp.h>

/** Static class for slash/message "leave" command */
class Leave final {
public:
	/** Exec the slash "leave" command
	 * @param event The slash command event reference
	 */
	static void Exec(const dpp::slashcommand_t &event);
	/** Exec the message "leave" command
	 * @param event The new message event reference
	 */
	static void Exec(const dpp::message_create_t &event);
private:
	/** Exec the any type of the command using the necessary data
	 * @param guild_id The snowflake reference with the guild ID
	 * @param channel_id The snowflake reference with the channel ID to write the message
	 * @throws BragiException If cannot exec the command correctly
	 * @return The message object to send to the channel
	 */
	static dpp::message Exec(const dpp::snowflake &guild_id, const dpp::snowflake &channel_id);
};

#endif
