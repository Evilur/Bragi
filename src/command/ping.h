#ifndef BRAGI_PING_H
#define BRAGI_PING_H

#include <dpp/dpp.h>
#include <dpp/dispatcher.h>

/** Static class for slash/message "ping" command */
class Ping final {
public:
	/** Exec the slash "ping" command
	 * @param event The slash command event reference
	 */
	static void Exec(const dpp::slashcommand_t &event);
	/** Exec the message "ping" command
	 * @param event The new message event reference
	 */
	static void Exec(const dpp::message_create_t &event);
private:
	/** Exec the any type of the command using the necessary data
	 * @param channel_id The snowflake reference with the channel ID to write the message
	 * @return The message object to send to the channel
	 */
	static dpp::message Exec(const dpp::snowflake &channel_id);
};


#endif
