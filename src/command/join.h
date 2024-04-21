#ifndef BRAGI_JOIN_H
#define BRAGI_JOIN_H

#include <dpp/dpp.h>
#include <dpp/dispatcher.h>

/**
 * Static class for slash/message "join" command
 */
class Join final {
public:
	/**
	 * Exec the slash "join" command
	 * @param event A slash event reference
	 */
	static void Exec(const dpp::slashcommand_t &event);
	static void Exec(const dpp::message_create_t &event);
	
	/* return the message with status of the executing
	 * throw a BragiException */
	static dpp::message Exec(const dpp::snowflake &guild_id, const dpp::snowflake &user_id, const dpp::snowflake &channel_id);
};


#endif
