#ifndef BRAGI_GUILD_PLAYER_H
#define BRAGI_GUILD_PLAYER_H

#include <dpp/cluster.h>
#include "track/track.h"

class GuildPlayer {
public:
	const dpp::snowflake guild_id;
	
	explicit GuildPlayer(dpp::snowflake* guild_id);
	
	/* Play track */
	void PlayTrack(dpp::cluster &bot, dpp::snowflake user_id, dpp::snowflake channel_id, const Track *track, dpp::message* message = nullptr);
	/* Check player for ready */
	bool IsPLayerReady();
	/* Connect to the voice channel */
	void ConnectVoice(dpp::snowflake channel_id);
	/* Reconnect to the voice channel */
	void Reconnect();
	
	/* Get a guild from the array
	 * return: a founded guild from the array */
	static GuildPlayer* Get(dpp::snowflake guild_id);
private:
	dpp::voiceconn* voiceconn;
	
	static inline const unsigned int guilds_delta = 8;
	static inline unsigned int _max_guild_count = guilds_delta;
	static inline unsigned int _guild_count = 0;
	static inline GuildPlayer** _guilds = new GuildPlayer*[guilds_delta];
	
	/* Add a new guild to the array
	 * return: a new guild object */
	static GuildPlayer* Add(dpp::snowflake guild_id);
};

#endif
