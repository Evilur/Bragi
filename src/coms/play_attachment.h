#ifndef BRAGI_PLAY_ATTACHMENT_H
#define BRAGI_PLAY_ATTACHMENT_H

#include <dpp/cluster.h>
#include "player/track.h"

class PlayAttachment {
public:
	static void Exec(dpp::cluster &bot, const dpp::slashcommand_t &event);
	static void Exec(dpp::cluster &bot, const dpp::message_create_t &event);
private:
	/* return the message with status of the executing
	 * throw a BragiException */
	static dpp::message Exec(dpp::cluster &bot, dpp::snowflake user_id, dpp::snowflake guild_id, dpp::snowflake channel_id, const Track *track);
};

#endif
