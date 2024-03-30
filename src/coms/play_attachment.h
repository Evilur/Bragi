#ifndef BRAGI_PLAY_ATTACHMENT_H
#define BRAGI_PLAY_ATTACHMENT_H

#include <dpp/cluster.h>
#include "track/track.h"

class PlayAttachment {
public:
	static void Exec(dpp::cluster &bot, const dpp::slashcommand_t &event);
	static void Exec(dpp::cluster &bot, const dpp::message_create_t &event);
private:
	static dpp::message Message(dpp::cluster &bot, dpp::snowflake user_id, dpp::snowflake guild_id, dpp::snowflake channel_id, const Track *track);
};

#endif
