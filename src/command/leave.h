#ifndef BRAGI_LEAVE_H
#define BRAGI_LEAVE_H

#include <dpp/cluster.h>

class Leave {
public:
	static void Exec(const dpp::slashcommand_t &event);
	static void Exec(const dpp::message_create_t &event);
private:
	/* return the message with status of the executing
	 * throw a BragiException */
	static dpp::message Exec(const dpp::snowflake &guild_id, const dpp::snowflake &channel_id);
};

#endif
